#include <td_base.h>
#include <td_core.h>

#include "smith.h"

int compare_tomp
(const void* p1, const void* p2)
{
	struct tdc_task* const * a = p1;
	struct tdc_task* const * b = p2;
	return (*a)->steps[0].length*(*b)->weight - (*b)->steps[0].length*(*a)->weight;
}

int sort_tasks
(struct tdc_job* job)
{
	qsort(job->tasks, job->n_tasks, sizeof(*job->tasks), compare_tomp);
	return EXIT_SUCCESS;
}

int problem
(size_t strategy, struct tdc_job* job)
{
	switch (strategy) {
		case 0:
			return sort_tasks(job);
		default:
			tdb_warn("Strategy not implemented");
			return EXIT_FAILURE;
	}
}
