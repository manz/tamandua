#include <td_base.h>
#include <td_core.h>

#include "dispatch.h"

struct fill_heap {
	size_t fill;
	size_t machine;
};

struct fill_heap* create_heap
(size_t size)
{
	struct fill_heap* ret;
	ret = tdb_calloc(size, sizeof(*ret));
	for (size_t i=0; i<size; i++) {
		ret[i].machine = i;
	}
	return ret;
}

void swap_items
(struct fill_heap* self, size_t a, size_t b)
{
	struct fill_heap tmp;
	tmp = self[a];
	self[a] = self[b];
	self[b] = tmp;
}

size_t increment_first
(size_t length, struct fill_heap* self, size_t size)
{
	size_t i, j, ret;
	swap_items(self, 0, size-1);
	/* bubble down the root */
	for (i=0; (i*2+1)<(size-1);i=j) {
		j=i*2+1;
		if ((j+1)>=(size-1) || self[j].fill < self[j+1].fill) {
			if (self[j].fill < self[i].fill) {
				swap_items(self, i, j);
			}
		} else if ((j+1)<(size-1) && self[j+1].fill < self[i].fill) {
			swap_items(self, i, j+1);
			j++;
		}
	}
	self[size-1].fill += length;
	ret = self[size-1].machine;
	/* bubble up old root */
	for (i=size-1; i>0 && self[(i-1)/2].fill > self[i].fill; i=(i-1)/2) {
		swap_items(self, i, (i-1)/2);
	}
	return ret;
}

void delete_heap
(struct fill_heap* self)
{
	free(self);
}

int compare_length
(const void* p1, const void* p2)
{
	struct tdc_task* const * a = p1;
	struct tdc_task* const * b = p2;
	return (*b)->steps[0].length - (*a)->steps[0].length;
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
	struct tdc_task** tasks = job->tasks;
	struct fill_heap* fill_heap = create_heap(job->n_machines);
	/* iterate on all tasks */
	for(i=0; i<job->n_tasks; i++) {
		tasks[i]->steps[0].machine = increment_first(tasks[i]->steps[0].length, fill_heap, job->n_machines);;
	}
	delete_heap(fill_heap);
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
		default:
			tdb_warn("Strategy not implemented");
			return EXIT_FAILURE;
	}
}
