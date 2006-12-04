#ifndef TASKLIST_H
#define TASKLIST_H

extern "C" {
	#include <td_core.h>
	#include <td_base.h>
};

class TaskList {
	private:
		size_t **ftasks;
		size_t fcount;
	public:
		TaskList(size_t **t, size_t c);
		~TaskList();
		
		size_t getCount();
		const size_t getWeight(int i);
		const size_t getMachine(int i);
};

#endif /* TASKLIST_H */

