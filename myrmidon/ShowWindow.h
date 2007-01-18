#ifndef SHOWWINDOW_H
#define SHOWWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QToolTip>
#include <QGraphicsSceneEvent>
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
        TaskItem (Task *t, bool isWeighted, int step, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
				void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
        void setColor(QColor c);
				QColor color();
				QString toolTip();
		protected:
				void mousePressEvent ( QGraphicsSceneMouseEvent * event );
		private:
        Task *task;
				QColor fColor;
				QString fToolTip;
};

#endif /* SHOWWINDOW_H */
