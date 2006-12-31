#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include "td_wrap.h"
#include "Job.h"
#include "TaskList.h"

/** Classe MainWindow.
 * Fenètre principale.
 */
class MainWindow : public QMainWindow {
	Q_OBJECT
	private:
		td_wrap *fwrap;
	public:
		MainWindow();
		~MainWindow();
		
	public slots:
		void showResult(Job *job);
};

#endif /* MAINWINDOW_H */

