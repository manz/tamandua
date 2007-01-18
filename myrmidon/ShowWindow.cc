#include "ShowWindow.h"

ShowWindow::ShowWindow( Job *j, QWidget *parent) : QGraphicsScene( ){
    job = j;

    QString *text = new QString;
    text->append( QString::fromUtf8( "Problème "));
    text->append( job->problem()->name());
    text->append( QString::fromUtf8( ", stratégie "));
    text->append( job->problem()->strategy( job->strategyNumber())->name());
    
    QGraphicsTextItem *textItem = new QGraphicsTextItem;
    textItem = this->addText( *text);
    
    int x, y, x1, y1;

    TaskItem *taskItem;
    QColor colorRect;

		double *machines = new double[job->machineCount()];
		int max_len=0;

		Task *task;
		for (int m=0; m<job->tasksCount(); m++) {
			task = job->task(m);
			for (int k=0; k<task->stepCount(); k++) {
				if (machines[task->stepMachine(k)] < task->stepStartTime(k)
						+ task->stepLength(k))
					machines[task->stepMachine(k)] = task->stepStartTime(k)
						+ task->stepLength(k);
				if (machines[max_len] < machines[task->stepMachine(k)])
					max_len = task->stepMachine(k);
			}
		}

    bool tabBool[ job->machineCount()];
    for( int i = 0; i < job->machineCount(); i++) {
        tabBool[i] = true;
    }
    for( int i = 0; i < job->tasksCount(); i++) {
        QRect rectTask;
        Task *taskP = job->task( i);
        if( tabBool[ taskP->stepMachine( 0)]) {
            colorRect = Qt::blue;
        }
        else {
            colorRect = Qt::red;
        }
        tabBool[ taskP->stepMachine( 0)] = !tabBool[ taskP->stepMachine( 0)];
        for( int j = 0; j < taskP->stepCount(); j++) {
            x = 10 + 2 * taskP->stepStartTime(j);
            y = (int) textItem->boundingRect().bottom() + (30 * ( 1 +taskP->stepMachine(j))) + (50 * taskP->stepMachine(j));

            rectTask.setX(x);
            rectTask.setY(y);
            //rectTask.setX( 10 + 2 * taskP->stepStartTime( j));
            //rectTask.setY( (int) textItem->boundingRect().bottom() + ( 10 * ( 1 + taskP->stepMachine( j)))+ ( 50 * taskP->stepMachine( j)));
            if (j>0) {
                addLine(QLineF(x1, y1+50, x+taskP->stepLength(j), y), QPen(colorRect)); 
            }

            if (i==job->tasksCount()-1 && taskP->stepCount() > 1) {
							QGraphicsTextItem *len = addText(QString("%1").arg(taskP->stepLength(j)+taskP->stepStartTime(j)));
							QPointF pt;
							pt.setX(10+2*(taskP->stepLength(j)+taskP->stepStartTime(j)));
							pt.setY(y);
							len->setPos(pt);
						}
						if (taskP->stepCount()==1 && (taskP->stepLength(j) + taskP->stepStartTime(j) == machines[taskP->stepMachine(j)])) {
							QGraphicsTextItem *len = addText(QString("%1").arg(taskP->stepLength(j)+taskP->stepStartTime(j)));
							if (taskP->stepMachine(j) == max_len)
								len->setDefaultTextColor(Qt::red);
							QPointF pt;
							pt.setX(10+2*(taskP->stepLength(j)+taskP->stepStartTime(j)));
							pt.setY(y);
							len->setPos(pt);
							printf("gué ?\n");
						}


            rectTask.setHeight(50);
            rectTask.setWidth( 2 * taskP->stepLength( j));

            TaskItem *it=new TaskItem(taskP,job->problem()->isWeighted(), j, 0, this);
            it->setColor(colorRect);
            it->setRect(rectTask);
            x1=x+taskP->stepLength(j);
            y1=y;
          }
				
        }
		delete machines;
}

TaskItem::TaskItem( Task *t, bool isWeighted, int step, QGraphicsItem * parent , QGraphicsScene * scene ) : QGraphicsRectItem( parent, scene) {
    task = t;
		fToolTip = QString::fromUtf8("id: %1\nDurée: %2\nStartime: %3").arg(task->id()).arg(task->stepLength(step)).arg(task->stepStartTime(step));
		if (isWeighted)
				 fToolTip=fToolTip+QString("\nPoids: %4").arg(t->weight());

}


void TaskItem::paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) {
    QPainterPath path;
    QLinearGradient grad(rect().x(), rect().y(), rect().x()+rect().width(), rect().y()+rect().height());

    grad.setColorAt(0,  color());
    grad.setColorAt(1, color().dark());
    painter->setPen(Qt::NoPen);
    painter->setBrush(grad);
    painter->drawRect(rect());
}

QString TaskItem::toolTip() {
	return fToolTip;
}

void TaskItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
	QToolTip::showText(QPoint(event->screenPos().x(), event->screenPos().y()), fToolTip);
}

void TaskItem::setColor(QColor c) {
    fColor = c;
}

QColor TaskItem::color() {
    return fColor;
}

