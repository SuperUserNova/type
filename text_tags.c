#include "editor.h"

void setup_text_tags(NovaType *editor) {
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
