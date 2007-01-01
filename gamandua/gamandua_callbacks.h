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

#endif /* GAMANDUA_CALLBACKS_H */
