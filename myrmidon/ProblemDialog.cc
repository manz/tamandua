#include "ProblemDialog.h"

ProblemDialog::ProblemDialog(Wrap *wrap) :
	fWrap(wrap),
	QDialog()
{
	Context *c = fWrap->context();
	int pbcount = c->problemCount();

	setWindowTitle(QString::fromUtf8("Séléction du probleme."));
	
	if (pbcount == 0) {
		qDebug("Error !!! pas de problemes chargés (intéret limité)");
	}
	else {
		// layout du widget
		QVBoxLayout *wlayout = new QVBoxLayout();
		// layout du groupBox
		QVBoxLayout *layout = new QVBoxLayout();
		
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(validate()));
		connect(this, SIGNAL(problemSelected()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		
		QLabel *desc;
		QRadioButton *name;
		Problem *p;
		for (int i=0;i<pbcount;i++) {
			p=c->problem(i);
			desc = new QLabel(p->description());
			name = new QRadioButton(p->name());
			layout->addWidget(name);
			layout->addWidget(desc);
			fRadio.push_back(name);
		}

		if (fRadio.size() > 0)	
			fRadio.at(0)->setChecked(true);

		QGroupBox *problem = new QGroupBox("Problemes", this);
		
		problem->setLayout(layout);
		wlayout->addWidget(problem);
		wlayout->addWidget(buttonBox);
		this->setLayout(wlayout);
	}
}

ProblemDialog::~ProblemDialog()
{
}

int ProblemDialog::problem() {
	return fSProblem;
}

void ProblemDialog::setProblem(int i) {
	fSProblem=i;
}

void ProblemDialog::validate() {
	for (int i=0;i<fRadio.size();i++) {
		if (fRadio.at(i)->isChecked())
			setProblem(i); //result=i;
	}
	emit problemSelected();
}

