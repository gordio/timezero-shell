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


tzLogin al_list[MAX_AUTOLOGIN_ITEMS];
GtkWidget *al_widget[MAX_AUTOLOGIN_ITEMS];

GtkWidget *al_window, *al_add_window;
GtkWidget *al_image_proff, *al_image_clan, *al_image_rank;
GtkBox *al_hbox, *al_box, *al_buttons_box;

GtkWidget *al_remove_button, *al_edit_button;

GtkEntry *login_entry, *pass_entry;
GtkWidget *file_chooser;


static void al_window_create();
static void al_window_move();
static void al_move_cb();

static void al_list_clean();
static void al_list_buttons_redraw();
static void al_list_load();
static void al_list_save();

static void add_window_add_item();
static void add_window_edit_item();
static void al_item_remove();
static void al_item_activate();

static void al_item_activate_cb();
static void al_item_add_cb();
static void al_item_edit_cb();
static void al_item_remove_cb();
static void window_add_item_close_cb();


void
al_window_create(void)
{
	vlog("Create autologin window.");

	GtkButton *button;

	al_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(al_window), false);
	gtk_window_set_modal(GTK_WINDOW(al_window), false);
	gtk_window_set_decorated(GTK_WINDOW(al_window), false);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(al_window), true);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(al_window), true);

	GdkGeometry window_hints;

	window_hints.min_width = 160;
	window_hints.min_height = 50;
	gtk_window_set_geometry_hints(GTK_WINDOW(al_window), NULL, &window_hints, GDK_HINT_MIN_SIZE);

	// gtk_window_set_urgency_hint(al_window, false);
	gtk_window_set_focus_on_map(GTK_WINDOW(al_window), true);
	gtk_window_set_transient_for(GTK_WINDOW(al_window), GTK_WINDOW(window));
	gtk_container_set_border_width(GTK_CONTAINER(al_window), 2);

	al_box = GTK_BOX(gtk_vbox_new(false, 0));
	gtk_container_add(GTK_CONTAINER(al_window), GTK_WIDGET(al_box));

	// buttons
	al_buttons_box = GTK_BOX(gtk_hbox_new(false, 2));
	gtk_box_pack_start(al_box, GTK_WIDGET(al_buttons_box), false, false, 0);

	al_remove_button = image_toggle_button_new("img/button/delete.png", "", _("Remove"), &al_item_remove_cb);
	gtk_box_pack_start(al_buttons_box, GTK_WIDGET(al_remove_button), false, false, 0);

	al_edit_button = image_toggle_button_new("img/button/edit.png", "", _("Edit"), &al_item_edit_cb);
	gtk_box_pack_start(al_buttons_box, GTK_WIDGET(al_edit_button), false, false, 0);

	button = GTK_BUTTON(image_button_new("img/button/add.png", _("Add"), _("Add"), &al_item_add_cb));
	gtk_box_pack_end(al_buttons_box, GTK_WIDGET(button), false, false, 0);


	al_list_load();
	al_list_buttons_redraw();

	g_signal_connect(G_OBJECT(window), "check-resize", G_CALLBACK(&al_move_cb), NULL);
	g_signal_connect(G_OBJECT(window), "event", G_CALLBACK(&al_move_cb), NULL);

	// check automatic logon with autologin
	if (default_autologin) {
		for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS && al_list[i].login; i++) {
			// create both strings lovercase
			char *login = g_utf8_strdown(al_list[i].login, -1);
			char *nick = g_utf8_strdown(default_autologin, -1);

			if (strcmp(login, nick) == 0) {
				ilog("Activate autologin item: %s", al_list[i].login);
				al_item_activate(&al_list[i]);

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

void
al_window_show(void)
{
	if (!al_window) {
		al_window_create();
	}

	if (al_window) {
		al_window_move();
		gtk_widget_show_all(al_window);
		al_window_move();
	}
}

void
al_window_hide(void)
{
	if (al_window) {
		gtk_widget_hide(al_window);
	}
}

bool
al_list_update_by_player(tzPlayer *p)
{
	vlog("Update autologin info from chat player info");

	for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		// check pointers. make SIGSEG for NULL in strcmp()
		if (!al_list[i].login) {
			continue;
		}

		// make both strings lovercase for disable casese
		char *login = g_utf8_strdown(al_list[i].login, -1);
		char *nick = g_utf8_strdown(p->nick, -1);

		// Check and update
		if (strcmp(login, nick) == 0) {
			if (p->clan) {
				if (al_list[i].clan) {
					free(al_list[i].clan);
				}
				al_list[i].clan = g_strdup(p->clan);
			}

			al_list[i].rank = get_rank_num_from_ranks(p->rank);
			al_list[i].level = p->level;

			if (al_list[i].profession) {
				free(al_list[i].profession);
			}

			bool woman = (p->state & 8192) || 0;
			int prof = ((p->state >> 5) & 63);

			char *tmp_mem = malloc(4);
			if (woman) {
				snprintf(tmp_mem, 3, "%iw", prof);
			} else {
				snprintf(tmp_mem, 3, "%i", prof);
			}

			al_list[i].profession = tmp_mem;

			al_list_buttons_redraw();
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

static void
al_item_window(tzLogin *l)
{
	vlog("Create autologin window - add/edit item");

	GtkWidget *w;
	GtkBox *vbox;

	al_add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if (!l) {
		gtk_window_set_title(GTK_WINDOW(al_add_window), _("Add autologin item"));
	} else {
		gtk_window_set_title(GTK_WINDOW(al_add_window), _("Edit autologin item"));
	}
	gtk_window_set_position(GTK_WINDOW(al_add_window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(al_add_window), false);
	gtk_window_set_modal(GTK_WINDOW(al_add_window), true);
	gtk_window_set_focus_on_map(GTK_WINDOW(al_add_window), false);
	gtk_window_set_transient_for(GTK_WINDOW(al_add_window), GTK_WINDOW(al_window));
	gtk_container_set_border_width(GTK_CONTAINER(al_add_window), 5);


	vbox = GTK_BOX(gtk_vbox_new(false, 2));

	gtk_container_add(GTK_CONTAINER(al_add_window), GTK_WIDGET(vbox));

	// Login
	w = gtk_label_new(_("Login:"));
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

	// Key file
	w = gtk_label_new(_("Key file:"));
	gtk_label_set_justify(GTK_LABEL(w), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(vbox, w, false, true, 0);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.swf");
	gtk_file_filter_add_mime_type(filter, "application/x-shockwave-flash");
	gtk_file_filter_set_name(filter, _("TimeZero Secret Key"));

	file_chooser = gtk_file_chooser_button_new(_("Choose player Secret Key"), GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), filter);
	if (l && l->key_file_path) {
		gtk_file_chooser_set_uri(GTK_FILE_CHOOSER(file_chooser), l->key_file_path);
	}
	gtk_box_pack_start(vbox, file_chooser, false, true, 0);


	GtkSeparator *hsep = GTK_SEPARATOR(gtk_hseparator_new());
	gtk_box_pack_start(vbox, GTK_WIDGET(hsep), false, false, 0);

	// Buttons layout box
	GtkButtonBox *hbbox = GTK_BUTTON_BOX(gtk_hbutton_box_new());

	gtk_button_box_set_layout(hbbox, GTK_BUTTONBOX_END);
	gtk_box_set_spacing(GTK_BOX(hbbox), 5);
	gtk_box_pack_end(vbox, GTK_WIDGET(hbbox), true, false, 2);

	// add item
	w = gtk_button_new_from_stock(GTK_STOCK_OK);
	if (l && l->login) {
		// update item
		g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&add_window_edit_item), l);
	} else {
		// add item
		g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&add_window_add_item), NULL);
	}
	gtk_box_pack_end(GTK_BOX(hbbox), w, false, false, 50);

	// cancel (close window)
	w = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(w), "clicked", G_CALLBACK(&window_add_item_close_cb), NULL);
	gtk_box_pack_end(GTK_BOX(hbbox), w, true, false, 0);

	gtk_widget_show_all(al_add_window);
}

static void
add_window_add_item(GtkButton *b, gpointer user_data)
{
	vlog("Add autologin item.");

	const char *tmp;
	tzLogin *login_item = NULL;

	// check exist
	for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login && strcmp(al_list[i].login, gtk_entry_get_text(login_entry)) == 0) {
			wlog("Login exist. Choice new or remove old.");

			return;
		}
	}

	// find empty pointer (slot)
	for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
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

	// key_file
	tmp = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(file_chooser));
	if (tmp) {
		login_item->key_file_path = strdup(tmp);
	} else {
		login_item->key_file_path = NULL;
	}

	// all done
	al_list_buttons_redraw();
	al_list_save();

	gtk_widget_destroy(al_add_window);

	al_list_buttons_redraw();
	return;
}

static void
add_window_edit_item(GtkButton *b, tzLogin *login_item)
{
	vlog("Edit autologin item '%s'", login_item->login);

	const char *tmp;

	// login
	tmp = gtk_entry_get_text(login_entry);
	if (login_item && login_item->login) {
		free(login_item->login);
	} else {
		elog("Item '%s' don't exist.");

		return;
	}

	login_item->login = strdup(tmp);

	// password
	tmp = gtk_entry_get_text(pass_entry);
	if (login_item->password) {
		free(login_item->password);
	}
	if (strcmp(tmp, "") == 0) {
		wlog(_("Password is empty!"));
		login_item->password = NULL;
	} else {
		login_item->password = strdup(tmp);
	}

	// key_file
	tmp = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(file_chooser));
	if (tmp) {
		login_item->key_file_path = strdup(tmp);
	} else {
		login_item->key_file_path = NULL;
	}


	al_list_buttons_redraw();
	al_list_save();

	// all done
	gtk_widget_destroy(al_add_window);
}

static void
al_item_add_cb(GtkButton *b, gpointer user_data)
{
	al_item_window(NULL);
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

static void
al_item_remove(tzLogin *login_item)
{
	vlog("Remove login '%s", login_item->login);

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
	if (login_item->key_file_path) {
		free(login_item->key_file_path);
		login_item->key_file_path = NULL;
	}
}


static void
al_list_clean(void)
{
	vlog("Clean autologin list.");

	for (uint i = 0; i < countof(al_list); ++i) {
		if (al_list[i].login != NULL) {
			free(al_list[i].login);
		}
		if (al_list[i].password != NULL) {
			free(al_list[i].password);
		}
		if (al_list[i].profession != NULL) {
			free(al_list[i].profession);
		}
		if (al_list[i].clan != NULL) {
			free(al_list[i].clan);
		}
		if (al_list[i].key_file_path != NULL) {
			free(al_list[i].key_file_path);
		}
	}
}

static void
al_list_buttons_redraw(void)
{
	vlog("Redraw autologin buttons.");

	GtkWidget *button;

	// clean autologin widgets
	for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_widget[i]) {
			gtk_widget_destroy(al_widget[i]);
			al_widget[i] = NULL;
		}
	}

	// Recreate autologin button widgets
	for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login) {
			button = gtk_button_new();

			// Find "empty button widget"
			for (uint i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
				if (!al_widget[i]) {
					al_widget[i] = button;
					vlog("Can't find empty button");
					break;
				}
			}

			gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
			gtk_button_set_focus_on_click(GTK_BUTTON(button), false);

			gtk_container_add(GTK_CONTAINER(button), GTK_WIDGET(al_list_widget_create(&al_list[i])));

			g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(&al_item_activate_cb), &al_list[i]);

			gtk_box_pack_start(GTK_BOX(al_box), button, false, false, 2);
		}
	}

	gtk_widget_show_all(GTK_WIDGET(al_box));
	al_window_move();
}

static void
al_list_load(void)
{
	vlog("Load autologins from file.");

	json_object *jso, *jso_login_array, *jso_login_item, *tmp;

	al_list_clean();

	if (!g_file_test(AL_FILE_NAME, G_FILE_TEST_EXISTS)) {
		ilog(_("Create autologins file."));
		al_list_save();
		return;
	}

	jso = json_object_from_file(AL_FILE_NAME);
	jso_login_array = json_object_object_get(jso, "items");

	for (int i = 0; i < json_object_array_length(jso_login_array); ++i) {
		jso_login_item = json_object_array_get_idx(jso_login_array, i);

		for (int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
			if (al_list[i].login == NULL) {

				// Login
				tmp = json_object_object_get(jso_login_item, "login");
				if (tmp == NULL) {
					elog("Can't parse autologin item [%i]", i);
					break;
				}
				al_list[i].login = strdup(json_object_get_string(tmp));

				// Password
				tmp = json_object_object_get(jso_login_item, "password");
				if (tmp != NULL) {
					al_list[i].password = strdup(json_object_get_string(tmp));
				}

				// Secret Key
				tmp = json_object_object_get(jso_login_item, "key_file");
				if (tmp != NULL) {
					al_list[i].key_file_path = strdup(json_object_get_string(tmp));
				}

				// Level
				tmp = json_object_object_get(jso_login_item, "level");
				if (tmp == NULL) {
					al_list[i].level = 0;
				} else {
					al_list[i].level = json_object_get_int(tmp);
				}

				// Rank
				tmp = json_object_object_get(jso_login_item, "rank");
				if (tmp == NULL) {
					al_list[i].rank = 0;
				} else {
					al_list[i].rank = json_object_get_int(tmp);
				}

				// Job
				tmp = json_object_object_get(jso_login_item, "profession");
				if (tmp != NULL) {
					al_list[i].profession = strdup(json_object_get_string(tmp));
				}

				// Clan
				tmp = json_object_object_get(jso_login_item, "clan");
				if (tmp != NULL) {
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

	json_object *jso, *jso_login_array, *jso_login_item;

	jso = json_object_new_object();
	jso_login_array = json_object_new_array();

	for (int i = 0; i < MAX_AUTOLOGIN_ITEMS; ++i) {
		if (al_list[i].login != NULL) {
			jso_login_item = json_object_new_object();
			json_object_object_add(jso_login_item, "login", json_object_new_string(al_list[i].login));
			if (al_list[i].password != NULL) {
				json_object_object_add(jso_login_item, "password", json_object_new_string(al_list[i].password));
			}
			if (al_list[i].key_file_path != NULL) {
				json_object_object_add(jso_login_item, "key_file_path", json_object_new_string(al_list[i].key_file_path));
			}
			if (al_list[i].clan != NULL) {
				json_object_object_add(jso_login_item, "clan", json_object_new_string(al_list[i].clan));
			}
			if (al_list[i].level != 0) {
				json_object_object_add(jso_login_item, "level", json_object_new_int(al_list[i].level));
			}
			if (al_list[i].profession != NULL) {
				json_object_object_add(jso_login_item, "profession", json_object_new_string(al_list[i].profession));
			}
			if (al_list[i].rank != 0) {
				json_object_object_add(jso_login_item, "rank", json_object_new_int(al_list[i].rank));
			}
			json_object_array_put_idx(jso_login_array, i, jso_login_item);
		}
	}
	json_object_object_add(jso, "items", jso_login_array);

	json_object_to_file(AL_FILE_NAME, jso);
	json_object_put(jso);
}

static void
al_window_move(void)
{
	vlog("Move autologin window");

	int root_x, root_y, root_w, root_h;
	int x, y, w, h;

	gtk_window_get_position(GTK_WINDOW(window), &root_x, &root_y);
	gtk_window_get_size(GTK_WINDOW(window), &root_w, &root_h);
	gtk_window_get_size(GTK_WINDOW(al_window), &w, &h);

	x = root_x;
	y = root_y;
	y += root_h / 2; // window center
	y -= h / 2;  // login window center

	gtk_window_move(GTK_WINDOW(al_window), x, y);
}

static void
al_item_activate_cb(GtkWidget *w, tzLogin *login_item)
{
	vlog("Activate button callback");

	// EDIT
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_edit_button))) {
		al_item_window(login_item);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_edit_button), false);

		return;
	}

	// REMOVE
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(al_remove_button))) {
		al_item_remove(login_item);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(al_remove_button), false);

		al_list_buttons_redraw();
		al_window_move();
		al_list_save();

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
al_move_cb(void)
{
	al_window_move();
}

static void
al_item_activate(tzLogin *login_item)
{
	vlog("Activite autologin item '%s'", login_item->login);


	tzSetVar("_level0.skin_login.mc_login.login.text", login_item->login);

	if (login_item->password) {
		tzSetVar("_level0.skin_login.mc_login.psw.text", login_item->password);
		tzSetVar("_level0.skin_login.mc_login.btn_enter.releasing", "");
	}
}
