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
		/** Retourne un pointeur sur la structure wrappée.
			* A n'utiliser que depuis le wrapper.
			* @see Wrap
			*/
		struct tdc_problem* problem();

		/** Retourne le numero de la strategie.
			* A n'utiliser que depuis le wrapper.
			* @see Wrap
			*/
		int strategy();

  public:
		Strategy(struct tdc_problem *pb, int s);
		~Strategy();

		/** Retourne le nom de la strategie.
			* @returns une QString.
			*/
		QString name();
};

#endif /* STRATEGY_H */

