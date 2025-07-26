namespace NovaType {
    public File ensure_sunt_extension(File original) {
        if (!original.get_basename().has_suffix(".sunt")) {
            return File.new_for_path(original.get_path() + ".sunt");
        }
        return original;
    }
}
