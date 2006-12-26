#include "Wrap.h"

Wrap::Wrap() {
	tdb_init(stdout);	
	fRContext = tdc_init("problems/", 0, 1);
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
