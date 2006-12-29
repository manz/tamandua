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
#include "wrap/Wrap.h"

using namespace std;

class ProblemDialog : public QDialog {
	Q_OBJECT
	private:
		Wrap *fWrap;
		vector<QRadioButton*> fRadio;

	public:
		ProblemDialog(Wrap *wrap);
		~ProblemDialog();

	public slots:
		void validate();

	signals:
		void problemSelected(int pb);

};

#endif /* PROBLEMDIALOG_H */

