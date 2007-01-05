#include "gamandua.h"
#include "gamandua_callbacks.h"

int colors[5] = {
  0x729fcf,
  0x3465a4,
  0xef2929,
  0xcc0000,
  0xfce94f
};

#define COLOR_STEP 15
int id_colors[5] = {
  0xbdd169,
  0xf09534
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
  gtk_widget_set_sensitive(gui->btn_exec, 1);
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
  Gui *gui;
  struct tdc_job *job=NULL;

  if (!(gui = gamandua->gui)) return FALSE;

  job = tdc_checkout();
  if (!job) return TRUE;

  /*
  if (gamandua->job)
    tdc_delete_job(gamandua->jobo);
  */

  gamandua->job = job;
  if (!(gui = gamandua->gui)) return FALSE;
  append_drawing(GTK_NOTEBOOK(gui->drawing_notebook));
  g_object_set_data(G_OBJECT(gui->drawing_area), "job", job);
  cb_drawing_area_exposed(NULL, NULL);
  return FALSE;
}

void
cb_drawing_notebook_change_current(GtkNotebook *notebook, GtkNotebookPage *page, 
                                   size_t p, void *data)
{
  Gui *gui;
  Canvas *canvas;
  GtkWidget *label;

  gui = gamandua->gui;
  if (!gui) return;

  canvas = g_slist_nth_data(gui->drawings, p);
  if (!canvas)
    {
      gui->drawing_area = NULL;
      gui->scrolled = NULL;
      return;
    }

  gui->drawing_area = canvas->drawing_area;
  gui->scrolled = canvas->scrolled;

  /* Reinitialisation de la frame des infos */
  gamandua->selection.task = NULL;
  gui->frame_info = init_frame_info();
}

void
cb_combo_changed(GtkComboBox *combo, void *data)
{
  int i;
  int n_pages;
  gamandua->problem = tdc_get_problem(gtk_combo_box_get_active(combo));
  gamandua->gui->combo_strat = init_combo_strat();
  gamandua->gui->spin_machine = init_spin_machine();

  if (gamandua->gui->notebook)
    {
      n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(gamandua->gui->notebook));
      for (i=0; i<n_pages; i++)
        {
          remove_page(gamandua->gui->notebook);
        }
      append_page(gamandua->gui->notebook);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(gamandua->gui->spin_pop), 1);
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
  const struct tdc_problem *problem;
  
  Canvas *canvas;
  Gui *gui;
  size_t i, j;
  int width, height;

  GdkDrawable *drawable;
  GdkGC *gc;
  GtkRequisition requisition;

  gui = gamandua->gui;
  canvas = get_current_canvas();
  if (!canvas) return;
  job = g_object_get_data(G_OBJECT(canvas->drawing_area), "job");
  if (!gui || !job) return;
  problem = job->problem;

  /* Nettois le drawing area */
  gdk_window_clear(gui->drawing_area->window);

  if (problem != job->problem) return;

  drawable = GDK_DRAWABLE(gui->drawing_area->window);
  gc = gdk_gc_new(drawable);

  /* Redimensionnement si le widget est trop petit pour toutes les taches */
  gdk_drawable_get_size(drawable, &width, &height);
  gtk_widget_size_request(gui->drawing_area, &requisition);
  if ((width != (int)job->timespan+40 
       || height != (int)(50*job->n_machines+50))
      && ((int)job->timespan+40 != requisition.width 
          || (int)(50*job->n_machines+50) != requisition.height))
    {
      gtk_drawing_area_size(GTK_DRAWING_AREA(gui->drawing_area), 
                            (int)job->timespan+40, 
                            (int)50*job->n_machines+50);
      if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
      return;
    }

  gdk_rgb_gc_set_foreground(gc, 0xffffff);
  gdk_draw_rectangle(drawable, gc, TRUE, 0, 0, width, height);

  PangoLayout *layout;
  PangoContext *context;
  char *str;
  asprintf(&str, 
           "Problème <i>%s</i>, Stratégie <i>%s</i>\n"
           "<b>%d</b> tâche%c ordonnancée%c sur <b>%d</b> Machine%c",
           problem->name, problem->strategies[job->strategy], job->n_tasks, 
           job->n_tasks > 1 ? 's': '\0', job->n_tasks > 1 ? 's': '\0',
           job->n_machines, job->n_machines > 1 ? 's': '\0');
  context = gtk_widget_get_pango_context(gui->drawing_area);
  layout  = pango_layout_new(context);
  pango_layout_set_markup(layout, str, -1);
  gdk_rgb_gc_set_foreground(gc, 0x000000);
  gdk_draw_layout(drawable, gc, 5, 5, layout);

  free(str);
  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);

  struct tdc_task *task;
  size_t max_length = 0;
  double *machines;
  size_t n_steps = tdc_problem_get_n_steps(problem);
  machines = calloc(job->n_machines, sizeof(double));

  for (i=0; i<job->n_tasks; i++)
    {
      task = job->tasks[i];
      for (j=0; j<n_steps; j++)
        {
          if (machines[task->steps[j].machine] < task->steps[j].start_time + task->steps[j].length)
            machines[task->steps[j].machine] = task->steps[j].start_time + task->steps[j].length;
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
          if (task->n_steps > 1)
            color = id_colors[draw[task->steps[j].machine]%2]+COLOR_STEP*task->id;
          else
            color = colors[draw[task->steps[j].machine]%2 
             + (task->steps[j].machine == max_length ? 2:0)];

          draw_task(task->steps[j].start_time, task->steps[j].machine*50+40, 
                    task->steps[j].length, 40, 
                    color);
          draw[task->steps[j].machine]++;

          if (task->steps[j].start_time + task->steps[j].length == machines[task->steps[j].machine])
            draw_task_txt_size(machines[task->steps[j].machine],
                               task->steps[j].start_time + task->steps[j].length + 10,
                               task->steps[j].machine*50+50,
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
  Canvas *canvas;
  Gui *gui;
  struct tdc_job *job;
  const struct tdc_problem *problem;
  double x = 0, y = 0;
  size_t i, j, k=0;
  struct tdc_task *task;

  gui = gamandua->gui;
  canvas = get_current_canvas();
  problem = gamandua->problem;
  if (!canvas) return;
  job = g_object_get_data(G_OBJECT(canvas->drawing_area), "job");
  if (!gui || !job) return;
  problem = job->problem;

  x = ev->x + gtk_adjustment_get_value(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(canvas->scrolled)));
  y = ev->y + gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(canvas->scrolled)));

  for (i=0; i<job->n_tasks; i++)
    {
      task = job->tasks[i];
      for (j=0; j<task->n_steps; j++)
        {
          if (task->steps[j].machine == (size_t)(y-40)/50)
            {
              if ((double)(task->steps[j].start_time + 5) <= x
                  && (double)(task->steps[j].length + task->steps[j].start_time + 5) > x)
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

                  asprintf(&str, "Durée : %d", task->steps[j].length);
                  label = gtk_label_new(str);
                  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
                  free(str);

                  asprintf(&str, "Machine : %d", task->steps[j].machine);
                  label = gtk_label_new(str);
                  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
                  free(str);

                  asprintf(&str, "Débute à : %d", task->steps[j].start_time);
                  label = gtk_label_new(str);
                  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 5);
                  free(str);
                  gtk_widget_show_all(vbox);

                  if (gamandua->selection.task)
                    {
                      size_t step = gamandua->selection.step;
                      old_task = gamandua->selection.task;
                      draw_task(old_task->steps[step].start_time, 
                                old_task->steps[step].machine*50+40, 
                                old_task->steps[step].length, 40, 
                                gamandua->selection.color);
                    }

                  gamandua->selection.step = j;
                  gamandua->selection.task = task;
                  if (task->n_steps > 1)
                    gamandua->selection.color = id_colors[k%2]+COLOR_STEP*task->id;
                  else
                    gamandua->selection.color = colors[k%2 
                     + (task->steps[j].machine == gamandua->max_length ? 2:0)];

                  /* On highlight la nouvelle tache selectionne */
                  draw_selection();
                  return;
                }
              k++;
            } 
        }
    }
  if (gamandua->selection.task && i == job->n_tasks)
    {
      struct tdc_task *old_task;
      size_t step = gamandua->selection.step;
      old_task = gamandua->selection.task;
      draw_task(old_task->steps[step].start_time, 
                old_task->steps[step].machine*50+40, 
                old_task->steps[step].length, 40, 
                gamandua->selection.color);
      gamandua->selection.task = NULL;
      init_frame_info();
    }
}

void
cb_unsensitive_btn_exec(void *w, void *data)
{
  Gui *gui;

  gui = gamandua->gui;
  if (!gui) return;

  gtk_widget_set_sensitive(gui->btn_exec, 0);
}

void
cb_drawing_page_close(GtkButton *button, void *data)
{
  Gui *gui;
  Canvas *canvas;
  GtkNotebook *notebook;
  size_t p;

  gui = gamandua->gui;
  if (!gui) return;
  notebook = GTK_NOTEBOOK(gui->drawing_notebook);
  p = gtk_notebook_page_num(notebook, data);

  canvas = g_slist_nth_data(gui->drawings, p);
  if (!canvas)
    {
      gui->drawings = g_slist_remove(gui->drawings, NULL);
      gtk_notebook_remove_page(notebook, p);
      gtk_widget_set_sensitive(gui->btn_help, 1);
      return;
    }

  tdc_delete_job(g_object_get_data(G_OBJECT(canvas->drawing_area), "job"));
  gui->drawings = g_slist_remove(gui->drawings, canvas);
  gtk_notebook_remove_page(notebook, p);
}

void
cb_btn_help_clicked(GtkButton *button, void *data)
{
  Gui *gui;
  GtkWidget *align;
  GtkWidget *label;

  gui = gamandua->gui;
  if (!gui) return;

  append_help(GTK_NOTEBOOK(gui->drawing_notebook));
}

void
cb_btn_clear_clicked(GtkButton *button, void *data)
{
  Gui *gui;
  Canvas *canvas;
  gui = gamandua->gui;
  GSList *list;
  GSList *next;
  size_t i;

  if (!gui) return;

  for (list = gui->drawings, i=0; list!=NULL; list = next, i++)
    {
      next = g_slist_next(list);
      canvas = list->data;
      gui->drawings = g_slist_delete_link(gui->drawings, list);
      if (canvas) 
        {
          tdc_delete_job(g_object_get_data(G_OBJECT(canvas->drawing_area), "job"));
          free(canvas);
        }
      gtk_notebook_remove_page(GTK_NOTEBOOK(gui->drawing_notebook), 0);
    }
  gtk_widget_set_sensitive(gui->btn_help, 1);
}
