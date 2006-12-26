#ifndef WRAP_H
#define WRAP_H

extern "C" {
#include <td_base.h>
#include <td_core.h>
};

#include "Context.h"

class Wrap {
  private:
    Context *fContext;
		struct tdc_context *fRContext;
	public:
		Wrap();
		~Wrap();
		Context* context();
};

#endif
