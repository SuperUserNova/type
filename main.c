#include "editor.h"

static void activate(GtkApplication *app, gpointer user_data) {
    NovaType *editor = g_new0(NovaType, 1);
    editor->app = app;
    
    create_editor_window(editor);
    gtk_window_present(editor->window);
}

int main(int argc, char **argv) {
    AdwApplication *app = adw_application_new("org.SuperUserNova.NovaType", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
