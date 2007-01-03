const char name[]        =   "Flowshop3";
const char description[] =   "Algorithme de répartion de tâches "
                             "sur trois machines "
                             "en fonction de leurs temps d'excution." ;
const char *strategies[] = { "Flowshopt3"  };

int problem(size_t strategies, struct tdc_job *job);
TDC_PROBLEM(name, description, strategies, problem, 3,0,3);

int compare(const void *p1, const void *p2);
