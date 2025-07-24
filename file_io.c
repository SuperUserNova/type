#include "editor.h"

void save_sunt_file(NovaType *editor, const char *filename) {
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
        gboolean is_bold = FALSE, is_italic = FALSE, is_heading1 = FALSE, is_heading2 = FALSE;
        
        for (GSList *l = tags; l != NULL; l = l->next) {
            GtkTextTag *tag = GTK_TEXT_TAG(l->data);
            const char *name = NULL;
            g_object_get(tag, "name", &name, NULL);
            
            if (name) {
                if (strcmp(name, "bold") == 0) is_bold = TRUE;
                if (strcmp(name, "italic") == 0) is_italic = TRUE;
                if (strcmp(name, "heading1") == 0) is_heading1 = TRUE;
                if (strcmp(name, "heading2") == 0) is_heading2 = TRUE;
            }
        }
        g_slist_free(tags);
        
        // Write formatting codes (headers take priority)
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

void load_sunt_file(NovaType *editor, const char *filename) {
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
        if (strncmp(pos, "<h1>", 4) == 0) {
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
        } else if (strncmp(pos, "<bi>", 4) == 0) {
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
        } else {
            char temp[2] = {*pos, '\0'};
            gtk_text_buffer_insert(editor->text_buffer, &iter, temp, -1);
            pos++;
        }
    }
    
    fclose(file);
    g_print("Loaded %s\n", filename);
}
