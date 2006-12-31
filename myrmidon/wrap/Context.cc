#include "Context.h"

Context::Context(struct tdc_context *c) {
	fContext = c;

	for (int i=0;i<problemCount();i++) {
		fProblems.push_back(new Problem(fContext->problems[i]));
	}
}

Context::~Context() {
	for (int i=0;i<fProblems.size();i++)
		delete fProblems.at(i);
}

int Context::problemCount() {
	return fContext->n_problems;
}

Problem* Context::problem(int i) {
	return fProblems.at(i);
}

