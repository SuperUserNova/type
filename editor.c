#include "editor.h"

void create_editor_window(NovaType *editor) {
    // Create main window
    editor->window = GTK_WINDOW(adw_application_window_new(editor->app));
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
    
    // Heading buttons
    editor->heading1_btn = GTK_BUTTON(gtk_button_new_with_label("H1"));
    editor->heading2_btn = GTK_BUTTON(gtk_button_new_with_label("H2"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->heading1_btn), "Heading 1 (Ctrl+1)");
    gtk_widget_set_tooltip_text(GTK_WIDGET(editor->heading2_btn), "Heading 2 (Ctrl+2)");

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
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_size_request(scrolled, A4_PAPER_WIDTH, -1);

    // Create main box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(editor->header_bar));
    gtk_box_append(GTK_BOX(box), center_box);

    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(editor->window), box);
    
    // Connect signals
    g_signal_connect(editor->font_btn, "font-set", G_CALLBACK(on_font_set), editor);
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
}
