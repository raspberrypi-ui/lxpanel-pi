## Process this file with automake to produce Makefile.in

## common flags for all plugins
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	$(PACKAGE_CFLAGS) \
	$(G_CAST_CHECKS) \
	-Wall

AM_LDFLAGS = \
	-module -avoid-version -shared -export-dynamic \
	-no-undefined -rpath $(libdir)/lxpanel/plugins \
	$(top_builddir)/src/liblxpanel.la \
	$(PACKAGE_LIBS)

BUILT_SOURCES = $(top_builddir)/src/liblxpanel.la

$(top_builddir)/src/liblxpanel.la:
	cd $(top_builddir)/src && $(MAKE) liblxpanel.la

## built-in plugins
noinst_LIBRARIES = libbuiltin_plugins.a

libbuiltin_plugins_a_CFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)/lxpanel"\" \
	-DPACKAGE_UI_DIR=\""$(datadir)/lxpanel/ui"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

PLUGINS_SOURCES = \
	dclock.c \
	launchtaskbar.c \
	task-button.c \
	launch-button.c \
	separator.c \
	tray.c

libbuiltin_plugins_a_SOURCES = \
	$(PLUGINS_SOURCES)

libbuiltin_plugins_a_LIBADD =

EXTRA_DIST = \
	task-button.h \
	launch-button.h \
	icon.xpm

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/lxpanel/plugins/*.la
