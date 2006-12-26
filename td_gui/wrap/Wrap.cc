#include "Wrap.h"

Wrap::Wrap(QString &path) {
	tdb_init(stdout);	

	QByteArray ba( path.toLatin1() );
	const char *cpath = ba.data();

	fRContext = tdc_init(cpath, 0, 1);
	fContext = new Context(fRContext);
}

Wrap::~Wrap() {
	delete fContext;
	tdc_exit();
	tdb_exit();
}

Context* Wrap::context() {
	return fContext;
}
