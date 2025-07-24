#ifndef EDITOR_H
#define EDITOR_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define A4_PAPER_WIDTH 595

typedef struct {
    GtkApplication *app;
    GtkWindow *window;
    GtkTextView *text_view;
    GtkTextBuffer *text_buffer;
    AdwHeaderBar *header_bar;
    GtkFontDialogButton *font_btn;
    GtkButton *heading1_btn;
    GtkButton *heading2_btn;
    GtkButton *bold_btn;
    GtkButton *italic_btn;
    GtkButton *save_btn;
    GtkButton *open_btn;
    GtkButton *new_btn;
    char *current_file;
} NovaType;

// Function declarations from editor.c
void create_editor_window(NovaType *editor);

// Function declarations from formatting.c
void on_font_set(GtkFontDialogButton *button, NovaType *editor);
void on_heading1_clicked(GtkButton *button, NovaType *editor);
void on_heading2_clicked(GtkButton *button, NovaType *editor);
void on_bold_clicked(GtkButton *button, NovaType *editor);
void on_italic_clicked(GtkButton *button, NovaType *editor);

// Function declarations from file_io.c
void save_sunt_file(NovaType *editor, const char *filename);
void load_sunt_file(NovaType *editor, const char *filename);

// Function declarations from callbacks.c
void save_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor);
void on_save_clicked(GtkButton *button, NovaType *editor);
void open_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor);
void on_open_clicked(GtkButton *button, NovaType *editor);
void on_new_clicked(GtkButton *button, NovaType *editor);
gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, 
                        guint keycode, GdkModifierType state, NovaType *editor);

// Function declarations from text_tags.c
void setup_text_tags(NovaType *editor);

#endif // EDITOR_H
