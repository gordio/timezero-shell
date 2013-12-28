#include <stdio.h>
#include <stdlib.h>
#include "general.h"

#include "sort.h"


/*
 * Сортировка по уровню
 */

void
tzplayer_swap(player_t *a, player_t *b)
{
	player_t t = *a;

	*a = *b;
	*b = t;
}

void
room_array_qsort_by_level(player_t list[], int beg, int end)
{
	player_t key;
	int i, j, k;

	if (beg < end) {
		k = (beg + end) / 2; // chose pivot
		tzplayer_swap(&list[beg], &list[k]);
		key = list[beg];
		i = beg + 1;
		j = end;

		while (i <= j) {
			while ((i <= end) && (list[i].level <= key.level))
				i++;

			while ((j >= beg) && (list[j].level > key.level))
				j--;

			if (i < j)
				tzplayer_swap(&list[i], &list[j]);
		}

		// swap two elebegeendts
		tzplayer_swap(&list[beg], &list[j]);
		// recursively sort the lesser list
		room_array_qsort_by_level(list, beg, j - 1);
		room_array_qsort_by_level(list, j + 1, end);
	}
}


void
room_array_sort_by_level(player_t list[])
{
	for (unsigned int i = 0; i < MAX_ROOM_NICKS; i++) {
		for (int j = 1; j < MAX_ROOM_NICKS; j++) {
			if (list[j-1].level > list[j].level) {
				tzplayer_swap(&list[j-1], &list[j]);
			}
		}
	}

	return;
}

/*
 * Сортировка по уровню > по рангу
 */
void
room_array_sort_by_level_by_rank(player_t list[])
{
	unsigned int beg = 0, end = 0;

	for (unsigned int i = 0; i < MAX_ROOM_NICKS; i++) {
		// пропускаем отсортированный элемент
		if (list[i].level == list[i+1].level) {
			end = i + 1;
			continue;
		}

		for (unsigned int j = beg; j <= end && beg != end; j++) {
			for (unsigned int k = beg; k <= end; k++) {
				if (list[k].rank > list[k+1].rank && list[k].level == list[k+1].level) {
					tzplayer_swap(&list[k], &list[k+1]);
				}
			}
		}

		beg = i + 1;
		end = i + 1;
	}
}
