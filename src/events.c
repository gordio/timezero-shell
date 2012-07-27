#include <gdk/gdkkeysyms.h>
#include "general.h"
#include "window.h"
#include "chat.h"
#include "flash.h"
#include "tz-interface.h"

#include "events.h"

static GdkCursor *cur_hand = NULL;


// Hapend if click on TIME in messages frame
bool
tag_time_cb(GtkTextTag *tag, GObject *o, GdkEvent *e, GtkTextIter *i, gpointer user_data)
{
	GtkTextIter *i_start, *i_end;

	switch (e->type) {
		case GDK_BUTTON_PRESS:
			i_start = gtk_text_iter_copy(i);
			if (gtk_text_iter_begins_tag(i_start, tag) == 0) {
				gtk_text_iter_backward_to_tag_toggle(i_start, tag);
			}

			i_end = gtk_text_iter_copy(i);
			gtk_text_iter_forward_line(i_end);
			// remove ended char if this no last line
			if (gtk_text_iter_get_char(i_end) != 0) {
				gtk_text_iter_backward_char(i_end);
			}

			insert_to_entry(gtk_text_iter_get_text(i_start, i_end));

			gtk_text_iter_free(i_start);
			gtk_text_iter_free(i_end);

			return TRUE;

		default:
			return FALSE;
	}
}

// Hapend if click on Nickname in messages frame
bool
tag_nick_cb(GtkTextTag *tag, GObject *o, GdkEventButton *e, GtkTextIter *i, gpointer user_data)
{
	GtkTextIter *i_start, *i_end;
	const char *nick;

	switch (e->type) {
		case GDK_2BUTTON_PRESS:
			i_start = gtk_text_iter_copy(i);
			if (gtk_text_iter_begins_tag(i_start, tag) == 0) {
				gtk_text_iter_backward_to_tag_toggle(i_start, tag);
			}

			i_end = gtk_text_iter_copy(i);
			if (gtk_text_iter_ends_tag(i_end, tag) == 0) {
				gtk_text_iter_forward_to_tag_toggle(i_end, tag);
			}

			insert_nick_to_entry(gtk_text_iter_get_text(i_start, i_end), true);

			gtk_text_iter_free(i_start);
			gtk_text_iter_free(i_end);
			return true;

		case GDK_BUTTON_PRESS:
			i_start = gtk_text_iter_copy(i);
			if (gtk_text_iter_begins_tag(i_start, tag) == 0) {
				gtk_text_iter_backward_to_tag_toggle(i_start, tag);
			}

			i_end = gtk_text_iter_copy(i);
			if (gtk_text_iter_ends_tag(i_end, tag) == 0) {
				gtk_text_iter_forward_to_tag_toggle(i_end, tag);
			}

			nick = gtk_text_iter_get_text(i_start, i_end);

			if (e->button == 3) {
				tz_info_open(nick);
			} else {
				insert_nick_to_entry(nick, false);
			}

			gtk_text_iter_free(i_start);
			gtk_text_iter_free(i_end);

			return true;

		default:
			return false;
	}
}

/*// example
bool
on_tag_event(GtkTextTag *tag, GObject *o, GdkEvent *e, const GtkTextIter *i, gpointer user_data)
{
	GtkTextIter *iter_line_start, *iter_line_end;

	iter_line_start = gtk_text_iter_copy(i);
	iter_line_end = gtk_text_iter_copy(i);
	gtk_text_iter_backward_line(iter_line_start);
	gtk_text_iter_forward_line(iter_line_end);
	switch (e->type) {
	case GDK_ENTER_NOTIFY:
		break;
	case GDK_LEAVE_NOTIFY:
		break;
	case GDK_MOTION_NOTIFY:
		break;
	case GDK_2BUTTON_PRESS:
		break;
	case GDK_3BUTTON_PRESS:
		break;
	case GDK_BUTTON_PRESS:
		[>g_print(">>%s", gtk_text_buffer_get_text(msgViewBuffer, iter_line_start, iter_line_end, false));<]
		break;
	case GDK_BUTTON_RELEASE:
		//g_print("Button released from Bold Text\n");
		//g_print("   -- x: [%.2f], y: [%.2f]\n",
		//event->button.x, event->button.y);
		break;
	default:
		g_print("Some event...\n");
		break;
	}
	return TRUE;
}*/


static int old_size = 0;
void
flash_resize_cb(GtkContainer *container, gpointer user_data)
{
	int cur_size = gtk_paned_get_position(GTK_PANED(main_panels));
	if (cur_size != 0 && old_size != cur_size) {
		old_size = cur_size;
		vlog("Send signal to flash -> update_size");
		tzFlashResize();
	}
}

// use for change cursor pointer on links
bool
chat_text_view_event_cb(GtkWidget *w, GdkEventMotion *event, GSList *tags)
{
	GtkTextWindowType type;
	GtkTextIter iter;
	GdkWindow *win;
	gint x, y, buf_x, buf_y;

	type = gtk_text_view_get_window_type(GTK_TEXT_VIEW(w), event->window);
	if (type != GTK_TEXT_WINDOW_TEXT) {
		return false;
	}

	/* Get where the pointer really is. */
	win = gtk_text_view_get_window(GTK_TEXT_VIEW(w), type);
	if (!win) {
		return false;
	}

	gdk_window_get_pointer(win, &x, &y, NULL);

	/* Get the iter where the cursor is at */
	gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(w), type, x, y, &buf_x, &buf_y);

	gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(w), &iter, buf_x, buf_y);

	GtkTextTag *tag;
	bool this_tag = false;

	for (uint i = 0; i < g_slist_length(tags); ++i) {
		tag = g_slist_nth_data(tags, i);
		if (gtk_text_iter_has_tag(&iter, GTK_TEXT_TAG(tag))) {
			this_tag = true;
			break;
		}
	}

	if (this_tag) {
		if (cur_hand == NULL) {
			cur_hand = gdk_cursor_new(GDK_HAND2);
		}
		gdk_window_set_cursor(win, cur_hand);
	} else {
		gdk_window_set_cursor(win, NULL);
	}

	return FALSE;
}

bool
keypress_cb(GtkWidget *w, GdkEventKey *e, gpointer user_data)
{
	/*if (event->state == (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {
		if (gdk_keyval_to_lower(event->keyval) == GDK_x) {
			return TRUE;
		}
	}*/
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

void
room_click_info_cb(GtkWidget *w, GdkEvent *e, gpointer data)
{
	tz_info_open(gtk_label_get_text(data));
}

bool
nick_label_cb(GtkWidget *w, gchar *uri, gpointer user_data)
{
	insert_nick_to_entry(gtk_label_get_text(GTK_LABEL(w)), FALSE);
	return true;
}

// set/unset hand_cursor for window
void
set_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data)
{
	if (cur_hand == NULL) {
		cur_hand = gdk_cursor_new(GDK_HAND2);
	}
	/*gdk_window_set_cursor(w->window, cur_hand);*/
	gdk_window_set_cursor(gtk_widget_get_window(w), cur_hand);
}
void
unset_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data)
{
	gdk_window_set_cursor(gtk_widget_get_window(w), NULL);
}
