panellib_sources = files(
  'misc.c',
  'configurator.c',
  'dbg.c',
  'ev.c',
  'icon-grid.c',
  'panel.c',
  'panel-plugin-move.c',
  'plugin.c',
  'conf.c',
  'space.c',
  'input-button.c',
  'notify.c',
)

panel_sources = files(
  'bg.c',
  'gtk-run.c',
  'main.c',
  'menu-policy.c'
)

control_sources = files(
  'lxpanelctl.c'
)

headers = files(
  'conf.h',
  'icon-grid.h',
  'misc.h',
  'panel.h',
  'plugin.h'
)

liblxpanel = both_libraries(meson.project_name(), panellib_sources,
        dependencies: deps,
        install: true,
        install_dir: get_option('libdir') / 'lxpanel-pi'
)

executable('lxpanel-pi', panel_sources,
        dependencies: deps,
        install: true,
        export_dynamic: true,
        link_with : [ liblxpanel.get_static_lib(), liblxpanelplugins ]
)

executable('lxpanelctl-pi', control_sources,
        dependencies: deps,
        install: true
)

install_data(headers, install_dir: join_paths(get_option('includedir'), 'lxpanel-pi'))

