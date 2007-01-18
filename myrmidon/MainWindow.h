#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>

#include "ShowWindow.h"

#include "ProblemDialog.h"
#include "wrap/Wrap.h"

class SimulItem : public QGraphicsPathItem {
    protected:
        void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
        
    private:
        QGraphicsView *win;
        
    public:
        SimulItem( QGraphicsView *newWin);
};

class MainWindow : public QMainWindow {
    Q_OBJECT
	
    private:
        Wrap *fWrap;
        QMenu *fFileMenu;
        QMenu *fEditMenu;
        QAction *fQuit;
        ProblemDialog *problemDialog;
        QGraphicsScene *scene;
        float lastItemPosY;
        SimulItem *lastItem;
		
    public:
        MainWindow(Wrap *wrap, QApplication *app);
        ~MainWindow();
		
    signals:
        void closeWin();
	   
    public slots:
        bool close();
        void newSimulation();
        void showResult(Job *j);
        
    protected:
        void hideEvent ( QHideEvent * event );
};

#endif /* MAINWINDOW_H */
