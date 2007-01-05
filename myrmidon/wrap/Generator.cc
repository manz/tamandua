#include "Generator.h"

Generator::Generator() {
}

void Generator::setWeightMax(int value) {
	fWMax = value;
}

int Generator::weightMax() {
	return fWMax;
}

void Generator::setWeightMin(int value) {
	fWMin = value;
}

int Generator::weightMin() {
	return fWMin;
}

void Generator::addLengthMax(int value) {
	fLMax.push_back(value);
}

int Generator::lengthMax(int step) {
	return fLMax.at(step);
}

void Generator::addLengthMin(int value) {
	fLMin.push_back(value);
}

int Generator::lengthMin(int step) {
	return fLMin.at(step);
}

void Generator::setMachineCount(int value) {
	fMachineCount = value;
}

int Generator::machineCount() {
	return fMachineCount;
}

void Generator::setTaskCount(int value) {
	fTaskCount = value;
}

int Generator::taskCount() {
	return fTaskCount;
}

void Generator::setProblemNumber(int value) {
	fProblemNumber = value;
}

int Generator::problemNumber() {
	return fProblemNumber;
}

void Generator::setStrategyNumber(int value) {
	fStrategyNumber = value;
}

int Generator::strategyNumber() {
	return fStrategyNumber;
}

int Generator::populationCount() {
	return 1;
}
