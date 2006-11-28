#ifndef TD_WRAP_H
#define TD_WRAP_H

#include <QObject>
#include <vector>
#include <algorithm>

extern "C" {
  #include <td_base.h>
  #include <td_core.h>
}

using namespace std;

class td_wrap : public QObject
{
  Q_OBJECT

	private:
		tdc_context *context;
		std::vector<pair<tdc_job*, tdc_conf*> > jobs;
	
	public:
		td_wrap();
		~td_wrap();
		
		void addJob(int tasks_c, int machine_c, int max, int strategy);
	
	public slots:
		void commit();

};

#endif /* TD_WRAP_H */
