#include <td_base.h>
#include <td_core.h>

#include "flowshop2.h"


int johnson
(struct tdc_job *job)
{
  size_t i;
  
  for(i=0 ; i<job->n_tasks ; i++)
    {
      /* Par defaut toutes les machines ont un numéro 0,
       * on ajoute le bon identifiant de la seconde machine.
       */
      job->tasks[i]->steps[1].machine = 1;
    }
  /* Algorithme de Jonhson */
  qsort(job->tasks, job->n_tasks, sizeof(*job->tasks), compare);
  /* fin algo */
  return EXIT_SUCCESS;
}

int problem
(size_t strategy, struct tdc_job *job)
{
  return johnson(job);
}

/* Vérifie si une tâche est dans la pile T ou R.
 * Une tâche est dans T si et seulement si elle s'execute plus ou aussi
 * rapidement sur la première machine que sur la seconde.
 * Renvoie vrai si elle est dans T faux si dans R
 */
int isinT
(const struct tdc_task *self)
{
  return (self->steps[0].length <= self->steps[1].length);
}

/* Reorganise, couplé avec un qsort, les tâches de telle façon que
 * celle s'executant le plus rapidement sur la première machine soient
 * en début de liste par ordre croissant,
 * et celle s'executant le plus rapidement sur la seconde machine soient
 * en fin de liste pas ordre decroissant.
 */
int compare
(const void *p1, const void *p2)
{
  const struct tdc_task* const *a = p1;
  const struct tdc_task* const *b = p2;
  
  if(isinT(*a))
    {
      if(isinT(*b))
        {
          return ((*a)->steps[0].length - (*b)->steps[0].length);
        }
      else
        {
          return -1;
        }
    }
  else
    {
      if(isinT(*b))
        {
          return 1;
        }
      else
        {
          return ((*b)->steps[1].length - (*a)->steps[1].length);
        }
    }
}

