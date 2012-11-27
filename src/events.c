#include "general.h"
#include "window.h"
#include "chat.h"
#include "flash.h"
#include "tz-interface.h"

#include "events.h"

static GdkCursor *cur_hand = NULL;


/* Чат {{{ */
// Калбек для чатового времени
bool
tag_time_cb(GtkTextTag *tag, GObject *o, GdkEvent *e, GtkTextIter *i, gpointer p)
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

			return true;

		default:
			return false;
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

	for (unsigned int i = 0; i < g_slist_length(tags); ++i) {
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

	return false;
}
/* }}} */


void
room_click_info_cb(GtkWidget *w, GdkEvent *e, gpointer data)
{
	tz_info_open(gtk_label_get_text(data));
}

bool
nick_label_cb(GtkWidget *w, gchar *uri, gpointer user_data)
{
	insert_nick_to_entry(gtk_label_get_text(GTK_LABEL(w)), false);
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

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
