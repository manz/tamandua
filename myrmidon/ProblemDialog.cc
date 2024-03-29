#include "ProblemDialog.h"

ProblemDialog::ProblemDialog(Wrap *wrap, QWidget *parent) : QDialog(parent)
{
        fWrap = wrap;
	Context *c = fWrap->context();
	int pbcount = c->problemCount();

	fLengthBox=NULL;

	fGenerator=NULL;
	setWindowTitle(QString::fromUtf8("Séléction du probleme."));
	
	if (pbcount == 0) {
		// n'arrive jamais la base ne permet pas de demarrer si ya pas de problemes
		qDebug("Error !!! pas de problemes chargés (intéret limité)");
	}
	else {
		fDialogLayout = new QGridLayout();
		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);

		buttonBox->addButton(QString::fromUtf8("Ok"), QDialogButtonBox::AcceptRole);
		buttonBox->addButton(QString::fromUtf8("Annuler"), QDialogButtonBox::RejectRole);
				
		QPushButton *buttonOk = new QPushButton( QString::fromUtf8( "Ok"));
		QPushButton *buttonCancel = new QPushButton( QString::fromUtf8( "Annuler"));

		QLabel *lbpb = new QLabel("Probleme :", this);
		QLabel *lbst = new QLabel("Strategie :", this);

		fChkCompare = new QCheckBox("Comparaison des strategies", this);

		fPbDesc = new QLabel(this);
		fPbDesc->setWordWrap(true);

		fPbCombo = new QComboBox(this);
		fStCombo = new QComboBox(this);
	
		Problem *pb;
		Strategy *strat;

		// remplissage du combo des problemes.
		for (int i=0; i<pbcount; i++) {
			pb=c->problem(i);
			fPbCombo->insertItem(i, pb->name());
		}
		//création des groupbox
		_CreateDescription();
		_CreateWeight();
		_CreateMachine();

		updateSt(0);

		fDialogLayout->addWidget(lbpb, 0, 0);
		fDialogLayout->addWidget(fPbCombo, 0, 1);
		fDialogLayout->addWidget(lbst, 1, 0);
		fDialogLayout->addWidget(fStCombo, 1, 1);
		
		fDialogLayout->addWidget(fChkCompare, 2, 0, 1, 2);
	
		fDialogLayout->addWidget(fDescBox, 3, 0, 1, 2);

		fDialogLayout->addWidget(fLengthBox, 4, 0, 1, 2);
		fDialogLayout->addWidget(fWeightBox, 5, 0, 1, 2);

		fDialogLayout->addWidget(fMachineBox, 6, 0, 1, 2);

		fDialogLayout->addWidget(buttonBox, 7, 0, 1, 2);

		connect(fPbCombo, SIGNAL(activated(int)), this, SLOT(updateSt(int)));
		connect(buttonBox, SIGNAL( accepted()), this, SLOT(accepted()));
		connect(buttonBox, SIGNAL( rejected()), this, SLOT(rejected()));
		connect(fChkCompare, SIGNAL(stateChanged(int)), this, SLOT(setCheckBoxVisible(int)));
		this->setLayout(fDialogLayout);
		fDialogLayout->setSizeConstraint(QLayout::SetFixedSize);
	}
}

void ProblemDialog::_CreateMachine() {
	fMachineBox = new QGroupBox("Taches et Machines", this);
	QGridLayout *layout = new QGridLayout();

	fMachineSpin = new QSpinBox(this);
	fMachineSpin->setMinimum(1);

	fTaskSpin = new QSpinBox(this);
	fTaskSpin->setMinimum(1);
	fTaskSpin->setValue(7);

	QLabel *label = new QLabel("Nombre de Machines", this);
	QLabel *label2 = new QLabel("Nombre de taches", this);
	layout->addWidget(label, 0, 0);
	layout->addWidget(fMachineSpin, 0, 1);
	layout->addWidget(label2, 1, 0);
	layout->addWidget(fTaskSpin, 1, 1);

	fMachineBox->setLayout(layout);
	layout->setSizeConstraint(QLayout::SetMinimumSize);

}

void ProblemDialog::_CreateDescription() {
	fDescBox = new QGroupBox("Description", this);
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(fPbDesc);
	fDescBox->setLayout(layout);
	layout->setSizeConstraint(QLayout::SetMinimumSize);
}

void ProblemDialog::_CreateLength() {
	if (!fLengthBox)
		fLengthBox = new QGroupBox("Longueur", this);

	QGridLayout *layout;

	if (!fLengthBox->layout()) {
		layout=new QGridLayout();
	}
	else {
		layout=(QGridLayout*)fLengthBox->layout();
	}

	if (fStepsLenMin.size()>0) {
		for (int i=0;i<fStepsLenMin.size(); i++) {
			layout->removeWidget(fStepsLenMin.at(i));
			layout->removeWidget(fStepsLenMax.at(i));
			layout->removeWidget(fStepsLblMax.at(i));
			layout->removeWidget(fStepsLblMin.at(i));
			delete fStepsLenMin.at(i);
			delete fStepsLenMax.at(i);
			delete fStepsLblMin.at(i);
			delete fStepsLblMax.at(i);
		}
		fStepsLenMin.clear();
		fStepsLenMax.clear();
		fStepsLblMin.clear();
		fStepsLblMax.clear();
	}
	
	Problem *problem=fWrap->context()->problem(this->problemSelected());

	printf("steps %d\n", problem->stepsCount());

	for (int i=0;i<problem->stepsCount(); i++) {
		fStepsLblMax.push_back(new QLabel("Maximum", this));
		fStepsLblMin.push_back(new QLabel("Minimum", this));
		
		fStepsLenMin.push_back(new QSpinBox(this));
		fStepsLenMax.push_back(new QSpinBox(this));
		fStepsLenMin.at(i)->setMinimum(1);
		fStepsLenMax.at(i)->setMinimum(1);
		fStepsLenMax.at(i)->setValue(42);

		layout->addWidget(fStepsLblMin.at(i), i, 0);
		layout->addWidget(fStepsLenMin.at(i), i, 1);
		layout->setColumnMinimumWidth(2, 50);
		layout->addWidget(fStepsLblMax.at(i), i, 3);
		layout->addWidget(fStepsLenMax.at(i), i ,4);
	}


	fLengthBox->setLayout(layout);
	layout->setSizeConstraint(QLayout::SetFixedSize);
}

void ProblemDialog::_CreateWeight() {
	fWeightBox = new QGroupBox("Poids", this);
	QGridLayout *layout=new QGridLayout();

	fSpinWeightMax = new QSpinBox(this);
	fSpinWeightMax->setMinimum(1);
	fSpinWeightMax->setValue(42);
	
	fSpinWeightMin = new QSpinBox(this);
	fSpinWeightMin->setMinimum(1);

	fLblWeightMin = new QLabel("Minimum", this);
	fLblWeightMax = new QLabel("Maximum", this);
		
	layout->addWidget(fLblWeightMin, 0, 0);
	layout->addWidget(fSpinWeightMin, 0, 1);
	layout->setColumnMinimumWidth(2, 50);
	layout->addWidget(fLblWeightMax, 0, 3);
	layout->addWidget(fSpinWeightMax, 0, 4);
	fWeightBox->setLayout(layout);
	layout->setSizeConstraint(QLayout::SetFixedSize);
}


ProblemDialog::~ProblemDialog()
{
    delete fGenerator;
}

Generator* ProblemDialog::generator() {
	return fGenerator;
}

int ProblemDialog::problemSelected() {
	return fPbCombo->currentIndex();
}
 
int ProblemDialog::strategySelected() {
	return fStCombo->currentIndex();
}

void ProblemDialog::updateSt(int p) {
	Problem *pb=fWrap->context()->problem(p);
	Strategy *st;
	
	fPbDesc->setText(pb->description());

	fStCombo->clear();
	for (int i=0;i<pb->strategiesCount(); i++) {
		st=pb->strategy(i);
		fStCombo->insertItem(i, st->name());
	}

	if (pb->stepsCount()>1) {
		fMachineSpin->setValue(pb->stepsCount());
		fMachineSpin->setEnabled(false);
	}
	else fMachineSpin->setEnabled(true);

	if (pb->strategiesCount()==1) {
		fStCombo->setEnabled(false);
		fChkCompare->setEnabled(false);
	}
	else {
		fStCombo->setEnabled(true);
		fChkCompare->setEnabled(true);
	}	

	if (pb->isWeighted()) {
		fMachineSpin->setEnabled(false);
		fWeightBox->setVisible(true);
	}
	else {
		fWeightBox->setVisible(false);
	}
	
	_CreateLength();
}

void ProblemDialog::setCheckBoxVisible(int state) {
	switch (state) {
		case Qt::Checked:
			fStCombo->setEnabled(false);
			break;
		case Qt::Unchecked:
			fStCombo->setEnabled(true);
			break;
		default:
			break;
	}
}

void ProblemDialog::accepted() {
	fGenerator = new Generator();

	fGenerator->setProblemNumber(fPbCombo->currentIndex());
	fGenerator->setStrategyNumber(fStCombo->currentIndex());

	fGenerator->setMachineCount(fMachineSpin->value());
	fGenerator->setTaskCount(fTaskSpin->value());

	for (int i=0;i<fStepsLenMin.size();i++) {
	   int min = fStepsLenMin.at(i)->value();
	   int max = fStepsLenMax.at(i)->value();
	   if( min > max) {
	       int tmp = min;
	       min = max;
	       max = tmp;
	       }
		fGenerator->addLengthMax( max);
		fGenerator->addLengthMin( min);
	}

	fGenerator->setWeightMax(fSpinWeightMax->value());
	fGenerator->setWeightMin(fSpinWeightMin->value());

  fGenerator->setCompareStrategies(fChkCompare->isChecked());
	fWrap->simulate(fGenerator);
	accept();
}

void ProblemDialog::rejected() {
	reject();
}

