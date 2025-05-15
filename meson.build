project('lxpanel-pi', 'c')

share_dir = join_paths(get_option('prefix'), 'share')
resource_dir = join_paths(share_dir, 'lxpanel-pi')
ui_dir = join_paths(resource_dir, 'ui')
sysconf_dir = join_paths(get_option('prefix'), get_option('sysconfdir'))

gettext_package = meson.project_name()

add_project_arguments('-DPACKAGE_DATA_DIR="' + resource_dir + '"', language : [ 'c' ])
add_project_arguments('-DPACKAGE_UI_DIR="' + ui_dir + '"', language : [ 'c' ])
add_project_arguments('-DPACKAGE_LOCALE_DIR="' + share_dir + '/locale"', language : [ 'c' ])
add_project_arguments('-DPACKAGE_LIB_DIR="' + get_option ('prefix') + '/' + get_option('libdir') + '"', language : [ 'c' ])
add_project_arguments('-DGETTEXT_PACKAGE="' + gettext_package + '"', language : [ 'c' ])
add_project_arguments('-DVERSION="1.0"', language : [ 'c' ])

gtk = dependency ('gtk+-3.0')
keybind = dependency ('keybinder-3.0')
menucache = dependency ('libmenu-cache')
x11 = dependency ('x11')
gmodule = dependency ('gmodule-2.0')
libfm = dependency ('libfm-gtk3')
deps = [ gtk, keybind, menucache, x11, gmodule, libfm ]

subdir('plugins')
subdir('src')
subdir('po')
subdir('data')

pkgconfig = import('pkgconfig')
pkgconfig.generate(
    version:      meson.project_version(),
    filebase:     meson.project_name(),
    name:         meson.project_name(),
    description: 'Panel for Raspberry Pi X desktop',
    requires:     gtk,
    subdirs:     'wf-panel-pi',
    variables:    ['uidir=' + ui_dir,
                   'sysconfdir=' + sysconf_dir,
                   'resourcedir=' + resource_dir,
                   'plugindir=${libdir}/lxpanel-pi']
    )
