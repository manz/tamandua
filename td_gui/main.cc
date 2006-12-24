// includes Qt
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

// tamandua core
extern "C" {
#include <td_base.h>
#include <td_core.h>
};

// wrapper
#include "wrap/Context.h"

int main(int argc, char** argv) {
	// initialisation de la base
	tdb_init(stdout);
	// création d'un contexte
	struct tdc_context *c = tdc_init("problems/", 0, 1);
	// setcontext
	tdc_setcontext(c);
	Context cntxt(c);

	// constructeur par copie
	Problem tmp(cntxt.problem(0));

	for (int i=0; i<cntxt.problemCount(); i++) {
		printf("probleme: %s\n", cntxt.problem(i).name());
		tmp=cntxt.problem(i);
		printf("description: %s\n", tmp.description());
		for (int j=0;j<tmp.strategiesCount();j++) {
			printf("strategie[%d] : %s\n", j, tmp.strategy(j).name());
		}
	}

	// terminaison du core
	tdc_exit();
	tdb_exit();

}

