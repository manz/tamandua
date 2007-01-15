#include <QGraphicsView>
#include <QDockWidget>

#include "MainWindow.h"
#include "ShowWindow.h"

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
        //QDockWidget *dockWidget = new QDockWidget( this);;

	ShowWindow *showWindow = new ShowWindow( j);
	
	QGraphicsView *windowView= new QGraphicsView( showWindow);
	windowView->setAlignment( Qt::AlignLeft | Qt::AlignTop);
	windowView->show();
	//dockWidget->setWidget( (QWidget*) &windowView);
	//addDockWidget( Qt::TopDockWidgetArea, dockWidget);
	printf( "Affichage\n");
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
