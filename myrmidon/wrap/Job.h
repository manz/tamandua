#ifndef JOB_H
#define JOB_H

#include <vector>

extern "C" {
#include <td_base.h>
#include <td_core.h>
}

#include "Problem.h"
#include "Task.h"

using namespace std;

/** Classe Job.
 * Abstraction de struct tdc_job, pour séparer le core de la Gui.
 * Cette classe ne doit pas être instanciée par l'utilisateur.
 */
class Job {
	private:
		struct tdc_job *fJob;
		Problem *fProblem;

		vector<Task*> fTasks;
	
	public:
		Job(struct tdc_job *j);
		~Job();

		Problem *problem();
		int machineCount();
		int strategyNumber();
		int tasksCount();
		Task *task(int i);
};

#endif /* JOB_H */
