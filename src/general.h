#ifndef __GENERAL_H
#define __GENERAL_H

#include <stdbool.h>

#define MAX_ROOM_NICKS 1500
#define MAX_CHAT_MESSAGE 2600
#define MAX_NICK_SIZE 16
#define MAX_CLAN_SIZE 30
#define NICK_SIZE 512 // size in memory for Nickname



typedef struct _player {
	unsigned long long int battleid;
	int group;
	unsigned int state;
	char *clan;
	char *nick;
	int level;
	int rank;
	int minlevel;
	int aggr; // Фракция
	int stake;
} player_t;

#endif
