#include "td_base.h"

#define INC_PRIVATE_TD_CORE_H
#include "td_core.h"

struct tdc_context* tdc_init
(const char* problems_dir, size_t workers, size_t init_rand)
{
	struct tdc_context* ret;
	DIR* probdir;
	struct dirent* file;
	void* problib;
	char* filename;
	size_t i,probdirlen,probslots=4;
	struct tdc_problem* problem;
	int closeret;
	struct timeval timeval;
	if (init_rand) {
		gettimeofday(&timeval, NULL);
		srand((unsigned int)timeval.tv_usec);
	}
	ret = tdb_malloc(sizeof(*ret));
	ret->problems_count = 0;
	ret->problems = tdb_calloc(probslots, sizeof(*(ret->problems)));

	/* Scan directory for problem shared object */
	probdir = opendir(problems_dir);
	tdb_die_if(probdir == NULL, strerror(errno));
	probdirlen = strlen(problems_dir);
	/* avoid many copy of foldername and cope with slashless foldernames */
	filename = tdb_malloc(probdirlen+2);
	strcpy(filename, problems_dir);
	if (problems_dir[probdirlen-1] != '/') {
		probdirlen++;
		filename[probdirlen-1] = '/';
		filename[probdirlen] = '\0';
	}

	while (errno=0, (file=readdir(probdir)) != NULL) {
		if (strcmp(".so", file->d_name+strlen(file->d_name)-3) != 0) {
			/* skip files not ending in .so */
			continue;
		}
		filename = tdb_realloc(filename, strlen(file->d_name)+probdirlen+1);
		strcpy(filename+probdirlen, file->d_name);
		problib = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
		if (problib == NULL) {
			tdb_warn(dlerror());
			continue;
		}
		problem = dlsym(problib, "tdc_desc");
		if (problem == NULL) {
			tdb_warn(dlerror());
			continue;
		}
		/* the tdc_desc record is ok, let's add it */
		/* first check if tdc.problems needs to be enlarged */
		if (ret->problems_count == probslots) {
			probslots += probslots/2; /* 1.5 reallocation ratio */
			ret->problems = tdb_realloc(ret->problems, sizeof(*ret->problems)*probslots);
		}
		ret->problems[ret->problems_count] = tdb_malloc(sizeof(*problem));
		memcpy(ret->problems[ret->problems_count], problem, sizeof(*problem));
		ret->problems[ret->problems_count]->library = problib;
		ret->problems_count += 1;
	}
	tdb_die_if(ret->problems_count == 0,"No valid problems found");
	free(filename);
	tdb_die_if(errno!=0, strerror(errno));
	closeret = closedir(probdir);
	tdb_die_if(closeret!=0, strerror(errno));

	/* finished with problems, now the queues */
	pthread_mutex_init(&ret->todo_queue_lock, NULL);
	pthread_mutex_init(&ret->done_queue_lock, NULL);
	pthread_cond_init(&ret->todo_queue_sig, NULL);
	pthread_cond_init(&ret->done_queue_sig, NULL);
	ret->todo_queue_head = ret->todo_queue_tail = NULL;
	ret->done_queue_head = ret->done_queue_tail = NULL;

	ret->exit = 0;

	if (workers<1) {
		workers = tdb_getcpucount();
	}
	ret->threads_count = workers;
	ret->threads = tdb_calloc(ret->threads_count, sizeof(*ret->threads));
	for (i=0; i<ret->threads_count; i++) {
		pthread_create(&ret->threads[i], NULL, tdc_worker_thread, ret);

	}

	return ret;
}

void tdc_setcontext
(struct tdc_context* context)
{
	tdc = context;
}

struct tdc_context* tdc_getcontext
(void)
{
	return tdc;
}

struct tdc_conf* tdc_buildconf
(struct tdc_problem* problem)
{
	struct tdc_conf* ret;
	size_t i;
	ret = tdb_malloc(sizeof(*ret));
	ret->problem = problem;
	ret->tasks_count = 1;

	ret->globals = tdb_calloc(problem->globals_count, sizeof(*ret->globals));
	for (i=0; i<problem->globals_count; i++) {
		ret->globals[i] = problem->globals[i].min;
	}

	ret->min = tdb_calloc(problem->properties_count, sizeof(*ret->min));
	ret->max = tdb_calloc(problem->properties_count, sizeof(*ret->max));
	for (i=0; i<problem->properties_count; i++) {
		ret->min[i] = problem->properties[i].min;
		ret->max[i] = problem->properties[i].max;
	}

	return ret;
}

void tdc_delconf
(struct tdc_conf* conf)
{
	free(conf->globals);
	free(conf->min);
	free(conf->max);
	free(conf);
}

struct tdc_job* tdc_buildjob
(struct tdc_conf* conf)
{
	struct tdc_job* ret;
	size_t i, j;
	
	ret = tdb_malloc(sizeof(*ret));
	ret->problem = conf->problem;
	ret->tasks_count = conf->tasks_count;

	ret->globals = tdb_calloc(conf->problem->globals_count, sizeof(*ret->globals));
	memcpy(ret->globals, conf->globals,
	       sizeof(*ret->globals)*conf->problem->globals_count);

	ret->tasks = tdb_calloc(conf->tasks_count+1, sizeof(*ret->tasks));
	for (i=0; i<conf->tasks_count; i++) {
		ret->tasks[i] = tdb_calloc(conf->problem->properties_count,
		                        sizeof(*ret->tasks[i]));
		for (j=0; j<conf->problem->properties_count; j++) {
			/* randomly generate data only if asked */
			if (conf->problem->properties[j].type & TDC_INPUT) {
				ret->tasks[i][j] = (int)(rand()/(double)RAND_MAX
				                   *(conf->max[j] - conf->min[j] +1))
				                   +conf->min[j];
			} else {
				ret->tasks[i][j] = conf->min[j];
			}
		}
	}
	return ret;
}

struct tdc_job* tdc_copyjob
(struct tdc_job* job)
{
	struct tdc_job* ret;
	size_t i, j;
	ret = tdb_malloc(sizeof(*ret));
	ret->problem = job->problem;
	ret->tasks_count = job->tasks_count;
	ret->globals = tdb_calloc(job->problem->globals_count, sizeof(*ret->globals));
	memcpy(ret->globals, job->globals,
	       sizeof(*ret->globals)*job->problem->globals_count);

	ret->tasks = tdb_calloc(job->tasks_count+1, sizeof(*ret->tasks));
	for (i=0; i<job->tasks_count; i++) {
		ret->tasks[i] = tdb_calloc(job->problem->properties_count,
		                        sizeof(*ret->tasks[i]));
		memcpy(ret->tasks[i], job->tasks[i],
		       sizeof(*ret->tasks[i])*job->problem->properties_count);
	}
	return ret;
}

void tdc_pushqueue
(struct tdc_job_queue* item, struct tdc_job_queue** queue_head,
 struct tdc_job_queue** queue_tail, pthread_mutex_t* lock, pthread_cond_t* signal)
{
	item->next = NULL;
	pthread_mutex_lock(lock);
		item->prev = *queue_tail;
		if (*queue_tail != NULL) {
			(*queue_tail)->next = item;
		} else {
			*queue_head = item;
		}
		*queue_tail = item;
		pthread_cond_signal(signal);
	pthread_mutex_unlock(lock);
}

/* because of mutex variations and others, popqueue is not really
 * factorisable, sorry... (in fact I must check this assertion :p)
 */

void tdc_commit
(struct tdc_job* job)
{
	struct tdc_job_queue* item;
	item = tdb_malloc(sizeof(*item));
	item->job = job;
	tdc_pushqueue(item, &tdc->todo_queue_head, &tdc->todo_queue_tail,
	              &tdc->todo_queue_lock, &tdc->todo_queue_sig);
}

void* tdc_worker_thread
(void* data)
{
	struct tdc_context* context = data;
	struct tdc_job_queue *current, *next;
	int ret;

	tdb_debug("Started worker thread");
	/* get a job from todo queue */
	while (1) {
		/* here is a popqueue on todo queue */
		/* this one is locking */
		pthread_mutex_lock(&context->todo_queue_lock);
			while (context->todo_queue_head == NULL && context->exit == 0) {
				pthread_cond_wait(&context->todo_queue_sig, &context->todo_queue_lock);
			}
			if (context->exit == 1) {
				pthread_mutex_unlock(&context->todo_queue_lock);
				tdb_debug("Killed worker thread");
				pthread_exit(NULL);
			}
			current = context->todo_queue_head;
			next = context->todo_queue_head->next;
			context->todo_queue_head = next;
			if (context->todo_queue_head != NULL) {
				context->todo_queue_head->prev = NULL;
			} else {
				context->todo_queue_tail = NULL;
			}
		pthread_mutex_unlock(&context->todo_queue_lock);

		/* process the job */
		ret = current->job->problem->process(current->job->strategy, current->job);
		if (ret == EXIT_SUCCESS) {
			tdb_debug("processing complete :)");
		} else {
			tdb_debug("processing failed :(");
		}

		/* add job to the done queue */
		tdc_pushqueue(current, &context->done_queue_head, &context->done_queue_tail,
		              &context->done_queue_lock, &context->done_queue_sig);
	}
}


struct tdc_job* tdc_checkout
(void)
{
	struct tdc_job* ret=NULL;
	struct tdc_job_queue* next;
	/* here is a popqueue on done queue */
	/* this one is not locking */
	pthread_mutex_lock(&tdc->done_queue_lock);
		if (tdc->done_queue_head != NULL) {
			ret = tdc->done_queue_head->job;
			next = tdc->done_queue_head->next;
			free(tdc->done_queue_head);
			tdc->done_queue_head = next;
			if (tdc->done_queue_head != NULL) {
				tdc->done_queue_head->prev = NULL;
			} else {
				tdc->done_queue_tail = NULL;
			}
		}
	pthread_mutex_unlock(&tdc->done_queue_lock);
	return ret;
}

struct tdc_job* tdc_force_checkout
(void)
{
	struct tdc_job* ret;
	struct tdc_job_queue* next;
	/* here is a popqueue on done queue */
	/* this one is locking */
	pthread_mutex_lock(&tdc->done_queue_lock);
		while (tdc->done_queue_head == NULL) {
			pthread_cond_wait(&tdc->done_queue_sig, &tdc->done_queue_lock);
		}
		ret = tdc->done_queue_head->job;
		next = tdc->done_queue_head->next;
		free(tdc->done_queue_head);
		tdc->done_queue_head = next;
		if (tdc->done_queue_head != NULL) {
			tdc->done_queue_head->prev = NULL;
		} else {
			tdc->done_queue_tail = NULL;
		}
	pthread_mutex_unlock(&tdc->done_queue_lock);
	return ret;
}

void tdc_deljob
(struct tdc_job* job)
{
	size_t i;
	for (i=0; i<job->tasks_count; i++) {
		free(job->tasks[i]);
	}
	free(job->tasks);
	free(job->globals);
	free(job);
}

void tdc_exit
(void)
{
	size_t i;
	int pthreadret;
	struct tdc_job_queue* next;

	/* free threads */
	pthread_mutex_lock(&tdc->todo_queue_lock);
		tdc->exit=1;
		pthread_cond_broadcast(&tdc->todo_queue_sig);
	pthread_mutex_unlock(&tdc->todo_queue_lock);
	
	for (i=0; i<tdc->threads_count; i++) {
		pthreadret = pthread_join(tdc->threads[i], NULL);
		tdb_die_if(pthreadret != 0, strerror(pthreadret));
	}
	free(tdc->threads);

	/* free problems */
	for (i=0; i<tdc->problems_count; i++) {
		dlclose(tdc->problems[i]->library);
		free(tdc->problems[i]);
	}
	free(tdc->problems);

	/* free queues */
	for(; tdc->todo_queue_head!=NULL; tdc->todo_queue_head=next) {
		next=tdc->todo_queue_head->next;
		tdc_deljob(tdc->todo_queue_head->job);
		free(tdc->todo_queue_head);
	}
	for(; tdc->done_queue_head!=NULL; tdc->done_queue_head=next) {
		next=tdc->done_queue_head->next;
		tdc_deljob(tdc->done_queue_head->job);
		free(tdc->done_queue_head);
	}

	/* free context and remove it */
	free(tdc), tdc=NULL;
	tdb_debug("Tamandua core context exited");
}

void tdc_test
(void)
{
	struct tdc_conf* conf;
	struct tdc_job* job;
	conf = tdc_buildconf(tdc->problems[0]);
	conf->tasks_count = 4;
	conf->max[0] = 3;
	conf->globals[0] = 4;
	job = tdc_buildjob(conf);
	job->strategy = 1;
	tdc_commit(job), job=NULL;
	job = tdc_force_checkout();
	/* use results */
	tdc_deljob(job);
	tdc_delconf(conf);
}
