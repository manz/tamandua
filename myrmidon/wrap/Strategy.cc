#include "Strategy.h"

Strategy::Strategy(const struct tdc_problem *pb, int s) {
	fProblem=pb;
	fStrategy=s;
}

Strategy::~Strategy() {
}

QString Strategy::name() {
	return QString::fromUtf8(fProblem->strategies[fStrategy]);
}

const struct tdc_problem *Strategy::problem() {
	return fProblem;
}

int Strategy::strategy() {
	return fStrategy;
}
