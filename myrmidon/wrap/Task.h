#ifndef TASK_H
#define TASK_H

class Task {
	private:
	public:
		Task();
		~Task();

		int id();
		int weight();
		int stepCount();

		int stepLength(int i);
		int stepMachine(int i);
		int stepStartTime(int i);
};

#endif /* TASK_H */

