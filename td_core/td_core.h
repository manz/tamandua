#ifndef INC_TD_CORE_H
#define INC_TD_CORE_H 1

#include <pthread.h> /* posix threads */
#include <dlfcn.h> /* dl open */
#include <dirent.h> /* file managment */
#include <sys/time.h> /* random seed init */
#include <td_base.h>

#ifdef INC_PRIVATE_TD_CORE_H
struct tdc_context* tdc;
#else
extern struct tdc_context* tdc;
#endif /*INC_PRIVATE_TD_CORE_H*/

struct tdc_context {
	size_t problems_count;
	struct tdc_problem** problems;

	pthread_mutex_t todo_queue_lock;
	pthread_cond_t todo_queue_sig;
	struct tdc_job_queue* todo_queue_head;
	struct tdc_job_queue* todo_queue_tail;

	pthread_mutex_t done_queue_lock;
	pthread_cond_t done_queue_sig;
	struct tdc_job_queue* done_queue_head;
	struct tdc_job_queue* done_queue_tail;
	
	size_t threads_count;
	pthread_t* threads;

	size_t exit;
};

struct tdc_job_queue {
	struct tdc_job* job;
	struct tdc_job_queue* prev;
	struct tdc_job_queue* next;
};

enum tdc_property_type {
	/* available to both globals and tasks */
	TDC_INPUT=1, /* will be generated from the interface */
	/* available only to globals */
	TDC_TEXT=2, /* will be displayed in a text resume */
	TDC_MARK=4, /* will produce a plane */
	/* display possibilities for tasks only */
	TDC_OFFSET=2, /* will set the task offset from the previous */
	TDC_LENGTH=4, /* will set the task length */
	TDC_COLUMN=8  /* will set the task column */
};

struct tdc_property {
	enum tdc_property_type type;
	size_t min;
	size_t max;
	char* name;
};

struct tdc_problem {
	const char* name;
	void* library;
	int (*const process)(size_t strategy, struct tdc_job* job);
	const size_t strategies_count;
	const char** strategies;
	const size_t globals_count;
	const struct tdc_property* globals;
	const size_t properties_count;
	const struct tdc_property* properties;
};

#define TDC_DECLARE(func, name, strats, globals, props) \
struct tdc_problem tdc_desc = { \
	name, NULL, func, \
	sizeof(strats)/sizeof(strats[0]), strats, \
	sizeof(globals)/sizeof(strats[0]), globals, \
	sizeof(props)/sizeof(strats[0]), props }

struct tdc_conf {
	struct tdc_problem* problem;
	size_t tasks_count;
	size_t* globals;
	size_t* min;
	size_t* max;
};

struct tdc_job {
	struct tdc_problem* problem;
	size_t strategy;
	size_t tasks_count;
	size_t* globals;
	size_t** tasks;
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

/**
 * Cree un configurateur pour un probleme donne.
 * Cette ressource est a la charge du client.
 */
struct tdc_conf* tdc_buildconf
(struct tdc_problem* problem);

/**
 * Supprime un configurateur.
 */
void tdc_delconf
(struct tdc_conf* conf);

/**
 * Cree un set de donne d'apres un configurateur.
 */
struct tdc_job* tdc_buildjob
(struct tdc_conf* conf);

/**
 * Ajoute un element en fin de jobqueue.
 */
void tdc_pushqueue
(struct tdc_job_queue* item, struct tdc_job_queue** queue_head,
 struct tdc_job_queue** queue_tail, pthread_mutex_t* lock, pthread_cond_t* signal);

/**
 * Duplique un job. le faire UNIQUEMENT avant un commit ou après un checkout.
 */
struct tdc_job* tdc_copyjob
(struct tdc_job* job);



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
void tdc_deljob
(struct tdc_job* job);

/**
 * Desactive le contexte en cours et libere les ressources des que cela est 
 * possible. (ce qui inclue les jobs non-checkoutes, mais pas les conf).
 */
void tdc_exit
(void);

/**
 * Quelques tests basiques d'utilisation du core.
 */
void tdc_test
(void);

#endif /*INC_TD_CORE_H*/
