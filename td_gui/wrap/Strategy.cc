#include "Strategy.h"

Strategy::Strategy(struct tdc_problem *pb, int s) {
	fProblem=pb;
	fStrategy=s;
}

Strategy::~Strategy() {
}

const char* Strategy::name() {
	return fProblem->strategies[fStrategy];
}

struct tdc_problem *Strategy::problem() {
	return fProblem;
}

int Strategy::strategy() {
	return fStrategy;
};
