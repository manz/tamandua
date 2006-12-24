#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
extern "C" {
#include <td_base.h>
#include <td_core.h>
}
#include "Strategy.h"

using namespace std;

class Problem {
  private:
    struct tdc_problem* fProblem;
		vector<Strategy> fStrategies;
  
	public:
		Problem(struct tdc_problem* pb);
		~Problem();
    Strategy strategy (int i);
		int strategiesCount();
    bool isWeighted ();

		const char* name();
		const char* description();
};

#endif /* PROBLEM_H */
