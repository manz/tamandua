#ifndef STRATEGYDIALOG_H
#define STRATEGYDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include "wrap/Problem.h"

class StrategyDialog : public QDialog {
	Q_OBJECT
	private:
		//Strategy fStrategy;
		Problem *fProblem;
	public:
		StrategyDialog(Problem *p);
		~StrategyDialog();
};

#endif /* STRATEGYDIALOG_H */

