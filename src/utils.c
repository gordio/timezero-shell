#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "general.h"
#include "events.h"
#include "autologin.h"

#include "utils.h"


/* IMAGE CACHING */
struct imgCache_t {
	char *path;
	GtkWidget *image;

	struct imgCache_t *next;
};

// Глобальные кеши
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
/* GTK  {{{ */
/* create_nick_box: Создает виджет игрока (для автологина) */
GtkWidget *
al_list_widget_create(tzLogin *player)
{
	GtkWidget *nick_box;
	GtkWidget *w;

	nick_box = gtk_hbox_new(false, 0);

	char *tmp_mem = malloc(MEM_TMP);
	char *tmp_str;

	// CLAN
	if (player->clan) {
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
		snprintf(tmp_mem, MEM_TMP, "%i", player->rank);
		tmp_str = g_strconcat("i/rank/", tmp_mem, ".gif", NULL);
		w = gtk_image_new_from_file(tmp_str);
		free(tmp_str);
	} else {
		w = gtk_image_new_from_file("img/rank/nil.gif");
	}
	gtk_box_pack_end(GTK_BOX(nick_box), w, false, false, 0);

	// PROFESSION
	if (player->profession) {
		tmp_str = g_strconcat("i/i", player->profession, ".gif", NULL);
		w = get_image(img_cache_profession, tmp_str);
		free(tmp_str);
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
	//int battle = (p->state & 16) || 0;

	bool bot = (bool) (p->state & 2048) || 0;
	//int friend = (p->state & 4096);
	bool woman = (p->state & 8192) || 0;
	int casino = (p->state & 16384);
	//int bloodbtl = (p->state & 32768);
	//int grp = (p->state & 24) ? p->group : 0;
	int prof = ((p->state >> 5) & 63);

	if (bot) {
		p->aggr = -1;
	}

	if (claim) {
		strcpy(n, "4");
	} else if (casino) {
		strcpy(n, "7");
	} else if (p->aggr == 1) {
		strcpy(n, "11");
	} else if (p->aggr == 2) {
		strcpy(n, "12");
	} else if (p->aggr == 3) {
		strcpy(n, "13");
	} else if (bot) {
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
	if (p->clan) {
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

// Истина если скролбар в конце
bool
adjustment_is_bottom(GtkAdjustment *a)
{
	double lower = gtk_adjustment_get_lower(a);
	double upper = gtk_adjustment_get_upper(a) - gtk_adjustment_get_page_size(a) + lower;
	double value = gtk_adjustment_get_value(a);

	return (value == upper) ? true : false;
}

// Добавляет изображение к кнопке
void
gtk_button_add_image(GtkButton *button, char *image_path)
{
	GtkWidget *image;

	image = gtk_image_new_from_file(image_path);

	g_object_set(gtk_widget_get_settings(GTK_WIDGET(button)), "gtk-button-images", true, NULL);
	gtk_button_set_image(button, image);
}

/* }}} */

// Find rank index
int
get_rank_num_from_ranks(const int rank_count)
{
	int rank[] = {20, 60, 120, 250, 600, 1100, 1800, 2500, 3200, 4000, 5000, 6000, 7200, 10000, 15000};

	for (unsigned int i = 0; i < countof(rank); i++) {
		if (rank_count < rank[i]) {
			return i + 1;
		}
	}

	return countof(rank) + 1;
}

/* STRINGS {{{ */
/* strdup: Возвращает копию строки, или NULL если не хватает памяти */
char *
strdup(const char const *str)
{
	char *dst = malloc(strlen(str) + 1);

	if (dst) {
		strcpy(dst, str);
	}

	return dst; // return NULL if no memory
}

/* rem_substr: Удаляет подстроку substr в строке str */
void
rem_substr(char *str, const char const *substr)
{
	unsigned int substr_len = strlen(substr);
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

/* reverse: переворачиваем строку s на месте */
void
reverse(char *str)
{
	char c;

	for (size_t i = 0, j = strlen(str)-1; i < j; i++, j--) {
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}
}

/* itoa: конвертируем n в символы s */
void
itoa(int n, char *str)
{
	int i = 0, sign = 0;

	if ((sign = n) < 0) {
		n = -n;
	}

	do {
		str[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0) {
		str[i++] = '-';
	}

	str[i] = '\0';

	reverse(str);
}

/* replace: Заменяем old на new в строке str */
// FIXME: Have problem
char *
replace(const char *str, const char const *old, const char const *new)
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
rep_substr(const char *str, const char *old, const char *new)
{
	char *ret;
	int i = 0;
	size_t str_len = strlen(str);
	size_t new_len = strlen(new);
	size_t old_len = strlen(old);

	ret = malloc(str_len - old_len + new_len + 1);

	if (!ret) {
		exit(EXIT_FAILURE);
	}

	while (*str) {
		if (strstr(str, old) == str) {
			strcpy(&ret[i], new);
			i += new_len;
			str += old_len;
		} else {
			ret[i++] = *str++;
		}
	}
	ret[i] = '\0';

	return ret;
}

int
str_hash(const char const *str, int hash_tablesize)
{
	unsigned n = 0;

	for (n = 0; *str; str++) {
		n = 31 * n + *str;
	}

	return n % hash_tablesize;
}

/* }}} */

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix ft=c: */
