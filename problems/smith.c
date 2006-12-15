#include <td_base.h>
#include <td_core.h>

const char tdc_name[] = "Smith";
const char* tdc_strategies[] = {
	"Smith"
};
const struct tdc_property tdc_globals[] = {
	{TDC_INPUT, 1, 100, "Machine"}
};
const struct tdc_property tdc_properties[] = {
	{TDC_INPUT|TDC_LENGTH, 1, UINT_MAX, "Durée"  },
	{TDC_INPUT|TDC_LENGTH, 1, UINT_MAX, "Poid"   },
	{TDC_COLUMN,           0, UINT_MAX, "Machine"}
};

int compare_tomp
(const void* p1, const void* p2)
{
	const struct {
		double tomp;
		size_t id;
	} *d1, *d2;
	d1 = p1;
	d2 = p2;
	if (d1->tomp < d2->tomp)
    return -1;
  if (d1->tomp > d2->tomp)
    return 1;
 
  return 0;
}

int smith
(struct tdc_job* job)
{
	size_t i;
	size_t *tmp[job->tasks_count];
	/* Temps Operatoire Minimum Pondere aka TOMP */
	struct {
		double quotien;
		size_t id;
	} tomp[job->tasks_count]; 

	for (i=0; job->tasks[i]!=NULL; i++) {
			tomp[i].quotien = (double)job->tasks[i][0] / (double)job->tasks[i][1];
			tomp[i].id = i;
			tmp[i] = job->tasks[i];
	}
	qsort(tomp, job->tasks_count, sizeof(*tomp), compare_tomp);
	for (i=0; i<job->tasks_count; i++) {
			job->tasks[i] = tmp[tomp[i].id];
      job->tasks[i][2] = i%job->globals[0];
	}

  for (i=0; job->tasks[i]!= NULL; i++) {
		tdb_debug("Tâche de longueur %u et de poid %u sur la machine %u",
		         job->tasks[i][0], job->tasks[i][1], job->tasks[i][2]);
	}
  return EXIT_SUCCESS;
}

int problem
(size_t strategy, struct tdc_job* job)
{
	return smith(job);
	/*
	switch (strategy) {
		case 0:
			return smith(job);
		default:
			tdb_warn("Strategy not implemented");
			return EXIT_FAILURE;
	}
	*/
}

TDC_DECLARE(problem, tdc_name, tdc_strategies, tdc_globals, tdc_properties);
