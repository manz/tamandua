#include "Wrap.h"

Wrap::Wrap(QString &path) {
	fJobCount = 0;
	tdb_init(stdout, 0, NULL);	
	// conversion de la QString en const char*
	QByteArray ba( path.toLatin1() );
	const char *cpath = ba.data();

	fRContext = tdc_init(cpath, 0, 1);
	tdc_setcontext(fRContext);

	fContext = new Context(fRContext);
	fTimer = new QTimer();
	connect(fTimer, SIGNAL(timeout()), this, SLOT(_CheckOut()));
}

Wrap::~Wrap() {
	delete fContext;
	delete fTimer;
	tdc_exit();
	tdb_exit();
}

Context* Wrap::context() {
	return fContext;
}

void Wrap::simulate(Generator *g) {
	struct tdc_problem *pb = fRContext->problems[g->problemNumber()];
	struct tdc_generator *tdgen = tdc_create_generator(pb, g->populationCount());
	struct tdc_population_generator *pop;
		
	if (tdc_problem_n_machines_settable(pb))
		tdgen->n_machines = g->machineCount();

	for (int i=0;i<g->populationCount();i++) {
		pop = &tdgen->populations[i];
		pop->n_tasks=g->taskCount();

		if (tdc_problem_weight_enabled(pb)) {
			pop->weight.min = g->weightMin();
			pop->weight.max = g->weightMax();
		}

		for (int j=0; j<pb->tasks.steps; j++) {
			tdb_interval_set_min(&pop->lengths[j], g->lengthMin(j));
			tdb_interval_set_max(&pop->lengths[j], g->lengthMax(j));
		}
	}
	
	struct tdc_job *job;
	if (g->compareStrategies()) {
		job = tdc_create_job(tdgen);
		
		struct tdc_job *copy;

		for (int i=0;i<pb->n_strategies;i++) {
			copy = tdc_copy_job(job);
			job->strategy=i;
			tdc_commit( job);
			job = copy;
			fJobCount++;
		}
	}
	else {
	       fJobCount++;
		job = tdc_create_job(tdgen);
		job->strategy=g->strategyNumber();
		
		tdc_commit(job);
	}
	fTimer->start(100);
}

void Wrap::_CheckOut() {
	struct tdc_job *res = tdc_checkout();
	if (res) {
		// le job à bien été traité.
		fJobCount--;
		
		if (!fJobCount) {
			fTimer->stop();
		}
		
		emit result(new Job(res));
	} 
}
