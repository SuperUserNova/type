using Gtk;
using Adw;

public class NovaType.Application : Adw.Application {
    public Application() {
        Object(
            application_id: "org.SuperUserNova.Type",
            flags: ApplicationFlags.HANDLES_OPEN
        );
    }

    protected override void activate() {
        new MainWindow(this).present();
    }

    protected override void open(File[] files, string hint) {
        foreach (var file in files) {
            new MainWindow(this, file).present();
        }
    }
}
