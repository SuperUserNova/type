using Gtk;
using Adw;
using Gee;

public class NovaType.DocumentManager {
    public string load(File file, Gtk.TextBuffer? buffer = null) throws Error {
        uint8[] data;
        file.load_contents(null, out data, null);
        string content = (string) data;
    
        if (buffer != null) {
            deserialize_to_buffer(content, buffer);
            return buffer.text; // Return plain text for display
        }
    
        return content;
    }

    public void save(File file, string content, Gtk.TextBuffer buffer) throws Error {
        var serialized = serialize_buffer(buffer);
        FileOutputStream stream = file.replace(null, false, FileCreateFlags.NONE);
        stream.write(serialized.data);
        stream.close();
    }

    private string serialize_buffer(Gtk.TextBuffer buffer) {
        var result = new StringBuilder();
        Gtk.TextIter start, end;
        buffer.get_bounds(out start, out end);
    
        var iter = start;
        while (!iter.equal(end)) {
            var current_tags = new Gee.HashSet<string>();
            var tag_list = iter.get_tags();
        
            // Get current formatting
            foreach (var tag in tag_list) {
                if (tag.name != null) {
                    current_tags.add(tag.name);
                }
            }
        
            // Find the end of this formatting run
            var run_end = iter;
            while (!run_end.equal(end)) {
                var next_tags = new Gee.HashSet<string>();
                var next_tag_list = run_end.get_tags();
            
                foreach (var tag in next_tag_list) {
                    if (tag.name != null) {
                        next_tags.add(tag.name);
                    }
                }
            
                // If tags changed, stop here
                if (!tags_equal(current_tags, next_tags)) {
                    break;
                }
            
                if (!run_end.forward_char()) break;
            }
        
            // Get the text for this run
            var text = buffer.get_text(iter, run_end, false);
        
            // Wrap with tags
            var wrapped = text;
            if (current_tags.contains("bold")) wrapped = @"<bold>$wrapped</bold>";
            if (current_tags.contains("italic")) wrapped = @"<italic>$wrapped</italic>";
            if (current_tags.contains("underline")) wrapped = @"<underline>$wrapped</underline>";
            if (current_tags.contains("h1")) wrapped = @"<h1>$wrapped</h1>";
            if (current_tags.contains("h2")) wrapped = @"<h2>$wrapped</h2>";
        
            result.append(wrapped);
            iter = run_end;
        }
    
        return result.str;
    }

    private bool tags_equal(Gee.HashSet<string> set1, Gee.HashSet<string> set2) {
        if (set1.size != set2.size) return false;
        foreach (var tag in set1) {
            if (!set2.contains(tag)) return false;
        }
        return true;
    }
    
    public void deserialize_to_buffer(string content, Gtk.TextBuffer buffer) {
    
        buffer.text = "";
        parse_content(content, buffer, new Gee.ArrayList<string>());
    
    }

    private void parse_content(string content, Gtk.TextBuffer buffer, Gee.ArrayList<string> active_tags) {
    
        var pos = 0;
        
        while (pos < content.length) {
            var next_tag = content.index_of("<", pos);
            
            if (next_tag == -1) {
                // No more tags, insert remaining text with current formatting
                
                if (pos < content.length) {
                
                    insert_formatted_text(content.substring(pos), buffer, active_tags);
                
                }
                
                break;
            
            }
        
            // Insert plain text before tag
            
            if (next_tag > pos) {
            
                insert_formatted_text(content.substring(pos, next_tag - pos), buffer, active_tags);
            
            }
        
            // Parse the tag
            var tag_end = content.index_of(">", next_tag);
            
            if (tag_end == -1) {
                
                pos = next_tag + 1;
                continue;
            
            }
        
            var tag_content = content.substring(next_tag + 1, tag_end - next_tag - 1);
        
            if (tag_content.has_prefix("/")) {
            
                // Closing tag - remove from active tags
                
                var tag_name = tag_content.substring(1);
                active_tags.remove(tag_name);
                pos = tag_end + 1;
            
            } else {
                
                // Opening tag - find matching closing tag and parse content
                
                var tag_name = tag_content;
                var closing_tag = @"</$tag_name>";
            
                // Find the matching closing tag (accounting for nesting)
                
                var content_start = tag_end + 1;
                var nesting_level = 1;
                var search_pos = content_start;
                var content_end = -1;
            
                while (search_pos < content.length && nesting_level > 0) {
                    var next_open = content.index_of(@"<$tag_name>", search_pos);
                    var next_close = content.index_of(closing_tag, search_pos);
                
                    if (next_close == -1) break;
                
                    if (next_open != -1 && next_open < next_close) {
                        
                        nesting_level++;
                        search_pos = next_open + tag_name.length + 2;
                    
                    } else {
                        
                        nesting_level--;
                        
                        if (nesting_level == 0) {
                        
                            content_end = next_close;
                        }
                        
                        search_pos = next_close + closing_tag.length;
                    
                    }
                
                }
            
                if (content_end != -1) {
                    
                    // Parse the content inside the tag with this tag added to active tags
                    
                    var inner_content = content.substring(content_start, content_end - content_start);
                    
                    var new_active_tags = new Gee.ArrayList<string>();
                    
                    new_active_tags.add_all(active_tags);
                    
                    new_active_tags.add(tag_name);
                
                    parse_content(inner_content, buffer, new_active_tags);
                    pos = content_end + closing_tag.length;
                
                } else {
                    
                    pos = tag_end + 1;
                
                }
            
            }
        
        }
    
    }

    private void insert_formatted_text(string text, Gtk.TextBuffer buffer, Gee.ArrayList<string> tags) {
        
        if (text.length == 0) return;
    
          Gtk.TextIter start_iter;
          buffer.get_end_iter(out start_iter);
          var start_mark = buffer.create_mark(null, start_iter, true);
    
          buffer.insert_at_cursor(text, -1);
    
          // Apply all active tags
          
          foreach (var tag_name in tags) {
              
              var text_tag = buffer.get_tag_table().lookup(tag_name);
              
              if (text_tag == null) {
              
                  text_tag = buffer.create_tag(tag_name);
                  setup_tag_properties(text_tag, tag_name);
              
              }
        
              Gtk.TextIter start, end;
              buffer.get_iter_at_mark(out start, start_mark);
              buffer.get_end_iter(out end);
              buffer.apply_tag(text_tag, start, end);
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

}

