#include "MainMenuBar.h"

MainMenuBar::MainMenuBar(Wrap *wrap) : QMenuBar() {

	fWrap = wrap;
	fFileMenu=this->addMenu("Fichier");
	fEditMenu=this->addMenu("Edition");
	
	QAction *newSimul = new QAction("Nouvelle Simulation", this);
	fFileMenu->addAction(newSimul);

	QAction *quit = new QAction("Quitter", this);
	fFileMenu->addAction(quit);

	connect(newSimul, SIGNAL(triggered()), this, SLOT(nouvelleSimulation()));
	connect(quit, SIGNAL(triggered()), this, SLOT(close()));
	move(0,0);
	if (this->window()) {
		this->window()->setWindowTitle("Myrmidon");
	}
}

MainMenuBar::~MainMenuBar() {
}

void MainMenuBar::nouvelleSimulation() {
	ProblemDialog pb(fWrap, this);
	int res = pb.exec();
	if (res) {
		printf("%d %d\n", pb.problemSelected(), pb.strategySelected());
	}
	else {
	}
	//printf("toto!=%d, %d\n", res, pb.result());
}


