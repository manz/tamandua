#include "gamandua.h"
#include "gamandua_callbacks.h"

void
cb_window_destroy(GtkObject *obj, void *data)
{
  gamandua->gui = NULL;
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
  size_t n_machines;
  size_t strat;
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
  gamandua->selection.task = NULL;

  n_pop = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_pop));
  gamandua->generator = tdc_create_generator(gamandua->problem, n_pop);
  apply_pop_settings(gamandua->generator);
  gamandua->base_job = tdc_create_job(gamandua->generator);

  if (tdc_problem_n_machines_settable(gamandua->problem))
    n_machines = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_machine));

  strat = (size_t)gtk_combo_box_get_active(GTK_COMBO_BOX(gui->combo_strat));
  if (strat >= gamandua->problem->n_strategies)
    {
      size_t i;
      for (i=0; i<gamandua->problem->n_strategies; i++)
        {
          apply_job_settings(gamandua->base_job, n_machines, i);
        }
    }
  else
    {
      apply_job_settings(gamandua->base_job, n_machines, strat);
    }
  gtk_widget_set_sensitive(gui->btn_export, 1);
}

int
cb_checkout_job(void *data)
{
  Gui *gui;
  GdkDrawable* prerender;
  struct tdc_job *job=NULL;

  if (!(gui = gamandua->gui)) return FALSE;

  job = tdc_checkout();
  if (!job) return TRUE;

  gamandua->job = job;
  if (!(gui = gamandua->gui)) return FALSE;
  append_drawing(GTK_NOTEBOOK(gui->drawing_notebook));
  g_object_set_data(G_OBJECT(gui->drawing_area), "job", job);

  prerender = GDK_DRAWABLE(job_to_drawable(GDK_DRAWABLE(gui->drawing_area->window), job));
  g_object_set_data(G_OBJECT(gui->drawing_area), "prerender", prerender);
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
      gtk_widget_set_sensitive(gui->btn_export, 0);
      return;
    }

  gui->drawing_area = canvas->drawing_area;
  gui->scrolled = canvas->scrolled;
  gtk_widget_set_sensitive(gui->btn_export, 1);
  gamandua->selection.task = NULL;
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
  int pwidth, pheight;

  GdkDrawable *drawable, *prerender;
  GdkGC *gc;
  GtkRequisition requisition;

  gui = gamandua->gui;
  canvas = get_current_canvas();
  if (!canvas) return;
  job = g_object_get_data(G_OBJECT(canvas->drawing_area), "job");
  if (!gui || !gui->drawing_area || !job) return;
  problem = job->problem;

  if (problem != job->problem) return;

  drawable = GDK_DRAWABLE(gui->drawing_area->window);
  gc = gdk_gc_new(drawable);

  gdk_drawable_get_size(drawable, &width, &height);
  gdk_rgb_gc_set_foreground(gc, 0xffffff);
  gdk_draw_rectangle(drawable, gc, TRUE, 0, 0, width, height);

  prerender = g_object_get_data(G_OBJECT(canvas->drawing_area), "prerender");
  gdk_drawable_get_size(prerender, &pwidth, &pheight);
  gdk_draw_drawable(drawable, gc, prerender, 0, 0, 0, 0, pwidth, pheight);
  gdk_draw_pixbuf(drawable, gc, gui->logopix, 0, 0, width-gui->logopix_w, height-gui->logopix_h, gui->logopix_w, gui->logopix_h, 0, 0, 0);
  
  /* Redimensionnement si le widget est trop petit pour toutes les taches */
  gtk_widget_size_request(gui->drawing_area, &requisition);
  if ((width != pwidth 
       || height != pheight)
      && (pwidth != requisition.width 
          || pheight != requisition.height))
    {
      gtk_drawing_area_size(GTK_DRAWING_AREA(gui->drawing_area), 
                            pwidth, 
                            pheight);
      if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
      return;
    }

  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
  draw_selection(drawable);
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
  GdkDrawable* drawable;
  GdkGC* gc;

  gui = gamandua->gui;
  canvas = get_current_canvas();
  problem = gamandua->problem;
  if (!canvas) return;
  job = g_object_get_data(G_OBJECT(canvas->drawing_area), "job");
  drawable = GDK_DRAWABLE(gui->drawing_area->window);

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
                  gamandua->selection.step = j;
                  gamandua->selection.task = task;
                  if (task->n_steps > 1)
                    gamandua->selection.color = id_colors[k%2]+COLOR_STEP*task->id;
                  else
                    gamandua->selection.color = colors[k%2 
                     + (task->steps[j].machine == gamandua->max_length ? 2:0)];

						cb_drawing_area_exposed(NULL, NULL);
                  return;
                }
              k++;
            } 
        }
    }
  if (gamandua->selection.task && i == job->n_tasks)
    {
		gamandua->selection.task = NULL;
      cb_drawing_area_exposed(NULL, NULL);
    }
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
    }
  else
    {
      tdc_delete_job(g_object_get_data(G_OBJECT(canvas->drawing_area), "job"));
		gdk_drawable_unref(g_object_get_data(G_OBJECT(canvas->drawing_area), "prerender"));
      gtk_notebook_remove_page(notebook, p);
      gui->drawings = g_slist_remove(gui->drawings, canvas);
      free(canvas);
    }

  if (!gui->drawings) 
    {
      gtk_widget_set_sensitive(gui->btn_clear, 0);
      gtk_widget_set_sensitive(gui->btn_export, 0);
      gamandua->n_results = 0;
      switch_to_logo(1);
    }
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
  gamandua->n_results = 0;
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
  gtk_widget_set_sensitive(gui->btn_clear, 0);
  gtk_widget_set_sensitive(gui->btn_export, 0);
  switch_to_logo(1);
}

void
cb_btn_export_clicked(GtkButton *button, void *data)
{
  Gui *gui;
  Canvas *canvas;
  int width, height;
  GdkPixbuf *pixbuf;
  GdkDrawable *prerender, *drawable;
  GdkGC *gc;
  GError *error = NULL;
  char *filename;
  GtkWidget *dialog;


  gui = gamandua->gui;
  if (!gui) return;
  canvas = get_current_canvas();
  if (!canvas) return;

  dialog = gtk_file_chooser_dialog_new ("Save File",
                                        GTK_WINDOW(gui->window),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                        NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

      filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		prerender = g_object_get_data(G_OBJECT(canvas->drawing_area), "prerender");
      gdk_drawable_get_size(prerender, &width, &height);
		drawable = gdk_pixmap_new(prerender, width, height, -1);
		gc = gdk_gc_new(drawable);
		gdk_draw_drawable(drawable, gc, prerender, 0, 0, 0, 0, width, height);
		gdk_draw_pixbuf(drawable, gc, gui->logopix, 0, 0, width-gui->logopix_w, height-gui->logopix_h, gui->logopix_w, gui->logopix_h, 0, 0, 0);
		if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
      pixbuf = gdk_pixbuf_get_from_drawable(NULL, drawable, NULL, 0, 0, 0, 0, width, height);
      gdk_pixbuf_save(pixbuf, filename, "png", &error, NULL);
		gdk_drawable_unref(drawable);
		gdk_pixbuf_unref(pixbuf);
      free(filename);
    }

  gtk_widget_destroy(dialog);
}

void
cb_keybindings(GtkWidget *widget, GdkEventKey *ev, void *data)
{
  Gui *gui;
  gui = gamandua->gui;
  if (!gui) return;

  if (!strcmp("p", ev->string))
    {
      gtk_combo_box_popup(GTK_COMBO_BOX(gui->combo_prob));
    }
  else if (!strcmp("s", ev->string) && GTK_WIDGET_SENSITIVE(gui->combo_strat))
    {
      gtk_combo_box_popup(GTK_COMBO_BOX(gui->combo_strat));
    }
  else if (!strcmp("c", ev->string))
    {
      gtk_button_clicked(GTK_BUTTON(gui->btn_calc));
    }
  else if (!strcmp("e", ev->string) && GTK_WIDGET_SENSITIVE(gui->btn_export))
    {
      gtk_button_clicked(GTK_BUTTON(gui->btn_export));
    }
  else if (!strcmp("n", ev->string) && GTK_WIDGET_SENSITIVE(gui->btn_clear))
    {
      gtk_button_clicked(GTK_BUTTON(gui->btn_clear));
    }
  else if (!strcmp("a", ev->string) && GTK_WIDGET_SENSITIVE(gui->btn_help))
    {
      gtk_button_clicked(GTK_BUTTON(gui->btn_help));
    }
  else if (!strcmp("q", ev->string))
    {
      gtk_button_clicked(GTK_BUTTON(gui->btn_quit));
    }
}
