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
		ProblemDialog *problemDialog;
		QGraphicsScene *scene;
		float lastItemPosY;
		QGraphicsRectItem *lastItem;
		
	public:
		MainWindow(Wrap *wrap);
		~MainWindow();
		
	public slots:
		void newSimulation();
		void showResult(Job *j);
};

class SimulItem : public QGraphicsRectItem {
    protected:
        void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
        
    private:
        QGraphicsView *win;
        
    public:
        SimulItem( QGraphicsView *newWin);
};
#endif /* MAINWINDOW_H */
