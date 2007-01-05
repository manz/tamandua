#ifndef GENERATOR_H
#define GENERATOR_H

#include "Strategy.h"

class Generator {
	private:
		Strategy *fStrategy;
		int fWMax;
		int fWMin;
		int fLMax;
		int fLMin;
		int fMachines;

	public:
		Generator(Strategy *s, int wmin, int wmax, int lmin, int lmax, int machines);

		int weightMax();
		int weightMin();
		int lengthMax();
		int lengthMin();
		int machines();
		Strategy *strategy();
};

#endif /* GENERATOR_H */

