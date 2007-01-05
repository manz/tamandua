const char name[]        =   "Flowshop2"   ;
const char description[] =   "Algorithme de répartion de tâches "
                             "en fonction de leurs temps d'excution." ;
const char *strategies[] = { "Johnson"  };

int problem(size_t strategies, struct tdc_job *job);
TDC_PROBLEM(name, description, strategies, problem, 2,0,2);

int compare(const void *p1, const void *p2);
