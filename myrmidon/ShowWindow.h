#ifndef SHOWWINDOW_H
#define SHOWWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPainter>
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
        TaskItem (Task *t, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
				void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
        void setColor(QColor c);
				QColor color();

    private:
        Task *task;
				QColor fColor;
};

#endif /* SHOWWINDOW_H */
