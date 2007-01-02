#include "gamandua.h"
#include "gamandua_callbacks.h"

/* Private Declarations */
static GtkWidget *init_window        (void);
static GtkWidget *init_buttonbox     (void);
static GtkWidget *init_label         (const char *str);
static GtkWidget *init_pop_btn       (int type);
static GtkWidget *init_drawing_area  (Gui *gui);
static GtkWidget *init_notebook      (void);
static void       init_packing       (Gui *gui);

int
main(int argc, char **argv)
{
  Gui *gui;

  gamandua = tdb_calloc(1, sizeof(Gamandua));

  /* Tamandua Init */
  tdb_init(stderr, &argc, argv);
  if (argc > 1)
    {
      tdc_setcontext(tdc_init(argv[1], 0, 1));
    }
  else
    {
      tdc_setcontext(tdc_init("/usr/local/share/tamandua/problems", 0, 1));
    }

  /* Gtk Init */
  gtk_init(&argc, &argv);
  
  gui = tdb_calloc(1, sizeof(Gui));
  gamandua->gui = gui;
  gui->window = init_window();
  gui->buttonbox = init_buttonbox();
  gui->combo_prob = init_combo_prob();
  gui->combo_strat = init_combo_strat();
  gui->spin_machine = init_spin_machine();
  gui->notebook = init_notebook();
  gui->drawing_area = init_drawing_area(gui);
  init_packing(gui);

  gtk_main();
  return EXIT_SUCCESS;
}

GtkWidget *
init_vbox(void)
{
  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  return vbox;
}

GtkWidget *
init_hbox(void)
{
  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  return hbox;
}

GtkWidget *
init_label(const char *str)
{
  GtkWidget *label;
  label = gtk_label_new(str);
  gtk_widget_show(label);
  return label;
}

GtkWidget *
init_window(void)
{
  GtkWindow *window;

  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_title(window, "Tamandua");
  gtk_window_set_wmclass(window, "tamandua", "tamandua");
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);
  gtk_window_resize(window, 800, 600);
  gtk_widget_show(GTK_WIDGET(window));
  gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(cb_window_destroy), NULL);

  return GTK_WIDGET(window);
}

GtkWidget *
init_buttonbox(void)
{
  Gui *gui;
  GtkWidget *box;
  GtkWidget *hbox;
  GtkWidget *icon;
  GtkWidget *btn_exec;
  GtkWidget *btn_calc;
  GtkWidget *btn_quit;

  gui = gamandua->gui;
  if (!gui) return NULL;

  box = gtk_hbutton_box_new();
  btn_exec = gtk_button_new();
  btn_calc = gtk_button_new_from_stock(GTK_STOCK_EXECUTE);

  hbox = init_hbox();
  gtk_container_add(GTK_CONTAINER(btn_exec), hbox);

  icon = gtk_image_new_from_stock(GTK_STOCK_CONVERT, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(hbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), init_label("Relancer"), TRUE, TRUE, 0);
  btn_quit = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  gtk_widget_set_sensitive(btn_exec, 0);
  gui->btn_exec = btn_exec;
  gtk_widget_show(btn_exec);
  gtk_widget_show(btn_calc);
  gtk_widget_show(btn_quit);
  gtk_box_pack_start(GTK_BOX(box), btn_quit, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_calc, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_exec, TRUE, TRUE, 0);

  gtk_signal_connect(GTK_OBJECT(btn_quit), "clicked", GTK_SIGNAL_FUNC(cb_btn_quit_clicked), NULL);
  gtk_signal_connect(GTK_OBJECT(btn_calc), "clicked", GTK_SIGNAL_FUNC(cb_btn_calc_clicked), NULL);
  gtk_signal_connect(GTK_OBJECT(btn_exec), "clicked", GTK_SIGNAL_FUNC(cb_btn_exec_clicked), NULL);

  gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_SPREAD);
  gtk_widget_show(box);
  return box;
}

GtkWidget *
init_combo_prob(void)
{
  GtkComboBox *combo;
  struct tdc_problem *problem;
  size_t i;

  combo = GTK_COMBO_BOX(gtk_combo_box_new_text());
  for (i=0; i<tdc_get_n_problems(); i++)
    {
      problem = tdc_get_problem(i);
      gtk_combo_box_append_text(combo, problem->name);
    }
  gtk_signal_connect(GTK_OBJECT(combo), "changed", 
                     GTK_SIGNAL_FUNC(cb_combo_changed), gamandua);
  gtk_signal_connect(GTK_OBJECT(combo), "changed", 
                     GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  gtk_widget_show(GTK_WIDGET(combo));

  return GTK_WIDGET(combo);
}

GtkWidget *
init_combo_strat(void)
{
  Gui *gui;
  GtkComboBox *combo;
  struct tdc_problem *problem;
  size_t i;

  gui = gamandua->gui;
  problem = gamandua->problem;
  if (!gui || !problem) return NULL;

  if (gui->combo_strat) 
    combo = GTK_COMBO_BOX(gui->combo_strat);
  else 
    combo = GTK_COMBO_BOX(gtk_combo_box_new_text());

  while (gtk_combo_box_get_active_text(combo))
    {
      gtk_combo_box_remove_text(combo, gtk_combo_box_get_active(combo));
      gtk_combo_box_set_active(combo, 0);
    }

  for (i=0; i < problem->n_strategies; i++)
    {
      gtk_combo_box_append_text(combo, problem->strategies[i]);
    }

  gtk_combo_box_set_active(combo, 0);
  gtk_widget_show(GTK_WIDGET(combo));
  return GTK_WIDGET(combo);
}

GtkWidget *
init_pop_btn(int type)
{
  GtkWidget *button;

  if (type == 1)
    {
      button = gtk_button_new_from_stock(GTK_STOCK_ADD);
    }
  else
    {
      button = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
    }

  gtk_widget_show(button);
  return button;
}

GtkWidget *
init_notebook(void)
{
  GtkWidget *notebook;

  notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
  gtk_widget_show(notebook);
  return notebook;
}

void
remove_page(GtkWidget *notebook)
{
  Gui    *gui;
  GSList *list;
  Page   *page;
  size_t n_pages;

  gui = gamandua->gui;
  if (!gui || !notebook) return;
  n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
  list = g_slist_last(gui->pages);
  page = list->data;
  gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), n_pages-1);
  //gtk_widget_destroy(page->root);
  free(page->steps_min);
  free(page->steps_max);
  free(page);

  gui->pages = g_slist_delete_link(gui->pages, list);
}

void
append_page(GtkWidget *notebook)
{
  Gui *gui;
  Page *page;
  struct tdc_problem *problem;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *root;
  GtkWidget *frame;
  GtkWidget *label;
  GtkWidget *align;
  size_t n_pages;
  size_t i;
  char *str;

  gui = gamandua->gui;
  problem = gamandua->problem;
  if (!gui||!problem) return;
  n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

  page = calloc(1, sizeof(Page));
  gui->pages = g_slist_append(gui->pages, page);
  page->root = init_vbox();

  /* task */
  frame = gtk_frame_new("Nombre de tâches");
  gtk_box_pack_start(GTK_BOX(page->root), frame, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  label = gtk_frame_get_label_widget(GTK_FRAME(frame));
  gtk_label_set_markup(GTK_LABEL(label), "<b>Nombre de tâches</b>");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);

  page->slider_task = gtk_hscale_new_with_range(1, 100, 1);
  gtk_range_set_value(GTK_RANGE(page->slider_task), 7);
  gtk_container_add(GTK_CONTAINER(frame), page->slider_task);
  gtk_signal_connect(GTK_OBJECT(page->slider_task), "value_changed", 
                     GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);

  if (problem->tasks.weighted)
    {
      /* Weight */
      frame = gtk_frame_new("Poids");
      gtk_box_pack_start(GTK_BOX(page->root), frame, FALSE, FALSE, 0);
      gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
      label = gtk_frame_get_label_widget(GTK_FRAME(frame));
      gtk_label_set_markup(GTK_LABEL(label), "<b>Poids</b>");
      gtk_container_set_border_width(GTK_CONTAINER(frame), 10);

      hbox = init_hbox();
      gtk_container_add(GTK_CONTAINER(frame), hbox);
      page->weight_min = gtk_spin_button_new_with_range(1, UINT_MAX, 1);
      page->weight_max = gtk_spin_button_new_with_range(1, UINT_MAX, 1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->weight_min),  1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->weight_max), 42);
      gtk_box_pack_start(GTK_BOX(hbox), init_label("Entre "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->weight_min, FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), init_label(" et "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->weight_max, FALSE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(page->weight_min), "value_changed", 
                         GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);
      gtk_signal_connect(GTK_OBJECT(page->weight_max), "value_changed", 
                         GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);
    }

  /* Length */
  frame = gtk_frame_new("Durées");
  gtk_box_pack_start(GTK_BOX(page->root), frame, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  label = gtk_frame_get_label_widget(GTK_FRAME(frame));
  gtk_label_set_markup(GTK_LABEL(label), "<b>Durées</b>");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(frame), vbox);

  page->steps_min = calloc(problem->tasks.steps, sizeof(GtkWidget *));
  page->steps_max = calloc(problem->tasks.steps, sizeof(GtkWidget *));
  for (i=0; i<problem->tasks.steps; i++)
    {
      hbox = init_hbox();
      gtk_container_add(GTK_CONTAINER(vbox), hbox);
      page->steps_min[i] = gtk_spin_button_new_with_range(1, UINT_MAX, 1);
      page->steps_max[i] = gtk_spin_button_new_with_range(1, UINT_MAX, 1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->steps_min[i]),  1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->steps_max[i]), 42);
      gtk_box_pack_start(GTK_BOX(hbox), init_label("Entre "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->steps_min[i], FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), init_label(" et "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->steps_max[i], FALSE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(page->steps_min[i]), "value_changed", 
                         GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);
      gtk_signal_connect(GTK_OBJECT(page->steps_max[i]), "value_changed", 
                         GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);
    }

  gtk_widget_show_all(page->root);

  asprintf(&str, "Pop %d", n_pages+1);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page->root, init_label(str));
  free(str);
}

GtkWidget *
init_drawing_area(Gui *gui)
{
  if (!gui) return NULL;

  gui->scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gui->scrolled),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_container_set_border_width(GTK_CONTAINER(gui->scrolled), 10);
  gui->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(gui->drawing_area), 100, 100);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(gui->scrolled), 
                                        gui->drawing_area);
  gtk_signal_connect(GTK_OBJECT(gui->drawing_area), "expose-event", 
                     GTK_SIGNAL_FUNC(cb_drawing_area_exposed), NULL);
  gtk_signal_connect(GTK_OBJECT(gui->scrolled), "button-press-event", 
                     GTK_SIGNAL_FUNC(cb_drawing_area_mouse_down), NULL);
  return gui->drawing_area;
}

GtkWidget *
init_spin_machine(void)
{
  Gui *gui;
  struct tdc_problem *problem;
  GtkWidget *spin;

  gui = gamandua->gui;
  problem = gamandua->problem;
  if (!gui || !problem) return NULL;

  if (!gui->spin_machine)
    {
      spin = gtk_spin_button_new_with_range(1, 100, 1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), 1);
    }
  else
    {
      spin = gui->spin_machine;
    }
  if (!tdc_problem_n_machines_settable(problem))
    {
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), tdc_problem_get_n_machines(problem));
      gtk_widget_set_sensitive(spin, 0);
    }
  else
    {
      gtk_widget_set_sensitive(spin, 1);
    }

  return spin;
}

static void
init_packing(Gui *gui)
{
  GtkWidget *vbox;
  GtkWidget *hbox;
  
  vbox = init_vbox();
  hbox = init_hbox();
  gtk_container_add(GTK_CONTAINER(gui->window), vbox);
  gtk_box_pack_end(GTK_BOX(vbox), gui->buttonbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), init_label("Problèmes :"), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), gui->combo_prob, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), init_label("Stratégies :"), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), gui->combo_strat, FALSE, FALSE, 0);

  gtk_box_pack_end(GTK_BOX(hbox), gui->spin_machine, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), init_label("   Machines :"), FALSE, FALSE, 0);

  gui->spin_pop = gtk_spin_button_new_with_range(1, 100, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_pop), 1);
  gtk_box_pack_end(GTK_BOX(hbox), gui->spin_pop, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), init_label("Populations :"), FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(gui->spin_pop), "value_changed", 
                     GTK_SIGNAL_FUNC(cb_spin_pop_changed), gui->notebook);
  gtk_signal_connect(GTK_OBJECT(gui->spin_pop), "value_changed", 
                     GTK_SIGNAL_FUNC(cb_unsensitive_btn_exec), NULL);

  hbox = init_hbox();
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(hbox), gui->scrolled, TRUE, TRUE, 0);
  vbox = init_vbox();
  gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(vbox), gui->notebook, TRUE, TRUE, 0);
  append_page(gui->notebook);

  GtkWidget *label;
  gui->frame_info = gtk_frame_new("Informations");
  gtk_frame_set_shadow_type(GTK_FRAME(gui->frame_info), GTK_SHADOW_ETCHED_IN);
  label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), "<i>Cliquez sur une tâche\n pour plus d'informations</i>");
  gtk_container_add(GTK_CONTAINER(gui->frame_info), label);
  gtk_box_pack_start(GTK_BOX(vbox), gui->frame_info, FALSE, FALSE, 10);

  gtk_widget_show_all(gui->window);
}

void
apply_pop_settings(struct tdc_generator *generator)
{
  Gui *gui;
  Page *page;
  GSList *list;
  size_t n_pop;
  size_t i;
  size_t j;
  struct tdc_population_generator *pop;

  gui = gamandua->gui;
  if (!gui) return;

  n_pop = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_pop));
  if (n_pop != generator->n_populations) return;
  
  list = gui->pages;
  for (i=0; i<n_pop, list; i++)
    {
      page = list->data;
      pop = &generator->populations[i];
      pop->n_tasks = (size_t)gtk_range_get_value(GTK_RANGE(page->slider_task));
      if (tdc_problem_weight_enabled(gamandua->problem))
        {
          pop->weight.min = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->weight_min));
          pop->weight.max = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->weight_max));
        }

      for (j=0; j<generator->problem->tasks.steps; j++)
        {
          pop->lengths[j].min = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->steps_min[j]));
          pop->lengths[j].max = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->steps_max[j]));
        }
      list = list->next;
    }
}

void
draw_task(int x, int y, int width, int height, int color)
{
  Gui *gui;
  GdkDrawable *drawable;
  GdkGC *gc;
  GdkPixbuf *pix;

  gui = gamandua->gui;
  if (!gui || !gui->drawing_area) return;
  drawable = GDK_DRAWABLE(gui->drawing_area->window);
  gc = gdk_gc_new(drawable);

  gdk_rgb_gc_set_foreground(gc, color);
  gdk_draw_rectangle(drawable, gc, TRUE, x, y, width, height);
  pix = gdk_pixbuf_new_from_file_at_scale("grad.png", width, height, 
                                          FALSE, NULL);
  gdk_draw_pixbuf(drawable, gc, pix, 0, 0, x, y, width, height, 0, 0, 0);

  if (GDK_IS_PIXBUF(pix)) gdk_pixbuf_unref(pix);
  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
}

void
draw_task_txt_size(int size, int x, int y, int color)
{
  Gui *gui;
  GdkDrawable *drawable;
  GdkGC *gc;
  PangoLayout *layout;
  PangoContext *context;
  char *str;

  gui = gamandua->gui;
  if (!gui || !gui->drawing_area) return;
  drawable = GDK_DRAWABLE(gui->drawing_area->window);
  gc = gdk_gc_new(drawable);

  context = gtk_widget_get_pango_context(gui->drawing_area);
  layout  = pango_layout_new(context);

  asprintf(&str, "<i>%d</i>", size);
  pango_layout_set_markup(layout, str, -1);
  gdk_draw_layout(drawable, gc, x, y, layout);
  free(str);

  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
}

void
draw_selection(void)
{
  struct tdc_task *task;
  size_t step;

  task = gamandua->selection.task;
  if (!task) return;
  
  step = gamandua->selection.step;
  draw_task(task->steps[step].start_time, task->steps[step].machine*50+10, 
            task->steps[step].length, 40, 
            0xfce94f);
}
