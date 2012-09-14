#ifndef __PROCVIEWER__
#define __PROCVIEWER__ 1

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gdk/gdk.h>
#include <glib.h>

#include <sys/types.h>
#include <dirent.h> 

#include <sys/stat.h>
#include <unistd.h>        

#define FILELEN 50
#define BUFFLEN 1024
#define UPDATE_TIME 5000
#define RTC_UPDATE_TIME 1000
#define TIME_UPDATE 30000
#define STATUSLEN 255
#define ERRORFILENOTFOUND "Could not find or open the file!"

#define VERSION "0.1.0"
#define PROGRAMNAME "procviewer"

#define DMESGDIR "/var/log/dmesg"
#define DMESG "dmesg"

#define NETDIR "/proc/net/dev"
#define NETDEV "netdev"

/* this enum is for the tree */
enum
{
   MAIN_COLUMN,
   N_COLUMNS
};

/* here is the structure to store the information 
for the the top info on the window */
typedef struct header_data {
    GtkWidget *label;
    GtkWidget *widget;
    char filename[FILELEN];
} header_data;

typedef struct simpletext {
	char rcode[255];
        GtkWidget *label;
        GtkWidget *textarea;
	GtkTextBuffer *buffer;
	char      filename[FILELEN];
	GdkFont   *textfont;
} simpletext;

static void destroy_event (GtkWidget *, gpointer );
static void update_data (struct header_data *);
static int update_load (struct header_data *);
static int uptime_update (struct header_data *);
static void populate_str (struct header_data *, char *labelname, char *);
static void getheaderinfo (struct header_data *);

#endif
