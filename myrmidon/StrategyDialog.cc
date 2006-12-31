#include "StrategyDialog.h"

StrategyDialog::StrategyDialog(Problem *p) : fProblem(p), QDialog() {
	setWindowTitle(QString::fromUtf8("Stratégies"));

	QVBoxLayout *widgetlayout = new QVBoxLayout;
	QGroupBox *stratbox = new QGroupBox(QString::fromUtf8("Stratégies:"), this);

	QVBoxLayout *boxlayout = new QVBoxLayout;
	QLabel *lbl;
	for (int i=0;i<fProblem->strategiesCount();i++) {
		lbl= new QLabel(fProblem->strategy(i)->name());
		boxlayout->addWidget(lbl);
	}

	stratbox->setLayout(boxlayout);

	widgetlayout->addWidget(stratbox);

	this->setLayout(widgetlayout);
}

StrategyDialog::~StrategyDialog() {
}
