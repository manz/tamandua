#include <td_base.h>
#include <td_core.h>

#include "dispatch.h"

int compare_length
(const void* p1, const void* p2)
{
	struct tdc_task* const * a = p1;
	struct tdc_task* const * b = p2;
	return (*a)->steps[0].length - (*b)->steps[0].length;
}

void sort_tasks
(struct tdc_job* job)
{
	qsort(job->tasks, job->n_tasks, sizeof(*job->tasks), compare_length);
}

int dispatch
(struct tdc_job* job)
{
	size_t i,j,min=0;
	size_t* fill;
	struct tdc_task** tasks = job->tasks;
	fill= tdb_calloc(job->n_machines, sizeof(*fill));
	/* iterate on all tasks */
	for(i=0; i<job->n_tasks; i++) {
		fill[min] += tasks[i]->steps[0].length;
		tasks[i]->steps[0].machine = min;
		/* find the new less filled machine */
		for (j=0; j<job->n_machines; j++) {
			if(fill[min] > fill[j]) {
				min = j;
			}
		}
	}
	for (i=0; i<job->n_tasks; i++) {
		tdb_debug("Tâche de longueur %u sur la machine %u",
		         job->tasks[i]->steps[0].length, job->tasks[i]->steps[0].machine);
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
