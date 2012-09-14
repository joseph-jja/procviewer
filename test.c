#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <glib.h>

#include <unistd.h>

int main (int argc, char *argv[])
{
    /* initialize a few things here */
    gtk_init (&argc, &argv);

    sleep(10);

    gtk_main ();
 
    return 0;
}


