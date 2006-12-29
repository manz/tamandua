const char name[] = "Smith";
const char description[] =
"Algorithme de répartition de tâches"
"en fonction de leurs quotiens durée/poids.";
const char* strategies[] = {
	"Rêgle de Smith"
};

int problem
(size_t strategy, struct tdc_job* job);

TDC_PROBLEM(name, description, strategies, problem, 1, 1, 1);
