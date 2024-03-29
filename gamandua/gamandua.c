#include "gamandua.h"
#include "gamandua_callbacks.h"
#include "gamandua_help.h"

/* Private Declarations */
static GtkWidget *init_window        (void);
static GtkWidget *init_buttonbox     (void);
static GtkWidget *init_buttonbox2    (void);
static GtkWidget *init_label         (const char *str);
static GtkWidget *init_pop_btn       (int type);
static GtkWidget *init_drawing_area  (Gui *gui);
static GtkWidget *init_notebook      (void);
static GtkWidget *init_notebook_label(const char *str);
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
      if(argv[1][0]=='-')
        {
          printf("usage: gamandua [problems_path\n");
          return EXIT_SUCCESS;
        }
      else
        {
          tdc_setcontext(tdc_init(argv[1], 0, 1));
        }
    }
  else
    {
      tdc_setcontext(tdc_init("/usr/share/tamandua/problems", 0, 1));
    }

  /* Gtk Init */
  gdk_init(&argc, &argv);
  gtk_init(&argc, &argv);
  
  gui = tdb_calloc(1, sizeof(Gui));
  gamandua->gui = gui;
  gui->window = init_window();
  gui->buttonbox = init_buttonbox();
  gui->buttonbox2 = init_buttonbox2();
  gui->combo_prob = init_combo_prob();
  gui->combo_strat = init_combo_strat();
  gui->spin_machine = init_spin_machine();
  gui->notebook = init_notebook();
  gui->drawing_area = init_drawing_area(gui);
  gui->gradpix = gdk_pixbuf_new_from_file(PACKAGE_DATA_DIR"grad.png", NULL);
  gui->logopix = gdk_pixbuf_new_from_file(PACKAGE_DATA_DIR"gamandua_shaded.png", NULL);
  gui->logopix_w = gdk_pixbuf_get_width(gui->logopix);
  gui->logopix_h = gdk_pixbuf_get_height(gui->logopix);
  gui->tooltippix = gdk_pixbuf_new_from_file(PACKAGE_DATA_DIR"tooltip.png", NULL);
  gui->tooltippix_w = gdk_pixbuf_get_width(gui->tooltippix);
  gui->tooltippix_h = gdk_pixbuf_get_height(gui->tooltippix);
  init_packing(gui);
  switch_to_logo(1);
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
  gtk_container_set_border_width(GTK_CONTAINER(window), 2);
  gtk_window_resize(window, 700, 400);
  gtk_widget_show(GTK_WIDGET(window));
  gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(cb_window_destroy), NULL);
  gtk_signal_connect_after(GTK_OBJECT(window), "key-press-event", 
                           GTK_SIGNAL_FUNC(cb_keybindings), NULL);
  return GTK_WIDGET(window);
}

GtkWidget *
init_buttonbox(void)
{
  Gui *gui;
  GtkWidget *box;
  GtkWidget *vbox;
  GtkWidget *icon;
  GtkWidget *btn_calc;
  GtkWidget *btn_export;

  gui = gamandua->gui;
  if (!gui) return NULL;
  box = gtk_hbox_new(TRUE, 5);

  btn_calc = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btn_calc), GTK_RELIEF_NONE);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(btn_calc), vbox);
  icon = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), init_label("Calculer"), TRUE, TRUE, 0);
  gui->btn_calc = btn_calc;

  btn_export = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btn_export), GTK_RELIEF_NONE);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(btn_export), vbox);
  icon = gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), init_label("Enregistrer"), TRUE, TRUE, 0);

  gtk_widget_show(btn_calc);
  gtk_widget_show(btn_export);
  gui->btn_export = btn_export;
  gtk_widget_set_sensitive(btn_export, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_calc, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_export, TRUE, TRUE, 0);

  gtk_signal_connect(GTK_OBJECT(btn_calc), "clicked", GTK_SIGNAL_FUNC(cb_btn_calc_clicked), NULL);
  gtk_signal_connect(GTK_OBJECT(btn_export), "clicked", GTK_SIGNAL_FUNC(cb_btn_export_clicked), NULL);

  gtk_widget_show(box);
  return box;
}

GtkWidget *
init_buttonbox2(void)
{
  Gui *gui;
  GtkWidget *box;
  GtkWidget *vbox;
  GtkWidget *icon;
  GtkWidget *btn_clear;
  GtkWidget *btn_help;
  GtkWidget *btn_quit;

  gui = gamandua->gui;
  if (!gui) return NULL;
  box = gtk_hbox_new(TRUE, 5);

  btn_clear = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btn_clear), GTK_RELIEF_NONE);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(btn_clear), vbox);
  icon = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), init_label("Nettoyer"), TRUE, TRUE, 0);

  btn_help = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btn_help), GTK_RELIEF_NONE);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(btn_help), vbox);
  icon = gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), init_label("Aide"), TRUE, TRUE, 0);

  btn_quit = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btn_quit), GTK_RELIEF_NONE);
  vbox = init_vbox();
  gtk_container_add(GTK_CONTAINER(btn_quit), vbox);
  icon = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(icon);
  gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), init_label("Quitter"), TRUE, TRUE, 0);

  gtk_widget_show(btn_clear);
  gtk_widget_show(btn_help);
  gtk_widget_show(btn_quit);
  gui->btn_clear = btn_clear;
  gui->btn_help = btn_help;
  gui->btn_quit = btn_quit;
  gtk_widget_set_sensitive(btn_clear, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_clear, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_help, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), btn_quit, TRUE, TRUE, 0);

  gtk_signal_connect(GTK_OBJECT(btn_clear), "clicked", GTK_SIGNAL_FUNC(cb_btn_clear_clicked), NULL);
  gtk_signal_connect(GTK_OBJECT(btn_help), "clicked", GTK_SIGNAL_FUNC(cb_btn_help_clicked), NULL);
  gtk_signal_connect(GTK_OBJECT(btn_quit), "clicked", GTK_SIGNAL_FUNC(cb_btn_quit_clicked), NULL);

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
  
  if (problem->n_strategies <= 1) 
    {
      gtk_widget_set_sensitive(GTK_WIDGET(combo), 0);
    }
  else 
    {
      gtk_combo_box_append_text(combo, "Toutes");
      gtk_widget_set_sensitive(GTK_WIDGET(combo), 1);
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
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), 1);
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

  page = tdb_calloc(1, sizeof(Page));
  gui->pages = g_slist_append(gui->pages, page);
  page->root = init_vbox();

  /* task */
  frame = gtk_frame_new("Nombre");
  gtk_box_pack_start(GTK_BOX(page->root), frame, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  label = gtk_frame_get_label_widget(GTK_FRAME(frame));
  gtk_label_set_markup(GTK_LABEL(label), "<b>Nombre de tâches</b>");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);

  page->slider_task = gtk_hscale_new_with_range(1, 100, 1);
  gtk_range_set_value(GTK_RANGE(page->slider_task), 7);
  gtk_container_add(GTK_CONTAINER(frame), page->slider_task);

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
      gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
      page->weight_min = gtk_spin_button_new_with_range(1, 999, 1);
      page->weight_max = gtk_spin_button_new_with_range(1, 999, 1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->weight_min),  1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->weight_max), 42);
      gtk_box_pack_start(GTK_BOX(hbox), init_label("Entre "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->weight_min, FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), init_label(" et "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->weight_max, FALSE, FALSE, 0);
    }

  /* Length */
  frame = gtk_frame_new("Durées");
  gtk_box_pack_start(GTK_BOX(page->root), frame, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
  label = gtk_frame_get_label_widget(GTK_FRAME(frame));
  gtk_label_set_markup(GTK_LABEL(label), "<b>Durées</b>");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  vbox = gtk_vbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(frame), vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

  page->steps_min = tdb_calloc(problem->tasks.steps, sizeof(GtkWidget *));
  page->steps_max = tdb_calloc(problem->tasks.steps, sizeof(GtkWidget *));
  for (i=0; i<problem->tasks.steps; i++)
    {
      hbox = init_hbox();
      gtk_container_add(GTK_CONTAINER(vbox), hbox);
      page->steps_min[i] = gtk_spin_button_new_with_range(1, 999, 1);
      page->steps_max[i] = gtk_spin_button_new_with_range(1, 999, 1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->steps_min[i]),  1);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(page->steps_max[i]), 42);
      gtk_box_pack_start(GTK_BOX(hbox), init_label("Entre "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->steps_min[i], FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), init_label(" et "), FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), page->steps_max[i], FALSE, FALSE, 0);
    }

  gtk_widget_show_all(page->root);

  asprintf(&str, "Pop %d", n_pages+1);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page->root, init_label(str));
  free(str);
}

void
append_help(GtkNotebook *notebook)
{
  GtkWidget *align;
  GtkWidget *hbox;
  GtkWidget *btn_close;
  GtkWidget *tbox;
  GtkWidget *label;
  GtkWidget *scrolled;

  if (!notebook) return;

  scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_container_set_border_width(GTK_CONTAINER(scrolled), 10);
  tbox = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW(tbox), FALSE);
  gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW(tbox), 4);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(tbox), 4);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tbox), GTK_WRAP_WORD);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tbox)), HELP_MESSAGE, -1);
  align = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), tbox);
  gtk_container_add(GTK_CONTAINER(align), scrolled);
  gtk_widget_show_all(align);

  hbox = init_hbox();
  label = init_label("Aide");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
  gtk_notebook_append_page(notebook, align, NULL);
  btn_close = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(btn_close), 
                       gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU));
  gtk_button_set_relief(GTK_BUTTON(btn_close), GTK_RELIEF_NONE);
  gtk_signal_connect(GTK_OBJECT(btn_close), "clicked",
                     GTK_SIGNAL_FUNC(cb_drawing_page_close), align);

  gtk_box_pack_start(GTK_BOX(hbox), label, 1, 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), btn_close, 0, 0, 0);
  gtk_widget_show_all(hbox);

  gtk_notebook_set_tab_label(notebook, align, hbox);
  gtk_widget_show_all(GTK_WIDGET(notebook));
  gtk_widget_set_sensitive(gamandua->gui->btn_help, 0);
  gamandua->gui->drawings = g_slist_append(gamandua->gui->drawings, NULL);
  gtk_notebook_set_current_page(notebook, gtk_notebook_get_n_pages(notebook)-1);
  gtk_widget_set_sensitive(gamandua->gui->btn_clear, 1);
  switch_to_logo(0);
}

void
append_drawing(GtkNotebook *notebook)
{
  GtkWidget *drawing_area;
  GtkWidget *scrolled;
  Canvas *canvas;
  char *str;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *btn_close;

  scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_container_set_border_width(GTK_CONTAINER(scrolled), 10);
  drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), 100, 100);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), 
                                        drawing_area);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "expose-event", 
                     GTK_SIGNAL_FUNC(cb_drawing_area_exposed), NULL);
  gtk_signal_connect(GTK_OBJECT(scrolled), "button-press-event", 
                     GTK_SIGNAL_FUNC(cb_drawing_area_mouse_down), NULL);

  gtk_notebook_append_page(notebook, scrolled, NULL);

  hbox = init_hbox();
  
  if (!gamandua->problem) return;
  asprintf(&str, "%s (%d)", gamandua->problem->name, ++gamandua->n_results);
  label = gtk_label_new(str);
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
  free(str);

  btn_close = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(btn_close), 
                       gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU));
  gtk_button_set_relief(GTK_BUTTON(btn_close), GTK_RELIEF_NONE);
  gtk_signal_connect(GTK_OBJECT(btn_close), "clicked",
                     GTK_SIGNAL_FUNC(cb_drawing_page_close), scrolled);

  gtk_box_pack_start(GTK_BOX(hbox), label, 1, 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), btn_close, 0, 0, 0);
  gtk_widget_show_all(hbox);

  gtk_notebook_set_tab_label(notebook, scrolled, hbox);
  gtk_widget_show_all(GTK_WIDGET(notebook));

  canvas = tdb_malloc(sizeof(Canvas));
  canvas->scrolled = scrolled;
  canvas->drawing_area = drawing_area;
  gamandua->gui->drawings = g_slist_append(gamandua->gui->drawings, canvas);

  gtk_notebook_set_current_page(notebook, gtk_notebook_get_n_pages(notebook)-1);
  gtk_widget_set_sensitive(gamandua->gui->btn_clear, 1);
  switch_to_logo(0);
}

GtkWidget *
init_drawing_area(Gui *gui)
{
  Canvas *canvas;
  if (!gui) return NULL;

  gui->drawing_notebook = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(gui->drawing_notebook), 1);
  gtk_signal_connect(GTK_OBJECT(gui->drawing_notebook), "switch-page", 
                     GTK_SIGNAL_FUNC(cb_drawing_notebook_change_current), NULL);

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
  GtkWidget *frame;
  GtkWidget *toolbar;
  GtkWidget *table;
  GtkWidget *align;
  GtkWidget *label;
  
  vbox = init_vbox();
  hbox = init_hbox();
  gtk_container_add(GTK_CONTAINER(gui->window), vbox);

  toolbar = gtk_toolbar_new();
  gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);
  gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

  table = gtk_table_new(2, 3, FALSE);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_table_attach_defaults(GTK_TABLE(table), gui->combo_prob, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), gui->combo_strat, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), gui->spin_machine, 2, 3, 0, 1);
  gtk_table_attach(GTK_TABLE(table), init_label("Problème"), 0, 1, 1, 2, 0, 0, 0, 5);
  gtk_table_attach(GTK_TABLE(table), init_label("Stratégie"), 1, 2, 1, 2, 0, 0, 0, 5);
  gtk_table_attach(GTK_TABLE(table), init_label("Machines"), 2, 3, 1, 2, 0, 0, 0, 5);
  gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), table, NULL, NULL);

  gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
  gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), gui->buttonbox, NULL, NULL);
  gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
  gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), gui->buttonbox2, NULL, NULL);

  hbox = init_hbox();
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

  vbox = init_vbox();
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), gui->drawing_notebook, TRUE, TRUE, 10);
  gui->hbox = hbox;

  align = gtk_alignment_new(0.5, 0, 0, 0);
  gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 0);
  hbox = init_hbox();
  gtk_container_add(GTK_CONTAINER(align), hbox);
  gtk_box_pack_start(GTK_BOX(hbox), init_label("Populations de tâches :"), FALSE, FALSE, 0);
  gui->spin_pop = gtk_spin_button_new_with_range(1, 100, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_pop), 1);
  gtk_box_pack_start(GTK_BOX(hbox), gui->spin_pop, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(gui->spin_pop), "value_changed", 
                     GTK_SIGNAL_FUNC(cb_spin_pop_changed), gui->notebook);

  gtk_box_pack_start(GTK_BOX(vbox), gui->notebook, TRUE, TRUE, 10);
  append_page(gui->notebook);
  gtk_widget_show_all(gui->window);
}

void
switch_to_logo(int val)
{
  Gui *gui;
  GtkWidget *image;

  gui = gamandua->gui;
  if (!gui || !gui->hbox) return;

  if (!(image = gui->logo))
    {
      image = gtk_image_new_from_file(PACKAGE_DATA_DIR"gamandua_logo.png");
      gtk_box_pack_start(GTK_BOX(gui->hbox), image, TRUE, TRUE, 20);
      gui->logo = image;
    }
  
  if (val)
    {
      gtk_widget_hide(gui->drawing_notebook);
      gtk_widget_show(gui->logo);
    }
  else
    {
      gtk_widget_show(gui->drawing_notebook);
      gtk_widget_hide(gui->logo);
    }
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
  size_t min, max;

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
          min = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->weight_min));
          max = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->weight_max));
          pop->weight.min = min < max ? min : max;
          pop->weight.max = min > max ? min : max;
        }

      for (j=0; j<generator->problem->tasks.steps; j++)
        {
          min = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->steps_min[j]));
          max = (size_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(page->steps_max[j]));
          pop->lengths[j].min = min < max ? min : max;
          pop->lengths[j].max = min > max ? min : max;
        }
      list = list->next;
    }
}

void
apply_job_settings(struct tdc_job *job, size_t n_machines, size_t strat)
{
  job = tdc_copy_job(gamandua->base_job);
  if (tdc_problem_n_machines_settable(gamandua->problem))
    job->n_machines = n_machines;
  job->strategy = strat;
  tdc_commit(job);
  g_timeout_add(50, cb_checkout_job, NULL);
}

GdkDrawable* job_to_drawable(GdkDrawable* win, struct tdc_job* job)
{
  GdkGC *gc;
  GdkDrawable *drawable;
  const struct tdc_problem* problem = job->problem;
  size_t width = (job->timespan+140)<300?300:(job->timespan+140);
  size_t height = 50*job->n_machines+70;
  size_t i, j;
  Gui* gui = gamandua->gui;

  drawable = GDK_DRAWABLE(gdk_pixmap_new(win, width, height, -1));
  gc = gdk_gc_new(drawable);

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
  if (G_IS_OBJECT(layout)) g_object_unref(layout);

  struct tdc_task *task;
  size_t max_length = 0;
  double *machines;
  size_t n_steps = tdc_problem_get_n_steps(problem);
  machines = tdb_calloc(job->n_machines, sizeof(*machines));

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
  draw = tdb_calloc(job->n_machines, sizeof(size_t));
  for (i=0; i<job->n_tasks; i++)
    {
      size_t col;
      int color;
      
      task = job->tasks[i];
      for (j=0; j<n_steps; j++)
        {
          if (task->n_steps > 1) {
            color = id_colors[draw[task->steps[j].machine]%2]+COLOR_STEP*task->id;
			}
          else {
            color = colors[draw[task->steps[j].machine]%2 
             + (task->steps[j].machine == max_length ? 2:0)];
			}

          draw_task(drawable, task->steps[j].start_time, task->steps[j].machine*50+40, 
                    task->steps[j].length, 40, 
                    color);
			/* dessin du 'wrapper' */
			if (j > 0) {
				GdkSegment segment ={
					4+task->steps[j-1].start_time+task->steps[j-1].length,
					task->steps[j-1].machine*50+80,
					5+task->steps[j].start_time,
					task->steps[j].machine*50+39
				};
				gdk_rgb_gc_set_foreground(gc, color);
				gdk_draw_arc(drawable, gc, FALSE, segment.x1, segment.y1-5, segment.x2-segment.x1, 9, 180*64, 90*64);
				gdk_draw_arc(drawable, gc, FALSE, segment.x1, segment.y1+5, segment.x2-segment.x1, 9, 0*64, 90*64);
			}
          draw[task->steps[j].machine]++;

          if (task->steps[j].start_time + task->steps[j].length == machines[task->steps[j].machine])
            draw_task_txt_size(drawable, machines[task->steps[j].machine],
                               task->steps[j].start_time + task->steps[j].length + 10,
                               task->steps[j].machine*50+50,
                               0x000000);
        }
    }
  free(draw);
  free(machines);
  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
  return drawable;
}

void
draw_task(GdkDrawable *drawable, int x, int y, int width, int height, int color)
{
  Gui *gui;
  GdkGC *gc;
  GdkPixbuf *pix;

  gui = gamandua->gui;
  if (!gui) return;
  gc = gdk_gc_new(drawable);

  gdk_rgb_gc_set_foreground(gc, color);
  gdk_draw_rectangle(drawable, gc, TRUE, x+5, y, width, height);
  pix = gdk_pixbuf_scale_simple(gui->gradpix, width, height, GDK_INTERP_NEAREST);
  gdk_draw_pixbuf(drawable, gc, pix, 0, 0, x+5, y, width, height, 0, 0, 0);

  if (GDK_IS_PIXBUF(pix)) gdk_pixbuf_unref(pix);
  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
}

void
draw_task_txt_size(GdkDrawable* drawable, int size, int x, int y, int color)
{
  Gui *gui;
  GdkGC *gc;
  PangoLayout *layout;
  PangoContext *context;
  char *str;

  gui = gamandua->gui;
  if (!gui || !gui->drawing_area) return;
  gc = gdk_gc_new(drawable);

  context = gtk_widget_get_pango_context(gui->drawing_area);
  layout  = pango_layout_new(context);

  asprintf(&str, "<i>%d</i>", size);
  pango_layout_set_markup(layout, str, -1);
  gdk_draw_layout(drawable, gc, x, y, layout);
  free(str);

  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
  if (G_IS_OBJECT(layout)) g_object_unref(layout);
}

void
draw_selection(GdkDrawable* drawable)
{
  struct tdc_task *task;
  size_t step;
  GdkGC* gc;
  Gui* gui;
  char* weight;
  char* msg;
  size_t x, y;
  PangoLayout *layout;
  PangoContext *context;
  struct tdc_job* job;
  PangoFontDescription *font_descr;

  gui = gamandua->gui;
  task = gamandua->selection.task;
  if (!gui || !gui->drawing_area || !task || !drawable) return;
  gc = gdk_gc_new(drawable);

  context = gtk_widget_get_pango_context(gui->drawing_area);
  layout  = pango_layout_new(context);

  job = g_object_get_data(G_OBJECT(gui->drawing_area), "job");
  
  step = gamandua->selection.step;
  x = task->steps[step].start_time+task->steps[step].length+3;
  y = task->steps[step].machine*50+55;
  gdk_draw_pixbuf(drawable, gc, gui->tooltippix, 0, 0, x, y, gui->tooltippix_w, gui->tooltippix_h, 0, 0, 0);

  if (job->problem->tasks.weighted)  {
		asprintf(&weight, "\n  Poids : %i", task->weight);
  } else {
		weight = NULL;
  }
  asprintf(&msg, "<b>Tâche n°%i</b>\n  Début : %i\n  Durée : %i%s",
   task->id, task->steps[step].start_time, task->steps[step].length, weight?weight:"");
  pango_layout_set_markup(layout, msg, -1);
  font_descr = pango_font_description_new();
  pango_font_description_set_size(font_descr, 8*PANGO_SCALE);
  pango_font_description_set_family(font_descr, "Vera");
  pango_layout_set_font_description(layout, font_descr);
  gdk_draw_layout(drawable, gc, x+15, y+8, layout);
  free(msg);
  free(weight);

  if (G_IS_OBJECT(gc)) gdk_gc_unref(gc);
  if (G_IS_OBJECT(layout)) g_object_unref(layout);

  draw_task(drawable, task->steps[step].start_time, task->steps[step].machine*50+40, 
            task->steps[step].length, 40, 
            0xfce94f);
}

Canvas*
get_current_canvas(void)
{
  Gui *gui;
  struct tdc_job *job;
  Canvas *canvas;

  gui = gamandua->gui;
  if (!gui) return NULL;
  canvas = g_slist_nth_data(gui->drawings, 
                            gtk_notebook_get_current_page(GTK_NOTEBOOK(gui->drawing_notebook)));
  return canvas;
}
