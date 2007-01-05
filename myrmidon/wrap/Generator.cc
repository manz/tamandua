#include "Generator.h"

Generator::Generator(Strategy *s, int wmin, int wmax, int lmin, int lmax, int machines) {
	fStrategy = s;
	fWMin = wmin;
	fWMax = wmax;
	fLMin = lmin;
	fLMax = lmax;
	fMachines = machines;
};

int Generator::weightMax() {
	return fWMax;
}

int Generator::weightMin() {
	return fWMin;
}

int Generator::lengthMax() {
	return fLMax;
}

int Generator::lengthMin() {
	return fLMin;
}

int Generator::machines() {
	return fMachines;
}

Strategy *Generator::strategy() {
	return fStrategy;
}

