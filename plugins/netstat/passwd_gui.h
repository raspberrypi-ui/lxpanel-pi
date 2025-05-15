#ifndef HAVE_NS_PASSWDGUI_H
#define HAVE_NS_PASSWDGUI_H

#include "netstat.h"

struct pgui *passwd_gui_new(ap_setting *aps);
#if !GTK_CHECK_VERSION(3, 0, 0)
void passwd_gui_set_style(struct pgui *pg, GtkStyle *style);
#endif
void passwd_gui_destroy(struct pgui *pg);
#endif
