#ifndef GAMANDUA_H
#define GAMANDUA_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <pango/pango.h>

#include <td_base.h>
#include <td_core.h>
typedef struct _Page Page;
struct _Page {
  GtkWidget *root;
  GtkWidget *slider_task;
  GtkWidget *weight_min;
  GtkWidget *weight_max;
  GtkWidget **steps_min;
  GtkWidget **steps_max;
};

typedef struct _Gui Gui;
struct _Gui
{
  GtkWidget *window;
  GtkWidget *combo_prob;
  GtkWidget *combo_strat;
  GtkWidget *spin_machine;
  GtkWidget *spin_pop;
  GtkWidget *scrolled;
  GtkWidget *drawing_area;
  GtkWidget *notebook;
  GtkWidget *buttonbox;
  GtkWidget *frame_info;

  GSList *pages;
};

typedef struct _Gamandua Gamandua;
struct _Gamandua
{
  struct tdc_generator *generator;
  struct tdc_problem   *problem;
  struct tdc_job       *base_job;
  struct tdc_job       *job;
  size_t max_length;
  Gui *gui;
  struct {
    struct tdc_task *task;
    int color;
  } selection;
};

Gamandua *gamandua;


GtkWidget *init_combo_prob(void);
GtkWidget *init_combo_strat(void);
GtkWidget *init_spin_machine(void);

void remove_page(GtkWidget *notebook);
void append_page(GtkWidget *notebook);
void apply_pop_settings(struct tdc_generator *generator);
void draw_task(int x, int y, int width, int height, int color);
void draw_task_txt_size(int size, int x, int y, int color);
void draw_selection(void);

#endif /* GAMANDUA_H */
