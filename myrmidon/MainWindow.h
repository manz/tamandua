#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>

#include "ShowWindow.h"

#include "ProblemDialog.h"
#include "wrap/Wrap.h"

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	private:
		Wrap *fWrap;
		QMenu *fFileMenu;
		QMenu *fEditMenu;
		std::vector <ShowWindow> *vWinSim; //liste des fenetres de simulation
		ProblemDialog *problemDialog;
		
	public:
		MainWindow(Wrap *wrap);
		~MainWindow();
		
	public slots:
		void newSimulation();
		void showResult(Job *j);
};

#endif /* MAINWINDOW_H */
