#include "Job.h"

Job::Job(struct tdc_job *j) :
	fJob(j)
{
	fProblem = new Problem(fJob->problem);
}

Problem *Job::problem() {
}

int Job::machineCount() {
	return fJob->n_machines;
}

int Job::strategyNumber() {
	return fJob->strategy;
}

int Job::tasksCount() {
	return fJob->n_tasks;
}

Job::~Job() {
	delete fProblem;
}

