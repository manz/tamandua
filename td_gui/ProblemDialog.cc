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
			desc = new QLabel(p.description());
			name = new QRadioButton(p.name());
			layout->addWidget(name);
			layout->addWidget(desc);
			fRadio.push_back(name);
		}
		if (fRadio.size() > 0)	
			fRadio.at(0)->setChecked(true);

		QGroupBox *problem = new QGroupBox("Problemes", this);
		QPushButton *butt = new QPushButton("Bouton");
		connect(butt, SIGNAL(clicked()), this, SLOT(validate()));
		problem->setLayout(layout);
		wlayout->addWidget(problem);
		wlayout->addWidget(butt);
		this->setLayout(wlayout);
	}
}

ProblemDialog::~ProblemDialog()
{
}

void ProblemDialog::validate() {
	int result=-1;
	for (int i=0;i<fRadio.size();i++) {
		if (fRadio.at(i)->isChecked())
			result=i;
	}

	if (result==-1)
		result=0;
	printf("ProbleSelected = %d\n", result);
	emit problemSelected(result);
}

