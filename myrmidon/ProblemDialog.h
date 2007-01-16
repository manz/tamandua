#ifndef PROBLEMDIALOG_H
#define PROBLEMDIALOG_H

#include <vector>

#include <QDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QDial>
#include "wrap/Wrap.h"
#include "wrap/Generator.h"

using namespace std;

class ProblemDialog : public QWidget {
	Q_OBJECT
	private:
		Wrap *fWrap;

		QComboBox *fPbCombo;
		QComboBox *fStCombo;
		
		QGroupBox *fDescBox;
		QLabel *fPbDesc;
		
		QCheckBox *fChkCompare;

		QGroupBox *fLengthBox;
		QGroupBox *fMachineBox;
		QGroupBox *fWeightBox;
		QLabel *fLblWeightMin;
		QLabel *fLblWeightMax;
		QSpinBox *fSpinWeightMin;
		QSpinBox *fSpinWeightMax;
		
		QGridLayout *fDialogLayout;

		Generator *fGenerator;

		vector<QSpinBox*> fStepsLenMin;
		vector<QSpinBox*> fStepsLenMax;

		vector<QLabel*> fStepsLblMax;
		vector<QLabel*> fStepsLblMin;

		QSpinBox *fMachineSpin;
		QSpinBox *fTaskSpin;

		void _CreateDescription();
		void _CreateLength();
		void _CreateWeight();
		void _CreateMachine();

	public:
		ProblemDialog(Wrap *wrap, QWidget *parent);
		~ProblemDialog();
		
		Generator *generator();

		int problemSelected();
		int strategySelected();
	
	public slots:
		void updateSt(int s);
		void accepted();
		void rejected();
};

#endif /* PROBLEMDIALOG_H */

