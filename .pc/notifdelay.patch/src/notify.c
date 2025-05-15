/*
Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "private.h"
#include "panel.h"
#include "plugin.h"

/*----------------------------------------------------------------------------*/
/* Macros and typedefs */
/*----------------------------------------------------------------------------*/

#define TEXT_WIDTH 40
#define SPACING 5

typedef struct {
    GtkWidget *popup;               /* Popup message window*/
    guint hide_timer;               /* Timer to hide message window */
    unsigned int seq;               /* Sequence number */
    guint hash;                     /* Hash of message string */
} NotifyWindow;


/*----------------------------------------------------------------------------*/
/* Global data */
/*----------------------------------------------------------------------------*/

static GList *nwins = NULL;         /* List of current notifications */
static unsigned int nseq = 0;       /* Sequence number for notifications */

/*----------------------------------------------------------------------------*/
/* Function prototypes */
/*----------------------------------------------------------------------------*/

static void show_message (LXPanel *panel, NotifyWindow *nw, char *str);
static gboolean hide_message (NotifyWindow *nw);
static void update_positions (GList *item, int offset);
static gboolean window_click (GtkWidget *widget, GdkEventButton *event, NotifyWindow *nw);

/*----------------------------------------------------------------------------*/
/* Private functions */
/*----------------------------------------------------------------------------*/

/* Calculate position; based on lxpanel_plugin_popup_set_position_helper */

static void notify_position_helper (LXPanel *p, GtkWidget *popup, gint *px, gint *py)
{
    GdkMonitor *monitor;
    GdkRectangle pop_geom, mon_geom, pan_geom;

    /* Get the geometry of the monitor on which the panel is displayed */
    monitor = gdk_display_get_monitor_at_window (gtk_widget_get_display (p->priv->box), gtk_widget_get_window (p->priv->box));
    gdk_monitor_get_geometry (monitor, &mon_geom);

    /* Get the geometry of the panel */
    gdk_window_get_frame_extents (gtk_widget_get_window (p->priv->box), &pan_geom);

    /* Get the geometry of the popup menu */
    gtk_widget_realize (popup);
    gdk_window_get_frame_extents (gtk_widget_get_window (popup), &pop_geom);

    /* By default, notifications go in the top right corner of the monitor with the panel */
    *px = mon_geom.x + mon_geom.width - pop_geom.width;
    *py = mon_geom.y;

    /* Shift if panel is in the way...*/
    if (p->priv->edge == EDGE_TOP) *py += pan_geom.height;
    if (p->priv->edge == EDGE_RIGHT) *px -= pan_geom.width;
}

/* Create a notification window and position appropriately */

static void show_message (LXPanel *panel, NotifyWindow *nw, char *str)
{
    GtkWidget *box, *item;
    GList *plugins;
    gint x, y;
    char *fmt, *cptr;

    /*
     * In order to get a window which looks exactly like a system tooltip, client-side decoration
     * must be requested for it. This cannot be done by any public API call in GTK+3.24, but there is an
     * internal call _gtk_window_request_csd which sets the csd_requested flag in the class' private data.
     * The code below is compatible with a hacked GTK+3 library which uses GTK_WINDOW_POPUP + 1 as the type
     * for a window with CSD requested. It should also not fall over with the standard library...
     */
    nw->popup = gtk_window_new (GTK_WINDOW_POPUP + 1);
    if (!nw->popup) nw->popup = gtk_window_new (GTK_WINDOW_POPUP);
    gtk_window_set_type_hint (GTK_WINDOW (nw->popup), GDK_WINDOW_TYPE_HINT_TOOLTIP);
    gtk_window_set_resizable (GTK_WINDOW (nw->popup), FALSE);

    GtkStyleContext *context = gtk_widget_get_style_context (nw->popup);
    gtk_style_context_add_class (context, GTK_STYLE_CLASS_TOOLTIP);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add (GTK_CONTAINER (nw->popup), box);

    fmt = g_strcompress (str);

    // setting gtk_label_set_max_width_chars looks awful, so we have to do this...
    cptr = fmt;
    x = 0;
    while (*cptr)
    {
        if (*cptr == ' ' && x >= TEXT_WIDTH) *cptr = '\n';
        if (*cptr == '\n') x = 0;
        cptr++;
        x++;
    }

    item = gtk_label_new (fmt);
    gtk_label_set_justify (GTK_LABEL (item), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start (GTK_BOX (box), item, FALSE, FALSE, 0);
    g_free (fmt);

    gtk_widget_show_all (nw->popup);
    gtk_widget_hide (nw->popup);

    notify_position_helper (panel, nw->popup, &x, &y);
    gdk_window_move (gtk_widget_get_window (nw->popup), x, y);

    gdk_window_set_events (gtk_widget_get_window (nw->popup), gdk_window_get_events (gtk_widget_get_window (nw->popup)) | GDK_BUTTON_PRESS_MASK);
    g_signal_connect (G_OBJECT (nw->popup), "button-press-event", G_CALLBACK (window_click), nw);
    gtk_widget_show (nw->popup);
    if (panel->priv->notify_timeout > 0) nw->hide_timer = g_timeout_add (panel->priv->notify_timeout * 1000, (GSourceFunc) hide_message, nw);
}

/* Destroy a notification window and remove from list */

static gboolean hide_message (NotifyWindow *nw)
{
    GList *item;
    int w, h;

    // shuffle notifications below up
    item = g_list_find (nwins, nw);
    gtk_window_get_size (GTK_WINDOW (nw->popup), &w, &h);
    update_positions (item->next, - (h + SPACING));

    if (nw->hide_timer) g_source_remove (nw->hide_timer);
    if (nw->popup) gtk_widget_destroy (nw->popup);
    nwins = g_list_remove (nwins, nw);
    g_free (nw);
    return FALSE;
}

/* Relocate notifications below the supplied item by the supplied vertical offset */

static void update_positions (GList *item, int offset)
{
    NotifyWindow *nw;
    int x, y;

    for (; item != NULL; item = item->next)
    {
        nw = (NotifyWindow *) item->data;
        gdk_window_get_position (gtk_widget_get_window (nw->popup), &x, &y);
        gdk_window_move (gtk_widget_get_window (nw->popup), x, y + offset);
    }
}

/* Handler for mouse click in notification window - closes window */

static gboolean window_click (GtkWidget *widget, GdkEventButton *event, NotifyWindow *nw)
{
    hide_message (nw);
    return FALSE;
}

/*----------------------------------------------------------------------------*/
/* Public API */
/*----------------------------------------------------------------------------*/

unsigned int lxpanel_notify (LXPanel *panel, char *message)
{
    NotifyWindow *nw;
    GList *item;
    int w, h;

    // check for notifications being disabled
    if (!panel->priv->notifications) return 0;

    // check to see if this notification is already in the list - just bump it to the top if so...
    guint hash = g_str_hash (message);

    // loop through windows in the list, looking for the hash
    for (item = nwins; item != NULL; item = item->next)
    {
        // if hash matches, hide the window
        nw = (NotifyWindow *) item->data;
        if (nw->hash == hash) hide_message (nw);
    }

    // create a new notification window and add it to the front of the list
    nw = g_new (NotifyWindow, 1);
    nwins = g_list_prepend (nwins, nw);

    // set the sequence number for this notification
    nseq++;
    if (nseq == -1) nseq++;     // use -1 for invalid sequence code
    nw->seq = nseq;
    nw->hash = hash;

    // show the window
    show_message (panel, nw, message);

    // shuffle existing notifications down
    gtk_window_get_size (GTK_WINDOW (nw->popup), &w, &h);
    update_positions (nwins->next, h + SPACING);

    return nseq;
}

void lxpanel_notify_clear (unsigned int seq)
{
    NotifyWindow *nw;
    GList *item;

    // loop through windows in the list, looking for the sequence number
    for (item = nwins; item != NULL; item = item->next)
    {
        // if sequence number matches, hide the window
        nw = (NotifyWindow *) item->data;
        if (nw->seq == seq)
        {
            hide_message (nw);
            return;
        }
    }
}


/* End of file */
/*----------------------------------------------------------------------------*/
