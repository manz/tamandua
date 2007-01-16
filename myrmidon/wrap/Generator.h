#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include "Problem.h"

using namespace std;

class Generator {
	private:
		int fProblemNumber;
		int fStrategyNumber;
		int fWMax;
		int fWMin;
		vector<int> fLMax;
		vector<int> fLMin;
		int fMachineCount;
		int fTaskCount;
		bool fCompareStrategies;

	public:
		Generator();
		void setStrategyNumber(int value);
		int strategyNumber();

		void setProblemNumber(int value);

		void setWeightMax(int value);
		void setWeightMin(int value);

		void addLengthMax(int value);
		void addLengthMin(int value);

		int weightMax();
		int weightMin();
		
		int lengthMax(int step);
		int lengthMin(int step);

		void setMachineCount(int value);
		int machineCount();

		void setTaskCount(int value);
		int taskCount();

		bool compareStrategies();
		void setCompareStrategies(bool val);

		int problemNumber();
		int populationCount();
};

#endif /* GENERATOR_H */

