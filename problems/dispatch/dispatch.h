const char name[] = "Dispatch";
const char description[] =
"Algorithme de répartition d'un ensemble de tâches "
"sur un nombre fixé de machines.";
const char* strategies[] = {
	"Méthode naïve",
	"Tâches triées"
};

int problem
(size_t strategy, struct tdc_job* job);

TDC_PROBLEM(name, description, strategies, problem, 0, 0, 1);
