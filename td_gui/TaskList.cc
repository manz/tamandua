#include "TaskList.h"

TaskList::TaskList(size_t **t, size_t c) {
	ftasks=t;
	fcount=c;
}

TaskList::~TaskList() {
}

size_t TaskList::getCount() {
	return fcount;
}

const size_t TaskList::getWeight(int i) {
	if (i<fcount) {
		return ftasks[i][0];
	}
}

const size_t TaskList::getMachine(int i) {
	if (i<fcount) {
		return ftasks[i][1];
	}
}

