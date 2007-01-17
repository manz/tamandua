#ifndef GAMANDUA_CALLBACKS_H
#define GAMANDUA_CALLBACKS_H

void cb_window_destroy(GtkObject *obj, void *data);
void cb_combo_changed(GtkComboBox *combo, void *data);
void cb_spin_pop_changed(GtkSpinButton *spin, void *data);
void cb_btn_quit_clicked(GtkButton *button, void *data);
void cb_btn_calc_clicked(GtkButton *button, void *data);
void cb_btn_exec_clicked(GtkButton *button, void *data);
void cb_drawing_area_exposed(GtkDrawingArea *area, void *data);
void cb_drawing_area_mouse_down(GtkWidget *widget, GdkEventButton *ev, void *data);
void cb_unsensitive_btn_exec(void *w, void *data);
void cb_drawing_area_exposed(GtkDrawingArea *area, void *data);
void cb_drawing_notebook_change_current(GtkNotebook *notebook, GtkNotebookPage *page, 
                                        size_t p, void *data);
void cb_drawing_page_close(GtkButton *button, void *data);
void cb_btn_clear_clicked(GtkButton *button, void *data);
void cb_btn_help_clicked(GtkButton *button, void *data);
void cb_btn_export_clicked(GtkButton *button, void *data);
int cb_checkout_job(void *data);
void cb_keybindings(GtkWidget *widget, GdkEventKey *ev, void *data);


#endif /* GAMANDUA_CALLBACKS_H */
