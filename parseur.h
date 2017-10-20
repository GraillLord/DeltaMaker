#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
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

typedef struct elems_str elems_str;
typedef struct entries_str entries_str;

struct elems_str {
	wchar_t prm[15];
	wchar_t name[40];
	wchar_t rest[500];
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

char *get_filename_ext(const char *);
const char *verify_filename_ext(const char *, const char *, const char *);
void chopN(wchar_t *, size_t);
//
GtkWidget *AddBtn(char *);
void alertDialog(char *);
void fileChooserDialog(GtkWidget *, gpointer);
//
int file_lines(FILE *);
void store_elems(FILE *, elems_str *);
void get_through_file(FILE *, long, char);
bool is_line_different(FILE *, FILE *, long, long);
void compare_elems(FILE *, elems_str *, elems_str *);
//
void *secondWindow();
DWORD WINAPI delta(void *);
gboolean fill();
//
void free_str(elems_str **, int);
void refresh_all();