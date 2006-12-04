#include "mainwindow.h"

MainWindow::MainWindow() : QMainWindow() {
	fwrap=new td_wrap();
	QPushButton *toto=new QPushButton("whee", this);

  fwrap->addJob(17, 5, 200, 1);
	fwrap->addJob(10, 5, 10, 1);
	
	connect(toto, SIGNAL(clicked()), fwrap, SLOT(commit()));
	connect(fwrap, SIGNAL(jobResult(Job*)), this, SLOT(showResult(Job*)));
}

MainWindow::~MainWindow() {
	delete fwrap;
}

void MainWindow::showResult(Job *job) {
	QMessageBox::information( this, "Application name here",
			QString("tasks count: %1").arg(job->getTaskList()->getCount()),
			"&Save", "&Discard", "Cancel",
			0,      // Enter == button 0
			2 );
}

