#ifndef INC_TD_COMMON_H
#define INC_TD_COMMON_H 1

#define _GNU_SOURCE

#include <stdlib.h> /* Standard libs */
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif /* __APPLE__ */

#include <pthread.h> /* posix threads */

#ifdef INC_PRIVATE_TD_COMMON_H
struct tdb_context* td;
#else
extern struct tdb_context* td;
#endif /*INC_PRIVATE_TD_COMMON_H*/

struct tdb_context {
	pthread_mutex_t logger_lock;
	FILE* logger;
};

void tdb_init(FILE* logger);

void tdb_exit(void);

/* get cpu count */
size_t tdb_getcpucount(void);

/* little perf hack */
#define tdb_die_if(cond, msg) do {if (cond) {tdb_die(msg, __LINE__, __FILE__);}}while(0)
void tdb_die(const char* msg, int lineno, const char* filename);
void tdb_warn(const char* msg);
void tdb_debug(const char* msg, ...);

void* tdb_malloc(size_t size);
void* tdb_calloc(size_t nmemb, size_t size);
void* tdb_realloc(void* ptr, size_t size);

#endif /*INC_TD_COMMON_H*/


