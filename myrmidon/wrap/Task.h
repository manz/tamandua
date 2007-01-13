#ifndef TASK_H
#define TASK_H

extern "C" {
	#include <td_base.h>
	#include <td_core.h>
}

class Task {
	private:
		struct tdc_task *fTask;
	public:
		Task(struct tdc_task *task);
		~Task();

		int id();
		int weight();
		int stepCount();

		int stepLength(int i);
		int stepMachine(int i);
		int stepStartTime(int i);
};

#endif /* TASK_H */

