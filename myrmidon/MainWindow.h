#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>

#include "ProblemDialog.h"
#include "wrap/Wrap.h"

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	private:
		Wrap *fWrap;
		QMenu *fFileMenu;
		QMenu *fEditMenu;
		
	public:
		MainWindow(Wrap *wrap);
		~MainWindow();
		
	public slots:
		void newSimulation();
		void showResult(Job *j);
};

#endif /* MAINWINDOW_H */
