#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "general.h"
#include "events.h"

#include "utils.h"

/* IMAGE CACHING */
struct imgCache_t {
	char *path;
	GtkWidget *image;

	struct imgCache_t *next;
};

struct imgCache_t *img_cache_clan = NULL;
struct imgCache_t *img_cache_profession = NULL;
struct imgCache_t *img_cache_rank = NULL;

static struct imgCache_t*
image_in_cache(struct imgCache_t *list, char *img_path)
{
	struct imgCache_t *item = list;

	// перебираем каждый элемент от начала
	while (item) {
		if (strcmp(img_path, item->path) == 0) {
			// есть в списке
			return item;
		}
		// делаем текущим следующий элемент
		item = item->next;
	}

	// нет в списке
	return NULL;
}

static GtkWidget*
get_image(struct imgCache_t *list, char *img_path)
{
	struct imgCache_t *item = list;

	// если список еще не создан
	if (!item) {
		// создаем элемент
		struct imgCache_t *new_item = malloc(sizeof(struct imgCache_t));
		new_item->image = gtk_image_new_from_file(img_path);
		new_item->path = strdup(img_path);
		new_item->next = NULL;

		// добавляем элемент в конец
		item = new_item;

		return new_item->image;
	}

	// есть ли уже в кеше
	if ((item = image_in_cache(list, img_path))) {
		// счетчик использований
		g_object_ref(item->image);
		return item->image;
	}

	// находим конец
	while (item->next) {
		item = item->next;
	}

	// создаем элемент
	struct imgCache_t *new_item = malloc(sizeof(struct imgCache_t));
	new_item->image = gtk_image_new_from_file(img_path);
	new_item->path = strdup(img_path);
	new_item->next = NULL;

	// добавляем элемент в конец
	item->next = new_item;

	return new_item->image;
}
/* end IMAGE CACHING */


#define ESCAPE_CHAR "\\"
char *
escape_str(char *data)
{
	char *str; // Валидная UTF8 строка
	if (g_utf8_validate(data, -1, NULL)) {
		str = data;
	} else {
		// FIXME: отрезать только валидную строку
		return NULL;
	}

	char *p = str;
	// не трож индусокод, индус будет зол!
	char *new_str = malloc(strlen(str) * 2 + 1);
	gunichar c;

	int escaped_len = 0, str_len = strlen(str), cur_str_pos = 0;

	// копируем строку, будет как начало
	memcpy(new_str, str, str_len);
	for (int i = 0; i < g_utf8_strlen(str, -1); ++i) {
		cur_str_pos = strlen(str) - strlen(p);
		c = g_utf8_get_char(p);
		if (c == '"' || c == '\\') {
			// добавляем к результирующей строке символ экранирования
			memcpy(new_str + cur_str_pos + escaped_len, ESCAPE_CHAR, strlen(ESCAPE_CHAR));
			// запоминаем размер добавленных символов экранирования
			escaped_len = escaped_len + strlen(ESCAPE_CHAR);
		}
		// добавляем к результирующей строке оставшийся кусок
		memcpy(new_str + cur_str_pos + escaped_len, p, str_len+1 - cur_str_pos);
		p = g_utf8_next_char(p);
	}

	return new_str;
}

#define MEM_TMP 8192
/* GTK */
/* create_nick_box: Создает виджет игрока (для автологина) */
GtkWidget *
al_list_widget_create(tzLogin *player)
{
	GtkWidget *nick_box;
	GtkWidget *w;

	nick_box = gtk_hbox_new(false, 0);

	// FIXME: Use max mem size (good choice g_strconcat)
	char *tmp_mem = malloc(MEM_TMP);
	char *tmp_str;

	// CLAN
	if (player->clan != NULL) {
		tmp_str = g_strconcat("i/clans/", player->clan, ".gif", NULL);
		w = get_image(img_cache_clan, tmp_str);
		free(tmp_str);
	} else {
		w = get_image(img_cache_clan, "img/clans/nil.gif");
	}
	gtk_box_pack_start(GTK_BOX(nick_box), w, false, false, 1);

	// LOGIN
	w = gtk_label_new(player->login);
	gtk_box_pack_start(GTK_BOX(nick_box), w, false, false, 1);

	// RANK
	if (player->rank != 0) {
		snprintf(tmp_mem, MEM_TMP, "i/rank/%i.gif", player->rank);
		w = gtk_image_new_from_file(tmp_mem);
	} else {
		w = gtk_image_new_from_file("img/rank/nil.gif");
	}
	gtk_box_pack_end(GTK_BOX(nick_box), w, false, false, 0);

	// PROFESSION
	if (player->profession != NULL) {
		tmp_str = g_strconcat("i/i", player->profession, ".gif", NULL);
		w = get_image(img_cache_profession, tmp_str);
	} else {
		w = get_image(img_cache_profession, "img/nil.gif");
	}
	gtk_box_pack_end(GTK_BOX(nick_box), w, false, false, 0);

	// LEVEL
	if (player->level != 0) {
		snprintf(tmp_mem, MEM_TMP, "[%i]", player->level);
		w = gtk_label_new(tmp_mem);
	} else {
		w = gtk_label_new("[-]");
	}
	gtk_box_pack_end(GTK_BOX(nick_box), w, false, false, 1);

	free(tmp_mem);

	return nick_box;
}

/* виджет игрока (для списка игроков в комнате) */
GtkWidget *
list_nickbox_create(tzPlayer *p)
{
	GtkWidget *nick_box;
	GtkWidget *label;
	GtkWidget *image;
	GtkWidget *event_box;
	char *n = malloc(3);

	char *tmp_mem = malloc(MEM_TMP);
	char *t;

	bool online = (bool) (p->state & 1) || 0;
	int sleep = (p->state & 2);
	//int bandit = (p->state & 4);
	int claim = (p->state & 8);
	int battle = (p->state & 16) || 0;

	bool bot = (bool) (p->state & 2048) || 0;
	//int friend = (p->state & 4096);
	bool woman = (p->state & 8192) || 0;
	//int casino = (p->state & 16384);
	//int bloodbtl = (p->state & 32768);
	//int grp = (p->state & 24) ? p->group : 0;
	int prof = ((p->state >> 5) & 63);

	if (bot) {
		p->aggr = -1;
	}

	if (claim) {
		//n = "4";
		strcpy(n, "4");
	//} else if (casino) {
		//n = 7;
	} else if (p->aggr == 1) {
		//n = "11";
		strcpy(n, "11");
	} else if (p->aggr == 2) {
		//n = "12";
		strcpy(n, "12");
	} else if (p->aggr == 3) {
		//n = "13";
		strcpy(n, "13");
	} else if (bot) {
		//n = "10";
		strcpy(n, "10");
	}

	nick_box = gtk_hbox_new(false, 0);

	// Status
	t = g_strconcat("img/status", n, ".png", NULL);
	if (!g_file_test(t, G_FILE_TEST_EXISTS)) {
		free(t);
		strcpy(n, "0");
		t = g_strconcat("img/status", n, ".png", NULL);
	}
	if (g_file_test(t, G_FILE_TEST_EXISTS)) {
		image = gtk_image_new_from_file(t);
		gtk_box_pack_start(GTK_BOX(nick_box), image, false, false, 0);
	}
	free(t);
	free(n);

	// Clan
	if (p->clan != NULL) {
		snprintf(tmp_mem, MEM_TMP-1, "i/clans/%s.gif", p->clan);
		if (g_file_test(tmp_mem, G_FILE_TEST_EXISTS)) {
			image = get_image(img_cache_clan, tmp_mem);
		} else {
			// TODO: Fetch;
			// FIXME: goto draw;
			image = get_image(img_cache_clan, "img/clans/nil.gif");
		}
	} else {
		image = get_image(img_cache_clan, "img/clans/nil.gif");
	}
	gtk_widget_set_tooltip_text(GTK_WIDGET(image), p->clan);
	gtk_box_pack_start(GTK_BOX(nick_box), image, false, false, 2);

	// NICKNAME
	label = gtk_label_new(p->nick);
	if (!online) {
		GdkColor color;
		gdk_color_parse("#808080", &color);
		gtk_widget_modify_fg(label, GTK_STATE_NORMAL, &color);
	}
	g_signal_connect(G_OBJECT(label), "button-press-event", G_CALLBACK(nick_label_cb), NULL);
	gtk_box_pack_start(GTK_BOX(nick_box), label, false, false, 0);

	// RANK
	event_box = gtk_event_box_new();
	g_signal_connect(event_box, "button-press-event", G_CALLBACK(&room_click_info_cb), label);
	g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(&set_cursor_hand_cb), NULL);
	g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(&unset_cursor_hand_cb), NULL);

	snprintf(tmp_mem, MEM_TMP-1, "i/rank/%i.gif", get_rank_num_from_ranks(p->rank));
	image = get_image(img_cache_rank, tmp_mem);
	gtk_container_add(GTK_CONTAINER(event_box), image);

	gtk_box_pack_end(GTK_BOX(nick_box), event_box, false, false, 0);

	// PROFFESSION
	event_box = gtk_event_box_new();
	g_signal_connect(event_box, "button-press-event", G_CALLBACK(&room_click_info_cb), label);
	g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(&set_cursor_hand_cb), NULL);
	g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(&unset_cursor_hand_cb), NULL);

	if (woman) {
		snprintf(tmp_mem, MEM_TMP-1, "i/i%iw.gif", prof);
	} else {
		snprintf(tmp_mem, MEM_TMP-1, "i/i%i.gif", prof);
	}
	image = get_image(img_cache_profession, tmp_mem);
	gtk_container_add(GTK_CONTAINER(event_box), image);

	gtk_box_pack_end(GTK_BOX(nick_box), event_box, false, false, 0);

	snprintf(tmp_mem, MEM_TMP-1, "[%i]", p->level);
	label = gtk_label_new(tmp_mem);
	gtk_box_pack_end(GTK_BOX(nick_box), label, false, false, 0);

	// LOCKED CHAT
	if (sleep == 2) {
		image = gtk_image_new_from_file("i/sleep.gif");
		gtk_widget_set_tooltip_text(GTK_WIDGET(image), _("User chat baned."));
		gtk_box_pack_end(GTK_BOX(nick_box), image, false, false, 0);
	}

	free(tmp_mem);

	return nick_box;
}

/* image_button_new: Создает кнопку с картинкой */
GtkWidget *
image_button_new(char *image_path, char *text, char *ttp_text, gpointer *e)
{
	GtkWidget *button = gtk_button_new();
	g_object_set(gtk_widget_get_settings(button), "gtk-button-images", true, NULL);

	GtkWidget *image = gtk_image_new_from_file(image_path);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_button_set_label(GTK_BUTTON(button), text);
	gtk_widget_set_tooltip_text(button, ttp_text);

	if (e != NULL) {
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(e), NULL);
	}

	return button;
}

/* image_toggle_button_new: Создает залипающую кнопку с картинкой */
GtkWidget *
image_toggle_button_new(char *image_path, char *text, char *ttp_text, gpointer *e)
{
	GtkWidget *button = gtk_toggle_button_new();
	g_object_set(gtk_widget_get_settings(button), "gtk-button-images", true, NULL);

	GtkWidget *image = gtk_image_new_from_file(image_path);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_button_set_label(GTK_BUTTON(button), text);

	if (e != NULL) {
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(e), NULL);
	}

	return button;
}

/* rem_substr: Удаляет подстроку substr в строке str */
void
rem_substr(char *str, char *substr)
{
	uint substr_len = strlen(substr);
	char *dst = strstr(str, substr);
	// не нашли подстроку в строке, возвращаемся, ничо не делаем
	if (dst == 0) {
		return;
	}

	// смещаемся на конец подстроки, оттуда будем двигать
	char *beg = dst + substr_len;

	// ищем конец str
	char *end = beg;
	while (*end) end++;

	// короткий путь, если beg == end, т.е. подстрока находится в конце
	// строки, можно в dst записать \0 и вернутся
	if (beg == end) {
		*dst = '\0';
		return;
	}

	// ну и двигаем обрубок на место предыдущей строки
	// +1 чтобы не забыть ноль
	memmove(dst, beg, end-beg+1);
}

/* Find rank index */
int
get_rank_num_from_ranks(int rank_count)
{
	int rank[] = {20, 60, 120, 250, 600, 1100, 1800, 2500, 3200, 4000, 5000, 6000, 7200, 10000, 15000};

	for (uint i = 0; i <= countof(rank); i++) {
		if (rank_count < rank[i]) {
			if (i > 14) {
				i = 14; // fixme
			}
			return i + 1;
		}
	}

	return 0;
}

GtkWidget *
chat_msg_view_new(void)
{
	GtkTextView *msgView;
	GtkTextBuffer *msgViewBuffer;
	GtkTextTag *tag;

	msgView = GTK_TEXT_VIEW(gtk_text_view_new());
	gtk_text_view_set_wrap_mode(msgView, GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(msgView, false);
	gtk_text_view_set_overwrite(msgView, false);
	gtk_text_view_set_cursor_visible(msgView, false);
	gtk_text_view_set_pixels_above_lines(msgView, 0);
	gtk_text_view_set_pixels_below_lines(msgView, 0);
	gtk_text_view_set_pixels_inside_wrap(msgView, -2);
	gtk_text_view_set_left_margin(msgView, 0);
	gtk_text_view_set_indent(msgView, -160);


	msgViewBuffer = gtk_text_view_get_buffer(msgView);

	tag = gtk_text_buffer_create_tag(msgViewBuffer, "monospace", "family", "monospace", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "system", "family", "italic", "foreground", CHAT_SYSTEM_COLOR, NULL);

	tag = gtk_text_buffer_create_tag(msgViewBuffer, "time", "family", "monospace", "foreground", CHAT_TIME_COLOR, NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "nickname", "family", "monospace", "foreground", CHAT_NICK_COLOR, "weight", "bold", NULL);
	g_signal_connect(G_OBJECT(tag), "event", G_CALLBACK(&tag_nick_cb), msgView);
	g_signal_connect(G_OBJECT(msgView), "motion-notify-event", G_CALLBACK(&chat_text_view_event_cb), tag);

	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c1", "foreground", "#D0D0D0", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c2", "foreground", "#ACC8E6", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c3", "foreground", "#CCA3C8", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c4", "foreground", "#B0B3AC", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c5", "foreground", "#BBCC99", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c6", "foreground", "#F29191", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c7", "foreground", "#ACC8E6", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c8", "foreground", "#F2ECB6", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c9", "foreground", "#E6CFE3", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c10", "foreground", "#ADC2D9", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c11", "foreground", "#F2DAB6", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c12", "foreground", "#F29D9D", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c13", "foreground", "#DEF2B6", NULL);
	tag = gtk_text_buffer_create_tag(msgViewBuffer, "c14", "foreground", "#F2D19D", NULL);

	return GTK_WIDGET(msgView);
}

/* reverse: переворачиваем строку s на месте */
void
reverse(char s[])
{
	char c;

	for (size_t i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/* itoa: конвертируем n в символы s */
void
itoa(int n, char s[])
{
	int i = 0, sign = 0;

	if ((sign = n) < 0)
		n = -n;

	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		s[i++] = '-';

	s[i] = '\0';

	reverse(s);
}
/*
[> Заменяет substr на to_str в str <]
char *replace_substr_in_str(const char *str, const char *substr, const char *to_str)
{
	char *begin = strstr(str, substr);
	size_t str_begin_count = strspn(s, substr);

	char *tmp_str = malloc(str_begin_count + strlen(to_str) +  + 1);

	if (str_begin_count > 0) {
		memcpy(tmp_str, str, str_begin_count);
	}
	memcpy(tmp_str + (begin - str), to_str, strlen(to_str));
	memcpy(tmp_str + (begin - str) + strlen(to_str), begin + strlen(substr), strlen(str) - (str - begin) - strlen(substr));
	tmp_str[strlen(str) - strlen(substr) + strlen(to_str) + 1] = '\0';
	return tmp_str;
}
*/

/* replace: Заменяем old на new в строке str */
// FIXME: Have problem
char *
replace(const char *str, const char *old, const char *new)
{
	char *ret, *r;
	const char *p, *q;
	size_t oldlen = strlen(old);
	size_t count, retlen, newlen = strlen(new);

	if (oldlen != newlen) {
		for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
			count++;
		}
		/* undefined if p - str > PTRDIFF_MAX */
		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else {
		retlen = strlen(str);
	}

	ret = malloc(retlen + 1);

	for (r = ret, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
		/* undefined if q - p > PTRDIFF_MAX */
		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, new, newlen);
		r += newlen;
	}
	strcpy(r, p);

	return ret;
}

char *
replace_substr_in_str(const char *str, const char *old, const char *new)
{
	char *ret;
	int i = 0;
	size_t str_len = strlen(str);
	size_t new_len = strlen(new);
	size_t old_len = strlen(old);

	ret = malloc(str_len - old_len + new_len + 1);

	if (ret == NULL)
		exit(EXIT_FAILURE);

	while (*str) {
		if (strstr(str, old) == str) {
			strcpy(&ret[i], new);
			i += new_len;
			str += old_len;
		} else
			ret[i++] = *str++;
	}
	ret[i] = '\0';

	return ret;
}
