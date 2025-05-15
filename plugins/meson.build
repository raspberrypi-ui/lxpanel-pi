pluginlib_sources = files(
	'dclock.c',
	'launchtaskbar.c',
	'task-button.c',
	'launch-button.c',
	'separator.c',
	'tray.c'
)

incdir = include_directories('../src')

liblxpanelplugins = static_library('lxpanel_static_plugins', pluginlib_sources,
        dependencies: deps,
        include_directories : incdir
)
