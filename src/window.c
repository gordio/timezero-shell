#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "general.h"
#include "main.h"
#include "flash.h"
#include "chat.h"
#include "events.h"
#include "conf.h"

GtkWindow *window = NULL;
GtkWidget *main_panels = NULL, *flash_panel = NULL;

bool window_fullscreen = false;

#include "window.h"


/* GTK {{{ */
void
create_main_window(void)
{
	vlog("Create general window");

	// Init window
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_title(window, "TimeZero Linux Client");
	gtk_window_set_default_icon_name("timezero");
	gtk_window_set_role(window, "main");


	// Запрещаем слишком уменьшаять окно
	GdkGeometry window_hints;
	window_hints.min_width = MIN_WINDOW_WIDTH;
	window_hints.min_height = MIN_WINDOW_HEIGHT;
	gtk_window_set_geometry_hints(window, NULL, &window_hints, GDK_HINT_MIN_SIZE);


	// Get size & position from config
	int top, left, width, height;
	char *method;
	conf_get_int("window.top", &top);
	conf_get_int("window.left", &left);
	conf_get_int("window.width", &width);
	conf_get_int("window.height", &height);
	conf_get("window.pos_method", &method);

	gtk_window_set_default_size(window, MIN_WINDOW_WIDTH, 650);

	if (strcmp(method, "save") == 0 && top != -1 && left != -1) {
		// устанавливаем сохраненные размеры и позицию
		gtk_window_move(window, top, left);
		gtk_window_resize(window, width, height);
	} else {
		gtk_window_set_position(window, GTK_WIN_POS_CENTER);
	}

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(&destroy_cb), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(&keypress_cb), NULL);
	g_signal_connect(G_OBJECT(window), "event", G_CALLBACK(&event_cb), NULL);

	// Create vertical stack
	main_panels = gtk_vpaned_new();
	gtk_container_add(GTK_CONTAINER(window), main_panels);

	// Create flash frame
	flash_panel = create_flash_frame();
	gtk_paned_pack1(GTK_PANED(main_panels), flash_panel, false, false);

	gtk_widget_set_size_request(GTK_WIDGET(flash_panel), -1, 440);

	int panel_size = -1;

	if (fullscreen) {
		window_fullscreen = true;
		gtk_window_fullscreen(window);

		// Загружаем с конфига размер флеш панели
		if (conf_get_int("flash.fullscreen_height", &panel_size))
			gtk_paned_set_position(GTK_PANED(main_panels), panel_size);
	} else {
		if (conf_get_int("flash.windowed_height", &panel_size))
			gtk_paned_set_position(GTK_PANED(main_panels), panel_size);
	}

	// после установки, дабы предотвратить ранний вызов обновления
	g_signal_connect(G_OBJECT(flash_panel), "size-allocate", G_CALLBACK(&flash_resize_cb), NULL);


	gtk_widget_show_all(GTK_WIDGET(window));

	gtk_widget_grab_focus(GTK_WIDGET(flash_panel));

	return;
}

void fullscreen_toggle()
{
	int panel_size = -1;

	if (!window_fullscreen) {
		window_fullscreen = true;

		// Только если это не Login screen
		if (!tz_fullscreen) {
			// Сохраняем в конфиг
			panel_size = gtk_paned_get_position(GTK_PANED(main_panels));
			conf_set_int("flash.windowed_height", panel_size);
		}

		gtk_window_fullscreen(GTK_WINDOW(window));

		if (!tz_fullscreen) {
			// Загружаем с конфига
			if (conf_get_int("flash.fullscreen_height", &panel_size)) {
				gtk_paned_set_position(GTK_PANED(main_panels), panel_size);
			}
		}
	} else {
		window_fullscreen = false;

		if (!tz_fullscreen) {
			panel_size = gtk_paned_get_position(GTK_PANED(main_panels));
			conf_set_int("flash.fullscreen_height", panel_size);
		}

		gtk_window_unfullscreen(GTK_WINDOW(window));

		if (!tz_fullscreen) {
			if (conf_get_int("flash.windowed_height", &panel_size)) {
				gtk_paned_set_position(GTK_PANED(main_panels), panel_size);
			}
		}
	}
}

void
tab_refresh()
{
	tz_logout();
}

void
update_position_config()
{
	int top, left, width, height;

	gtk_window_get_size(window, &width, &height);
	gtk_window_get_position(window, &top, &left);

	conf_set_int("window.top", top);
	conf_set_int("window.left", left);
	conf_set_int("window.width", width);
	conf_set_int("window.height", height);
	vlog("Position saved: top = %i, left = %i, width = %i, height = %i;",
	     top, left, width, height);
}
/* }}} */

/* CALLBACKS {{{ */
bool event_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
	switch (e->type) {
	case GDK_CONFIGURE:
		if (!window_fullscreen) {
			update_position_config();
		}

		return false;

	default:
		return false;
	}
}

bool keypress_cb(GtkWidget *w, GdkEventKey *e, gpointer p)
{
	/*if (event->state == (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {
		if (gdk_keyval_to_lower(event->keyval) == GDK_x) {
			return true;
		}
	}*/
	if (e->state & GDK_MOD1_MASK) {
		switch (e->keyval) {
		case GDK_1:
			chat_set_tab(1);
			return true;

		case GDK_2:
			chat_set_tab(2);
			return true;

		case GDK_3:
			chat_set_tab(3);
			return true;

		case GDK_4:
			chat_set_tab(4);
			return true;

		case GDK_5:
			chat_set_tab(5);
			return true;

		default:
			return false;
		}
	} else {
		switch (e->keyval) {
		case GDK_F11:
			fullscreen_toggle();
			return true;

		case GDK_F5:
			tab_refresh();
			return true;

		case GDK_F12:
			return true;

		default:
			return false;
		}
	}
}

static int old_size = 0;

void flash_resize_cb(GtkContainer *c, gpointer p)
{
	int cur_size = gtk_paned_get_position(GTK_PANED(main_panels));

	if (cur_size != 0 && old_size != cur_size) {
		old_size = cur_size;
		vlog("Send signal to flash -> update_size");
		tz_flash_resize();
	}
}

// При уничтожении главного окна
void destroy_cb(GtkWidget *w, gpointer o)
{
	int flash_size = gtk_paned_get_position(GTK_PANED(main_panels));

	// только если это не Login screen
	if (!tz_fullscreen) {
		if (window_fullscreen) {
			conf_set_int("flash.fullscreen_height", flash_size);
		} else {
			conf_set_int("flash.windowed_height", flash_size);
		}
	}


	conf_save();

	gtk_main_quit();
}
/* }}} */

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
