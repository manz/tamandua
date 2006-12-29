#ifndef PROBLEMDIALOG_H
#define PROBLEMDIALOG_H

#include <vector>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
#include "wrap/Wrap.h"

using namespace std;

class ProblemDialog : public QDialog {
	Q_OBJECT
	private:
		Wrap *fWrap;
		vector<QRadioButton*> fRadio;
		int fSProblem;

	public:
		ProblemDialog(Wrap *wrap);
		~ProblemDialog();

		int problem();
		void setProblem(int i);

	public slots:
		void validate();

	signals:
		void problemSelected();

};

#endif /* PROBLEMDIALOG_H */

