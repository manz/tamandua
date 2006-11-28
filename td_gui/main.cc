#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include "td_wrap.h"

int main
(int argc, char** argv)
{
  QApplication app(argc, argv);
  QWidget mainw;

  QVBoxLayout *lay=new QVBoxLayout();
  
  QLineEdit machinec("", &mainw);
  QLineEdit tasksc("", &mainw);
  QLineEdit tmin("", &mainw);
  QLineEdit tmax("", &mainw);
  QTextEdit result(&mainw);

  QPushButton push("run", &mainw);

  lay->addWidget(&machinec);
  lay->addWidget(&tasksc);
  lay->addWidget(&tmin);
  lay->addWidget(&tmax);
  lay->addWidget(&push);
  lay->addWidget(&result);
  
  mainw.setLayout(lay);
  mainw.show();
  td_wrap wrap;
  wrap.addJob(17, 5, 200, 1);
  wrap.addJob(10, 5, 10, 1);
  QObject::connect(&push, SIGNAL(clicked()), &wrap, SLOT(commit()));
  return app.exec();
}
