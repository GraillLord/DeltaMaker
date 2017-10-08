#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <windows.h>
#include "parseur.h"

/**************************/
/*** VARIABLES GLOBALES ***/
/**************************/

static GtkWidget *SndWindow;

FILE *fichier_s1 = NULL;
FILE *fichier_s2 = NULL;
FILE *fichier_d = NULL;
//
elems_str *elems_tab1 = NULL;
elems_str *elems_tab2 = NULL;
//
wchar_t title[700];
wchar_t sep = L'\u25B2';
//
char *data[3] = { NULL, NULL, NULL };
char *file_s1 = NULL;
char *file_s2 = NULL;
//
gdouble max = 1.0;
gdouble dfrac = 0.0;
//
unsigned int size_s1 = 0;
unsigned int size_s2 = 0;
int max_sep = 0;
int result = 0;
int num = 0;
bool isEnded = FALSE;
bool is2nd = FALSE;

int file_size(FILE* fichier) {
	bool is1stLine = TRUE;
	int i = 1;
	wchar_t c;

	while((c = fgetwc(fichier)) != WEOF) {
		if(is1stLine && c == 226)
			max_sep++;
		if(c == 10) {
			is1stLine = FALSE;
			i++;
		}
	}
	rewind(fichier);

	return i;
}

char* get_filename_ext(const char *filename) {
	char *dot = strrchr(filename, '.');
	if(!dot || dot == filename) {
		strcat((char *)filename, ".csv");
		return "csv";
	}
	else if(strcmp(dot+1, "csv") != 0)
		return "ERREUR";
	else
		return dot+1;
}

const char* verify_filename_ext(const char* file1, const char* file2, const char* file3) {
	if(strcmp(file1, "csv") != 0)
		return file1;
	else if(strcmp(file2, "csv") != 0)
		return file2;
	else if(strcmp(file3, "csv") != 0)
		return file3;
	return "true";
}

void chopN(wchar_t *str, size_t n) {
	assert(n != 0 && str != 0);
	size_t len = wcslen(str);
	if(n > len)
		return; // or: n = len;
	wmemmove(str, str+n, len-n+1);
}

void store_elems(FILE* fichier, elems_str* tab) {
	int i = 0, j = 0;
	int isComa = 0;
	bool is1stLine = TRUE;
	wchar_t c;

	while((c = fgetwc(fichier)) != 	WEOF) {
		if(is1stLine) {
			if(c == 10) {
				is1stLine = false;
				i = 0;
			}
			else if(!is2nd) {
				if(c == 226 || c == 44 || c == 59) {
					title[i] = L'\u25B2';
					i++;
				}
				else if(c != 178 && c != 8211) {
					title[i] = c;
					i++;
				}
			}
		}
		else if(c != 178 && c != 8211) {
			if(c == 10) {
				j++;
				isComa = 0;
				i = 0;
			}
			else if(c == 226) {
				isComa++;
				i = 0;
			}
			else if((c != 226 && !is1stLine) && isComa <= 0) {
				tab[j].prm[i] = c;
				i++;
			}
			else if((c != 226 && !is1stLine) && isComa > 0) {
				tab[j].rest[isComa-1].colon[i] = c;
				i++;
			}
		}
	}
}

void compare_elems(FILE* fichier_a, elems_str* tab1, elems_str* tab2) {
	/* variables */
	int l;
	unsigned int i, j, k = 0;
	bool isSame, will_break;

	while(title[k] > 127)
		k++;
	if(k > 0) {
		chopN(title, k);
		k = 0;
	}
	fwprintf(fichier_a, L"%s%cEVT\n", title, sep);
	for(i=0; i<size_s1; i++, k++) {
		isSame = false;
		for(j=0; j<size_s2; j++) {
			if(wcscmp(tab1[i].prm, tab2[j].prm) == 0) {
				isSame = TRUE;
				break;
			}
		}
		if(!isSame) {
			fwprintf(fichier_a, L"%s\u25B2", tab1[i].prm);
			for(l=0; l<max_sep; l++)
				fwprintf(fichier_a, L"%s\u25B2", tab1[i].rest[l].colon);
			fwprintf(fichier_a, L"R\n");
		}
		dfrac = (gdouble)k;
	}
	for(i=0; i<size_s2; i++, k++) {
		isSame = FALSE;
		for(j=0; j<size_s1; j++) {
			if(wcscmp(tab2[i].prm, tab1[j].prm) == 0) {
				isSame = TRUE;
				will_break = FALSE;
				for(l=0; l<max_sep; l++) {
					if(wcscmp(tab2[i].rest[l].colon, tab1[j].rest[l].colon) != 0) {
						fwprintf(fichier_a, L"%s\u25B2", tab2[i].prm);
						for(l=0; l<max_sep; l++)
							fwprintf(fichier_a, L"%s\u25B2", tab2[i].rest[l].colon);
						fwprintf(fichier_a, L"M\n");
						will_break = TRUE;
						break;
					}
				}
				if(will_break)
					break;
			}
		}
		if(!isSame) {
			fwprintf(fichier_a, L"%s\u25B2", tab2[i].prm);
			for(l=0; l<max_sep; l++) {
				fwprintf(fichier_a, L"%s\u25B2", tab2[i].rest[l].colon);
			}
			fwprintf(fichier_a, L"S\n");
			break;
		}
		dfrac = (gdouble)k;
	}
	dfrac = max;
}

GtkWidget *AddBtn(char *label) {
	GtkWidget *pButton;
	pButton = gtk_button_new_with_label(label);
	/* connexion du signal "clicked" du bouton */
	g_signal_connect(G_OBJECT(pButton), "clicked", G_CALLBACK(gtk_main_quit), NULL);

	return pButton;
}

void alertDialog(char *err) {
	/* variables */
	GtkWidget *pDialog;
	GtkWidget *pLabel;
	iconname = "icons/cross-flat.png";

	/* Creation boite de dialog avec un bouton ok */
	if(strcmp(err, "FINISHED !\n") != 0) {
		pDialog = gtk_dialog_new_with_buttons("ERREUR !",
			GTK_WINDOW(SndWindow),
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			NULL);
	}
	else {
		pDialog = gtk_dialog_new_with_buttons("Resultat !",
			GTK_WINDOW(SndWindow),
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			NULL);	
	}
	pLabel = gtk_label_new(err);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pDialog)->vbox), pLabel, TRUE, FALSE, 0);

	gtk_window_set_position(GTK_WINDOW(SndWindow), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_position(GTK_WINDOW(pDialog), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_icon_from_file(GTK_WINDOW(pDialog), iconname, NULL);

	gtk_widget_show_all(pDialog);
	gtk_dialog_run(GTK_DIALOG(pDialog));
	gtk_widget_destroy(GTK_WIDGET(pDialog));
}

void fileChooserDialog(GtkWidget *pWindow, gpointer gp) {
	/* variables */
	int val = GPOINTER_TO_INT(gp);
	GtkWidget *pDialog;
	gint res;

	/* creation dialog */
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	pDialog = gtk_file_chooser_dialog_new("Ouvrir fichier", 
		NULL, action,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL);

	res = gtk_dialog_run(GTK_DIALOG(pDialog));

	/* action quand clique sur ouvrir */
	if(res == GTK_RESPONSE_ACCEPT) {
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(pDialog);
		if(val == 1) {
			file_s1 = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
			gtk_button_set_label(GTK_BUTTON(pButtonF1), file_s1);
		}
		else {
			file_s2 = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
			gtk_button_set_label(GTK_BUTTON(pButtonF2), file_s2);
		}
	}
	gtk_widget_destroy(pDialog);
}

void *secondWindow() {
	/* variables */
	iconname = "icons/delta.jpg";
	DWORD exitthread;
	char err[80];

	/* création du thread */
	HANDLE thread = CreateThread(NULL, 0, (void*)delta, NULL, 0, 0);
	GetExitCodeThread(thread, &exitthread);

	/* Use the created fill function every 500 milliseconds */
	g_timeout_add(500, fill, GTK_PROGRESS_BAR(pProgress));

	while(!isEnded && exitthread == STILL_ACTIVE) {
		if(gtk_events_pending()) 
			gtk_main_iteration();
	}

	if(result >= 0) {
		if(result == 0) {
			strcpy(err, "ERREUR: Erreur inconnue !");
			alertDialog(err);
		}
		if(result == 1) {
			strcpy(err, "Impossible d'ouvrir le fichier 1 ");
			if(strcmp(data[0], "ERREUR.csv") == 0)
				strcat(err, "\ncar aucun fichier de sélectioné");
			else
				strcat(err, data[0]);
			alertDialog(err);
		}
		if(result == 2) {
			strcpy(err, "Impossible d'ouvrir le fichier 2 ");
			if(strcmp(data[1], "ERREUR.csv") == 0)
				strcat(err, "\ncar aucun fichier de sélectioné");
			else
				strcat(err, data[1]);
			alertDialog(err);
		}
		if(result == 3) {
			strcpy(err, "Impossible de creer le fichier ");
			strcat(err, data[2]);
			alertDialog(err);
		}
		if(result == 4) {
			strcpy(err, "Veuillez saisir un fichier csv");
			alertDialog(err);
		}

		return NULL;
	}

	/******************/
	/*** 2ND WINDOW ***/
	/******************/

	/* Création 2nd fenetre */
	SndWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(SndWindow), "chargement");
	gtk_window_set_icon_from_file(GTK_WINDOW(SndWindow), iconname, NULL);
	gtk_window_set_default_size(GTK_WINDOW(SndWindow), 480, 300);
	g_signal_connect(G_OBJECT(SndWindow), "delete-event", G_CALLBACK(gtk_widget_destroy), SndWindow);

	/* insertion buttons/labels 2st window */
	GtkWidget *pButtonCancel;
	GtkWidget *v2box = gtk_vbox_new(10, 50);
	gtk_container_add(GTK_CONTAINER(SndWindow), v2box);
	GtkWidget *h2box = gtk_hbox_new(0, 0);
	gtk_container_add(GTK_CONTAINER(v2box), h2box);

	/*Create a progressbar and add it to the window*/
	pProgress = gtk_progress_bar_new();
	gtk_container_add(GTK_CONTAINER(v2box), pProgress);

	pButtonCancel = gtk_button_new_with_label("Cancel");
	g_signal_connect_swapped(pButtonCancel, "clicked", G_CALLBACK(gtk_widget_destroy), SndWindow);
	gtk_container_add(GTK_CONTAINER(v2box), pButtonCancel);

	/* Show 2nd window */
	gtk_widget_show_all(SndWindow);

	/* alertdialog + refresh everything */
	alertDialog("FINISHED !\n");
	gtk_widget_destroy(SndWindow);
	refresh_all();

	return NULL;
}

DWORD WINAPI delta(void *p_data) {
	/* variables */
	int i;
	char *file[3];
	const gchar *cdata[3];
	cdata[0] = file_s1;
	cdata[1] = file_s2;
	cdata[2] = gtk_entry_get_text(GTK_ENTRY(entries));
	for(i=0; i<3; i++) {
		if(cdata[i] != NULL) {
			data[i] = strdup(cdata[i]);
			file[i] = get_filename_ext(data[i]);
		}
		else {
			cdata[i] = "ERREUR.csv";
			data[i] = strdup(cdata[i]);
			file[i] = get_filename_ext(data[i]);
		}
	}
	const char *verify_ext = verify_filename_ext(file[0], file[1], file[2]);
	if(strcmp(verify_ext, "true") == 0) {
		fichier_s1 = fopen(data[0], "r");
		fichier_s2 = fopen(data[1], "r");
		if(fichier_s1 != NULL && fichier_s2 != NULL) {
			fichier_d = fopen(data[2], "wb+");
			size_s1 = file_size(fichier_s1);
			size_s2 = file_size(fichier_s2);
			/*fwprintf(fichier_d, L"ggwp %d\n", max_sep);
			fclose(fichier_d);*/
			max = (gdouble)(size_s1+size_s2);
			elems_tab1 = calloc(size_s1, sizeof(elems_str));
			elems_tab2 = calloc(size_s2, sizeof(elems_str));
			//functions
			store_elems(fichier_s1, elems_tab1);
			is2nd = TRUE;
			store_elems(fichier_s2, elems_tab2);
			compare_elems(fichier_d, elems_tab1, elems_tab2);

			if(elems_tab1 != NULL)
				free(elems_tab1);
			if(elems_tab2 != NULL)
				free(elems_tab2);
			result = -1;
			fclose(fichier_d);
		}
		else if(fichier_s1 == NULL)
			result = 1;
		else if(fichier_s2 == NULL)
			result = 2;
		else if(fichier_d == NULL)
			result = 3;
		fclose(fichier_s1);
		fclose(fichier_s2);
	}
	else
		result = 4;

	isEnded = TRUE;
	(void) p_data;
	return 0;
}

gboolean fill() {
	/*Fill in the given fraction of the bar. Has to be between 0.0-1.0 inclusive*/
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pProgress), dfrac/max);

  	/*Ensures that the fraction stays below 1.0*/
	if(dfrac/max < 1.0) 
		return TRUE;
	else
		return FALSE;
}

void refresh_all() {
	max = 1.0;
	dfrac = 0.0;
	isEnded = FALSE;
}