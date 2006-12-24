#include "Problem.h"

Problem::Problem(struct tdc_problem *pb) : fProblem(pb) {
	for (int i=0;i<strategiesCount();i++) {
		fStrategies.push_back(Strategy(pb, i));
	}
}

Problem::~Problem() {
}

Strategy Problem::strategy(int i) {
	return fStrategies.at(i);
}

int Problem::strategiesCount() {
	return fProblem->n_strategies;
}

bool Problem::isWeighted() {
	return (fProblem->tasks.weighted != 0);
}

const char* Problem::name() {
	return fProblem->name;
}

const char* Problem::description() {
	return fProblem->description;
}

