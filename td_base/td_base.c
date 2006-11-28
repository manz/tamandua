#define INC_PRIVATE_TD_COMMON_H
#include "td_base.h"

void tdb_init
(FILE* logger)
{
	/* we can't use tdb_malloc with logging yet */
	td = malloc(sizeof(*td));
	if (td == NULL) {
		(void)fputs("\n[EE] Not enough memory to start\n", stderr);
		pthread_exit(NULL);
	}
	pthread_mutex_init(&td->logger_lock, NULL);
	td->logger = logger;
	if (td->logger == NULL) {
		td->logger = stderr;
	}
}

void tdb_exit
(void)
{
	/* should add refcounting of sublibs + cascaded exit */
	tdb_die_if(td == NULL, "exiting without initing first");
	free(td), td=NULL;
	pthread_exit(NULL);
}

size_t tdb_getcpucount
(void)
{
	long ret;
	/* try to guess value from processor count */
	/* XXX:  linux dependant
	 * maybe should add arch-dependant code separately,
	 * for this part, look at: http://ndevilla.free.fr/threads/index.html
	 */
	ret = sysconf(_SC_NPROCESSORS_CONF);
	if (ret == -1) {
		tdb_debug("Failed to guess processor count: %s", strerror(errno));
		ret = 1;
	} else {
		tdb_debug("Autoselected workers count (linux): %i", ret);
	}
	return (size_t)ret;
}

void tdb_fprintf(FILE *stream, const char* format, ...)
{
	int ret;
	va_list args, backup_args;
	va_start(args, format);
	va_copy(backup_args, args);
	ret = vfprintf(stream, format, args);
	if ((ret < 0) && (stream!=stderr)) {
		/* those are unchecked, can't do much better */
		(void)fputs("[EE] Can't write to standard logger", stderr);
		(void)vfprintf(stderr, format, backup_args);
	}
	va_end(backup_args);
	va_end(args);
}

int tdb_vfprintf(FILE *stream, const char* format, va_list args)
{
	int ret;
	va_list backup_args;
	va_copy(backup_args, args);
	ret = vfprintf(stream, format, args);
	if ((ret < 0) && (stream!=stderr)) {
		/* thoses are unchecked, can't do much better */
		(void)fputs("[EE] Can't write to standard logger", stderr);
		(void)vfprintf(stderr, format, backup_args);
	}
	va_end(backup_args);
	return ret;
}

void tdb_die
(const char* msg, int lineno, const char* filename)
{
	pthread_mutex_lock(&td->logger_lock);
	tdb_fprintf(td->logger, "[EE] %s:%i: %s\n", filename, lineno, msg);
	pthread_mutex_unlock(&td->logger_lock);
	/* Very hard failure, exit everything ignoring other threads */
	exit(EXIT_FAILURE);
}

void tdb_warn
(const char* msg)
{
	int check;
	pthread_mutex_lock(&td->logger_lock);
	tdb_fprintf(td->logger, "[WW] %s\n", msg);
	pthread_mutex_unlock(&td->logger_lock);
}

void tdb_debug
(const char* msg, ...)
{
	int check;
	va_list args;
	va_start(args, msg);
	pthread_mutex_lock(&td->logger_lock);
	tdb_fprintf(td->logger,"[DD] ");
	tdb_vfprintf(td->logger, msg, args);
	tdb_fprintf(td->logger,"\n");
	va_end(args);
	pthread_mutex_unlock(&td->logger_lock);
}

void* tdb_malloc
(size_t size)
{
	void* ptr;
	ptr = malloc(size);
	tdb_die_if(ptr == NULL, "Not nough memory");
	return ptr;
}

void* tdb_calloc
(size_t nmemb, size_t size)
{
	void* ptr;
	ptr = calloc(nmemb, size);
	tdb_die_if(ptr == NULL, "Not enough memory");
	return ptr;
}

void* tdb_realloc
(void* ptr, size_t size)
{
	/* Shouldn't generally do that, but as we exit on failure... */
	ptr = realloc(ptr, size);
	tdb_die_if(ptr == NULL, "Not enough memory");
	return ptr;
}
