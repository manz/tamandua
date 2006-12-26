#include "ProblemDialog.h"

ProblemDialog::ProblemDialog(Wrap *wrap) :
	fWrap(wrap),
	QDialog()
{
	Context *c = fWrap->context();
	int pbcount = c->problemCount();

	if (pbcount == 0) {
		qDebug("Error !!! pas de problemes chargés (intéret limité)");
	}
	else {
		// layout du widget
		QVBoxLayout *wlayout = new QVBoxLayout();
		// layout du groupBox
		QVBoxLayout *layout = new QVBoxLayout();
	
		QLabel *desc;
		QRadioButton *name;

		for (int i=0;i<pbcount;i++) {
			Problem p=c->problem(i);
			desc = new QLabel(QString::fromUtf8(p.description()));
			name = new QRadioButton(QString::fromUtf8(p.name()));
			layout->addWidget(name);
			layout->addWidget(desc);
			fRadio.push_back(name);
		}
		if (fRadio.size() > 0)	
			fRadio.at(0)->setChecked(true);

		QGroupBox *problem = new QGroupBox("Problemes", this);
		QPushButton *butt = new QPushButton("Bouton");

		problem->setLayout(layout);
		wlayout->addWidget(problem);
		wlayout->addWidget(butt);
		this->setLayout(wlayout);
	}
}

ProblemDialog::~ProblemDialog()
{
}

