#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
extern "C" {
#include <td_base.h>
#include <td_core.h>
}
#include "Problem.h"

using namespace std;

/** Abstraction de la structure tdc_context.
	*/
class Context {
  private:
    struct tdc_context* fContext;
		vector<Problem*> fProblems;
	public:
		/** Constructeur à un paramètre.
			* Normalement à ne pas utiliser en dehors du wrapper.
			* @param un pointeur sur un tdc_context.
			*/
		Context(struct tdc_context *);
		~Context();
		/** Retourne le nombre de problèmes.
			* @returns le nombre de problemes.
			*/
    int problemCount();

		/** Retourne le ieme-Probleme.
			* @returns un Problem.
			*/
    Problem* problem(int i);
};

#endif
