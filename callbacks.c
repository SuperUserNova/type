#include "editor.h"

void save_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor) {
    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        GFile *file = gtk_file_chooser_get_file(chooser);
        char *filename = g_file_get_path(file);
        
        // Ensure .sunt extension
        if (!g_str_has_suffix(filename, ".sunt")) {
            char *new_filename = g_strdup_printf("%s.sunt", filename);
            g_free(filename);
            filename = new_filename;
        }
        
        save_sunt_file(editor, filename);
        
        if (editor->current_file) g_free(editor->current_file);
        editor->current_file = g_strdup(filename);
        
        g_free(filename);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

void on_save_clicked(GtkButton *button, NovaType *editor) {
    if (editor->current_file) {
        save_sunt_file(editor, editor->current_file);
    } else {
        GtkFileChooserNative *dialog = gtk_file_chooser_native_new(
            "Save NovaType Document",
            editor->window,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Save",
            "_Cancel"
        );
        
        // Add .sunt filter
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "NovaType Documents (*.sunt)");
        gtk_file_filter_add_pattern(filter, "*.sunt");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        
        g_signal_connect(dialog, "response", G_CALLBACK(save_file_dialog_response), editor);
        gtk_native_dialog_show(GTK_NATIVE_DIALOG(dialog));
    }
}

void open_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor) {
    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        GFile *file = gtk_file_chooser_get_file(chooser);
        char *filename = g_file_get_path(file);
        
        load_sunt_file(editor, filename);
        
        if (editor->current_file) g_free(editor->current_file);
        editor->current_file = g_strdup(filename);
        
        g_free(filename);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

void on_open_clicked(GtkButton *button, NovaType *editor) {
    GtkFileChooserNative *dialog = gtk_file_chooser_native_new(
        "Open NovaType Document",
        editor->window,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Open",
        "_Cancel"
    );
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "NovaType Documents (*.sunt)");
    gtk_file_filter_add_pattern(filter, "*.sunt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    g_signal_connect(dialog, "response", G_CALLBACK(open_file_dialog_response), editor);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(dialog));
}

void on_new_clicked(GtkButton *button, NovaType *editor) {
    gtk_text_buffer_set_text(editor->text_buffer, "", -1);
    if (editor->current_file) {
        g_free(editor->current_file);
        editor->current_file = NULL;
    }
}

gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, 
                        guint keycode, GdkModifierType state, NovaType *editor) {
    if (state & GDK_CONTROL_MASK) {
        switch (keyval) {
            case GDK_KEY_b:
                on_bold_clicked(editor->bold_btn, editor);
                return TRUE;
            case GDK_KEY_i:
                on_italic_clicked(editor->italic_btn, editor);
                return TRUE;
            case GDK_KEY_s:
                on_save_clicked(editor->save_btn, editor);
                return TRUE;
            case GDK_KEY_o:
                on_open_clicked(editor->open_btn, editor);
                return TRUE;
            case GDK_KEY_n:
                on_new_clicked(editor->new_btn, editor);
                return TRUE;
            case GDK_KEY_1:
                on_heading1_clicked(editor->heading1_btn, editor);
                return TRUE;
            case GDK_KEY_2:
                on_heading2_clicked(editor->heading2_btn, editor);
                return TRUE;
        }
    }
    return FALSE;
}
