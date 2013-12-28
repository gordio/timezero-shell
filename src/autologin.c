#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <json.h>

#include "general.h"
#include "window.h"
#include "utils.h"
#include "main.h"
#include "flash.h"

#include "autologin.h"


static GtkWidget *al_window, *al_add_window;
static GtkWidget *al_remove_button, *al_edit_button;
static GtkEntry *login_entry, *pass_entry;
static login_t al_list[MAX_AUTOLOGIN_ITEMS];


static void al_window_create();
static void al_window_move();
static void al_item_activate();

static void al_list_clean();
static void al_list_load();
static void al_list_save();

static void al_item_add();
static void al_item_edit();
static void al_item_remove();

static void al_item_activate_cb();
static void al_item_add_cb();
static void al_item_edit_cb();
static void al_item_remove_cb();
static bool al_move_cb();
static void window_add_item_close_cb();


// API
void
al_window_show(void)
{
	if (al_window) {
		al_window_hide();
	}

	al_window_create();
}

void
al_window_hide(void)
{
	if (al_window) {
		gtk_widget_destroy(al_window);
		al_window = NULL;
	}
}

bool
al_list_update_by_player(player_t *p)
{
	vlog("Update autologin info from chat player info");

	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		// check pointers. make SIGSEG for NULL in strcmp()
		if (!al_list[i].login)
			continue;

		// make both strings lovercase for disable casese
		char *login = g_utf8_strdown(al_list[i].login, -1);
		char *nick = g_utf8_strdown(p->nick, -1);

		// Check and update
		if (strcmp(login, nick) == 0) {
			if (p->clan) {
				if (al_list[i].clan) {
					free(al_list[i].clan);
					al_list[i].clan = NULL;
				}

				al_list[i].clan = g_strdup(p->clan);
			}

			al_list[i].rank = get_rank_num_from_ranks(p->rank);
			al_list[i].level = p->level;

			if (al_list[i].profession) {
				free(al_list[i].profession);
				al_list[i].profession = NULL;
			}

			bool woman = (p->state & 8192) || 0;
			int prof = ((p->state >> 5) & 63);

			char *tmp_mem = malloc(4);
			snprintf(tmp_mem, 3, woman ? "%iw" : "%i", prof);

			al_list[i].profession = tmp_mem;

			al_list_save();

			free(login);
			free(nick);

			return true;
		}

		free(login);
		free(nick);
	}

	return false;
}

void
al_list_print(void)
{
	al_list_load();

	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login) {
			printf("%s\n", al_list[i].login);
		}
	}
}


// Windows
void
al_window_create(void)
{
	vlog("Create autologin window.");

	GtkWidget *widget;
	GtkBox *al_box;
	GtkBox *al_buttons_box;

	al_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(al_window), false);
	gtk_window_set_modal(GTK_WINDOW(al_window), false);
	gtk_window_set_decorated(GTK_WINDOW(al_window), false);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(al_window), true);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(al_window), true);

	GdkGeometry window_hints;
	window_hints.min_width = 180;
	window_hints.min_height = 50;
	gtk_window_set_geometry_hints(GTK_WINDOW(al_window), NULL, &window_hints, GDK_HINT_MIN_SIZE);

	gtk_window_set_focus_on_map(GTK_WINDOW(al_window), true);
	gtk_window_set_transient_for(GTK_WINDOW(al_window), GTK_WINDOW(window));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(al_window), true);
	gtk_container_set_border_width(GTK_CONTAINER(al_window), 2);

	al_box = GTK_BOX(gtk_vbox_new(false, 0));
	gtk_container_add(GTK_CONTAINER(al_window), GTK_WIDGET(al_box));

	// buttons
	al_buttons_box = GTK_BOX(gtk_hbox_new(false, 2));
	gtk_box_pack_start(al_box, GTK_WIDGET(al_buttons_box), false, false, 0);

	al_remove_button = gtk_toggle_button_new();
	gtk_button_add_image(GTK_BUTTON(al_remove_button), "img/button/delete.png");
	gtk_widget_set_tooltip_text(al_remove_button, _("Remove"));
	g_signal_connect(G_OBJECT(al_remove_button), "clicked", G_CALLBACK(al_item_remove_cb), NULL);
	gtk_box_pack_start(al_buttons_box, GTK_WIDGET(al_remove_button), false, false, 0);

	al_edit_button = gtk_toggle_button_new();
	gtk_button_add_image(GTK_BUTTON(al_edit_button), "img/button/edit.png");
	gtk_widget_set_tooltip_text(al_edit_button, _("Edit"));
	g_signal_connect(G_OBJECT(al_edit_button), "clicked", G_CALLBACK(al_item_edit_cb), NULL);
	gtk_box_pack_start(al_buttons_box, GTK_WIDGET(al_edit_button), false, false, 0);

	// Add button
	widget = gtk_button_new();
	gtk_button_set_label(GTK_BUTTON(widget), _("Add"));
	gtk_button_add_image(GTK_BUTTON(widget), "img/button/add.png");
	gtk_widget_set_tooltip_text(widget, _("Add"));
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(al_item_add_cb), NULL);
	gtk_box_pack_end(al_buttons_box, widget, false, false, 0);


	al_list_load();

	// autologin buttons
	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login) {
			widget = gtk_button_new();

			gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
			gtk_button_set_focus_on_click(GTK_BUTTON(widget), false);

			gtk_container_add(GTK_CONTAINER(widget), al_list_widget_create(&al_list[i]));

			g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(&al_item_activate_cb), &al_list[i]);

			gtk_box_pack_start(GTK_BOX(al_box), widget, false, false, 2);
		}
	}

	al_window_move();
	gtk_widget_show_all(GTK_WIDGET(al_window));
	al_window_move();

	g_signal_connect(G_OBJECT(window), "event", G_CALLBACK(&al_move_cb), NULL);

	// Auth on start
	if (default_autologin) {
		for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS && al_list[i].login; i++) {
			// create both strings lovercase
			char *login = g_utf8_strdown(al_list[i].login, -1);
			char *nick = g_utf8_strdown(default_autologin, -1);

			if (strcmp(login, nick) == 0) {
				ilog("Activate autologin item: %s", al_list[i].login);
				al_item_activate(&al_list[i]);
				al_window_hide();

				free(login);
				free(nick);

				return;
			} else {
				free(login);
				free(nick);
			}
		}
	}
}

static void
al_window_move(void)
{
	if (!al_window)
		return;

	vlog("Move autologin window");

	int root_x, root_y, root_w, root_h;
	int x, y, w, h;

	gtk_window_get_position(GTK_WINDOW(window), &root_x, &root_y);
	gtk_window_get_size(GTK_WINDOW(window), &root_w, &root_h);
	gtk_window_get_size(GTK_WINDOW(al_window), &w, &h);

	x = root_x;
	y = root_y;
	y += root_h / 2; // window center
	y -= h / 2;      // login window center

	gtk_window_move(GTK_WINDOW(al_window), x, y);
}

static void
al_item_window_create(login_t *l)
{
	vlog("Create autologin window - add/edit item");

	GtkWidget *w;
	GtkBox *vbox;

	al_add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// если передан логин то редактирование
	gtk_window_set_title(GTK_WINDOW(al_add_window), l ? _("Edit") : _("Add"));
	gtk_window_set_position(GTK_WINDOW(al_add_window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(al_add_window), false);
	gtk_window_set_modal(GTK_WINDOW(al_add_window), true);
	gtk_window_set_transient_for(GTK_WINDOW(al_add_window), GTK_WINDOW(al_window));
	gtk_container_set_border_width(GTK_CONTAINER(al_add_window), 5);


	vbox = GTK_BOX(gtk_vbox_new(false, 2));

	gtk_container_add(GTK_CONTAINER(al_add_window), GTK_WIDGET(vbox));


	// Login
	w = gtk_label_new(_("Login:*"));
	gtk_label_set_justify(GTK_LABEL(w), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(vbox, w, false, true, 0);

	login_entry = GTK_ENTRY(gtk_entry_new());

	if (l) {
		gtk_entry_set_text(login_entry, l->login);
	}

	gtk_box_pack_start(vbox, GTK_WIDGET(login_entry), false, true, 0);


	// Password
	w = gtk_label_new(_("Password:"));
	gtk_label_set_justify(GTK_LABEL(w), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(vbox, w, false, true, 0);

	pass_entry = GTK_ENTRY(gtk_entry_new());

	if (l && l->password) {
		gtk_entry_set_visibility(pass_entry, false);
		gtk_entry_set_text(pass_entry, l->password);
	}

	gtk_box_pack_start(vbox, GTK_WIDGET(pass_entry), false, true, 0);


	GtkSeparator *hsep = GTK_SEPARATOR(gtk_hseparator_new());
	gtk_box_pack_start(vbox, GTK_WIDGET(hsep), false, false, 0);

	// Buttons layout box
	GtkButtonBox *hbbox = GTK_BUTTON_BOX(gtk_hbutton_box_new());

	gtk_button_box_set_layout(hbbox, GTK_BUTTONBOX_END);
	gtk_box_set_spacing(GTK_BOX(hbbox), 5);
	gtk_box_pack_end(vbox, GTK_WIDGET(hbbox), true, false, 2);

	// cancel (close window)
	w = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&window_add_item_close_cb), NULL);
	gtk_box_pack_end(GTK_BOX(hbbox), w, true, false, 0);

	// add item
	w = gtk_button_new_from_stock(GTK_STOCK_OK);

	if (l && l->login) {
		// update item
		g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&al_item_edit), l);
	} else {
		// add item
		g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&al_item_add), NULL);
	}

	gtk_box_pack_end(GTK_BOX(hbbox), w, false, false, 50);

	// show window
	gtk_widget_show_all(al_add_window);
}


inline void
al_item_activate(login_t *login_item)
{
	vlog("Activite autologin item '%s'", login_item->login);

	tz_autoauth(login_item->login, login_item->password);
}

static void
al_item_add(GtkButton *b, gpointer user_data)
{
	vlog("Add autologin item.");

	const char *tmp;
	login_t *login_item = NULL;

	// check exist
	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login && strcmp(al_list[i].login, gtk_entry_get_text(login_entry)) == 0) {
			wlog("Login exist. Choice new or remove old.");
			return;
		}
	}

	// find empty pointer (slot)
	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (!al_list[i].login) {
			login_item = &al_list[i];
			break;
		}
	}

	// check max logins
	if (!login_item) {
		elog("Don't exist empty slot.");
		return;
	}

	// login
	tmp = gtk_entry_get_text(login_entry);

	if (strcmp(tmp, "") == 0) {
		elog(_("Login is empty"));
		return;
	}

	login_item->login = strdup(tmp);

	// password
	tmp = gtk_entry_get_text(pass_entry);

	if (strcmp(tmp, "") == 0) {
		wlog(_("Password is empty!"));
	} else {
		login_item->password = strdup(tmp);
	}

	al_list_save();


	gtk_widget_destroy(al_add_window);
	al_window_show();
}

static void
al_item_edit(GtkButton *b, login_t *login_item)
{
	vlog("Edit autologin item '%s'", login_item->login);

	const char *tmp;

	// login
	tmp = gtk_entry_get_text(login_entry);

	if (login_item && login_item->login) {
		free(login_item->login);
		login_item->login = NULL;
	} else {
		elog("Item '%s' don't exist.");

		return;
	}

	login_item->login = strdup(tmp);

	// password
	tmp = gtk_entry_get_text(pass_entry);

	if (login_item->password) {
		free(login_item->password);
		login_item->password = NULL;
	}

	if (strcmp(tmp, "") == 0) {
		wlog(_("Password is empty!"));
	} else {
		login_item->password = strdup(tmp);
	}


	al_list_save();

	// all done
	gtk_widget_destroy(al_add_window);
	al_window_show();
}

static void
al_item_remove(login_t *login_item)
{
	vlog("Remove login '%s'", login_item->login);

	if (login_item->login) {
		free(login_item->login);
		login_item->login = NULL;
	}

	if (login_item->password) {
		free(login_item->password);
		login_item->password = NULL;
	}

	if (login_item->profession) {
		free(login_item->profession);
		login_item->profession = NULL;
	}

	if (login_item->clan) {
		free(login_item->clan);
		login_item->clan = NULL;
	}

	if (login_item->rank) {
		login_item->rank = 0;
	}
}


static void
al_list_clean(void)
{
	vlog("Clean autologin list.");

	for (unsigned int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login) {
			free(al_list[i].login);
			al_list[i].login = NULL;
		}

		if (al_list[i].password) {
			free(al_list[i].password);
			al_list[i].password = NULL;
		}

		if (al_list[i].profession) {
			free(al_list[i].profession);
			al_list[i].profession = NULL;
		}

		if (al_list[i].clan) {
			free(al_list[i].clan);
			al_list[i].clan = NULL;
		}

		if (al_list[i].rank) {
			al_list[i].rank = 0;
		}
	}
}

static void
al_list_load(void)
{
	vlog("Load autologins from file.");

	json_object *jso, *jso_login_array, *jso_login_item, *tmp;

	al_list_clean();

	if (!g_file_test(AL_FILE_NAME, G_FILE_TEST_EXISTS)) {
		ilog(_("Create empty autologins file."));
		al_list_save();

		return;
	}

	jso = json_object_from_file(AL_FILE_NAME);
	jso_login_array = json_object_object_get(jso, "items");

	for (int i = 0; i < json_object_array_length(jso_login_array); ++i) {
		jso_login_item = json_object_array_get_idx(jso_login_array, i);

		for (int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
			if (!al_list[i].login) {

				// Login
				tmp = json_object_object_get(jso_login_item, "login");

				if (!tmp) {
					elog("Can't parse autologin item [%i]", i);
					break;
				}

				al_list[i].login = strdup(json_object_get_string(tmp));

				// Password
				tmp = json_object_object_get(jso_login_item, "password");

				if (tmp) {
					al_list[i].password = strdup(json_object_get_string(tmp));
				}

				// Level
				tmp = json_object_object_get(jso_login_item, "level");

				if (!tmp) {
					al_list[i].level = 0;
				} else {
					al_list[i].level = json_object_get_int(tmp);
				}

				// Rank
				tmp = json_object_object_get(jso_login_item, "rank");

				if (!tmp) {
					al_list[i].rank = 0;
				} else {
					al_list[i].rank = json_object_get_int(tmp);
				}

				// Job
				tmp = json_object_object_get(jso_login_item, "profession");

				if (tmp) {
					al_list[i].profession = strdup(json_object_get_string(tmp));
				}

				// Clan
				tmp = json_object_object_get(jso_login_item, "clan");

				if (tmp) {
					al_list[i].clan = strdup(json_object_get_string(tmp));
				}

				break;
			}
		}
	}

	json_object_put(jso);
}

static void
al_list_save(void)
{
	vlog("Save autologins to file.");

	int elements = 0;
	json_object *jso, *jso_login_array, *jso_login_item;

	jso = json_object_new_object();
	jso_login_array = json_object_new_array();

	for (int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login) {
			jso_login_item = json_object_new_object();

			json_object_object_add(jso_login_item, "login", json_object_new_string(al_list[i].login));

			if (al_list[i].password) {
				json_object_object_add(jso_login_item, "password", json_object_new_string(al_list[i].password));
			}

			if (al_list[i].clan) {
				json_object_object_add(jso_login_item, "clan", json_object_new_string(al_list[i].clan));
			}

			if (al_list[i].level != 0) {
				json_object_object_add(jso_login_item, "level", json_object_new_int(al_list[i].level));
			}

			if (al_list[i].profession) {
				json_object_object_add(jso_login_item, "profession", json_object_new_string(al_list[i].profession));
			}

			if (al_list[i].rank != 0) {
				json_object_object_add(jso_login_item, "rank", json_object_new_int(al_list[i].rank));
			}

			json_object_array_put_idx(jso_login_array, elements, jso_login_item);
			elements++;
		}
	}

	json_object_object_add(jso, "items", jso_login_array);

	json_object_to_file(AL_FILE_NAME, jso);
	json_object_put(jso);
}


static bool
al_move_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
	switch (e->type) {
	case GDK_CONFIGURE:
		al_window_move();
		return false;

	default:
		return false;
	}
}

static void
al_item_activate_cb(GtkWidget *w, login_t *login_item)
{
	vlog("Activate button callback");

	// if EDIT mode enabled
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_edit_button))) {
		al_item_window_create(login_item);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_edit_button), false);

		return;
	}

	// if REMOVE mode enabled
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_remove_button))) {
		al_item_remove(login_item);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_remove_button), false);

		al_list_save();
		al_window_show();

		return;
	}

	al_item_activate(login_item);
}

static void
window_add_item_close_cb(GtkWidget *w)
{
	gtk_widget_destroy(al_add_window);
}

static void
al_item_add_cb(GtkButton *b, gpointer user_data)
{
	al_item_window_create(NULL);
}

static void
al_item_edit_cb(GtkButton *b, gpointer user_data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_edit_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_remove_button), false);
	}
}

static void
al_item_remove_cb(GtkButton *b, gpointer user_data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_remove_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_edit_button), false);
	}
}
