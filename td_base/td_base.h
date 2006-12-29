#ifndef INC_TD_BASE_H
#define INC_TD_BASE_H 1

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

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
	int* argc;
	char** argv;
};

void tdb_init(FILE* logger, int* argc, char** argv);

void tdb_exit(void);

/* get cpu count */
size_t tdb_getcpucount(void);

float tdb_rand(void);

/* interval managment */
struct tdb_interval {
	size_t min;
	size_t max;
};

size_t tdb_interval_set_min
(struct tdb_interval* interval, size_t val);

size_t tdb_interval_set_max
(struct tdb_interval* interval, size_t val);

size_t tdb_interval_rand
(const struct tdb_interval* interval);

int* tdb_get_argc
(void);

char** tdb_get_argv
(void);

/* little perf hack */
#define tdb_die_if(cond, msg) do {if (cond) {tdb_die(msg, __LINE__, __FILE__);}}while(0)
void tdb_die(const char* msg, int lineno, const char* filename);
void tdb_warn(const char* msg);
void tdb_debug(const char* msg, ...);

void* tdb_malloc(size_t size);
void* tdb_calloc(size_t nmemb, size_t size);
void* tdb_realloc(void* ptr, size_t size);

#define tdb_vla_malloc(ptr, count, lastarg) \
tdb_malloc(sizeof(*(ptr))+count*sizeof(*(lastarg)))

#define tdb_vla_realloc(ptr, count, lastarg) \
tdb_realloc(ptr, sizeof(*(ptr))+count*sizeof(*(lastarg)))

static inline size_t tdb_uimax
(const size_t a, const size_t b)
{
	return a>b?a:b;
}

static inline size_t tdb_nextpowof2
(size_t val)
{
	val--;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	return val+1;
}

static inline float tdb_clampf
(float min, float val, float max)
{
	if (val <= min) return min;
	if (val >= max) return max;
	return val;
}

static inline size_t tdb_clampu
(size_t min, size_t val, size_t max)
{
	if (val <= min) return min;
	if (val >= max) return max;
	return val;
}

#endif /*INC_TD_BASE_H*/
