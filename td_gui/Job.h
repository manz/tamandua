#ifndef JOB_H
#define JOB_H

#include <td_base.h>
#include <td_core.h>

#include "TaskList.h"

class Job {
	private:
		struct tdc_job *fjob;
		TaskList *ftasks;
	public:
		Job(struct tdc_job *j);
		~Job();

	  TaskList* getTaskList();
};

#endif /* JOB_H */
