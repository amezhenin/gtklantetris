#include <gtk/gtk.h>
 
#include "tetris.h"

GtkWidget *help_window;
GtkWidget *about_window;
GtkWidget *highscore_window;

void hide_about(GtkMenuItem *widget, gpointer user_data)
{
	gtk_widget_destroy(about_window);
	about_window = NULL;
}
void show_about(GtkMenuItem     *menuitem,
        gpointer         user_data)
{
	if(about_window!=NULL) return;
	
    GtkWidget *about_label;
	GtkWidget *close_button;
	GtkWidget *vbox;
	GtkWidget* hbox;	

  	about_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (about_window), "destroy",
                                G_CALLBACK (hide_about), NULL);	
    vbox = gtk_vbox_new(FALSE,3);
    hbox = gtk_hbox_new(FALSE,3);
	gtk_container_add(GTK_CONTAINER(about_window),vbox);

    #ifdef MAEMO
    	GtkWidget *scroll;
    	gtk_window_set_resizable (GTK_WINDOW(about_window),TRUE);
    	gtk_widget_set_size_request (about_window, 400, 250);
    	scroll = gtk_scrolled_window_new(NULL,NULL);
  		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                    GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
		gtk_container_add(GTK_CONTAINER(vbox),scroll);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), hbox);
    #else
    	gtk_window_set_resizable (GTK_WINDOW(about_window),FALSE);
    	gtk_container_add(GTK_CONTAINER(vbox),hbox);
	#endif 
  	
  	GtkWidget *image;
    image = gtk_image_new_from_stock (GTK_STOCK_ABOUT, GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,10);
    
  
    about_label = gtk_label_new(    
				    "\n   GtkLanTetris v0.1\n\n"
				    "Extended tetris with 2-players mode.\n"
				    "Play with friend,computer or over the internet.\n\n"
                    "2008, Mezhenin Artyom\n"
                    /*"\tDenisov Denis (computer opponent)\n\n"*/
                    "Based on source code GTK Tetris v0.6.2\n"
                    "This program is distributed under the terms of GPL.\n");

	gtk_box_pack_start(GTK_BOX(hbox),about_label,FALSE,FALSE,3);

	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,0);
	
	close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	
	g_signal_connect ((gpointer) close_button, "clicked",
                G_CALLBACK (hide_about),NULL); 
	gtk_box_pack_end(GTK_BOX(hbox),close_button,FALSE,FALSE,3);
  	GTK_WIDGET_SET_FLAGS(close_button, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(close_button);
	
    gtk_widget_show_all (about_window);
}

void hide_help(GtkMenuItem     *menuitem,
           gpointer         user_data)
{
	gtk_widget_destroy(help_window);
	help_window = NULL;
}

void show_help(GtkMenuItem     *menuitem,
           gpointer         user_data)
{
	if(help_window!=NULL) return;
    
    GtkWidget *help_label;
    GtkWidget *hbox;
	GtkWidget *close_button;
	GtkWidget *vbox;

	help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (help_window), "destroy",
                                G_CALLBACK (hide_help), NULL);	
    vbox = gtk_vbox_new(FALSE,3);
	gtk_container_add(GTK_CONTAINER(help_window),vbox);
    hbox = gtk_hbox_new(FALSE,30);
    #ifdef MAEMO
    	GtkWidget *scroll;
    	gtk_window_set_resizable (GTK_WINDOW(help_window),TRUE);
    	gtk_widget_set_size_request (help_window, 450, 350);
    	scroll = gtk_scrolled_window_new(NULL,NULL);
  		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                    GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
		gtk_container_add(GTK_CONTAINER(vbox),scroll);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), hbox);
    #else
    	gtk_window_set_resizable (GTK_WINDOW(help_window),FALSE);
    	gtk_container_add(GTK_CONTAINER(vbox),hbox);
	#endif	

	
    help_label = gtk_label_new( "\nKeyboard:\n"
                    "move right\n"
                    "move left\n"
                    "move down\n"
                    "rotate ccw\n"
                    "rotate cw\n"
                    "drop block\n\n"
                    "Use mouse to move blocks\n\n"
                    "40\n100\n"
                    "300\n1200\n"
                     "Score: score*level\n"

                    "Drop bonus: rows*level");
    gtk_misc_set_alignment(GTK_MISC(help_label),0,0);   
    gtk_label_set_justify(GTK_LABEL(help_label),GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(hbox),help_label,TRUE,TRUE,TRUE);
    	
    help_label = gtk_label_new( "\nPlayer 1:\n"
                    "D\n"
                    "A\n"
                    "S(in single game)\n"
                    "W\n"
                    "X\n"
                    "Space and S\n\n\n\n"
                    "Single\n"
                    "Double\n"
                    "Triple\n"
                    "TETRIS" );
    gtk_misc_set_alignment(GTK_MISC(help_label),0,0);   
    gtk_label_set_justify(GTK_LABEL(help_label),GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(hbox),help_label,TRUE,TRUE,TRUE);
    
    help_label = gtk_label_new( "\nPlayer 2:\n"
                    "Right\n"
                    "Left\n"
                    "-\n"
                    "Up\n"
                    "-\n"
                    "Down\n" );
    gtk_misc_set_alignment(GTK_MISC(help_label),0,0);   
    gtk_label_set_justify(GTK_LABEL(help_label),GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(hbox),help_label,TRUE,TRUE,TRUE);

	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,0);
	
	close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	
	g_signal_connect ((gpointer) close_button, "clicked",
                G_CALLBACK (hide_help),NULL); 
	gtk_box_pack_end(GTK_BOX(hbox),close_button,FALSE,FALSE,3);
  	GTK_WIDGET_SET_FLAGS(close_button, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(close_button);
	

    gtk_widget_show_all (help_window);
    
}
void hide_highscore(GtkMenuItem *widget, gpointer user_data)
{
	gtk_widget_destroy(highscore_window);
	highscore_window = NULL;
}

/* Show High-scores dialog */
void show_highscore(int place)
{
	if(highscore_window != NULL) return;
	
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *close_button;
	GtkWidget *vbox;
	GtkWidget* hbox;
	
	int temp;
	char dummy[40];
    
    highscore_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (highscore_window), "destroy",
                                G_CALLBACK (hide_highscore), NULL);	
    table = gtk_table_new(NUM_HIGHSCORE+1,5,FALSE);
    	
    vbox = gtk_vbox_new(FALSE,3);
	gtk_container_add(GTK_CONTAINER(highscore_window),vbox);
    
    #ifdef MAEMO
    	GtkWidget *scroll;
    	gtk_window_set_resizable (GTK_WINDOW(highscore_window),TRUE);
    	gtk_widget_set_size_request (highscore_window, 350, 260);
    	scroll = gtk_scrolled_window_new(NULL,NULL);
  		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                    GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
		gtk_container_add(GTK_CONTAINER(vbox),scroll);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), table);
    #else
    	gtk_window_set_resizable (GTK_WINDOW(highscore_window),FALSE);
    	gtk_container_add(GTK_CONTAINER(vbox),table);
	#endif                                    
	
	label = gtk_label_new(" # ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
	gtk_widget_set_usize(label,25,0);
	
	label = gtk_label_new(" Name: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,0,1);
	gtk_widget_set_usize(label,100,0);
	
	label = gtk_label_new(" Lines: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,0,1);
	
	label = gtk_label_new(" Level: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,3,4,0,1);
	
	label = gtk_label_new(" Score: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,0,1);
	
	for(temp=0;temp < NUM_HIGHSCORE;temp++)
	{
		sprintf(dummy,"%d",temp+1);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);	
		
		sprintf(dummy,"%s",highscore[temp].name);
      		label = gtk_label_new(dummy);

		if(place && place-1 == temp)
		  set_gtk_color_style(label,0,0,0xffff);
		gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%d",highscore[temp].lines);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%d",highscore[temp].level);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,3,4,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%lu",highscore[temp].score);
		label = gtk_label_new(dummy);
		if(place && place-1 == temp)
		  set_gtk_color_style(label,0xffff,0,0);
		gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),1,0);
	}

	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,0);
	
	close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	
	g_signal_connect ((gpointer) close_button, "clicked",
                G_CALLBACK (hide_highscore),NULL); 
	gtk_box_pack_end(GTK_BOX(hbox),close_button,FALSE,FALSE,3);
  	GTK_WIDGET_SET_FLAGS(close_button, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(close_button);
	
    gtk_widget_show_all (highscore_window);
}

void show_preferences(GtkMenuItem     *menuitem,
           gpointer         user_data)
{
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *table;
    GtkAdjustment *adj;
    GtkWidget *pref_dlg;
    GtkWidget *radio_plr1;
    GtkWidget *radio_plr2;
    GtkWidget *show_next_block;
    GtkWidget *easy_mode;    
    GtkWidget *spin_level;
    GtkWidget *spin_noise_level;
    GtkWidget *spin_noise_height;    
        
    pref_dlg = gtk_dialog_new_with_buttons ("Preferences",
                                GTK_WINDOW(main_window),
                                GTK_DIALOG_NO_SEPARATOR,
                                GTK_STOCK_APPLY,
                                GTK_RESPONSE_APPLY, 
                                GTK_STOCK_CLOSE,
                                GTK_RESPONSE_CANCEL,                               
                                NULL);
    
    gtk_window_set_resizable (GTK_WINDOW(pref_dlg),FALSE); 
    gtk_container_border_width(GTK_CONTAINER (GTK_DIALOG(pref_dlg)),3);                                
    
    show_next_block = gtk_check_button_new_with_mnemonic  ("Show next block");
    if (options.shw_nxt) 
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(show_next_block), TRUE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pref_dlg)->vbox),show_next_block,TRUE,TRUE,0);                    
    
    easy_mode = gtk_check_button_new_with_mnemonic  ("Easy mode(only for multiplayer)");
    if (options.easy_mode) 
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(easy_mode), TRUE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pref_dlg)->vbox),easy_mode,TRUE,TRUE,0);  
    
    frame = gtk_frame_new("Who can move blocks by mouse?");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pref_dlg)->vbox),frame,TRUE,TRUE,0);
    
    GtkWidget* hbox;
    hbox = gtk_hbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(frame),hbox);
    
    radio_plr1 = gtk_radio_button_new_with_label (NULL, "Player 1");
    gtk_widget_show(radio_plr1);  
	gtk_box_pack_start(GTK_BOX(hbox),radio_plr1,TRUE,TRUE,0);
	
	radio_plr2 = gtk_radio_button_new_with_label_from_widget 
					(GTK_RADIO_BUTTON (radio_plr1), "Player 2");
    gtk_widget_show(radio_plr2);                                                     
	gtk_box_pack_start(GTK_BOX(hbox),radio_plr2,TRUE,TRUE,0);
	if(options.mouse_plr == 2)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio_plr2), TRUE);

	
    frame = gtk_frame_new("Level Settings");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pref_dlg)->vbox),frame,TRUE,TRUE,0);
    
    table = gtk_table_new(3,2,TRUE);
    gtk_container_add(GTK_CONTAINER(frame),table);
    
    label = gtk_label_new("Start level:");
    adj = (GtkAdjustment *)gtk_adjustment_new(options.level,0,
                            NUM_LEVELS-1,1,1,0);
    spin_level = gtk_spin_button_new(adj,0,0);    
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
    gtk_table_attach_defaults(GTK_TABLE(table),spin_level,1,2,0,1);
    
    label = gtk_label_new("Noise level:");
    adj = (GtkAdjustment *)gtk_adjustment_new(options.noise_l,0,MAX_X-1,1,1,0);
    spin_noise_level = gtk_spin_button_new(adj,0,0);  
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,1,2);
    gtk_table_attach_defaults(GTK_TABLE(table),spin_noise_level,1,2,1,2);
    
    label = gtk_label_new("Noise height:");
    adj = (GtkAdjustment *)gtk_adjustment_new(options.noise_h,0,MAX_Y-4,1,1,0);
    spin_noise_height = gtk_spin_button_new(adj,0,0); 
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
    gtk_table_attach_defaults(GTK_TABLE(table),spin_noise_height,1,2,2,3);
    
    gtk_widget_show_all(pref_dlg);
    gint res = gtk_dialog_run (GTK_DIALOG (pref_dlg)); 
    if(res == GTK_RESPONSE_APPLY)
    {
        options.level = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_level));
        options.noise_l = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_level));
        options.noise_h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_height));
        options.shw_nxt = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (show_next_block)); 
        options.easy_mode = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (easy_mode)); 
		
		if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radio_plr1)))
		{
			options.mouse_plr = 1;
			def_mouse_plr = &player1;
		}
		else
		{
			options.mouse_plr = 2;
			def_mouse_plr = &player2;
		}
        
        update_game_values();
    }
    gtk_widget_destroy(pref_dlg);
}
