#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "ProblemDialog.h"
#include "wrap/Wrap.h"

class MainMenuBar : public QMenuBar {
	Q_OBJECT
	private:
		QMenu *fFileMenu;
		QMenu *fEditMenu;
		Wrap *fWrap;

	public:
		MainMenuBar(Wrap*);
		~MainMenuBar();

	public slots:
		void nouvelleSimulation();

};

#endif /* MAINMENUBAR_H */

