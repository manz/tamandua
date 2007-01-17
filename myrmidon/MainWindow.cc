#include <QGraphicsView>
#include <QDockWidget>

#include "MainWindow.h"
#include "ShowWindow.h"

MainWindow::MainWindow(Wrap *wrap) : QMainWindow() {
    fWrap = wrap;
    problemDialog = NULL;
    lastItemPosY = 0;
    lastItem = NULL;
	
    fFileMenu=this->menuBar()->addMenu("Fichier");
    fEditMenu=this->menuBar()->addMenu("Edition");
	
    QAction *newSimul = new QAction("Nouvelle Simulation", this);
    fFileMenu->addAction(newSimul);

    QAction *quit = new QAction("Quitter", this);
    fFileMenu->addAction(quit);
    
    scene = new QGraphicsScene();
    
    QGraphicsView *view = new QGraphicsView( scene, this);
    view->setAlignment( Qt::AlignCenter);
    view->show();
    
    
    setCentralWidget( view);
    
    connect(newSimul, SIGNAL(triggered()), this, SLOT(newSimulation()));
    connect(quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(fWrap, SIGNAL(result(Job*)), this, SLOT(showResult(Job*)));
    //move(0,0);
}

MainWindow::~MainWindow() {
	
}

void MainWindow::showResult(Job *j) {
    ShowWindow *showWindow = new ShowWindow( j);
	
    QGraphicsView *windowView= new QGraphicsView( showWindow);
    windowView->setAlignment( Qt::AlignLeft | Qt::AlignTop);
    windowView->show();
    
    SimulItem *rectItem = new SimulItem( windowView);
    rectItem->setPen( QPen( Qt::black));
    rectItem->setBrush( QBrush( Qt::NoBrush));
    
    QGraphicsTextItem *textItem;
    QString text;
    text.append( QString::fromUtf8( "Problème "));
    text.append( j->problem()->name());
    text.append( QString::fromUtf8( ", stratégie "));
    text.append( j->problem()->strategy( j->strategyNumber())->name());
    text.append( "\n");
    text.append( "\tNombre de taches : ");
    QString stringNum;
    stringNum.setNum( j->tasksCount());
    text.append( stringNum);
    text.append( "\n");
    text.append( "\tNombre de machines : ");
    stringNum.setNum( j->machineCount());
    text.append( stringNum);
    
    textItem = new QGraphicsTextItem( text);
    
    if( lastItem)
        rectItem->setRect( 0, 15 + lastItem->boundingRect().bottom(), 20 + textItem->boundingRect().width(), 20 + textItem->boundingRect().height());
    else
        rectItem->setRect( 0, 15, 20 + textItem->boundingRect().width(), 20 + textItem->boundingRect().height());
    textItem->setPos( QPointF( rectItem->rect().center().x() - (textItem->boundingRect().width() / 2), rectItem->rect().center().y() - (textItem->boundingRect().height() / 2)));
    
    textItem->setParentItem( rectItem);
    rectItem->setParentItem( lastItem);
    
    lastItem = rectItem;
    //rectItem->setPos( QPointF( rectItem->boundingRect().x(), 15 + lastItemPosY ));
    
    //textItem->setPos( QPointF( rectItem->rect().center().x() - (textItem->boundingRect().width() / 2), rectItem->rect().center().y() - (textItem->boundingRect().height() / 2)));
    
    //lastItemPosY += rectItem->boundingRect().bottom();
    scene->addItem( rectItem);
}

void MainWindow::newSimulation() {
    if( NULL == problemDialog) {
        ProblemDialog *pb = new ProblemDialog(fWrap, 0);
        problemDialog = pb;
        pb->show();
    }
    else {
        problemDialog->show();
    }
    /*int res = pb.exec();
    if (res) {
        printf("%d %d\n", pb.problemSelected(), pb.strategySelected());
        printf("%d\n", pb.generator());
        fWrap->simulate(pb.generator());
    }
    else {
    }*/
	//printf("toto!=%d, %d\n", res, pb.result());
}

SimulItem::SimulItem( QGraphicsView *newWin) : QGraphicsRectItem() {
    win = newWin;
}

void SimulItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    this->setBrush( QBrush( Qt::black));
}

void SimulItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    this->setBrush( QBrush( Qt::NoBrush));
    if( win->isHidden() )
        win->show();
    else
        win->raise();
}
