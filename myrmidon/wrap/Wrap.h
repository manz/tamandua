#ifndef WRAP_H
#define WRAP_H

#include <QObject>
#include <QString>

extern "C" {
#include <td_base.h>
#include <td_core.h>
};

#include "Context.h"
#include "Generator.h"

class Wrap : public QObject {
	Q_OBJECT
  private:
    Context *fContext;
		struct tdc_context *fRContext;
	public:
//		Wrap();
		Wrap(QString &path);
		~Wrap();
		Context* context();

		void simulate(Generator *g);
};

#endif
