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
#include <QCleanlooksStyle>
// tamandua core
extern "C" {
#include <td_base.h>
#include <td_core.h>
};

// wrapper
#include "wrap/Context.h"
#include "ProblemDialog.h"
#include "MainWindow.h"

                 int main(int argc, char** argv) {
// initialisation de la base
             QApplication app(argc, argv);
#ifdef __APPLE__
             QString pluginpath = app.applicationDirPath().append("/../PlugIns");
#else
             QString pluginpath;
             if (argc > 1) {
                 pluginpath = QString(argv[1]);
             }
             else {
                 pluginpath = QString("/usr/local/share/tamandua/problems");
             }
             app.setStyle( new QCleanlooksStyle());
#endif
             Wrap *fWrap=new Wrap(pluginpath);
             MainWindow *win = new MainWindow(fWrap);
             win->setAttribute( Qt::WA_DeleteOnClose, true);
             app.connect( win, SIGNAL( destroyed()), &app, SLOT( closeAllWindows()));
             win->show();

             return app.exec();
                 }
