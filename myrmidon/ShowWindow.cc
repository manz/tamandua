#include "ShowWindow.h"

ShowWindow::ShowWindow( Job *j, QWidget *parent) : QGraphicsScene( ){
    job = j;
    
    setBackgroundBrush( Qt::white);
    QString *text = new QString;
    text->append( QString::fromUtf8( "Problème "));
    text->append( job->problem()->name());
    text->append( QString::fromUtf8( ", stratégie "));
    text->append( job->problem()->strategy( job->strategyNumber())->name());
    
    QGraphicsTextItem *textItem = new QGraphicsTextItem;
    textItem = this->addText( *text);
    
    TaskItem *taskItem;
    QColor colorRect;
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
            rectTask.setX( 10 + 2 * taskP->stepStartTime( j));
            rectTask.setY( (int) textItem->boundingRect().bottom() + ( 10 * ( 1 + taskP->stepMachine( j)))+ ( 50 * taskP->stepMachine( j)));
            rectTask.setHeight( 50);
            rectTask.setWidth( 2 * taskP->stepLength( j));
            printf("Tache : id = %d, step = %d", i, j);
            printf("Rectangle : x = %d, y= %d, width = %d, height = %d\n", rectTask.x(), rectTask.y(), rectTask.width(), rectTask.height());
            addRect( rectTask, QPen( Qt::black), QBrush( colorRect));
        }
    }
}

TaskItem::TaskItem( Task *t, QGraphicsItem * parent , QGraphicsScene * scene ) : QGraphicsRectItem( parent, scene) {
    task = t;
}