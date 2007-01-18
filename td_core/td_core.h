#ifndef INC_TD_CORE_H
#define INC_TD_CORE_H 1

#include <pthread.h> /* posix threads */
#include <dlfcn.h> /* dl open */
#include <dirent.h> /* file managment */
#include <sys/time.h> /* random seed init */

static const size_t TDC_WEIGHT_MAX = 1<<((CHAR_BIT*sizeof(size_t))/2);
static const size_t TDC_LENGTH_MAX = 1<<((CHAR_BIT*sizeof(size_t))/2);
static const size_t TDC_TASKS_MAX = 1<<((CHAR_BIT*sizeof(size_t))/2);


#ifdef INC_PRIVATE_TD_CORE_H
struct tdc_context* tdc;
#else
extern struct tdc_context* tdc;
#endif /*INC_PRIVATE_TD_CORE_H*/

struct tdc_context {
	size_t n_problems;
	struct tdc_problem** problems;

	pthread_mutex_t todo_queue_lock;
	pthread_cond_t todo_queue_sig;
	struct tdc_job_queue* todo_queue_head;
	struct tdc_job_queue* todo_queue_tail;

	pthread_mutex_t done_queue_lock;
	pthread_cond_t done_queue_sig;
	struct tdc_job_queue* done_queue_head;
	struct tdc_job_queue* done_queue_tail;
	
	size_t n_threads;
	pthread_t* threads;

	size_t exit;
};

struct tdc_job_queue {
	struct tdc_job* job;
	struct tdc_job_queue* prev;
	struct tdc_job_queue* next;
};

struct tdc_problem {
	const char* name;
	const char* description;
	const size_t n_strategies;
	const char** strategies;
	const size_t n_machines;
	struct {
		const size_t weighted;
		const size_t steps;
	} tasks;
	void* library;
	int (*const process)(size_t strategy, struct tdc_job* job);
};

#define TDC_PROBLEM(name, desc, strats, func, machines, weighted, steps) \
struct tdc_problem tdc_desc = { \
	name, desc, \
	sizeof(strats)/sizeof(strats[0]), strats, \
	machines, {weighted, steps}, \
	NULL, func} \

struct tdc_job {
	const struct tdc_problem* problem;
	size_t n_machines;
	size_t timespan;
	size_t strategy;
	size_t n_tasks;
	struct tdc_task* tasks[];
};

struct tdc_task {
	size_t id;
	size_t weight;
	size_t n_steps;
	struct {
		size_t length;
		size_t machine;
		size_t start_time;
	} steps[];
};

struct tdc_population_generator {
	const struct tdc_generator *parent;
	size_t n_tasks;
	struct tdb_interval weight;
	struct tdb_interval* lengths;
};

struct tdc_generator {
	const struct tdc_problem* problem;
	size_t n_machines;
	size_t n_populations;
	struct tdc_population_generator* populations;
};

/**
 * Initalise un environnement de calcul reference par context.
 */
struct tdc_context* tdc_init
(const char* problems_dir, size_t workers, size_t init_rand);

/**
 * Change le contexte en cours.
 */
void tdc_setcontext
(struct tdc_context* context);

/**
 * Obtient le contexte en cours.
 */
struct tdc_context* tdc_getcontext
(void);

size_t tdc_get_n_problems
(void);

struct tdc_problem* tdc_get_problem
(size_t index);

int tdc_problem_n_machines_settable
(const struct tdc_problem* self);

int tdc_problem_weight_enabled
(const struct tdc_problem* self);

size_t tdc_problem_get_n_steps
(const struct tdc_problem* self);

size_t tdc_problem_get_n_machines
(const struct tdc_problem* self);

/**
 * Cree un generateur de donnees pour un probleme.
 * Cette ressource est a la charge du client.
 */
struct tdc_generator* tdc_create_generator
(const struct tdc_problem* problem, size_t n_populations);

/**
 * Supprime un generateur.
 */
void tdc_delete_generator
(struct tdc_generator* const self);

void tdc_generator_add_population
(struct tdc_generator* const self);

void tdv_generator_delete_population
(struct tdc_generator* const self);

/**
 * Cree un set de donne d'apres un configurateur.
 */
struct tdc_job* tdc_create_job
(const struct tdc_generator* const generator);

/**
 * Ajoute un element en fin de jobqueue.
 */
void tdc_pushqueue
(struct tdc_job_queue* item, struct tdc_job_queue** queue_head,
 struct tdc_job_queue** queue_tail, pthread_mutex_t* lock, pthread_cond_t* signal);

/**
 * Duplique un job. le faire UNIQUEMENT avant un commit ou après un checkout.
 */
struct tdc_job* tdc_copy_job
(const struct tdc_job* const self);



/**
 * Envoie le job pour etre calcule par son solveur.
 * Des que cette commande est executee, on ne doit plus toucher au job jusqu'a
 * son checkout.
 */
void tdc_commit
(struct tdc_job* job);

/**
 * Thread de calcul...
 */
void* tdc_worker_thread
(void* data);

/**
 * Receptione un job apres calcul, ou NULL s'il n'y en a aucun de pret.
 * Les jobs sont renvoyes dans l'ordre de commit.
 * Une fois checkoute, la gestion de la suppression du job est a la charge du 
 * client.
 */
struct tdc_job* tdc_checkout
(void);


/**
 * Receptionne un job apres calcul, appel bloquant, renvoie forcement un job.
 * Les jobs sont renvoyes dans l'ordre de commit.
 * Une fois checkoute, la gestion de la suppression du job est a la charge du 
 * client.
 */
struct tdc_job* tdc_force_checkout
(void);

/**
 * Supprime un job, UNIQUEMENT apres checkout ou avant commit !
 */
void tdc_delete_job
(struct tdc_job* const self);

/**
 * Desactive le contexte en cours et libere les ressources des que cela est 
 * possible. (ce qui inclue les jobs non-checkoutes, mais pas les conf).
 */
void tdc_exit
(void);

#endif /*INC_TD_CORE_H*/
