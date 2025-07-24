#include "editor.h"

void on_font_set(GtkFontDialogButton *button, NovaType *editor) {
    PangoFontDescription *font_desc = gtk_font_dialog_button_get_font_desc(button);
    GtkTextIter start, end;
    
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        char *font_str = pango_font_description_to_string(font_desc);
        GtkTextTag *tag = gtk_text_tag_new(font_str);
        g_object_set(tag, "font-desc", font_desc, NULL);
        gtk_text_tag_table_add(gtk_text_buffer_get_tag_table(editor->text_buffer), tag);
        gtk_text_buffer_apply_tag(editor->text_buffer, tag, &start, &end);
        g_free(font_str);
    }
}

void on_heading1_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        // Remove any existing header tags first
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading1", &start, &end);
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading2", &start, &end);
        // Apply heading1
        gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "heading1", &start, &end);
        
        // Reset cursor position to prevent heading from affecting new text
        gtk_text_buffer_place_cursor(editor->text_buffer, &end);
    }
}

void on_heading2_clicked(GtkButton *button, NovaType *editor) {
    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(editor->text_buffer, &start, &end)) {
        // Remove any existing header tags first
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading1", &start, &end);
        gtk_text_buffer_remove_tag_by_name(editor->text_buffer, "heading2", &start, &end);
        // Apply heading2
        gtk_text_buffer_apply_tag_by_name(editor->text_buffer, "heading2", &start, &end);
        
        // Reset cursor position to prevent heading from affecting new text
        gtk_text_buffer_place_cursor(editor->text_buffer, &end);
    }
}

void on_bold_clicked(GtkButton *button, NovaType *editor) {
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

void on_italic_clicked(GtkButton *button, NovaType *editor) {
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
