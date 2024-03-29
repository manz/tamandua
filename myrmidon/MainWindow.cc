#include <QGraphicsView>
#include <QDockWidget>
#include <QApplication>

#include "MainWindow.h"
#include "ShowWindow.h"

MainWindow::MainWindow(Wrap *wrap) : QMainWindow() {
    fWrap = wrap;
    problemDialog = NULL;
    lastItemPosY = 0;
    lastItem = NULL;

    resize( 600, 600);
	
    fFileMenu=this->menuBar()->addMenu("Simulations");
   // fEditMenu=this->menuBar()->addMenu("Edition");
	
    QAction *newSimul = new QAction("Nouvelle Simulation", this);
		newSimul->setShortcut(tr("Ctrl+N"));
		fFileMenu->addAction(newSimul);

    fQuit = new QAction("Quitter", this);
		fQuit->setShortcut(tr("Ctrl+Q"));

    fFileMenu->addAction(fQuit);
    
    scene = new QGraphicsScene();
    
    QGraphicsView *view = new QGraphicsView( scene, this);
    view->setAlignment( Qt::AlignCenter);
    view->show();
    
    
    setCentralWidget( view);
    
    connect(newSimul, SIGNAL(triggered()), this, SLOT(newSimulation()));
    connect(fQuit, SIGNAL(triggered()), this, SLOT( close()));
    connect(fWrap, SIGNAL(result(Job*)), this, SLOT(showResult(Job*)));
}

MainWindow::~MainWindow() {
	
}

void MainWindow::showResult(Job *j) {
    ShowWindow *showWindow = new ShowWindow( j);
	
    QGraphicsView *windowView= new QGraphicsView( showWindow);
    windowView->setAlignment( Qt::AlignLeft | Qt::AlignTop);
    windowView->setMinimumSize( 200, 200); //taille minimale de la fenetre !
    int sizeX = (int) windowView->sceneRect().width() + 15 , sizeY = (int) windowView->sceneRect().height() + 15;
    if( sizeX > 600 )
        sizeX = 600;
    if( sizeY > 600 )
        sizeY = 600;
    windowView->resize( sizeX, sizeY);
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
    
    QPainterPath roundRectPath;
    int x = (int) textItem->boundingRect().width();
    int y = (int) textItem->boundingRect().height();
    roundRectPath.moveTo( x + 20.0, 5.0);
    roundRectPath.arcTo( x, 0.0, 20.0, 10.0, 0.0, 90.0);
    roundRectPath.lineTo( 10.0, 0.0);
    roundRectPath.arcTo( 0.0, 0.0, 20.0, 10.0, 90.0, 90.0);
    roundRectPath.lineTo( 0.0, y + 5.0);
    roundRectPath.arcTo( 0.0, y, 20.0, 10.0, 180.0, 90.0);
    roundRectPath.lineTo( x + 10.0, y + 9.0);
    roundRectPath.arcTo( x, y, 20.0, 10.0, 270.0, 90.0);
    roundRectPath.closeSubpath();
     
    rectItem->setPath( roundRectPath);
    if( lastItem) {
        rectItem->setPos( QPointF( 0., 15. + lastItem->boundingRect().bottom()));
    }
    else {
        rectItem->setPos( QPointF( 0., 15.));
    }
    
    rectItem->setPen( QPen( Qt::NoPen));
    
    QColor color( Qt::green);
    color.setAlpha( 50);
    rectItem->setBrush( QBrush( color));
    
    textItem->setPos( QPointF( rectItem->boundingRect().center().x() - (textItem->boundingRect().width() / 2), rectItem->boundingRect().center().y() - (textItem->boundingRect().height() / 2)));
    
    textItem->setParentItem( rectItem);
    rectItem->setParentItem( lastItem);
    
    lastItem = rectItem;
    
    scene->addItem( rectItem);
}

void MainWindow::newSimulation() {
    if( NULL == problemDialog) {
        ProblemDialog *pb = new ProblemDialog(fWrap, this);
        problemDialog = pb;
        pb->show();
    }
    else {
        problemDialog->show();
    }
}

SimulItem::SimulItem( QGraphicsView *newWin) : QGraphicsPathItem() {
    win = newWin;
}

void SimulItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    QColor color( Qt::green);
    color.setAlpha( 150);
    this->setBrush( QBrush( color));
}

void SimulItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    QColor color( Qt::green);
    color.setAlpha( 50);
    this->setBrush( QBrush( color));
    if( win->isHidden() )
        win->show();
    else
        win->raise();
}
