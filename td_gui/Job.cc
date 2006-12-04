#include "Job.h"

Job::Job(struct tdc_job *j) {
	fjob=j;
	ftasks=new TaskList(fjob->tasks, fjob->tasks_count);
}

Job::~Job() {
	delete ftasks;
}

TaskList* Job::getTaskList() {
	return ftasks;
}
