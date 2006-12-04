#include "td_wrap.h"

td_wrap::td_wrap() :
	QObject()
{
	tdb_init(stdout);
	fcontext = tdc_init("/Users/manz/Developement/youou/trunk", 0, 1);
	tdc_setcontext(fcontext);
}

td_wrap::~td_wrap()
{
	if (fcontext) {
		tdc_exit();
		tdb_exit();
	}
}

void td_wrap::addJob(int tasks_c, int machine_c, int max, int strategy)
{
	struct tdc_conf* conf;
	struct tdc_job* job;
	printf("New Job: tasks count %d, machines %d, max %d, strat %d\n", tasks_c, machine_c, max, strategy);

	conf = tdc_buildconf(tdc->problems[0]);

	conf->tasks_count = tasks_c;
	conf->max[0] = max;
	conf->min[0] = 2;
	conf->globals[0] = machine_c;
	
	job = tdc_buildjob(conf);
	job->strategy = strategy;
	pair <tdc_job*, tdc_conf* > p(job, conf);
	fjobs.push_back(p);
}

void td_wrap::commit() {
	if (fjobs.size()>=1) {
		std::vector<pair<tdc_job*, tdc_conf*> >::iterator it;
		tdc_job *job;
		tdc_conf *conf;
		
		for (it = fjobs.begin(); it!=fjobs.end(); it++) {
			job=it->first;
			conf=it->second;
			tdc_commit(job), job=NULL;
			job=tdc_force_checkout();

			// compute the time used by each machine
			int *taskz=(int*)tdb_calloc(sizeof(int), job->tasks_count);
			for (int i=0;i<job->tasks_count; i++) {
				taskz[job->tasks[i][1]]+=job->tasks[i][0];
			}
			
			// get the max load (ie total time)
			long int maxload=taskz[0];
			for (int i=1;i<job->tasks_count;i++) {
				if (taskz[i]>maxload)
					maxload=taskz[i];
			}
      
			free(taskz);
      
			printf("Maxload : %ld\n", maxload);
			tdc_deljob(job);
			tdc_delconf(conf);
    
		}
		fjobs.erase(fjobs.begin(), fjobs.end());
	}
}
