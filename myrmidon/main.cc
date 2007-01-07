// includes Qt
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QGraphicsScene>
#include <QGraphicsView>
// tamandua core
extern "C" {
#include <td_base.h>
#include <td_core.h>
};

// wrapper
#include "wrap/Context.h"
#include "ProblemDialog.h"
#include "MainMenuBar.h"

int main(int argc, char** argv) {
	// initialisation de la base
	QApplication app(argc, argv);
#ifdef __APPLE__
	QString pluginpath = app.applicationDirPath().append("/../PlugIns");
#else
	QString pluginpath = ("/usr/local/share/tamandua/problems");
#endif
	QGraphicsScene scene;
	scene.addText("Hello, world!");

	QGraphicsView view(&scene);
	view.show();


	 scene.setBackgroundBrush(Qt::blue);

	  // a gradient background
	  QRadialGradient gradient(0, 0, 50);
		 gradient.setSpread(QGradient::RepeatSpread);
		  scene.setBackgroundBrush(gradient);

	Wrap *fWrap=new Wrap(pluginpath);
	MainMenuBar menu(fWrap);
	menu.show();
/*
	QMenuBar menu;
	menu.addMenu("Fichier");
	menu.addMenu("Edition");
	menu.addMenu("Aide");
	menu.show();
	*/
/*
	ProblemDialog pb(fWrap);
	int res = pb.exec();
	*/
/*
	printf("problemSelected %d\n", pb.problemSelected());
	printf("strategySelected %d\n", pb.strategySelected());
	printf("result %d\n", res);
*/
	return app.exec();

}
