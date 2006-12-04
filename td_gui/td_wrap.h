#ifndef TD_WRAP_H
#define TD_WRAP_H

#include <QObject>
#include <vector>
#include <algorithm>

extern "C" {
  #include <td_base.h>
  #include <td_core.h>
}

#include "Job.h"
#include "TaskList.h"

using namespace std;

/** Classe td_wrap.
 * Abstraction des operations sur le core.
 */

class td_wrap : public QObject
{
  Q_OBJECT

	private:
		tdc_context *fcontext;
		std::vector<pair<tdc_job*, tdc_conf*> > fjobs;
	
	public:
		td_wrap();
		~td_wrap();
		
		void addJob(int tasks_c, int machine_c, int max, int strategy);
	
	public slots:
		void commit();

	signals:
		void jobResult(Job *job);

};

#endif /* TD_WRAP_H */
