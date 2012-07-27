#include "../general.h"
#include "../utils.h"
#include "../chat.h"
#include "../flash.h"


GtkWidget *
create_chat_smiles_window(void)
{
	GtkWidget *window;
	GtkWidget *notebook;

	window = gtk_window_new(GTK_WINDOW_POPUP);
	notebook = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(window), notebook);

	return window;
}