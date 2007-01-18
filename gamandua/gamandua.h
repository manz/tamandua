#ifndef INC_GAMANDUA_H
#define INC_GAMANDUA_H 1

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

#define PACKAGE_DATA_DIR "/usr/local/share/tamandua/gamandua/"

static const int colors[] = {
  0x729fcf,
  0x3465a4,
  0xef2929,
  0xcc0000,
  0xfce94f
};

#define COLOR_STEP 15
static const int id_colors[] = {
  0xbdd169,
  0xf09534
};

static const int wrap_colors[] = {
  0xeeeeee,
  0xdddddd
};



typedef struct _Page Page;
struct _Page {
  GtkWidget *root;
  GtkWidget *slider_task;
  GtkWidget *weight_min;
  GtkWidget *weight_max;
  GtkWidget **steps_min;
  GtkWidget **steps_max;
};

typedef struct _Canvas Canvas;
struct _Canvas
{
  GtkWidget *drawing_area;
  GtkWidget *scrolled;
  struct tdc_job *job;
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
  GtkWidget *drawing_notebook;
  GtkWidget *notebook;
  GtkWidget *buttonbox;
  GtkWidget *buttonbox2;
  GtkWidget *btn_calc;
  GtkWidget *btn_help;
  GtkWidget *btn_clear;
  GtkWidget *btn_export;
  GtkWidget *btn_quit;
  GtkWidget *logo;
  GtkWidget *hbox;
  GdkPixbuf *gradpix;
  GdkPixbuf *logopix;
  int logopix_w;
  int logopix_h;
  GdkPixbuf *tooltippix;
  int tooltippix_w;
  int tooltippix_h;
  GSList *drawings;
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
  size_t n_results;
  Gui *gui;
  struct {
    size_t step;
    struct tdc_task *task;
    int color;
  } selection;
};

Gamandua *gamandua;


GtkWidget *init_combo_prob(void);
GtkWidget *init_combo_strat(void);
GtkWidget *init_spin_machine(void);
void init_menu(void);

void remove_page(GtkWidget *notebook);
void append_page(GtkWidget *notebook);
void apply_pop_settings(struct tdc_generator *generator);
void apply_job_settings(struct tdc_job *job, size_t n_machines, size_t strat);
void draw_task(GdkDrawable* drawable, int x, int y, int width, int height, int color);
void draw_task_txt_size(GdkDrawable* drawable, int size, int x, int y, int color);
void draw_selection(GdkDrawable* drawable);
void append_help(GtkNotebook *notebook);
void append_drawing(GtkNotebook *notebook);
Canvas *get_current_canvas(void);
void switch_to_logo(int logo);
GdkDrawable* job_to_drawable(GdkDrawable* win, struct tdc_job* job);

#endif /* INC_GAMANDUA_H */
