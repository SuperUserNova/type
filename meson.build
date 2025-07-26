project('novatype', ['vala', 'c'])

gnome = import('gnome')

# Compile resources
resources = gnome.compile_resources(
  'novatype-resources',
  'data/resources.gresource.xml',
  c_name: 'novatype',
  source_dir: 'data'
)

dependencies = [
  dependency('gtk4'),
  dependency('libadwaita-1'),
  dependency('gio-2.0'),
  dependency('pango'),
  dependency('pangocairo'),
  dependency('gobject-introspection-1.0'),
  dependency('graphene-gobject-1.0'),
  dependency('gdk-pixbuf-2.0'),
  dependency('gee-0.8')
]

sources = files(
  'novatype.vala',
  'src/Application.vala',
  'src/MainWindow.vala',
  'src/DocumentManager.vala',
  'src/Utils.vala'
)

executable(
  'novatype',
  sources,
  resources,
  dependencies: dependencies,
  install: true
)

# Install desktop file
install_data(
  'data/org.SuperUserNova.Type.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications')
)
