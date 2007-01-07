#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <QString>

extern "C" {
#include <td_base.h>
#include <td_core.h>
}
#include "Strategy.h"

using namespace std;

/** Abstraction de la structure tdc_problem.
	*/
class Problem {
  private:
    const struct tdc_problem* fProblem;
		vector<Strategy*> fStrategies;
  
	public:
		/** Constructeur à un paramètre.
			* A n'utiliser que depuis le wrapper.
			*/
		Problem(const struct tdc_problem* pb);

		/** Destructeur.
			*/
		~Problem();

		/** Retourne la ieme Strategie du probleme.
			* @returns un objet Strategy.
			*/
    Strategy* strategy (int i);

		/** Retourne le nombre de strategies du probleme.
			* @returns un entier.
			*/
		int strategiesCount();

		/** Retourne vrai si la longueur des taches du probleme
			* est pondérée par des poids. faux sinon.
			* @returns un booleen.
			*/
    bool isWeighted();

		/** Retourne le nombre de steps du probleme.
		 */
		int stepsCount();

		/** Retourne le nom du probleme.
			* @returns une QString.
		  */
		QString name();

		/** Retourne la description du probleme.
			*/
		QString description();

	protected:
		struct tdc_problem *realProblem();
};

#endif /* PROBLEM_H */
