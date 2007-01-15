#include "MainWindow.h"

MainWindow::MainWindow(Wrap *wrap) : QMainWindow() {
	fWrap = wrap;
	
	fFileMenu=this->menuBar()->addMenu("Fichier");
	fEditMenu=this->menuBar()->addMenu("Edition");
	
	QAction *newSimul = new QAction("Nouvelle Simulation", this);
	fFileMenu->addAction(newSimul);

	QAction *quit = new QAction("Quitter", this);
	fFileMenu->addAction(quit);

	connect(newSimul, SIGNAL(triggered()), this, SLOT(newSimulation()));
	connect(quit, SIGNAL(triggered()), this, SLOT(close()));
	connect(fWrap, SIGNAL(result(Job*)), this, SLOT(showResult(Job*)));
	move(0,0);
}

MainWindow::~MainWindow() {
	
}

void MainWindow::showResult(Job *j) {
	// display
}

void MainWindow::newSimulation() {
	ProblemDialog pb(fWrap, 0);
	int res = pb.exec();
	if (res) {
		printf("%d %d\n", pb.problemSelected(), pb.strategySelected());
		printf("%d\n", pb.generator());
		fWrap->simulate(pb.generator());
	}
	else {
	}
	//printf("toto!=%d, %d\n", res, pb.result());
}
