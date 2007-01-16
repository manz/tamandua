#ifndef WRAP_H
#define WRAP_H

#include <QObject>
#include <QString>
#include <QTimer>

extern "C" {
	#include <td_base.h>
	#include <td_core.h>
};

#include "Context.h"
#include "Generator.h"
#include "Job.h"

class Wrap : public QObject {
	Q_OBJECT
  private:
		int fJobCount;
    Context *fContext;
		struct tdc_context *fRContext;
		struct tdc_job *fJob;
		QTimer *fTimer;
		
	private slots:
		void _CheckOut(void);
		
	public:
		Wrap(QString &path);
		~Wrap();
		Context* context();

		void simulate(Generator *g);
		
	signals:
		// pointeur à la charge du client
		void result(Job *j);
};

#endif
