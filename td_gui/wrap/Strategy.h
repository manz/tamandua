#ifndef STRATEGY_H
#define STRATEGY_H

#include <QString>

extern "C" {
#include <td_base.h>
#include <td_core.h>
};

class Strategy {
  private:
    struct tdc_problem* fProblem;
    int fStrategy;

	protected:
		struct tdc_problem* problem();
		int strategy();

  public:
		Strategy(struct tdc_problem *pb, int s);
		~Strategy();

		QString name();
};

#endif /* STRATEGY_H */
