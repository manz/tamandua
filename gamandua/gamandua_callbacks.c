#include "gamandua.h"
#include "gamandua_callbacks.h"

int colors[5] = {
  0x729fcf,
  0x3465a4,
  0xef2929,
  0xcc0000,
  0xfce94f
};

#define ID_COLORS_COUNT 3
int id_colors[3] = {
  0xe9b96e,
  0x729fcf,
  0xad7fa8
};
static int cb_checkout_job(void *data);

void
cb_window_destroy(GtkObject *obj, void *data)
{
  gtk_object_destroy(obj);
  free(gamandua);
  gtk_main_quit();
}

void
cb_btn_quit_clicked(GtkButton *button, void *data)
{
  Gui *gui;
  gui = gamandua->gui;
  gtk_widget_destroy(gui->window);
  free(gamandua);
  gtk_main_quit();
}

void
cb_btn_calc_clicked(GtkButton *button, void *data)
{
  struct tdc_generator *generator;
  size_t n_pop;
  Gui *gui;

  gui = gamandua->gui;
  if (!gui) return;

  if (gamandua->generator)
    {
      tdc_delete_generator(gamandua->generator);
      gamandua->generator = NULL;
    }
  if (gamandua->base_job)
    {
      tdc_delete_job(gamandua->base_job);
      gamandua->base_job = NULL;
    }

  n_pop = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_pop));
  gamandua->generator = tdc_create_generator(gamandua->problem, n_pop);
  apply_pop_settings(gamandua->generator);
  gamandua->base_job = tdc_create_job(gamandua->generator);

  /* Exec part */
  struct tdc_job *job;
  gamandua->selection.task = NULL;
  job = tdc_copy_job(gamandua->base_job);
  job->strategy = (size_t)gtk_combo_box_get_active(GTK_COMBO_BOX(gui->combo_strat));
  if (tdc_problem_n_machines_settable(gamandua->problem))
    job->n_machines = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_machine));

  tdc_commit(job);
  g_timeout_add(500, cb_checkout_job, NULL);
}

void
cb_btn_exec_clicked(GtkButton *button, void *data)
{
  Gui *gui;
  struct tdc_generator *generator;
  struct tdc_job *job;

  gui = gamandua->gui;
  if (!gui) return;

  generator = gamandua->generator;
  job = gamandua->base_job;
  if (!generator || !job)
    return;

  gamandua->selection.task = NULL;
  job = tdc_copy_job(job);
  job->strategy = (size_t)gtk_combo_box_get_active(GTK_COMBO_BOX(gui->combo_strat));
  if (tdc_problem_n_machines_settable(gamandua->problem))
    job->n_machines = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_machine));

  tdc_commit(job);
  g_timeout_add(500, cb_checkout_job, NULL);
}

int
cb_checkout_job(void *data)
{
  struct tdc_job *job=NULL;

  job = tdc_checkout();
  if (!job) return TRUE;

  if (gamandua->job)
    tdc_delete_job(gamandua->job);

  gamandua->job = job;
  cb_drawing_area_exposed(NULL, NULL);
  return FALSE;
}

void
cb_combo_changed(GtkComboBox *combo, void *data)
{
  int i;
  int n_pages;
  gamandua->problem = tdc_get_problem(gtk_combo_box_get_active(combo));
  gamandua->gui->combo_strat = init_combo_strat();

  if (gamandua->gui->notebook)
    {
      n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(gamandua->gui->notebook));
      for (i=0; i<n_pages; i++)
        {
          remove_page(gamandua->gui->notebook);
        }
      append_page(gamandua->gui->notebook);
    }

  if (gamandua->generator)
    {
      tdc_delete_generator(gamandua->generator);
      gamandua->generator = NULL;
    }
  if (gamandua->base_job)
    {
      tdc_delete_job(gamandua->base_job);
      gamandua->base_job = NULL;
    }
}

void
cb_spin_pop_changed(GtkSpinButton *spin, void *data)
{
  GtkWidget *notebook = data;
  int val;
  int i;
  int loop;
  void (*func) (GtkWidget *notebook);

  if (!notebook) return;

  val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin));
  val -= gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

  loop = val > 0 ? val : -val;
  for (i=0; i<loop; i++)
    {
      if (val > 0)
        {
          append_page(notebook);
          if (gamandua->generator)
            tdc_generator_add_population(gamandua->generator);
          gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 
                                        gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook))-1);
        }
      else
        {
          remove_page(notebook);
          if (gamandua->generator)
            tdv_generator_delete_population(gamandua->generator);
        }
    }
}

void
cb_drawing_area_exposed(GtkDrawingArea *area, void *data)
{
  struct tdc_job *job=NULL;
  struct tdc_problem *problem = NULL;
  
  Gui *gui;
  size_t i, j;
  int width, height;

  GdkDrawable *drawable;
  GdkGC *gc;
  GtkRequisition requisition;

  gui = gamandua->gui;
  job = gamandua->job;
  problem = gamandua->problem;
  if (!gui || !job || !problem) return;

  /* Nettois le drawing area */
  gdk_window_clear(gui->drawing_area->window);
  drawable = GDK_DRAWABLE(gui->drawing_area->window);
  gc = gdk_gc_new(drawable);

  /* Redimensionnement si le widget est trop petit pour toutes les taches */
  gdk_drawable_get_size(drawable, &width, &height);
  gtk_widget_size_request(gui->drawing_area, &requisition);
  if ((width != (int)job->timespan+40 
       || height != (int)(50*job->n_machines))
      && ((int)job->timespan+40 != requisition.width 
          || (int)(50*job->n_machines) != requisition.height))
    {
      gtk_drawing_area_size(GTK_DRAWING_AREA(gui->drawing_area), 
                            (int)job->timespan+40, 
                            (int)50*job->n_machines);
      if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
      return;
    }

  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);


  struct tdc_task *task;
  size_t max_length = 0;
  size_t *machines;
  size_t n_steps = tdc_problem_get_n_steps(problem);
  machines = calloc(job->n_machines, sizeof(size_t));

  for (i=0; i<job->n_tasks; i++)
    {
      task = job->tasks[i];
      for (j=0; j<n_steps; j++)
        {
          machines[task->steps[j].machine] += task->steps[j].length;
          if (machines[max_length] < machines[task->steps[j].machine])
            max_length = task->steps[j].machine;
        }
    }
  gamandua->max_length = max_length;

  size_t *draw;
  draw = calloc(job->n_machines, sizeof(size_t));
  for (i=0; i<job->n_tasks; i++)
    {
      size_t col;
      int color;
 
      task = job->tasks[i];
      for (j=0; j<n_steps; j++)
        {
          color = colors[draw[task->steps[j].machine]%2 
           + (task->steps[j].machine == max_length ? 2:0)];

          draw_task(task->steps[j].start_time, task->steps[j].machine*50+10, 
                    task->steps[j].length, 40, 
                    color);
          draw[task->steps[j].machine]++;

          if (task->steps[j].start_time + task->steps[j].length == machines[task->steps[j].machine])
            draw_task_txt_size(machines[task->steps[j].machine],
                               task->steps[j].start_time + task->steps[j].length + 10,
                               task->steps[j].machine*50+20,
                               0x000000);
        }
    }
  free(draw);
  free(machines);

  draw_selection();
}

void
cb_drawing_area_mouse_down(GtkWidget *widget, GdkEventButton *ev, void *data)
{
  Gui *gui;
  struct tdc_job *job;
  struct tdc_problem *problem;
  double x = 0, y = 0;
  size_t i, j, k=0;
  struct tdc_task *task;

  gui = gamandua->gui;
  problem = gamandua->problem;
  job = gamandua->job;
  if (!gui || !problem || !job) return;

  x = ev->x + gtk_adjustment_get_value(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(gui->scrolled)));
  y = ev->y + gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(gui->scrolled)));

  for (i=0; i<job->n_tasks; i++)
    {
      task = job->tasks[i];
      if (task->steps[0].machine == (size_t)y/50)
        {
          if (task->steps[0].start_time <= x
              && task->steps[0].length + task->steps[0].start_time > x)
            {
              GtkWidget *vbox;
              GtkWidget *label;
              char *str;
              struct tdc_task *old_task;

              /* On affiche les properties de la taches */
              gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(gui->frame_info)));
              vbox = gtk_vbox_new(FALSE, 0);
              gtk_container_add(GTK_CONTAINER(gui->frame_info), vbox);
              asprintf(&str, "ID : %d", task->id);
              label = gtk_label_new(str);
              gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
              free(str);

              if (problem->tasks.weighted)
                {
                  asprintf(&str, "Poid : %d", task->weight);
                  label = gtk_label_new(str);
                  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
                  free(str);
                }

              asprintf(&str, "Durée : %d", task->steps[0].length);
              label = gtk_label_new(str);
              gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
              free(str);

              asprintf(&str, "Machine : %d", task->steps[0].machine);
              label = gtk_label_new(str);
              gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
              free(str);

              asprintf(&str, "Débute à : %d", task->steps[0].start_time);
              label = gtk_label_new(str);
              gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
              free(str);
              gtk_widget_show_all(vbox);

              /* On highlight la nouvelle tache selectionne */
              if (gamandua->selection.task)
                {
                  old_task = gamandua->selection.task;
                  draw_task(old_task->steps[0].start_time, 
                            old_task->steps[0].machine*50+10, 
                            old_task->steps[0].length, 40, 
                            gamandua->selection.color);
                }

              gamandua->selection.task = task;
              gamandua->selection.color = colors[k%2 
               + (task->steps[0].machine == gamandua->max_length ? 2:0)];

              draw_selection();
            }
          k++;
        } 
    }
}

