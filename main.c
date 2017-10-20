#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <windows.h>
#include "parseur.h"

int main(int argc, char *argv[]) {

	static GtkWidget *MainWindow = NULL;
	iconname = "icons/delta.jpg";

	/* Initialisation de GTK+ */
	gtk_init(&argc, &argv);

	/******************/
	/*** 1ST WINDOW ***/
	/******************/

	/* Création de la fenetre */
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(MainWindow), "DeltaMaker");
	gtk_window_set_icon_from_file(GTK_WINDOW(MainWindow), iconname, NULL);
	gtk_window_set_default_size(GTK_WINDOW(MainWindow), 640, 400);
	g_signal_connect(G_OBJECT(MainWindow), "destroy", G_CALLBACK(exit), 0);

	/* insertion buttons/labels 1st window */
	GtkWidget *pButtonOk;
	GtkWidget *vbox = gtk_vbox_new(10, 50);
	gtk_container_add(GTK_CONTAINER(MainWindow), vbox);
	GtkWidget *hbox = gtk_hbox_new(0, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	GtkWidget *txtbox = gtk_hbox_new(0, 0);
	gtk_container_add(GTK_CONTAINER(hbox), txtbox);

	/* labels + file dialog buttons 1st Window */
	pButtonF1 = gtk_button_new_with_label("Selection fichier 1");
	g_signal_connect(pButtonF1, "clicked", G_CALLBACK(fileChooserDialog), GINT_TO_POINTER(1));
	gtk_container_add(GTK_CONTAINER(vbox), pButtonF1);
	pButtonF2 = gtk_button_new_with_label("Selection fichier 2");
	g_signal_connect(pButtonF2, "clicked", G_CALLBACK(fileChooserDialog), GINT_TO_POINTER(2));
	gtk_container_add(GTK_CONTAINER(vbox), pButtonF2);
	entries = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entries), 0);
	gtk_container_add(GTK_CONTAINER(vbox), entries);

	/*Create a progressbar and add it to te window*/
	pProgress = gtk_progress_bar_new();
	gtk_container_add(GTK_CONTAINER(vbox), pProgress);

	pButtonOk = gtk_button_new_with_label("OK");
	g_signal_connect_swapped(pButtonOk, "clicked", G_CALLBACK(secondWindow), entries);
	gtk_container_add(GTK_CONTAINER(vbox), pButtonOk);

	/****************/
	/*** MAIN LOOP **/
	/****************/

	/* Affichage et boucle evenementielle */
	gtk_widget_show_all(MainWindow);
	gtk_main();

	return EXIT_SUCCESS;
}