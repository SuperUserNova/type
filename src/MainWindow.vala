using Pango;
public class NovaType.MainWindow : Adw.ApplicationWindow {
    

    private Gtk.TextView text_view;
    
    private File? current_file = null;
    
    private Gtk.ToggleButton bold_button;
    
    private Gtk.ToggleButton italic_button;
    
    private Gtk.ToggleButton underline_button;
    
    private Gtk.ToggleButton h1_button;
    
    private Gtk.ToggleButton h2_button;
    
    private ulong h1_handler_id;
    
    private ulong h2_handler_id;
    
    private DocumentManager doc_manager;
    
    


    public MainWindow(Adw.Application app, File? file = null) {
        
        Object(application: app);
        current_file = file;
        doc_manager = new DocumentManager();

        
        // Window setup
        
        set_default_size(800, 600);
        set_title("NovaType");

        
        // Create header bar
        
        var header = new Adw.HeaderBar();

        
        // Create formatting toolbar
        
        var formatting_box = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 5);
        
        formatting_box.add_css_class("linked");
        
        
        // Bold button
        
        bold_button = new Gtk.ToggleButton();
        
        bold_button.set_icon_name("format-text-bold-symbolic");
        bold_button.tooltip_text = "Bold";
        formatting_box.append(bold_button);


        //Italic button

        italic_button = new Gtk.ToggleButton();
        
        italic_button.set_icon_name("format-text-italic-symbolic");
        italic_button.tooltip_text = "Italic";
        formatting_box.append(italic_button);

        // Underline button
        
        underline_button = new Gtk.ToggleButton();
        
        underline_button.set_icon_name("format-text-underline-symbolic");
        underline_button.tooltip_text = "Underline";
        formatting_box.append(underline_button);


        /* Headers Section */
        

        // Header-1 button
        
        h1_button = new Gtk.ToggleButton();
        
        h1_button.set_label("H1");
        h1_button.tooltip_text = "Heading 1";
        formatting_box.append(h1_button);


        // Header-2 button
        
        h2_button = new Gtk.ToggleButton();
        
        h2_button.set_label("H2");
        h2_button.tooltip_text = "Heading 2";
        formatting_box.append(h2_button);

        header.pack_start(formatting_box);

        
        /* Headers Section End */


        // Create Menu 

        var menu = new Gtk.MenuButton();
      
        menu.set_icon_name("open-menu-symbolic");
      
        // Menu model
        var menu_model = new Menu();
      
      
        menu_model.append("New", "win.new");
        menu_model.append("Open", "win.open");
        menu_model.append("Save", "win.save");
        menu_model.append("Save As...", "win.save_as");
        menu.set_menu_model(menu_model);
        header.pack_end(menu);
      
      
        // Text view
      
        text_view = new Gtk.TextView() {
          vexpand = true,
          margin_top = 10,
          margin_bottom = 10,
          margin_start = 10,
          margin_end = 10
        };
      
        // Scrolled Window

        var scrolled_window = new Gtk.ScrolledWindow();
        scrolled_window.set_child(text_view);

        // Main layout
        var box = new Gtk.Box(Gtk.Orientation.VERTICAL, 0);
        
        box.append(header);
        box.append(scrolled_window);
        set_content(box);

        
        /* Actions Creation Area */
        
        // "New"

        var new_action = new SimpleAction("new", null);
        
        new_action.activate.connect(() => new_document());
        add_action(new_action);

        
        // "Open"

        var open_action = new SimpleAction("open", null);
        
        open_action.activate.connect(() => open_file());
        add_action(open_action);

        
        // "Sve"

        var save_action = new SimpleAction("save", null);
        
        save_action.activate.connect(() => save_file());
        add_action(save_action);


        // "Save As"

        var save_as_action = new SimpleAction("save_as", null);
        
        save_as_action.activate.connect(() => save_file_as());
        add_action(save_as_action);
        
        /* Actions Creation End */

        
        // Formatting button connections
        bold_button.toggled.connect(() => apply_formatting("bold"));
        
        italic_button.toggled.connect(() => apply_formatting("italic"));
        
        underline_button.toggled.connect(() => apply_formatting("underline"));

        h1_handler_id = h1_button.toggled.connect(() => apply_formatting("h1"));
        h2_handler_id = h2_button.toggled.connect(() => apply_formatting("h2"));

        
        // Update formatting buttons when cursor moves
        text_view.buffer.notify["cursor-position"].connect(update_formatting_buttons);

        // Load initial file
        
        if (current_file != null) load_file();
    
    }

    private void new_document() {
    
        current_file = null;
        text_view.buffer.text = "";
        set_title("NovaType - New Document");
    
    }

    private void open_file() {
    
        var file_chooser = new Gtk.FileChooserNative(
            "Open File",
            this,
            Gtk.FileChooserAction.OPEN,
            "_Open",
            "_Cancel"
        );


        file_chooser.response.connect((response) => {
            if (response == Gtk.ResponseType.ACCEPT) {
                current_file = file_chooser.get_file();
                load_file();
            }
        });


        file_chooser.show();
    
    }

    private void save_file() {
        
        if (current_file == null) save_file_as();
        
        else doc_manager.save(current_file, text_view.buffer.text, text_view.buffer);
    
    }

    private void save_file_as() {
    
        var file_chooser = new Gtk.FileChooserNative(
            "Save File",
            this,
            Gtk.FileChooserAction.SAVE,
            "_Save",
            "_Cancel"
        );
        
        // Set .sunt filter
        var filter = new Gtk.FileFilter();
        
        filter.add_pattern("*.sunt");
        filter.name = "NovaType Documents (*.sunt)";
        file_chooser.add_filter(filter);
        
        file_chooser.response.connect((response) => {
        
            if (response == Gtk.ResponseType.ACCEPT) {
            
                current_file = ensure_sunt_extension(file_chooser.get_file());
                doc_manager.save(current_file, text_view.buffer.text, text_view.buffer);
                set_title(@"NovaType - $(current_file.get_basename())");
            
            }
        
        });

        file_chooser.show();
    
    }

    private void load_file() {
        
        if (current_file == null) return;
    
        try {
        
            doc_manager.load(current_file, text_view.buffer);
            set_title(@"NovaType - $(current_file.get_basename())");
        
        } catch (Error e) {
            
            warning("Failed to load file: %s", e.message);
        
        }
      
    }
    
    private void apply_formatting(string format) {
    
        Gtk.TextIter start, end;
        
        var buffer = text_view.buffer;
    
        if (buffer.get_selection_bounds(out start, out end)) {
        
            // Handle heading mutual exclusivity
            
            if (format == "h1" || format == "h2") {
            
                var h1_tag = buffer.get_tag_table().lookup("h1");
                var h2_tag = buffer.get_tag_table().lookup("h2");
            
                if (h1_tag != null) buffer.remove_tag(h1_tag, start, end);
                
                if (h2_tag != null) buffer.remove_tag(h2_tag, start, end);
            
                // Temporarily disconnect signals to avoid recursion
                
                h1_button.disconnect(h1_handler_id);
                h2_button.disconnect(h2_handler_id);
            
                // Reset the other button
                
                if (format == "h1") {
                    
                    h2_button.active = false;
                
                } else {
                
                    h1_button.active = false;
                
                }
            
                // Reconnect signals
            
                h1_handler_id = h1_button.toggled.connect(() => apply_formatting("h1"));
            
                h2_handler_id = h2_button.toggled.connect(() => apply_formatting("h2"));
            
            }
        
            var tag = buffer.get_tag_table().lookup(format);
            
            if (tag == null) {
                
                tag = buffer.create_tag(format);
                
                setup_tag_properties(tag, format);
            
            }
        
            if (get_format_button(format).active) {
                
                buffer.apply_tag(tag, start, end);
            
            } else {
                
                buffer.remove_tag(tag, start, end);
            
            }
        
        }
    
    }
    
    private void setup_tag_properties(Gtk.TextTag tag, string format) {
        
        switch (format) {
            

            case "bold":
                
                tag.weight = Pango.Weight.BOLD;
                
                break;
            

            case "italic":
                
                tag.style = Pango.Style.ITALIC;
                
                break;
            

            case "underline":
                
                tag.underline = Pango.Underline.SINGLE;
                
                break;
            

            case "h1":
                
                tag.scale = 1.5;
                tag.weight = Pango.Weight.BOLD;
                
                break;
            
            
            case "h2":
                
                tag.scale = 1.3;
                tag.weight = Pango.Weight.BOLD;
                
                break;
        
        }
    
    }

    private Gtk.ToggleButton get_format_button(string format) {
        
        switch (format) {
            
            case "bold": return bold_button;
            
            case "italic": return italic_button;
            
            case "underline": return underline_button;
            
            case "h1": return h1_button;
            
            case "h2": return h2_button;
            
            default: return bold_button;
        
        }
    }

    private void update_formatting_buttons() {
        
        var buffer = text_view.buffer;
        
        Gtk.TextIter start, end;
    
        // Check if there's a selection, otherwise use cursor position
        
        if (buffer.get_selection_bounds(out start, out end)) {
        
            // For selections, check if the entire selection has the formatting
            
            var bold_tag = buffer.get_tag_table().lookup("bold");
            
            var italic_tag = buffer.get_tag_table().lookup("italic");
            
            var underline_tag = buffer.get_tag_table().lookup("underline");
            
            var h1_tag = buffer.get_tag_table().lookup("h1");
            
            var h2_tag = buffer.get_tag_table().lookup("h2");
        
            bold_button.active = bold_tag != null && selection_has_tag(buffer, start, end, bold_tag);
            italic_button.active = italic_tag != null && selection_has_tag(buffer, start, end, italic_tag);
            underline_button.active = underline_tag != null && selection_has_tag(buffer, start, end, underline_tag);
            h1_button.active = h1_tag != null && selection_has_tag(buffer, start, end, h1_tag);
            h2_button.active = h2_tag != null && selection_has_tag(buffer, start, end, h2_tag);
        
        } else {
            
            // For cursor position, check current formatting
            
            buffer.get_iter_at_mark(out start, buffer.get_insert());
            
            var bold_tag = buffer.get_tag_table().lookup("bold");
            
            var italic_tag = buffer.get_tag_table().lookup("italic");
            
            var underline_tag = buffer.get_tag_table().lookup("underline");
            
            var h1_tag = buffer.get_tag_table().lookup("h1");
            
            var h2_tag = buffer.get_tag_table().lookup("h2");
        
            bold_button.active = bold_tag != null && start.has_tag(bold_tag);
            italic_button.active = italic_tag != null && start.has_tag(italic_tag);
            underline_button.active = underline_tag != null && start.has_tag(underline_tag);
            h1_button.active = h1_tag != null && start.has_tag(h1_tag);
            h2_button.active = h2_tag != null && start.has_tag(h2_tag);
        
        }
    
    }

    private bool selection_has_tag(Gtk.TextBuffer buffer, Gtk.TextIter start, Gtk.TextIter end, Gtk.TextTag tag) {
        
        var iter = start;
        
        while (!iter.equal(end)) {
        
            if (!iter.has_tag(tag)) {
                
                return false;
            
            }
        

            if (!iter.forward_char()) break;
        
        }
        
        return true;
    
    }

}
