#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
extern "C" {
#include <td_base.h>
#include <td_core.h>
}
#include "Problem.h"

using namespace std;

class Context {
  private:
    struct tdc_context* fContext;
		vector<Problem> fProblems;
	public:
		Context(struct tdc_context *);
    int problemCount();
    Problem problem(int i);
};

#endif
