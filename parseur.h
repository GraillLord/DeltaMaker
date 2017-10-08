#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <windows.h>

/**************************/
/*** VARIABLES GLOBALES ***/
/**************************/

const gchar *iconname;
const gchar *iconcross;
GtkWidget *pButtonF1;
GtkWidget *pButtonF2;
GtkWidget *pProgress;
GtkWidget *entries;

/******************/
/*** STRUCTURES ***/
/******************/

typedef struct data_str data_str;
typedef struct elems_str elems_str;
typedef struct entries_str entries_str;

struct data_str {
	wchar_t colon[200];
};

struct elems_str {
	wchar_t prm[50];
	data_str rest[];
};

struct entries_str {
	char entryn1[100];
	char entryn2[100];
	GtkWidget *entry1;
	GtkWidget *entry2;
};

/******************/
/*** PROTOTYPES ***/
/******************/

int file_size(FILE *);
char *get_filename_ext(const char *);
const char *verify_filename_ext(const char *, const char *, const char *);
void chopN(wchar_t *, size_t);
//
GtkWidget *AddBtn(char *);
void alertDialog(char *);
void fileChooserDialog(GtkWidget *, gpointer);
//
void store_elems(FILE *, elems_str *);
void compare_elems(FILE *, elems_str *, elems_str *);
//
void *secondWindow();
DWORD WINAPI delta(void *);
gboolean fill();
void refresh_all();