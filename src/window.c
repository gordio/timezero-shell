/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include "general.h"
#include "main.h"
#include "flash.h"
#include "chat.h"
#include "events.h"
#include "tz-interface.h"

GtkWidget *window = NULL;
GtkWidget *main_panels = NULL, *flash_panel = NULL;

bool window_fullscreen = false;

#include "window.h"

void
create_main_window(void)
{
	vlog("Create general window");

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "TimeZero Linux Client");
	gtk_window_set_default_icon_name("timezero");

	if (!no_theme) {
		g_object_set(gtk_widget_get_settings(window), "gtk-theme-name", "TimeZero", NULL);
	}

	// Load window minimum sizes table
	GdkGeometry window_hints;
	window_hints.min_width = MIN_WINDOW_WIDTH;
	window_hints.min_height = MIN_WINDOW_HEIGHT;
	gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL, &window_hints, GDK_HINT_MIN_SIZE);
	// Set default (TODO saved) window size
	gtk_window_set_default_size(GTK_WINDOW(window), MIN_WINDOW_WIDTH, 650);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(&ev_destroy), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(&keypress_cb), NULL);
	// FIXME: Normal work
	// g_signal_connect(G_OBJECT(window), "check-resize", G_CALLBACK(&resize_flash_cb), NULL);

	// Create vertical stack
	main_panels = gtk_vpaned_new();
	gtk_container_add(GTK_CONTAINER(window), main_panels);

	// Create flash frame
	flash_panel = create_flash_frame();
	gtk_paned_pack1(GTK_PANED(main_panels), flash_panel, false, false);
	// Send update size to flash
	g_signal_connect(G_OBJECT(flash_panel), "size-allocate", &flash_resize_cb, NULL);

	if (fullscreen) {
		gtk_window_fullscreen(GTK_WINDOW(window));
	}

	// Set default (TODO saved) flash frame size
	gtk_widget_set_size_request(GTK_WIDGET(flash_panel), -1, 440);

	gtk_widget_show_all(GTK_WIDGET(window));

	gtk_widget_grab_focus(GTK_WIDGET(flash_panel));

	return;
}

void fullscreen_toggle()
{
	int panel_size = -1;
	if (!window_fullscreen) {
		window_fullscreen = true;
		// TODO: save frame size
		panel_size = gtk_paned_get_position(GTK_PANED(main_panels));

		gtk_window_fullscreen(GTK_WINDOW(window));
		// TODO: load frame size
		panel_size = gtk_paned_get_position(GTK_PANED(main_panels));

		tzFlashResize();
	} else {
		window_fullscreen = false;
		// TODO: save frame size
		panel_size = gtk_paned_get_position(GTK_PANED(main_panels));

		gtk_window_unfullscreen(GTK_WINDOW(window));
		// TODO: load frame size
		panel_size = gtk_paned_get_position(GTK_PANED(main_panels));

		tzFlashResize();
	}
}

void tab_refresh()
{
	tzLogout();
}

void ev_destroy(GtkWidget *w, gpointer data)
{
	printf("%i\n", gtk_paned_get_position(GTK_PANED(main_panels)));
	// TODO: Save to config

	gtk_main_quit();
}
