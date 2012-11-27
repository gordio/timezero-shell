#ifndef __AUTOLOGIN_H
#define __AUTOLOGIN_H


#define AL_FILE_NAME "autologins.json"
#define MAX_LOGIN_CHARS 80
#define MAX_PASSWORD_CHARS 80
#define MAX_AUTOLOGIN_ITEMS 20

typedef struct _login {
	char *login;
	char *password;
	char *clan;
	char *profession;
	unsigned int level;
	unsigned int rank;
	char *key_file_path;
} login_t;


void al_window_show();
void al_window_hide();

bool al_list_update_by_player();
void al_list_print();

#endif
