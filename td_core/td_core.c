#include "td_base.h"

#define INC_PRIVATE_TD_CORE_H
#include "td_core.h"

static void tdc_init_problems
(struct tdc_context* const self, const char* problems_dir)
{
	DIR* probdir;
	struct dirent* file;
	void* problib;
	char* filename;
	size_t probdirlen;
	size_t probslots=4;
	struct tdc_problem* problem;
	int closeret;
	self->n_problems = 0;
	self->problems = tdb_calloc(probslots, sizeof(*(self->problems)));
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
		if (self->n_problems == probslots) {
			probslots += probslots/2; /* 1.5 reallocation ratio */
			self->problems = tdb_realloc(self->problems, sizeof(*self->problems)*probslots);
		}
		self->problems[self->n_problems] = tdb_malloc(sizeof(*problem));
		memcpy(self->problems[self->n_problems], problem, sizeof(*problem));
		self->problems[self->n_problems]->library = problib;
		self->n_problems += 1;
	}
	tdb_die_if(self->n_problems == 0, "No valid problems found");
	free(filename);
	tdb_die_if(errno!=0, strerror(errno));
	closeret = closedir(probdir);
	tdb_die_if(closeret!=0, strerror(errno));
}

static void tdc_init_threads
(struct tdc_context* const self, size_t workers)
{
	pthread_mutex_init(&self->todo_queue_lock, NULL);
	pthread_mutex_init(&self->done_queue_lock, NULL);
	pthread_cond_init(&self->todo_queue_sig, NULL);
	pthread_cond_init(&self->done_queue_sig, NULL);
	self->todo_queue_head = self->todo_queue_tail = NULL;
	self->done_queue_head = self->done_queue_tail = NULL;
	self->exit = 0;
	if (workers<1) {
		workers = tdb_getcpucount();
	}
	self->n_threads = workers;
	self->threads = tdb_calloc(self->n_threads, sizeof(*self->threads));
	for (size_t i=0; i<self->n_threads; i++) {
		pthread_create(&self->threads[i], NULL, tdc_worker_thread, self);
	}
}

struct tdc_context* tdc_init
(const char* problems_dir, size_t workers, size_t init_rand)
{
	struct tdc_context* ret;
	if (init_rand) {
		struct timeval timeval;
		gettimeofday(&timeval, NULL);
		srand((unsigned int)timeval.tv_usec);
	}
	ret = tdb_malloc(sizeof(*ret));
	tdc_init_problems(ret, problems_dir);
	tdc_init_threads(ret, workers);
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

size_t tdc_get_n_problems
(void)
{
	return tdc->n_problems;
}

struct tdc_problem* tdc_get_problem
(size_t index)
{
	index = tdb_clampu(0, index, tdc->n_problems);
	return tdc->problems[index];
}

int tdc_problem_n_machines_settable
(const struct tdc_problem* self)
{
	return (self->n_machines==0);
}

int tdc_problem_weight_enabled
(const struct tdc_problem* self)
{
	return self->tasks.weighted;
}

size_t tdc_problem_get_n_steps
(const struct tdc_problem* self)
{
	return self->tasks.steps;
}

size_t tdc_problem_get_n_machines
(const struct tdc_problem* self)
{
	if (self->n_machines != 0) {
		return self->n_machines;
	}
	/* default value if configurable */
	return 1;
}

static void tdc_init_population_generator
(struct tdc_population_generator* const self,
 const struct tdc_generator* const parent)
{
	self->parent = parent;
	self->n_tasks = 1;
	self->weight.min = 1;
	self->weight.max = TDC_WEIGHT_MAX;
	self->lengths = tdb_calloc(parent->problem->tasks.steps,
	                           sizeof(*(self->lengths)));
	for (size_t i=0; i<parent->problem->tasks.steps; i++) {
		self->lengths[i].min = 1;
		self->lengths[i].max = TDC_LENGTH_MAX;
	}
}

struct tdc_generator* tdc_create_generator
(const struct tdc_problem* problem, size_t n_populations)
{
	struct tdc_generator* ret;
	ret = tdb_malloc(sizeof(*ret));
	ret->problem = problem;
	if (problem->n_machines == 0) {
		ret->n_machines = 1;
	} else {
		ret->n_machines = problem->n_machines;
	}
	ret->n_populations = n_populations;
	ret->populations = tdb_calloc(n_populations, sizeof(*(ret->populations)));
	for (size_t i=0; i<n_populations; i++) {
		tdc_init_population_generator(&(ret->populations[i]), ret);
	}
	return ret;
}

void tdc_delete_generator
(struct tdc_generator* const self)
{
	for (size_t i=0; i<self->n_populations; i++) {
		free(self->populations[i].lengths);
	}
	free(self->populations);
	free(self);
}

void tdc_generator_add_population
(struct tdc_generator* const self)
{
	self->populations = tdb_realloc(self->populations,
	                                (1+self->n_populations)
	                                *sizeof(*(self->populations)));
	tdc_init_population_generator(&(self->populations[self->n_populations]), self);
	self->n_populations += 1;
}

void tdv_generator_delete_population
(struct tdc_generator* const self)
{
	self->n_populations -= 1;
	free(self->populations[self->n_populations].lengths);
	self->populations = tdb_realloc(self->populations,
	                                self->n_populations
	                                *sizeof(*(self->populations)));
}

static void tdc_init_population
(const struct tdc_population_generator* const self, struct tdc_task** tasks,
 size_t offset)
{
	for (size_t i=offset; i<offset+self->n_tasks; i++) {
		tasks[i] = tdb_vla_malloc(tasks[i], self->parent->problem->tasks.steps,
		                          tasks[i]->steps);
		tasks[i]->id = i;
		tasks[i]->weight = tdb_interval_rand(&(self->weight));
		tasks[i]->n_steps = self->parent->problem->tasks.steps;
		for (size_t j=0; j<tasks[i]->n_steps; j++) {
			tasks[i]->steps[j].length = tdb_interval_rand(&(self->lengths[j]));
			tasks[i]->steps[j].machine = 0;
			tasks[i]->steps[j].start_time = 0;
		}
	}
}

struct tdc_job* tdc_create_job
(const struct tdc_generator* const self)
{
	struct tdc_job* ret;
	size_t n_tasks=0;
	for (size_t i=0; i<self->n_populations; i++) {
		n_tasks += self->populations[i].n_tasks;
	}
	ret = tdb_vla_malloc(ret, n_tasks, ret->tasks);
	ret->problem = self->problem;
	ret->n_machines = self->n_machines;
	ret->n_tasks = n_tasks;
	size_t offset = 0;
	for (size_t i=0; i<self->n_populations; i++) {
		tdc_init_population(&(self->populations[i]), ret->tasks, offset);
		offset += self->populations[i].n_tasks;
	}
	return ret;
}

struct tdc_job* tdc_copy_job
(const struct tdc_job* const self)
{
	struct tdc_job* ret;
	ret = tdb_vla_malloc(ret, self->n_tasks, ret->tasks);
	ret->problem = self->problem;
	ret->strategy = self->strategy;
	ret->n_machines = self->n_machines;
	ret->n_tasks = self->n_tasks;
	for (size_t i=0; i<self->n_tasks; i++) {
		ret->tasks[i] = tdb_vla_malloc(ret->tasks[i], self->tasks[0]->n_steps,
		                               ret->tasks[i]->steps);
		ret->tasks[i]->id = self->tasks[i]->id;
		ret->tasks[i]->weight = self->tasks[i]->weight;
		ret->tasks[i]->n_steps = self->tasks[i]->n_steps;
		for (size_t j=0; j<self->tasks[0]->n_steps; j++) {
			ret->tasks[i]->steps[j].length = self->tasks[i]->steps[j].length;
			ret->tasks[i]->steps[j].machine = self->tasks[i]->steps[j].machine;
			ret->tasks[i]->steps[j].start_time = self->tasks[i]->steps[j].start_time;
		}
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

/* Adjust start_time to help interface work and reflect real timeline */
static void tdc_adjust_schedule
(struct tdc_job* self)
{
	size_t * const restrict freetime = tdb_calloc(self->n_machines,
	                                              sizeof(*freetime));
	struct tdc_task** tasks = self->tasks;
	for (size_t i=0; i<self->n_tasks;i++) {
		tasks[i]->steps[0].start_time = freetime[tasks[i]->steps[0].machine];
		freetime[tasks[i]->steps[0].machine] += tasks[i]->steps[0].length;
		for (size_t j=1; j<self->tasks[i]->n_steps; j++) {
			tasks[i]->steps[j].start_time =
				tdb_uimax(freetime[tasks[i]->steps[j-1].machine],
			             freetime[tasks[i]->steps[j].machine]);
			freetime[tasks[i]->steps[j].machine] = tasks[i]->steps[j].start_time
			                                       + tasks[i]->steps[j].length;
		}
	}
	size_t timespan=0;
	for (size_t i=0; i<self->n_machines; i++) {
		timespan = tdb_uimax(timespan, freetime[i]);
	}
	self->timespan = timespan;
	free(freetime);
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
	if (ret!=NULL) {
		tdc_adjust_schedule(ret);
	}
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
	tdc_adjust_schedule(ret);
	return ret;
}



void tdc_delete_job
(struct tdc_job* const self)
{
	for (size_t i=0; i<self->n_tasks; i++) {
		free(self->tasks[i]);
	}
	free(self);
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
	
	for (i=0; i<tdc->n_threads; i++) {
		pthreadret = pthread_join(tdc->threads[i], NULL);
		tdb_die_if(pthreadret != 0, strerror(pthreadret));
	}
	free(tdc->threads);

	/* free problems */
	for (i=0; i<tdc->n_problems; i++) {
		dlclose(tdc->problems[i]->library);
		free(tdc->problems[i]);
	}
	free(tdc->problems);

	/* free queues */
	for(; tdc->todo_queue_head!=NULL; tdc->todo_queue_head=next) {
		next=tdc->todo_queue_head->next;
		tdc_delete_job(tdc->todo_queue_head->job);
		free(tdc->todo_queue_head);
	}
	for(; tdc->done_queue_head!=NULL; tdc->done_queue_head=next) {
		next=tdc->done_queue_head->next;
		tdc_delete_job(tdc->done_queue_head->job);
		free(tdc->done_queue_head);
	}

	/* free context and remove it */
	free(tdc), tdc=NULL;
	tdb_debug("Tamandua core context exited");
}

