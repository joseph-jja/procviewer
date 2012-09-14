#include "procviewer.h"


static void read_in_data (simpletext *proctext, char filename[FILELEN], 
		GdkFont *textfont)
{
	FILE *pFile;
	gint msglength = 0;
	GtkTextIter   start, end;                

        /* get text bounds and delete what is in the text */ 
	gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER(proctext->buffer), &start, &end);
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(proctext->buffer), &start, &end);
	
	if ((pFile=fopen(filename, "r")) == NULL) {
	    /* make text editable then get the length of the message 
	     * and then insert teh text and then set the text un editable again */ 
	    gtk_text_view_set_editable(GTK_TEXT_VIEW(proctext->textarea), TRUE);
	    msglength = strlen(ERRORFILENOTFOUND);	
	    gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(proctext->buffer), 
                            ERRORFILENOTFOUND, 
			    msglength);
            gtk_text_view_set_editable(GTK_TEXT_VIEW(proctext->textarea), FALSE);	
	} else {
		gchar *buffer, *data;
		gint buflen;
		buffer = (gchar*)g_malloc(BUFFLEN); 
		data = (gchar*)g_malloc(BUFFLEN); 
		
		while (!feof(pFile)) 
		{
			data = fgets(buffer, BUFFLEN, pFile);

                        buflen = strlen(buffer);
			
			if (feof(pFile)) {
			   break;
			}
	                /* make text editable then get the length of the message 
         	         * and then insert teh text and then set the text un editable again */ 
			gtk_text_view_set_editable(GTK_TEXT_VIEW(proctext->textarea), TRUE);
	                msglength = strlen(data);	
       	                gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(proctext->buffer), 
                            buffer, 
			    msglength);
                        gtk_text_view_set_editable(GTK_TEXT_VIEW(proctext->textarea), FALSE);
		}

		fclose(pFile);
		g_free(buffer); 
		g_free(data);
	}	
}  

gint call_update(simpletext *data) {

    GdkFont *font;
    font = NULL;

    if ((data) && (data->filename != NULL)) {
        read_in_data(data, data->filename, font);
	return TRUE;

    }
    return FALSE;
}



static void cb_selection_changed (GtkTreeSelection *selection, simpletext *treetext)
{
    gchar* filename, *name;
    GtkTreeIter iter;
    GtkTreeModel *model;
    int ud = 0; 
     
    if (atoi(treetext->rcode) != 0) {
        gtk_timeout_remove(atoi(treetext->rcode));
    }
    
    if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
        gtk_tree_model_get (model, &iter, MAIN_COLUMN, &name, -1);
        if (strncmp(name, NETDEV, strlen(NETDEV)) == 0) {
		filename = g_malloc(strlen(NETDIR) + 1);
		strcpy(filename, NETDIR);
	} else if (strncmp(name, DMESG, strlen(DMESG)) == 0) {
		filename = g_malloc(strlen(DMESGDIR) + 1);
		strcpy(filename, DMESGDIR);
	} else if (strncmp(name, "/proc", strlen("/proc")) == 0) {
		filename = g_malloc(strlen(DMESGDIR) + 1);
		strcpy(filename, DMESGDIR);
	} else {
		filename = g_malloc(strlen(name) + strlen("/proc/")+1);
                strcpy(filename,"/proc/");
                strcat(filename,name);
	}
	//g_print("%s\n", filename);
	memset(treetext->filename, '\0', sizeof(treetext->filename));
	memcpy(treetext->filename, filename, strlen(filename));
	ud = gtk_timeout_add(UPDATE_TIME, (GtkFunction)call_update, treetext);
        sprintf(treetext->rcode, "%d", ud);
        read_in_data(treetext, filename, NULL);
        g_free(filename);
    }
    g_free(name);
}

static void create_text (simpletext *create_text) {
			
	/* first lets create the scrolled region that the text area will go into */
	create_text->label = gtk_scrolled_window_new (NULL, NULL);
        gtk_container_set_border_width (GTK_CONTAINER (create_text->label), 2);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (create_text->label),
                                          GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show(create_text->label);
	
	/* now create the new gtk2.0 text widget */
	create_text->textarea = gtk_text_view_new ();
	create_text->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (create_text->textarea));
	gtk_widget_show(create_text->textarea);

        /* here is the container packing */
	gtk_container_add(GTK_CONTAINER(create_text->label),create_text->textarea); 
	
	/* set an initial size of 80 by 10 so that we can have 5 on a tab */
	gtk_widget_set_size_request (create_text->textarea, -1, -1);
	
	/* set editable duh */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(create_text->textarea), FALSE);
	
	/* turn off word wrap */
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(create_text->textarea), FALSE); 
}

int main (int argc, char *argv[])
{
    DIR *dp, *sp;
    struct dirent *ep;
    gchar *ctemp;
    
    GtkTreeStore *store;
    GtkWidget *tree;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkTreeIter iter1, iter2;
    GtkTreeSelection *selection;
	
	/* this here declairs the main note book page
	and the main vbox and the main window*/
	GtkWidget *mainbox, *scrolled_win;
	GtkWidget *topbox, *tabletop, *bottombox;
	GtkWidget *notebook_main;
	GtkWidget *window;
	
	/* quit and save buttons, eventually
	a menu will be used */
	GtkWidget *quitbutton;

        PangoFontDescription *font_desc;
	
	header_data load_data;
	header_data uptimedata;
	header_data cmdlinedata;
	header_data versiondata;
	
	simpletext treetext;

                /* initialize a few things here */
                gtk_init (&argc, &argv);

	strcpy(treetext.rcode, "0");
	
        /* now set up the font */
        font_desc = pango_font_description_from_string ("Sans 12");

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
	gtk_signal_connect (GTK_OBJECT (window), "delete_event", 
			GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
	
	/* some window things here */		
	gtk_widget_set_usize (window,700,550);
	gtk_window_set_title(GTK_WINDOW(window), "procviewer");
	
	/* A generic scrolled window */
    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_usize (scrolled_win, 250, 100);

	/* this here creates the notebook */
	notebook_main = gtk_frame_new("procviewer");
	
	gtk_widget_set_usize (notebook_main,625,300);

	mainbox = gtk_vbox_new (FALSE, 1);
	topbox = gtk_vbox_new (TRUE, 1);
	bottombox = gtk_hbox_new (FALSE, 1);
	tabletop = gtk_table_new(4,4,FALSE);

    gtk_container_add (GTK_CONTAINER(bottombox), scrolled_win);
    gtk_widget_show (scrolled_win);

	quitbutton = gtk_button_new_with_label ("Quit");
	gtk_widget_show(quitbutton);
	
	gtk_widget_show(topbox);
	gtk_widget_show(bottombox);
	gtk_widget_show(mainbox);
	gtk_widget_show(tabletop);
	
	gtk_box_pack_start (GTK_BOX(mainbox),topbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(mainbox), bottombox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(bottombox), notebook_main, TRUE, TRUE, 0);
	
	gtk_signal_connect (GTK_OBJECT (quitbutton), "clicked", 
			GTK_SIGNAL_FUNC (destroy_event), NULL);
	
	gtk_signal_connect_object (GTK_OBJECT (quitbutton), "clicked",
			GTK_SIGNAL_FUNC (gtk_widget_destroy), 
			GTK_OBJECT (window));
	
	gtk_container_add (GTK_CONTAINER (window), mainbox);
	
	gtk_box_pack_start (GTK_BOX(topbox), tabletop, FALSE, FALSE, 0);
	gtk_table_attach (GTK_TABLE(tabletop), quitbutton, 0, 4, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

	/* now we create the textwidget */	
	create_text (&treetext);

        gtk_widget_modify_font (GTK_WIDGET(treetext.textarea), font_desc);

	/* now we pack it */
	gtk_container_add(GTK_CONTAINER(notebook_main), treetext.label);	
        read_in_data(&treetext, DMESGDIR, NULL);

	/* Create the root tree */
	store = gtk_tree_store_new (N_COLUMNS,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_BOOLEAN);
	
	/* this is the top level of the tree */
	gtk_tree_store_append (store, &iter1, NULL);  /* Acquire a top-level iterator */
        gtk_tree_store_set (store, &iter1,
                    MAIN_COLUMN, "/proc",
                    -1);
		    
	gtk_tree_store_append (store, &iter2, &iter1);  /* Acquire a top-level iterator */
        gtk_tree_store_set (store, &iter2,
                    MAIN_COLUMN, "dmesg",
                    -1);	    
	
	dp = opendir("/proc");
        if (dp != NULL) {
			
            while((ep = readdir(dp)) != NULL) {
                ctemp = (gchar*)calloc(strlen(ep->d_name)+10, 1);
	        if (ctemp != NULL) { 
	        	strcpy(ctemp, "/proc/");
	        	strcat(ctemp, ep->d_name);
				sp = opendir(ctemp);
	        	if((sp == NULL) && 
	        			(strcmp(g_basename(ctemp), "kcore") != 0)
	        			&& (strcmp(g_basename(ctemp), "kmsg") != 0)
	        			&& (strcmp(g_basename(ctemp), "ksyms") != 0)) {
             
			             //struct stat buf;
				       
                         /* Create a subtree item, in much the same way */
                         /* Acquire a top-level iterator */
			 gtk_tree_store_append (store, &iter2, &iter1);  
                         gtk_tree_store_set (store, &iter2,
                                     MAIN_COLUMN, g_basename(ctemp),
                                         -1);
                       }
		       g_free(ctemp);
                    }
                }
            }
	    	       
        /* Create a view */
        tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

        gtk_widget_modify_font (GTK_WIDGET(tree), font_desc);

       /* The view now holds a reference.  We can get rid of our own
        * reference */
        g_object_unref (G_OBJECT (store));

        /* Create a cell render and arbitrarily make it red for demonstration
         * purposes */
        renderer = gtk_cell_renderer_text_new ();
        g_object_set (G_OBJECT (renderer),
                 "foreground", "red",
                 NULL);
         /* Second column.. title of the book. */
         renderer = gtk_cell_renderer_text_new ();
         column = gtk_tree_view_column_new_with_attributes ("Title",
                                                      renderer,
                                                      "text", MAIN_COLUMN,
                                                      NULL);
	 
	 selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	 //gtk_tree_selection_set_mode (selection, GTK_SELECT_SINGLE);
	 g_signal_connect (G_OBJECT(selection), "changed",
               G_CALLBACK(cb_selection_changed), &treetext);
	 
         gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

         gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scrolled_win),
            tree);
         gtk_widget_show(tree);

    /* header starts here */	
	populate_str(&cmdlinedata,"Kernel Command Line ", "/proc/cmdline");
	gtk_table_attach (GTK_TABLE(tabletop), cmdlinedata.label, 0, 1, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach (GTK_TABLE(tabletop), cmdlinedata.widget, 1,4,3,4, GTK_FILL, GTK_SHRINK, 0, 0);
	update_data(&cmdlinedata);
	
	populate_str(&uptimedata,"System Uptime", "/proc/uptime");
	gtk_table_attach (GTK_TABLE(tabletop), uptimedata.label, 0, 1, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach (GTK_TABLE(tabletop), uptimedata.widget, 1,2,2,3, GTK_EXPAND, GTK_SHRINK, 0, 0);
	uptime_update(&uptimedata);
	
	populate_str(&load_data,"Load Average (1,5,15min)", "/proc/loadavg");
	gtk_table_attach (GTK_TABLE(tabletop), load_data.label, 2, 3, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach (GTK_TABLE(tabletop), load_data.widget, 3,4,2,3, GTK_EXPAND, GTK_SHRINK, 0, 0);
	update_load(&load_data);
	
	populate_str(&versiondata,"System Version ","/proc/version");
	gtk_table_attach (GTK_TABLE(tabletop), versiondata.widget, 0,4,1,2, GTK_FILL, GTK_SHRINK, 0, 0);
	getheaderinfo(&versiondata);
    /* header ends here */	
	
	gtk_widget_show(notebook_main);
	
        gtk_widget_show (window);
	
	/* fill some structure here */
	gtk_timeout_add(TIME_UPDATE, (GtkFunction)uptime_update, &uptimedata);
	
	gtk_timeout_add(UPDATE_TIME+4, (GtkFunction)update_load, &load_data);
	
        pango_font_description_free (font_desc);

	gtk_main ();
 
    return 0;
}


static void destroy_event (GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static void update_data (struct header_data *sent_struct)
{
	FILE *pFile;

	if ((pFile=fopen(sent_struct->filename, "r")) == NULL) {
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),ERRORFILENOTFOUND);
	} else {
		gchar *buffer;
		int nchars;
		buffer = calloc(STATUSLEN,sizeof(gchar)); 

		nchars = fread(buffer, 1, STATUSLEN, pFile); 
		if (buffer[nchars-1] == '\n') {
		    buffer[nchars-1] = '\0';
		}
		gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget), TRUE);
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),buffer);
		gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget),FALSE);
		
		fclose(pFile);
		
		g_free(buffer);
		
	}
}
static int update_load (struct header_data *sent_struct)
{
	FILE *pFile;
	float load_onemin, load_fivemin, load_fifmin;
	gchar *buf, *buffer;
	int nchars;
		
	if ((pFile=fopen(sent_struct->filename, "r")) == NULL) {
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),ERRORFILENOTFOUND);
		return 0;
	}
	buffer = calloc(STATUSLEN,sizeof(gchar)); 
	buf = calloc(STATUSLEN,sizeof(gchar)); 
		
	nchars = fread(buffer, 1, STATUSLEN, pFile); 
	sscanf(buffer," %f %f %f ",&load_onemin, &load_fivemin, &load_fifmin);
	sprintf(buf," %.2f %.2f %.2f ",load_onemin, load_fivemin, load_fifmin);
	gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget), TRUE);
	gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),buf);
	gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget), FALSE);
	
	fclose(pFile);
	
	g_free(buf);
	g_free(buffer);
	
	return 1;
}
static int uptime_update (struct header_data *sent_struct)
{
	FILE *pFile;
	struct tm *realtime;
        time_t realseconds;
	gchar *buf, *tempbuf, *buffer;
	float uptime_secs, idle_secs;
	int upminutes, uphours, updays, nchars;
		

 	time(&realseconds);
	realtime = localtime(&realseconds);  	
		
	if ((pFile=fopen(sent_struct->filename, "r")) == NULL) {
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),ERRORFILENOTFOUND);
		return 0;
	} 

	buf = calloc(STATUSLEN,sizeof(gchar)); 
	tempbuf = calloc(STATUSLEN,sizeof(gchar)); 
	buffer = calloc(STATUSLEN,sizeof(gchar)); 
	
	sprintf(buf, "%2d:%02d%s  up ", realtime->tm_hour%12 ? realtime->tm_hour%12 : 12,
		realtime->tm_min, realtime->tm_hour > 11 ? "pm" : "am");
		
	nchars = fread(buffer, 1, STATUSLEN, pFile);
	sscanf(buffer," %f %f ",&uptime_secs, &idle_secs);
	updays = (int) uptime_secs / (60*60*24);
	if (updays) {
		sprintf(tempbuf , "%d day%s ", updays, (updays != 1) ? "s" : "");
		strcat(buf,tempbuf);
	}
	upminutes = (int) uptime_secs / 60;
  	uphours = upminutes / 60;
  	uphours = uphours % 24;
  	upminutes = upminutes % 60;
  	if(uphours) {
  		sprintf(tempbuf, "%2d:%02d ", uphours, upminutes);
		strcat(buf,tempbuf);
  	} else {
  		sprintf(tempbuf, "%d min ", upminutes);
		strcat(buf,tempbuf);
	}
	gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget), TRUE);
	gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),buf);
	gtk_entry_set_editable(GTK_ENTRY(sent_struct->widget), FALSE);
	
	fclose(pFile);
	
	g_free(buffer);
	g_free(tempbuf);
	g_free(buf);
	
	return 1;
}
static void populate_str (struct header_data *widget, gchar *labelname, gchar *filename) {
	widget->label = gtk_label_new(labelname);
	gtk_widget_show(widget->label);	
	widget->widget = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(widget->widget),FALSE);		
	gtk_widget_show(widget->widget);	
	strcpy(widget->filename,filename);
}

static void getheaderinfo (struct header_data *sent_struct)
{
	FILE *pFile;

	if ((pFile=fopen(sent_struct->filename, "r")) == NULL) {
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),ERRORFILENOTFOUND);
	} else {
		gchar buffer[255];

                memset(buffer, '\0', sizeof(buffer));	
		gtk_entry_set_text(GTK_ENTRY(sent_struct->widget),"System Version Info: ");

                fgets(buffer, sizeof(buffer)-1 , pFile); 
                memset(buffer+strlen(buffer)-1, '\0', 1);

		gtk_entry_append_text(GTK_ENTRY(sent_struct->widget),buffer);
		fclose(pFile);
	}
} 
