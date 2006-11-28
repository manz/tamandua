#include <td_base.h>
#include <td_core.h>

const char tdc_name[] = "Dispatch";
const char* tdc_strategies[] = {
	"Méthode naïve",
	"Tâches triées",
	"Prog. dynamique"
};
const struct tdc_property tdc_globals[] = {
	{TDC_INPUT, 1, 100, "Machine"}
};
const struct tdc_property tdc_properties[] = {
	{TDC_INPUT|TDC_LENGTH, 1, UINT_MAX, "Durée"},
	{TDC_COLUMN, 0, UINT_MAX, "Machine"}
};

int compare_length
(const void* p1, const void* p2)
{
	return (int)(((size_t**)p1)[0]-((size_t**)p2)[0]);
}

void sort_tasks
(struct tdc_job* job)
{
	size_t i;
	/* count tasks */
	for (i=0; job->tasks[i]!=NULL; i++) {}

	qsort(job->tasks, i, sizeof(*job->tasks), compare_length);
}

int dispatch
(struct tdc_job* job)
{
	size_t i,j,min=0;
	size_t* fill;
	fill= tdb_calloc(job->globals[0], sizeof(*fill));
	/* iterate on all tasks */
	for(i=0; job->tasks[i]!= NULL; i++) {
		fill[min] += job->tasks[i][0];
		job->tasks[i][1] = min;
		/* find the new less filled machine */
		for (j=0; j<job->globals[0]; j++) {
			if(fill[min] > fill[j]) {
				min = j;
			}
		}
	}
	for (i=0; job->tasks[i]!= NULL; i++) {
		tdb_debug("Tâche de longueur %u sur la machine %u",
		         job->tasks[i][0], job->tasks[i][1]);
	}
	free(fill);
	return EXIT_SUCCESS;
}

int problem
(size_t strategy, struct tdc_job* job)
{
	switch (strategy) {
		case 1:
			sort_tasks(job);
		case 0:
			return dispatch(job);
		case 2:
		default:
			tdb_warn("Strategy not implemented");
			return EXIT_FAILURE;
	}
}

TDC_DECLARE(problem, tdc_name, tdc_strategies, tdc_globals, tdc_properties);
