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
}

Wrap::~Wrap() {
	delete fContext;
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
	
	tdgen->n_machines = g->machineCount();

	for (int i=0;i<g->populationCount();i++) {
		pop = &tdgen->populations[i];
		pop->n_tasks=g->taskCount();

		if (tdc_problem_weight_enabled(pb)) {
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
	tdc_commit(job), job=NULL;
}
