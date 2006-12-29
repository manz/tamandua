#include "Context.h"

Context::Context(struct tdc_context *c) {
	fContext = c;

	for (int i=0;i<problemCount();i++) {
		fProblems.push_back(Problem(fContext->problems[i]));
	}
}

int Context::problemCount() {
	return fContext->n_problems;
}

Problem Context::problem(int i) {
	return fProblems.at(i);
}

