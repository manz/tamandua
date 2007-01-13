#include "Task.h"

Task::Task(struct tdc_task *task) {
	fTask = task;
}

Task::~Task() {
	
}

int Task::id() {
	return fTask->id;
}

int Task::weight() {
	return fTask->weight;
}

int Task::stepCount() {
	return fTask->n_steps;
}

int Task::stepLength(int i) {
	return fTask->steps[i].length;
}

int Task::stepMachine(int i) {
	return fTask->steps[i].machine;
}

int Task::stepStartTime(int i) {
	return fTask->steps[i].start_time;
}