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


static void on_font_set(GtkFontDialogButton *button, NovaType *editor) {
    PangoFontDescription *font_desc = gtk_font_dialog_button_get_font_desc(button);
    GtkTextIter start, end;
    
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        char *font_str = pango_font_description_to_string(font_desc);
        GtkTextTag *tag = gtk_text_tag_new(font_str);  // Use font_str as tag name
        g_object_set(tag, "font-desc", font_desc, NULL);
        gtk_text_tag_table_add(gtk_text_buffer_get_tag_table(editor->text_buffer), tag);
        gtk_text_buffer_apply_tag(editor->text_buffer, tag, &start, &end);
        g_free(font_str);
    }
}

static void on_heading1_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        // Remove any existing header tags first
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading1", &start, &end);
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading2", &start, &end);
        // Apply heading1
        gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "heading1", &start, &end);
    }
}

static void on_heading2_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        // Remove any existing header tags first
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading1", &start, &end);
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading2", &start, &end);
        // Apply heading2
        gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "heading2", &start, &end);
    }
}


// File format functions
static void save_sunt_file(NovaType *editor, const char *filename) {
    gboolean is_bold = FALSE, is_italic = FALSE, is_heading1 = FALSE, is_heading2 = FALSE;
    FILE *file = fopen(filename, "w");
    if (!file) {
        g_print("Error: Cannot save file %s\n", filename);
        return;
    }

    // Write header
    fprintf(file, "NovaType1.0\n");
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    
    // Iterate through text with formatting
    GtkTextIter iter = start;
    while (!gtk_text_iter_equal(&iter, &end)) {
        GtkTextIter next = iter;
        gtk_text_iter_forward_char(&next);
        
        // Get character
        char *text = gtk_text_iter_get_text(&iter, &next);
        
        // Get formatting tags
        GSList *tags = gtk_text_iter_get_tags(&iter);
        gboolean is_bold = FALSE, is_italic = FALSE;
        
        for (GSList *l = tags; l != NULL; l = l->next) {
            GtkTextTag *tag = GTK_TEXT_TAG(l->data);
            const char *name = NULL;
            g_object_get(tag, "name", &name, NULL);
            
          if (name) {
            if (strcmp(name, "bold") == 0) is_bold = TRUE;
            if (strcmp(name, "italic") == 0) is_italic = TRUE;
            if (strcmp(name, "heading1") == 0) is_heading1 = TRUE;  // Add this
            if (strcmp(name, "heading2") == 0) is_heading2 = TRUE;  // Add this
          }
        

        }

        g_slist_free(tags);
        
        // Write formatting codes
        if (is_heading1) {
          fprintf(file, "<h1>%s</h1>", text);
        } else if (is_heading2) {
          fprintf(file, "<h2>%s</h2>", text);
        } else if (is_bold && is_italic) {
            fprintf(file, "<bi>%s</bi>", text);
        } else if (is_bold) {
            fprintf(file, "<b>%s</b>", text);
        } else if (is_italic) {
            fprintf(file, "<i>%s</i>", text);
        } else {
            fprintf(file, "%s", text);
        }
        
        g_free(text);
        iter = next;
    }
    
    fclose(file);
    g_print("Saved to %s\n", filename);
}

static void load_sunt_file(NovaType *editor, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Error: Cannot open file %s\n", filename);
        return;
    }
    
    // Clear current text
    gtk_text_buffer_set_text(editor->text_buffer, "", -1);
    
    char line[1024];
    if (fgets(line, sizeof(line), file) && strncmp(line, "NovaType1.0", 7) != 0) {
        g_print("Error: Invalid SUNT file format\n");
        fclose(file);
        return;
    }
    
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(editor->text_buffer, &iter);
    
    char buffer[4096];
    size_t total_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[total_read] = '\0';
    
    // Parse formatting tags
    char *pos = buffer;
    while (*pos) {
        if (strncmp(pos, "<bi>", 4) == 0) {
            pos += 4;
            char *end = strstr(pos, "</bi>");
            if (end) {
                *end = '\0';
                gtk_text_buffer_insert_with_tags_by_name(editor->text_buffer, &iter, pos, -1, "bold", "italic", NULL);
                pos = end + 5;
            } else {
                gtk_text_buffer_insert(editor->text_buffer, &iter, pos, -1);
                break;
            }
        } else if (strncmp(pos, "<b>", 3) == 0) {
            pos += 3;
            char *end = strstr(pos, "</b>");
            if (end) {
                *end = '\0';
                gtk_text_buffer_insert_with_tags_by_name(editor->text_buffer, &iter, pos, -1, "bold", NULL);
                pos = end + 4;
            } else {
                gtk_text_buffer_insert(editor->text_buffer, &iter, pos, -1);
                break;
            }
        } else if (strncmp(pos, "<i>", 3) == 0) {
            pos += 3;
            char *end = strstr(pos, "</i>");
            if (end) {
                *end = '\0';
                gtk_text_buffer_insert_with_tags_by_name(editor->text_buffer, &iter, pos, -1, "italic", NULL);
                pos = end + 4;
            } else {
                gtk_text_buffer_insert(editor->text_buffer, &iter, pos, -1);
                break;
            }
        } else if (strncmp(pos, "<h1>", 4) == 0) {
          pos += 4;
          char *end = strstr(pos, "</h1>");
          if (end) {
            *end = '\0';
            gtk_text_buffer_insert_with_tags_by_name(editor->text_buffer, &iter, pos, -1, "heading1", NULL);
            pos = end + 5;
          } else {
            gtk_text_buffer_insert(editor->text_buffer, &iter, pos, -1);
            break;
          }
        } else if (strncmp(pos, "<h2>", 4) == 0) {
          pos += 4;
          char *end = strstr(pos, "</h2>");
          if (end) {
            *end = '\0';
            gtk_text_buffer_insert_with_tags_by_name(editor->text_buffer, &iter, pos, -1, "heading2", NULL);
            pos = end + 5;
        } else {
            gtk_text_buffer_insert(editor->text_buffer, &iter, pos, -1);
            break;
        }
        } else {
            char temp[2] = {*pos, '\0'};
            gtk_text_buffer_insert(editor->text_buffer, &iter, temp, -1);
            pos++;
        }
    }
    
    fclose(file);
    g_print("Loaded %s\n", filename);
}

// Button callbacks
static void on_bold_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        // Check if selection is already bold
        GtkTextTag *bold_tag = gtk_text_tag_table_lookup(
            gtk_text_buffer_get_tag_table(editor->text_buffer), "bold");
        
        if (gtk_text_iter_has_tag(&start, bold_tag)) {
            gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "bold", &start, &end);
        } else {
            gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "bold", &start, &end);
        }
    }
}

static void on_italic_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        GtkTextTag *italic_tag = gtk_text_tag_table_lookup(
            gtk_text_buffer_get_tag_table(editor->text_buffer), "italic");
        
        if (gtk_text_iter_has_tag(&start, italic_tag)) {
            gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "italic", &start, &end);
        } else {
            gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "italic", &start, &end);
        }
    }
}

static void save_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor) {
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

static void on_save_clicked(GtkButton *button, NovaType *editor) {
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

static void open_file_dialog_response(GtkNativeDialog *dialog, int response, NovaType *editor) {
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

static void on_open_clicked(GtkButton *button, NovaType *editor) {
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

static void on_new_clicked(GtkButton *button, NovaType *editor) {
    gtk_text_buffer_set_text(editor->text_buffer, "", -1);
    if (editor->current_file) {
        g_free(editor->current_file);
        editor->current_file = NULL;
    }
}

// Keyboard shortcuts
static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, 
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
                break;
            case GDK_KEY_2:
                on_heading2_clicked(editor->heading2_btn, editor);
                return TRUE;
                break;
        }
    }
      return FALSE;
}

// Update the setup_text_tags function:
static void setup_text_tags(NovaType *editor) {
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(editor->text_buffer);
    
    // Bold tag
    GtkTextTag *bold_tag = gtk_text_tag_new("bold");
    g_object_set(bold_tag, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add(tag_table, bold_tag);
    
    // Italic tag
    GtkTextTag *italic_tag = gtk_text_tag_new("italic");
    g_object_set(italic_tag, "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_tag_table_add(tag_table, italic_tag);
    
    // Heading1 tag
    GtkTextTag *heading1 = gtk_text_tag_new("heading1");
    g_object_set(heading1,
        "scale", PANGO_SCALE_XX_LARGE,
        "weight", PANGO_WEIGHT_BOLD,
        NULL);
    gtk_text_tag_table_add(tag_table, heading1);
    
    // Heading2 tag
    GtkTextTag *heading2 = gtk_text_tag_new("heading2");
    g_object_set(heading2,
        "scale", PANGO_SCALE_X_LARGE,
        "weight", PANGO_WEIGHT_BOLD,
        NULL);
    gtk_text_tag_table_add(tag_table, heading2);
}

static void activate(GtkApplication *app, gpointer user_data) {
    NovaType *editor = g_new0(NovaType, 1);
    editor->app = app;
    
    // Create main window
    editor->window = GTK_WINDOW(adw_application_window_new(app));
    gtk_window_set_title(editor->window, "NovaType");
    gtk_window_set_default_size(editor->window, 800, 600);
    
    // Create header bar
    editor->header_bar = ADW_HEADER_BAR(adw_header_bar_new());
    
    // Create toolbar buttons
    editor->new_btn = GTK_BUTTON(gtk_button_new_from_icon_name("document-new-symbolic"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->new_btn), "New (Ctrl+N)");
    
    editor->open_btn = GTK_BUTTON(gtk_button_new_from_icon_name("document-open-symbolic"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->open_btn), "Open (Ctrl+O)");
    
    editor->save_btn = GTK_BUTTON(gtk_button_new_from_icon_name("document-save-symbolic"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->save_btn), "Save (Ctrl+S)");
    
    editor->bold_btn = GTK_BUTTON(gtk_button_new_from_icon_name("format-text-bold-symbolic"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->bold_btn), "Bold (Ctrl+B)");
    
    editor->italic_btn = GTK_BUTTON(gtk_button_new_from_icon_name("format-text-italic-symbolic"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->italic_btn), "Italic (Ctrl+I)");

    // Font button

editor->font_btn = GTK_FONT_DIALOG_BUTTON(gtk_font_dialog_button_new(gtk_font_dialog_new()));
gtk_widget_set_tooltip_text(GTK_WIDGET(editor->font_btn), "Change Font");
g_signal_connect(editor->font_btn, "notify::font-set", G_CALLBACK(on_font_set), editor);
    
    // Heading buttons
    editor->heading1_btn = GTK_BUTTON(gtk_button_new_with_label("H1"));
    editor->heading2_btn = GTK_BUTTON(gtk_button_new_with_label("H2"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->heading1_btn), "Heading 1");
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->heading2_btn), "Heading 2");

    // Add buttons to header bar
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->new_btn));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->open_btn));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->save_btn));
    adw_header_bar_pack_start(editor->header_bar, gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->bold_btn));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->italic_btn));
    adw_header_bar_pack_start(editor->header_bar, gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->font_btn));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->heading1_btn));
    adw_header_bar_pack_start(editor->header_bar, GTK_WIDGET(editor->heading2_btn));
    
    // Create text view and buffer
    editor->text_buffer = gtk_text_buffer_new(NULL);
    editor->text_view = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(editor->text_buffer));
    gtk_text_view_set_wrap_mode(editor->text_view, GTK_WRAP_WORD);
    gtk_widget_set_margin_top(GTK_WIDGET(editor->text_view), 12);
    gtk_widget_set_margin_bottom(GTK_WIDGET(editor->text_view), 12);
    gtk_widget_set_margin_start(GTK_WIDGET(editor->text_view), 12);
    gtk_widget_set_margin_end(GTK_WIDGET(editor->text_view), 12);

    setup_text_tags(editor);
    
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), GTK_WIDGET(editor->text_view));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
                              GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // Create centering box
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *left_spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *right_spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_append(GTK_BOX(center_box), left_spacer);
    gtk_box_append(GTK_BOX(center_box), scrolled);
    gtk_box_append(GTK_BOX(center_box), right_spacer);

    gtk_widget_set_hexpand(left_spacer, TRUE);
    gtk_widget_set_hexpand(right_spacer, TRUE);
    gtk_widget_set_size_request(scrolled, A4_PAPER_WIDTH, -1);

    // Create main box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(editor->header_bar));
    gtk_box_append(GTK_BOX(box), center_box);
    

    adw_application_window_set_content(ADW_APPLICATION_WINDOW(editor->window), box);
    
    // Connect signals
    g_signal_connect(editor->bold_btn, "clicked", G_CALLBACK(on_bold_clicked), editor);
    g_signal_connect(editor->italic_btn, "clicked", G_CALLBACK(on_italic_clicked), editor);
    g_signal_connect(editor->save_btn, "clicked", G_CALLBACK(on_save_clicked), editor);
    g_signal_connect(editor->open_btn, "clicked", G_CALLBACK(on_open_clicked), editor);
    g_signal_connect(editor->new_btn, "clicked", G_CALLBACK(on_new_clicked), editor);
    g_signal_connect(editor->heading1_btn, "clicked", G_CALLBACK(on_heading1_clicked), editor);
    g_signal_connect(editor->heading2_btn, "clicked", G_CALLBACK(on_heading2_clicked), editor);


    // Add keyboard shortcuts
    GtkEventController *key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), editor);
    gtk_widget_add_controller(GTK_WIDGET(editor->window), key_controller);
    
    gtk_window_present(editor->window);
}

int main(int argc, char **argv) {
    AdwApplication *app = adw_application_new("org.SuperUserNova.NovaType", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
