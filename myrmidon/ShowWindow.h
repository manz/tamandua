#ifndef SHOWWINDOW_H
#define SHOWWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include "wrap/Job.h"

class Qwidget;
class QGraphicsItem;

class ShowWindow : public QGraphicsScene {
    Q_OBJECT
            
    public:
        ShowWindow( Job *j, QWidget *parent=0);
        
    private:
        Job *job;
};

class TaskItem : public QGraphicsRectItem {
            
    public:
        TaskItem( Task *t, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0 );
        
    private:
        Task *task;
};

#endif /* SHOWWINDOW_H */
