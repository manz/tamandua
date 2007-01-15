#include "Wrap.h"

Wrap::Wrap(QString &path) {
	tdb_init(stdout, 0, NULL);	
	// conversion de la QString en const char*
	QByteArray ba( path.toLatin1() );
	const char *cpath = ba.data();

	fRContext = tdc_init(cpath, 0, 1);
	tdc_setcontext(fRContext);

	//tdc_test();
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
	printf("Problem Number %d\n", g->problemNumber());

	struct tdc_problem *pb = fRContext->problems[g->problemNumber()];
	struct tdc_generator *tdgen = tdc_create_generator(pb, g->populationCount());
	struct tdc_population_generator *pop;
	
	printf("nom du probleme %s\n", pb->name);

	tdgen->n_machines = g->machineCount();

	for (int i=0;i<g->populationCount();i++) {
		pop = &tdgen->populations[i];
		pop->n_tasks=g->taskCount();

		if (tdc_problem_weight_enabled(pb)) {
			printf("Min : %d\nMax : %d\n", g->weightMin(), g->weightMax());
			pop->weight.min = g->weightMin();
			pop->weight.max = g->weightMax();
		}

		for (int j=0; j<pb->tasks.steps; j++) {
			printf("Min : %d\nMax : %d\n", g->lengthMin(j), g->lengthMax(j));
			tdb_interval_set_min(&pop->lengths[j], g->lengthMin(j));
			tdb_interval_set_max(&pop->lengths[j], g->lengthMax(j));
			/*
			pop->lengths[j].min = g->lengthMin(j);
			pop->lengths[j].max = g->lengthMax(j);
			*/
		}
	}
	
	struct tdc_job *job = tdc_create_job(tdgen);

	job->strategy=g->strategyNumber();

	/*
	if (tdc_problem_n_machines_settable(pb))
		job->n_machines=4;
		*/
	fTimer->start(100);
	tdc_commit(job), job=NULL;
//	tdc_checkout
}

void Wrap::_CheckOut() {
	struct tdc_job *res = tdc_checkout();
	if (res) {
		// le job à bien été traité.
		fTimer->stop();
		printf("Timer stop, result checkouted\n");
		emit result(new Job(res));
	} else {
		// sinon on attends :p
		printf("on attends ...\n");
	}
}
