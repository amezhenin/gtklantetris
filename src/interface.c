#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "tetris.h"

#ifdef MAEMO
#include <libosso.h>
#endif

#ifdef BIGBLOCKS
#include "../data/big_blocks.xpm"
#else
#ifdef BIGGERBLOCKS
#include "../data/bbig_blocks.xpm"
#else
#include "../data/blocks.xpm"
#endif
#endif

#ifndef APP_ICON
#define APP_ICON "gtklantetris.png"
#endif

#define MOUSE_CLICK_TIMEOUT 200
#define RESEND_MSG_TIMEOUT 100
#define RETRY_ACCEPT 100

char options_f[100];
char *pause_str[2]={"Pause\0","Resume\0"};
char *start_stop_str[2]={"Start game\0","Stop game\0"};

GtkWidget *menu_game_quick;
GtkWidget *menu_game_start;
GtkWidget *menu_game_stop;
GtkWidget *menu_game_quit;
GtkWidget *menu_game_pause;
GtkWidget *menu_single_radio;
GtkWidget *menu_twoplr_radio;
GtkWidget *menu_vscomp_radio;
GtkWidget *Start_stop_button;
GtkWidget *Start_stop_button_label;
GtkWidget *Pause_button;
GtkWidget *Pause_button_label;
GtkWidget *game_opp_border;  
GtkWidget *next_block_opp_border;

GtkWidget *menu_settings;
GtkWidget *menu_network;

GtkWidget *accept_dlg;
GtkWidget *accept_progr_bar;


gint accept_timer;

int level_speeds[NUM_LEVELS] = {1000,886,785,695,616,546,483,428,379,336,298,
                264,234,207,183,162,144,127,113,100};   

int move_block_by_mouse();

void update_game_values()
{
    char dummy[20] = "";

    sprintf(dummy,"%d",player1.current_score);
    gtk_label_set(GTK_LABEL(player1.score_label), dummy);
    sprintf(dummy,"%d",player1.current_level);
    gtk_label_set(GTK_LABEL(player1.level_label), dummy);
    sprintf(dummy,"%d",player1.current_lines);
    gtk_label_set(GTK_LABEL(player1.lines_label), dummy);
    
    if(game_type != SINGLE_GAME)
    {
        sprintf(dummy,"%d",player2.current_score);
        gtk_label_set(GTK_LABEL(player2.score_label), dummy);
        sprintf(dummy,"%d",player2.current_level);
        gtk_label_set(GTK_LABEL(player2.level_label), dummy);
        sprintf(dummy,"%d",player2.current_lines);
        gtk_label_set(GTK_LABEL(player2.lines_label), dummy);    
    }
}

gint keyboard_event_handler(GtkWidget *widget,
                GdkEventKey *event,
                gpointer data)
{
 
  if(game_state != GAME_RUNNING)
    return FALSE;
 
  if(game_type == TWO_PLAYERS)
    switch(event->keyval)
    {
    case  GDK_Up: 
      move_block(0,0,-1,&player2); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_s: case GDK_S:  
      drop_block(&player1);  
      update_game_values();
      return TRUE;
      break;
    case GDK_Left: 
      move_block(-1,0,0,&player2); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_Right: 
      move_block(1,0,0,&player2); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_Down:
      drop_block(&player2);  
      update_game_values();
      event->keyval=0;
      return TRUE;
     break;    
  }
  
  switch(event->keyval)
  {
    case GDK_x: case GDK_X:
      move_block(0,0,1,&player1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_w: case GDK_W: case GDK_Up: 
      move_block(0,0,-1,&player1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_s: case GDK_S:  
      move_down(&player1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_a: case GDK_A: case GDK_Left: 
      move_block(-1,0,0,&player1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_d: case GDK_D: case GDK_Right: 
      move_block(1,0,0,&player1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_space: case GDK_Down:
      drop_block(&player1);  
      update_game_values();
      event->keyval=0;
      return TRUE;
     break;
  }
  return FALSE;
}

void game_type_changed(GtkMenuItem     *menuitem,
              gpointer         user_data)
{
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_single_radio)))
    {
        gtk_widget_hide(game_opp_border);
        gtk_widget_hide(next_block_opp_border);
        gtk_widget_hide(player2.score_label);
        gtk_widget_hide(player2.level_label);
        gtk_widget_hide(player2.lines_label);
        game_type = SINGLE_GAME;
    } 
    else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_twoplr_radio)))
    {   
        game_type = TWO_PLAYERS;
        player1.name = "Player 1";
        player2.name = "Player 2";
        gtk_widget_show(game_opp_border);
        gtk_widget_show(next_block_opp_border);
        gtk_widget_show(player2.score_label);
        gtk_widget_show(player2.level_label);
        gtk_widget_show(player2.lines_label);
    }
    else
    {
        game_type = VS_COMPUTER;
        player1.name = "You";
        player2.name = "Computer";
        gtk_widget_show(game_opp_border);
        gtk_widget_show(next_block_opp_border);
        gtk_widget_show(player2.score_label);
        gtk_widget_show(player2.level_label);
        gtk_widget_show(player2.lines_label);
    }
    options.game_type = game_type;  
}

/*void game_show_next_block(GtkMenuItem     *menuitem,
              gpointer         user_data)
{
    options.shw_nxt = !options.shw_nxt;
    if(game_state != GAME_OVER) 
    {
        if(!options.shw_nxt)
            draw_block(0,0,player1.next_block,player1.next_frame,TRUE,&player1,TRUE);
        else
            draw_block(0,0,player1.next_block,player1.next_frame,FALSE,&player1,TRUE);
    }
}*/
void redraw_all()
{
    from_virtual(&player1);
    move_block(0,0,0,&player1); 
    
    if(game_type != SINGLE_GAME)
    {
        from_virtual(&player2);
        move_block(0,0,0,&player2); 
    }
    if(options.shw_nxt)
    {
        draw_block(0,0,player1.next_block,player1.next_frame,FALSE, &player1,TRUE);
        if(game_type != SINGLE_GAME)
             draw_block(0,0,player2.next_block,player2.next_frame,FALSE, &player2,TRUE);               
    }
    update_game_values();
}
/* Callback func. for expose event. */
gint game_area_expose_event(GtkWidget       *widget,
                GdkEventExpose  *event,
                gpointer         user_data)
{   
    
    gdk_draw_rectangle(widget->window,
               widget->style->black_gc,
               TRUE, 0, 0,
               widget->allocation.width,
               widget->allocation.height);
    
    if(game_state == GAME_OVER) return FALSE;
    redraw_all();
    return FALSE;
}

/* Game timer interaption. We must move current block down once. */
int game_loop_plr1()
{
    if(game_state == GAME_RUNNING)
    {
      player1.timer = g_timeout_add(level_speeds[player1.current_level],
                  (GtkFunction)game_loop_plr1,NULL);
      
      move_down(&player1);
      /* Uncomment this and computer will allways helps you )) */
      //bot_step(&player1);
      if(mouse_hold && def_mouse_plr==&player1)
      {
          mouse_dy -= BLOCK_HEIGHT;
          move_block_by_mouse();
      }
    }
    /* Don't run this timer again */
    return FALSE;
}


int game_loop_plr2()
{
    if(game_state == GAME_RUNNING)
    {
        int speed = (game_type == LAN_GAME) ? RESEND_MSG_TIMEOUT 
                            : level_speeds[player2.current_level];    
        player2.timer = g_timeout_add(speed,(GtkFunction)game_loop_plr2,NULL);
        
        if(mouse_hold && def_mouse_plr==&player2)
        {
            mouse_dy -= BLOCK_HEIGHT;
            move_block_by_mouse();
        }
        
        /* We have different types of game */
        if (game_type == LAN_GAME)
        {
            send_message(0,0);         
            if (recv_message()==-2) 
            {   	
        		close_connection();
        		game_over_init(&player2);
        	}
            /* Redrawing must be more intelectual. 
             * Search for difference bitween "new"(tmp_plr) 
             * and "old"(player2) structure */
            
            if( (player2.next_block != tmp_plr.next_block || 
                 player2.next_frame != tmp_plr.next_frame ) &&
                 options.shw_nxt )
            {
                /* Clear old block with black */
                draw_block(0,0,player2.next_block,player2.next_frame,TRUE, &player2,TRUE);
                /* Draw new block */
                draw_block(0,0,tmp_plr.next_block,tmp_plr.next_frame,FALSE, &player2,TRUE);
                /* Refresh variables */
                player2.next_block = tmp_plr.next_block;
                player2.next_frame = tmp_plr.next_frame;
            }
            
            if(player2.current_block != tmp_plr.current_block || 
               player2.current_frame != tmp_plr.current_frame ||
               player2.current_x != tmp_plr.current_x ||
               player2.current_y != tmp_plr.current_y)
            {
                //draw_block(int x,int y,int block,int frame,int clear,PlrState* plr,int next)
                /* Clear old block with black */
                draw_block(player2.current_x,player2.current_y,player2.current_block,player2.current_frame,TRUE, &player2,FALSE);
                /* Draw new block */
                draw_block(tmp_plr.current_x,tmp_plr.current_y,tmp_plr.current_block,tmp_plr.current_frame,FALSE, &player2,FALSE);
                /* Refresh variables */
                player2.current_block = tmp_plr.current_block;
                player2.current_frame = tmp_plr.current_frame;
                player2.current_x = tmp_plr.current_x;
                player2.current_y = tmp_plr.current_y;
            }
            
            if(player2.current_score != tmp_plr.current_score || 
               player2.current_level != tmp_plr.current_level ||
               player2.current_lines != tmp_plr.current_lines )
            {
                /* Refresh variables */
                player2.current_score = tmp_plr.current_score ; 
                player2.current_level = tmp_plr.current_level ;
                player2.current_lines = tmp_plr.current_lines ;
                
                update_game_values();
            }
            
            int i,j;
            for(i=0;i<MAX_Y;i++)
                for(j=0;j<MAX_X;j++)
                    if(player2.virtual[i][j] != tmp_plr.virtual[i][j])
                    {
                        set_block(j,i,tmp_plr.virtual[i][j],&player2,FALSE);
                        player2.virtual[i][j] = tmp_plr.virtual[i][j];
                    }
        } 
        else
        {
            if(game_type == VS_COMPUTER)
            { 
                bot_step(&player2);
            }
            move_down(&player2);
        }          
    }    
    /* Don't run this timer again */
    return FALSE;
}

void reset_timer(PlrState *plr)
{
    if(!(plr->timer)) return;
    g_source_remove(plr->timer);
    /* Bug :^( */
    if(plr == &player1)
    {
        plr->timer = g_timeout_add(level_speeds[plr->current_level],
                  (GtkFunction)game_loop_plr1,NULL);   
    }
    else
    {
        int speed = (game_type == LAN_GAME) ? RESEND_MSG_TIMEOUT 
                            : level_speeds[player2.current_level];    
        player2.timer = g_timeout_add(speed,(GtkFunction)game_loop_plr2,NULL);                     
    }
}

/* Mouse timer interaption */
int game_area_timer()
{
    /* Click was too slow */
    mouse_clicked = FALSE;
    /* Don't run this timer again */
    return FALSE;
}

/* Mouse pressed in game area */
gint game_area_pressed(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data)
{
    if(game_state !=GAME_RUNNING )     return FALSE;    
    if(def_mouse_plr == &player2 && game_type != TWO_PLAYERS)  return FALSE;  
    if(def_mouse_plr->event_box != widget) return FALSE;   
    
    gdouble x, y;    
    gdk_event_get_coords (event,&x,&y);
    int tx = x/BLOCK_WIDTH;
    int ty = y/BLOCK_HEIGHT;
    if(is_brick_of_block(tx, ty, def_mouse_plr))
    {   
        mouse_hold = TRUE;
        mouse_init = TRUE;
        mouse_clicked = TRUE;
        mouse_timer = g_timeout_add(MOUSE_CLICK_TIMEOUT,
                (GtkFunction)game_area_timer,NULL);      
    }

    return FALSE;    
}

/* Mouse released in game area */
gint game_area_released(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data)
{
    if(game_state!=GAME_RUNNING) return FALSE;
    if(mouse_timer) 
    {
        g_source_remove(mouse_timer);    
        mouse_timer = 0;
    }
    mouse_hold = FALSE;
    mouse_init = FALSE;
    if(mouse_clicked)
        move_block(0,0,-1,def_mouse_plr);
    mouse_clicked = FALSE;    
    return FALSE;    
}

/* Move current block if mouse far from it */
int move_block_by_mouse()
{
    if( mouse_dx*mouse_dy == 0) return FALSE;
    int sign;
    while(abs(mouse_dx/BLOCK_WIDTH) > 0 ) 
    {
        sign = mouse_dx/abs(mouse_dx);
        if(!valid_position(def_mouse_plr->current_x + sign,
                           def_mouse_plr->current_y,
                           def_mouse_plr->current_block , 
                           def_mouse_plr->current_frame, def_mouse_plr)) break;
        move_block(sign,0,0, def_mouse_plr);
        mouse_dx -= BLOCK_WIDTH * sign;
    }
        
    while(mouse_dy/BLOCK_HEIGHT > 0 && valid_position(def_mouse_plr->current_x,
                            def_mouse_plr->current_y + 1,
                            def_mouse_plr->current_block , 
                            def_mouse_plr->current_frame, def_mouse_plr)) 
    {
        move_down(def_mouse_plr);
        reset_timer(def_mouse_plr);
        mouse_dy -= BLOCK_HEIGHT;
    }
    return FALSE;
} 
/* Mouse moved in game area */
gint game_area_move(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data)
{    

    static gdouble px, py;
    if(game_state !=GAME_RUNNING ) return FALSE;
      
    if(mouse_hold)
    {
        gdouble x, y; 
        if(!gdk_event_get_coords (event,&x,&y))  
            printf("game_area_move: error gdk_event_get_coords failed\n");

        if(mouse_init)
        {
            mouse_dx = mouse_dy =0;
            mouse_init = FALSE;
        }
        else
        {
            mouse_dx += x-px; 
            mouse_dy += y-py;  
        }
        px = x;
        py = y;
        move_block_by_mouse();
    }
    return FALSE;    
}

void game_set_pause_b()
{
    if(game_state == GAME_PAUSED) 
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),FALSE);
    else 
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),TRUE);
    return;
}

/* Pause/continue game by removing/adding timer(s) */
void game_set_pause(GtkWidget    *menuitem,
            gpointer         user_data)
{
    if (game_state == GAME_OVER)
        {
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),
                        FALSE);
        return;
        }
    game_state = (game_state == GAME_PAUSED) ? GAME_RUNNING : GAME_PAUSED;
    //game_pause = !game_pause;
    if(game_state == GAME_PAUSED) {
        g_source_remove(player1.timer);
        if(game_type != SINGLE_GAME)
            g_source_remove(player2.timer);
        gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[1]);
    }
    else {
        player1.timer = g_timeout_add(level_speeds[player1.current_level],
                    (GtkFunction)game_loop_plr1,NULL);
        if (game_type != SINGLE_GAME)
        {
            //if (player2.timer) g_source_remove(player1.timer);
            int speed = (game_type == LAN_GAME) ? RESEND_MSG_TIMEOUT 
                                : level_speeds[player2.current_level];    
            player2.timer = g_timeout_add(speed,(GtkFunction)game_loop_plr2,NULL);
        }                
        gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[0]);
    }
}

/* Run this function, when the game is over. Show highscores or messages
 * for diffrent types of game */
void game_over_init(PlrState *plr)
{
    game_state = GAME_OVER;
    gtk_widget_set_sensitive(menu_settings, TRUE);
    gtk_widget_set_sensitive(menu_network, TRUE);
    //gtk_menu_ite
    if ( game_type == SINGLE_GAME)
    {
        int high_dummy;
        read_highscore();
        high_dummy = addto_highscore((char *)getenv("USER"),
                    player1.current_score,
                    player1.current_level,
                    player1.current_lines);
        if(player1.current_score)
        {
            write_highscore();
            show_highscore(high_dummy);
        }
    }
    else
    {
         char dummy[44] = "";
         sprintf(dummy,"Game over! %s lose!\n",plr->name);
         if(game_type == LAN_GAME)
         {
            /* Send "game over" message */ 
            send_message(0,1);
            /* Close all sockets */
            close_connection();
            /* Boot old options */
            memcpy(&options,&tmp_opt,sizeof(options));
            /* Return old game type settings */
            game_type_changed(NULL,NULL);
         }
         //printf("%s",dummy);
         
         /* Display message who lose. Use different types of messages
          * for best look */
         #ifdef MAEMO
         hildon_banner_show_information(GTK_WIDGET(main_window), NULL, dummy);
         #else
         GtkWidget *dialog, *label;
         dialog = gtk_dialog_new_with_buttons ("Game over",
                               GTK_WINDOW(main_window),
                               GTK_DIALOG_MODAL| GTK_DIALOG_NO_SEPARATOR,
                               GTK_STOCK_OK,
                               GTK_RESPONSE_NONE,
                               NULL);
         label = gtk_label_new (dummy);

         g_signal_connect_swapped (dialog,
                             "response", 
                             G_CALLBACK (gtk_widget_destroy),
                             dialog);

         gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      label);
         gtk_widget_show_all (dialog);
         gtk_dialog_run (GTK_DIALOG (dialog));
         #endif
    }

    gdk_draw_rectangle(player1.game_area->window,
        player1.game_area->style->black_gc,
        TRUE,
        0,0,
        player1.game_area->allocation.width,
        player1.game_area->allocation.height);

    gdk_draw_rectangle(player1.next_block_area->window,
        player1.next_block_area->style->black_gc,
        TRUE,
        0,0,
        player1.next_block_area->allocation.width,
        player1.next_block_area->allocation.height);
    
    if (game_type != SINGLE_GAME)
    {
        gdk_draw_rectangle(player2.game_area->window,
            player2.game_area->style->black_gc,
            TRUE,
            0,0,
            player2.game_area->allocation.width,
            player2.game_area->allocation.height);

        gdk_draw_rectangle(player2.next_block_area->window,
            player2.next_block_area->style->black_gc,
            TRUE,
            0,0,
            player2.next_block_area->allocation.width,
            player2.next_block_area->allocation.height);
    }
    
    game_set_pause(GTK_WIDGET(menu_game_pause),NULL);
    gtk_label_set(GTK_LABEL(Start_stop_button_label),start_stop_str[0]);
    gtk_widget_set_sensitive(menu_game_quick,TRUE);
    gtk_widget_set_sensitive(menu_game_start,TRUE);
    gtk_widget_set_sensitive(menu_game_stop,FALSE);
    gtk_widget_set_sensitive(Start_stop_button,TRUE);
    gtk_widget_grab_default(Start_stop_button);
    gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[0]);
    gtk_widget_set_sensitive(Pause_button,FALSE);
    gtk_widget_set_sensitive(menu_game_pause, FALSE);
}

void game_start_stop(GtkMenuItem     *widget,
             gpointer user_data)
{
    
    if(game_state == GAME_OVER)
    {
        game_state = GAME_RUNNING;
        gtk_widget_set_sensitive(menu_game_stop,TRUE);
        gtk_widget_set_sensitive(menu_game_quick,FALSE);
        gtk_widget_set_sensitive(menu_game_start,FALSE);
        gtk_widget_set_sensitive(Start_stop_button,TRUE);
        gtk_label_set(GTK_LABEL(Start_stop_button_label),start_stop_str[1]);
        gtk_widget_set_sensitive(Pause_button,TRUE);
        gtk_widget_set_sensitive(menu_game_pause, TRUE);
        gtk_widget_grab_default(Pause_button);
        game_init(&player1);
        make_noise(options.noise_l,options.noise_h,&player1);
        from_virtual(&player1);
        move_block(0,0,0,&player1);
        
        if (player1.timer) g_source_remove(player1.timer);
        player1.timer = g_timeout_add(level_speeds[player1.current_level],(GtkFunction)game_loop_plr1,NULL);
        
        if (game_type != SINGLE_GAME)
        {
            if (player2.timer) g_source_remove(player1.timer);
            int speed = (game_type == LAN_GAME) ? RESEND_MSG_TIMEOUT 
                                : level_speeds[player2.current_level];    
            player2.timer = g_timeout_add(speed,(GtkFunction)game_loop_plr2,NULL);
            
            /* Bug ( */
            reset_timer(&player1);
        }    
        
        if(game_type == VS_COMPUTER || game_type == TWO_PLAYERS) 
        {
            game_init(&player2);
            make_noise(options.noise_l,options.noise_h,&player2);
            from_virtual(&player2);
            move_block(0,0,0,&player2);

        }
        
        gtk_widget_set_sensitive(menu_settings, FALSE);
        gtk_widget_set_sensitive(menu_network, FALSE);
        
        update_game_values();
    }
    else
    {
        game_over_init(&player1);
    }
}

void show_highscore_wrapper(GtkMenuItem     *menuitem,
                gpointer         user_data)
{
    read_highscore();
    show_highscore(0);
}

void save_options()
{
    FILE *fp;
    if(!(fp = fopen(options_f,"wb"))){
        printf("gtklantetris: Write ERROR!\n");
        return;}
        
    //fwrite(&options,1,sizeof(options),fp);
    if(fwrite(&options,1,sizeof(options),fp)<sizeof(options))
    {
    	printf("error while writing options file\n");
    }
    fclose(fp);
}

void read_options()
{   
    FILE *fp;
    if((fp = fopen(options_f,"rb")))
    {
        //fread(&options,1,sizeof(options),fp);
        if(fread(&options,1,sizeof(options),fp)<sizeof(options))
    	{
    		printf("error while reading options file\n");
    	}
        fclose(fp);
    }
}

/* Try to accept connection. If success, then run game, 
 * else show notification */
int accept_loop()
{
    if(game_state != GAME_RUNNING)
    {
        accept_timer = g_timeout_add(RETRY_ACCEPT,
                  (GtkFunction)accept_loop,NULL);
        if(accept_connection() == 0)
        {
            /* distroy dialog */
            gtk_dialog_response( GTK_DIALOG(accept_dlg),GTK_RESPONSE_OK);          
        }
        else 
        {
            gtk_progress_bar_pulse (GTK_PROGRESS_BAR (accept_progr_bar));
        }
            
    }
    /* Don't run this timer again */
    return FALSE;
}
/* Tru to create game server and accept connection */
void network_create(GtkMenuItem *widget, gpointer user_data)
{
    if(create_game()==0)
    {
        accept_timer = g_timeout_add(RETRY_ACCEPT,
                    (GtkFunction)accept_loop,NULL);
        
        accept_dlg = gtk_dialog_new_with_buttons ("Accepting connection",
                            GTK_WINDOW(main_window),
                            /*GTK_DIALOG_MODAL|*/GTK_DIALOG_NO_SEPARATOR,
                            GTK_STOCK_CANCEL,
                            GTK_RESPONSE_CANCEL,
                            NULL);
        
       
        accept_progr_bar = gtk_progress_bar_new ();
        gtk_progress_bar_set_text (GTK_PROGRESS_BAR (accept_progr_bar), "Waiting for connection...");
        gtk_progress_bar_pulse (GTK_PROGRESS_BAR (accept_progr_bar));
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(accept_dlg)->vbox),
                    accept_progr_bar);
        gtk_widget_show_all (accept_dlg);
        gint result = gtk_dialog_run (GTK_DIALOG (accept_dlg)); 
        gtk_widget_destroy(accept_dlg);
        if(result != GTK_RESPONSE_OK)
        {
            close_connection();
            g_source_remove(accept_timer);
        }
        else
        {
        	/*save oprions*/
        	memcpy(&tmp_opt,&options,sizeof(options));
        	    
            game_type = LAN_GAME;
            player1.name = "You";
            player2.name = "Your opponent";
            gtk_widget_show(game_opp_border);
            gtk_widget_show(next_block_opp_border);
            gtk_widget_show(player2.score_label);
            gtk_widget_show(player2.level_label);
            gtk_widget_show(player2.lines_label); 

            game_start_stop(NULL,NULL);  

            gtk_widget_set_sensitive(menu_game_pause, FALSE);
            gtk_widget_set_sensitive(Pause_button, FALSE); 
                        
        }
    }
    else
    {
        #ifdef MAEMO
        hildon_banner_show_information(GTK_WIDGET(main_window), NULL,
            "Create server error");
        #else
        GtkWidget* error_dialog;
        error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
                              GTK_DIALOG_DESTROY_WITH_PARENT,
                              GTK_MESSAGE_ERROR,
                              GTK_BUTTONS_CLOSE,
                              "Create server error");
        /* non-modal dialog*/
        g_signal_connect_swapped (error_dialog, "response",
                           G_CALLBACK (gtk_widget_destroy),
                           error_dialog);
        gtk_widget_show( error_dialog);                    
        /* Modal variant */                      
        //gtk_dialog_run (GTK_DIALOG (error_dialog));
        //gtk_widget_destroy (error_dialog);
        #endif
    }  
}

/* Try to connect to server */
void network_connect(GtkMenuItem *widget, gpointer user_data)
{
	
    const char *addr = "127.0.0.1";
    
    GtkWidget* connect_dlg;
    GtkWidget* connect_entry;
    connect_dlg = gtk_dialog_new_with_buttons ("Enter IP-address",
                        GTK_WINDOW(main_window),
                        GTK_DIALOG_NO_SEPARATOR,
                        GTK_STOCK_OK,
                        GTK_RESPONSE_OK,
                        GTK_STOCK_CANCEL,
                        GTK_RESPONSE_CANCEL,
                        NULL);
    
    connect_entry = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY (connect_entry), addr );
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(connect_dlg)->vbox),
                connect_entry);
    gtk_widget_show_all (connect_dlg);
    gint result = gtk_dialog_run (GTK_DIALOG (connect_dlg)); 
    addr = gtk_entry_get_text( GTK_ENTRY (connect_entry));

    if(result == GTK_RESPONSE_OK)
    {
    	/* Save options, because we hace to syncronize opntions with 
    	 * server. */
    	memcpy(&tmp_opt,&options,sizeof(options));
    	
        if (connect_to_friend(addr)==0)
        {
            game_type = LAN_GAME;
            player1.name = "You";
            player2.name = "Your opponent";
            gtk_widget_show(game_opp_border);
            gtk_widget_show(next_block_opp_border);
            gtk_widget_show(player2.score_label);
            gtk_widget_show(player2.level_label);
            gtk_widget_show(player2.lines_label);      
                        
            game_start_stop(NULL,NULL);   
            
            gtk_widget_set_sensitive(menu_game_pause, FALSE);
            gtk_widget_set_sensitive(Pause_button, FALSE);       
        }
        else
        {
            #ifdef MAEMO
            hildon_banner_show_information(GTK_WIDGET(main_window), NULL, "Connection error");
            #else
            GtkWidget* error_dialog;
            error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  "Connection error");
            /* non-modal dialog*/
            g_signal_connect_swapped (error_dialog, "response",
                            G_CALLBACK (gtk_widget_destroy),
                            error_dialog);
            gtk_widget_show( error_dialog);               
            /* Modal variant */                      
            //gtk_dialog_run (GTK_DIALOG (error_dialog));
            //gtk_widget_destroy (error_dialog);
            #endif
        }  
    }    
    gtk_widget_destroy(connect_dlg);
}
/* Run when user distroy window or press Quit */
void quit_app(GtkMenuItem *widget, gpointer user_data)
{
	/* If we press "exit" while playing over the internet, we
	 * need to boot old options */
	if(game_type == LAN_GAME)
    {
		/* Boot old options */
        memcpy(&options,&tmp_opt,sizeof(options));
	}
	save_options();
	gtk_main_quit();
}

#ifdef MAEMO
/* D-BUS callback */
gint dbus_callback (const gchar *interface, const gchar *method,
	       GArray *arguments, gpointer data,
	       osso_rpc_t *retval)
{
    printf ("gtklantetris dbus: %s, %s\n", interface, method);
    
    if (!strcmp (method, "top_application"))
        gtk_window_present (GTK_WINDOW (data));
    
    retval->type = DBUS_TYPE_INVALID;
    return OSSO_OK;
}
#endif

/* Main function - init. user interface */
int main(int argc,char *argv[])
{      
    char dmmy[20];
    GtkWidget *v_box;
    GtkWidget *v_box_next_block;
    GtkWidget *h_box;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *right_side;
    GtkWidget *game_border;    
    GtkWidget *next_block_border;
    GdkBitmap *mask;  
    GtkWidget *menu_bar;
    GtkWidget *menu_game;
    GtkWidget *menu_game_menu;
    GtkWidget *menu_settings_menu;
    GtkWidget *menu_network_menu;
    GtkWidget *menu_network_create;
    GtkWidget *menu_network_connect;
    GtkWidget *menu_help;
    GtkWidget *menu_help_menu;
    GtkWidget *help1;
    GtkWidget *high_scores1;
    GtkWidget *separator2;
    GtkWidget *separator3;
    GtkWidget *about1;
    GtkWidget* score_hbox;
    GtkWidget* lines_hbox;
    GtkWidget* level_hbox;
    GtkWidget *score_label_static;
    GtkWidget *level_label_static;
    GtkWidget *lines_label_static;
    //GtkWidget *player1.event_box;
    GtkAccelGroup* accel_group;
    
    /* init game values */
    
    /* open config files, if they exist */    
    mode_t t=0;
    t = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // 755
    char def_path[111];
    get_def_path(def_path);
    /* Create folder, if not exist. Note: there was problem 
     * with MAEMO devices */
    mkdir(def_path,t);
    get_opt_file(options_f);
    get_hs_file(highscore_file);
    read_options();
    
    def_mouse_plr = (options.mouse_plr==2)? &player2 : &player1;
    mouse_timer = 0; 
    game_state = GAME_OVER;
    game_type = options.game_type;
    
    player1.current_x = player1.current_y = 0;
    player1.current_block = player1.current_frame = 0;
    player1.current_score = player1.current_lines = 0;
    player1.current_level = options.level; 
    player1.next_block = player1.next_frame = 0;
    player1.opp = &player2;
    player1.name = "Player 1";
    player1.timer = 0;
    
    player2.current_x = player2.current_y = 0;
    player2.current_block = player2.current_frame = 0;
    player2.current_score = player2.current_lines = 0;
    player2.current_level = options.level; 
    player2.next_block = player2.next_frame = 0;
    player2.opp = &player1;
    player2.name = "Player 2";
    player2.timer = 0;
    
    // seed random generator
    srandom(time(NULL));
    
    gtk_set_locale();
    gtk_init(&argc,&argv);
    
    accel_group = gtk_accel_group_new();
    
    /* Setup the HildonProgram, HildonWindow and application name. */
    #ifdef MAEMO
    
        HildonProgram* hildonprog;
        hildonprog = HILDON_PROGRAM(hildon_program_get_instance());
        main_window = HILDON_WINDOW(hildon_window_new());
        hildon_program_add_window(hildonprog,
                                HILDON_WINDOW(main_window));
        
        /* D-BUS initialization */
	    osso_context_t *ctxt;
        osso_return_t ret; 
        ctxt = osso_initialize ("ru.karelia.cs.gtklantetris", PACKAGE_VERSION, TRUE, NULL);
        if (ctxt == NULL)
        {
            fprintf (stderr, "osso_initialize failed.\n");
            exit (1);
        }
        
        ret = osso_rpc_set_default_cb_f (ctxt, dbus_callback, main_window);
        if (ret != OSSO_OK)
        {
            fprintf (stderr, "osso_rpc_set_default_cb_f failed: %d.\n", ret);
            exit (1);
        }                                
    #else
    	/* Create main window */
        main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);   
        /* Attach icons for all windows */
    	gtk_window_set_default_icon_from_file(APP_ICON,NULL);
    #endif          
                    
    g_set_application_name("GtkLanTetris");
    g_signal_connect (G_OBJECT (main_window), "destroy",
                                G_CALLBACK (quit_app), NULL);
    /*Don't allow to resize window*/
    gtk_window_set_resizable (GTK_WINDOW(main_window),FALSE);
    g_signal_connect ((gpointer) main_window, "key_press_event",
                G_CALLBACK (keyboard_event_handler),
                NULL); 

    // vertical box
    v_box = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(main_window),v_box);
    gtk_widget_show(v_box);
    
    /*Init. hildon menu for maemo devices if needed*/
    #ifdef MAEMO
        menu_bar = gtk_menu_new();
        hildon_program_set_common_menu  (hildonprog, GTK_MENU(menu_bar));
    #else
        menu_bar= gtk_menu_bar_new();
        gtk_box_pack_start(GTK_BOX(v_box),menu_bar,FALSE,FALSE,0);
    #endif
    gtk_widget_show(menu_bar);
        
    //Game sub-menu
    menu_game=gtk_menu_item_new_with_mnemonic ("_Game");
    gtk_widget_show(menu_game);
    gtk_container_add (GTK_CONTAINER (menu_bar), menu_game);
    
    menu_game_menu=gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_game), menu_game_menu);
    
    menu_game_quick = gtk_menu_item_new_with_mnemonic ("Start Game");
    gtk_widget_show (menu_game_quick);
    gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_quick);
    g_signal_connect ((gpointer) menu_game_quick, "activate",
                G_CALLBACK (game_start_stop),
                NULL);
    gtk_widget_add_accelerator (menu_game_quick, "activate", accel_group,
                    GDK_G, GDK_CONTROL_MASK,
                    GTK_ACCEL_VISIBLE);
    
    menu_game_stop = gtk_menu_item_new_with_mnemonic ("Stop Game");
    gtk_widget_show (menu_game_stop);
    gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_stop);
    g_signal_connect ((gpointer) menu_game_stop, "activate",
                G_CALLBACK (game_start_stop),
                NULL);
    gtk_widget_add_accelerator (menu_game_stop, "activate", accel_group,
                    GDK_O, GDK_CONTROL_MASK,
                    GTK_ACCEL_VISIBLE);
    gtk_widget_set_sensitive(menu_game_stop,FALSE);
    
    menu_game_pause = gtk_check_menu_item_new_with_mnemonic ("Pause");
    //gtk_image_menu_item_new_from_stock(GTK_STOCK_MEDIA_PAUSE,accel_group);
    gtk_widget_show (menu_game_pause);
    gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_pause);
    g_signal_connect ((gpointer) menu_game_pause, "activate",
                G_CALLBACK (game_set_pause),
                NULL);
    gtk_widget_add_accelerator (menu_game_pause, "activate", accel_group,
                    GDK_P, GDK_CONTROL_MASK,
                    GTK_ACCEL_VISIBLE);
    gtk_widget_set_sensitive(menu_game_pause, FALSE);                
    
        
    //Settings sub-menu
    menu_settings = gtk_menu_item_new_with_mnemonic ("_Settings");
    gtk_widget_show (menu_settings);
    gtk_container_add (GTK_CONTAINER (menu_bar), menu_settings);
    
    menu_settings_menu=gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_settings), 
                    menu_settings_menu);
    
    
    /* Radio buttons init */
    menu_single_radio = gtk_radio_menu_item_new_with_label (NULL, "1 player");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_settings_menu), menu_single_radio);
    gtk_widget_show (menu_single_radio);
    g_signal_connect ((gpointer) menu_single_radio, "activate",
                G_CALLBACK (game_type_changed), NULL);   
    
    menu_twoplr_radio = gtk_radio_menu_item_new_with_label_from_widget (GTK_RADIO_MENU_ITEM (menu_single_radio), "2 players");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_settings_menu), menu_twoplr_radio);
    gtk_widget_show (menu_twoplr_radio);
    g_signal_connect ((gpointer) menu_twoplr_radio, "activate",
                G_CALLBACK (game_type_changed),  NULL); 
    
    menu_vscomp_radio = gtk_radio_menu_item_new_with_label_from_widget (GTK_RADIO_MENU_ITEM (menu_single_radio), "Vs. computer");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_settings_menu), menu_vscomp_radio);
    gtk_widget_show (menu_vscomp_radio);
    g_signal_connect ((gpointer) menu_vscomp_radio, "activate",
                G_CALLBACK (game_type_changed),  NULL);    
    
    GtkWidget* tmp_sep;
    tmp_sep = gtk_menu_item_new ();
    gtk_widget_show (tmp_sep);
    gtk_container_add (GTK_CONTAINER (menu_settings_menu), tmp_sep);
    gtk_widget_set_sensitive (tmp_sep, FALSE);
    
    menu_game_start = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,accel_group);
    gtk_widget_show (menu_game_start);
    gtk_container_add (GTK_CONTAINER (menu_settings_menu), menu_game_start);
    g_signal_connect ((gpointer) menu_game_start, "activate",
                G_CALLBACK (show_preferences), NULL);         
    
    /* Network sub-menu */
    menu_network=gtk_menu_item_new_with_mnemonic ("_Network");
    gtk_widget_show (menu_network);
    gtk_container_add (GTK_CONTAINER (menu_bar), menu_network);
    
    menu_network_menu = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_network), 
                    menu_network_menu);
    //gtk_widget_show (menu_network_menu);

    //menu_network_create 
    GtkWidget *image;
    
    menu_network_create = gtk_image_menu_item_new_with_mnemonic("Create game");
    image = gtk_image_new_from_stock (GTK_STOCK_NEW, GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_network_create), image);
    gtk_widget_show(image);
    
    gtk_widget_show (menu_network_create);
    gtk_container_add (GTK_CONTAINER (menu_network_menu), menu_network_create);
    g_signal_connect ((gpointer) menu_network_create, "activate",
                G_CALLBACK (network_create), NULL);
                
    menu_network_connect = gtk_image_menu_item_new_from_stock(GTK_STOCK_CONNECT,accel_group);
            //gtk_menu_item_new_with_mnemonic ("Co_nnect");
    gtk_widget_show (menu_network_connect);
    gtk_container_add (GTK_CONTAINER (menu_network_menu), menu_network_connect);
    g_signal_connect ((gpointer) menu_network_connect, "activate",
                G_CALLBACK (network_connect), NULL);
    
    
    //Help sub-menu
    menu_help=gtk_menu_item_new_with_mnemonic ("_Help");
    gtk_widget_show (menu_help);
    gtk_container_add (GTK_CONTAINER (menu_bar), menu_help);
    
    menu_help_menu = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_help), 
                    menu_help_menu);
    
    help1 = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,accel_group);
    gtk_widget_show (help1);
    gtk_container_add (GTK_CONTAINER (menu_help_menu), help1);
    g_signal_connect ((gpointer) help1, "activate",
                G_CALLBACK (show_help),
                NULL);
    gtk_widget_add_accelerator (help1, "activate", 
                    accel_group,
                    GDK_F1, (GdkModifierType) 0,
                    GTK_ACCEL_VISIBLE);
    
    high_scores1 = gtk_menu_item_new_with_mnemonic ("High-scores");
    gtk_widget_show (high_scores1);
    gtk_container_add (GTK_CONTAINER (menu_help_menu), high_scores1);
    g_signal_connect ((gpointer) high_scores1, "activate",
                G_CALLBACK (show_highscore_wrapper),
                NULL);
            
    separator2 = gtk_menu_item_new ();
    gtk_widget_show (separator2);
    gtk_container_add (GTK_CONTAINER (menu_help_menu), separator2);
    gtk_widget_set_sensitive (separator2, FALSE);
    
    about1 = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,accel_group);
        //gtk_menu_item_new_with_mnemonic ("About");
    gtk_widget_show (about1);
    gtk_container_add (GTK_CONTAINER (menu_help_menu), about1);
    g_signal_connect ((gpointer) about1, "activate",
                G_CALLBACK (show_about),
                NULL);
                
    //Quit
    GtkWidget* quit_place;
    #ifdef MAEMO
        quit_place = menu_bar;
    #else
        quit_place = menu_game_menu;
    #endif 
    separator3 = gtk_menu_item_new ();
    gtk_widget_show (separator3);
    
    /*We use macros to define where to add widget*/
    gtk_container_add (GTK_CONTAINER (quit_place), separator3);
    gtk_widget_set_sensitive (separator3, FALSE);
    
    menu_game_quit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, accel_group);
    //gtk_menu_item_new_with_mnemonic ("Quit");
    gtk_widget_show (menu_game_quit);
    /*We use macros to define where to add widget*/
    gtk_container_add (GTK_CONTAINER (quit_place), menu_game_quit);
    g_signal_connect ((gpointer) menu_game_quit, "activate",
                G_CALLBACK (quit_app),
                NULL);
    gtk_widget_add_accelerator(menu_game_quit,"activate", accel_group,
                    GDK_X, GDK_CONTROL_MASK,
                    GTK_ACCEL_VISIBLE);    
    
    // horizontal box
    h_box = gtk_hbox_new(FALSE,1);
    gtk_widget_show(h_box);
    gtk_box_pack_start(GTK_BOX(v_box),h_box,FALSE,FALSE,0);
        
    // game_border
    game_border = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(game_border),GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(h_box),game_border,FALSE,FALSE,1);
    gtk_widget_show(game_border);
    
    /* player1.event_box
    * GtkDrawingArea can't receive button_press_event, 
    * that is why we need GtkEventBox
    */
    player1.event_box = gtk_event_box_new();
    gtk_widget_show(player1.event_box);
    gtk_container_add(GTK_CONTAINER(game_border),player1.event_box);

    /* Set events, that we will receive(press, release and move mouse) */
    gtk_widget_set_events(player1.event_box, 
                GDK_BUTTON_PRESS_MASK | GDK_MOTION_NOTIFY | GDK_BUTTON_RELEASE);
    
    // game_area
    player1.game_area = gtk_drawing_area_new();
    gtk_widget_show(player1.game_area);
    gtk_drawing_area_size(GTK_DRAWING_AREA(player1.game_area),
                MAX_X*BLOCK_WIDTH,MAX_Y*BLOCK_HEIGHT);
    g_signal_connect ((gpointer) player1.game_area, "expose_event",
                G_CALLBACK (game_area_expose_event), NULL);
    /* widget can recive expose events -> work by default  */
    //gtk_widget_set_events(player1.next_block_area, GDK_EXPOSURE_MASK);
    gtk_container_add(GTK_CONTAINER(player1.event_box),player1.game_area);
    
    
    // right_side
    right_side = gtk_vbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(h_box),right_side,FALSE,FALSE,0);
    gtk_widget_show(right_side);
    
    // v_box_next_block
    v_box_next_block = gtk_hbox_new(FALSE,1);
    gtk_widget_show(v_box_next_block);
    gtk_box_pack_start(GTK_BOX(right_side),v_box_next_block,FALSE,FALSE,0);
    
    // next_block_border
    next_block_border = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(next_block_border),GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(v_box_next_block),next_block_border,FALSE,FALSE,0);
    gtk_widget_show(next_block_border);
    
    // player1.next_block_area
    player1.next_block_area = gtk_drawing_area_new();
    gtk_widget_show(player1.next_block_area);
    gtk_drawing_area_size(GTK_DRAWING_AREA(player1.next_block_area),
                4*BLOCK_WIDTH,4*BLOCK_HEIGHT);
    g_signal_connect ((gpointer) player1.next_block_area, "expose_event",
                G_CALLBACK (game_area_expose_event), NULL);
    //gtk_widget_set_events(player1.next_block_area, GDK_EXPOSURE_MASK);
    gtk_container_add(GTK_CONTAINER(next_block_border),player1.next_block_area);
    
    //next_block_opp_border
    next_block_opp_border = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(next_block_opp_border),GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(v_box_next_block),next_block_opp_border,FALSE,FALSE,0);
    if(game_type!=SINGLE_GAME) gtk_widget_show(next_block_opp_border);
    
    //player2.next_block_area
    player2.next_block_area = gtk_drawing_area_new();
    gtk_widget_show(player2.next_block_area);
    gtk_drawing_area_size(GTK_DRAWING_AREA(player2.next_block_area),
                4*BLOCK_WIDTH,4*BLOCK_HEIGHT);
    
    g_signal_connect ((gpointer) player2.next_block_area, "expose_event",
                G_CALLBACK (game_area_expose_event), NULL);
    
    //gtk_widget_set_events(player1.next_block_area, GDK_EXPOSURE_MASK);
    gtk_container_add(GTK_CONTAINER(next_block_opp_border),player2.next_block_area);
    
    // the score,level and lines labels
    
    score_label_static = gtk_label_new("Score:");
    //gtk_label_set_justify(GTK_LABEL(score_label_static),GTK_JUSTIFY_RIGHT);
    gtk_widget_show(score_label_static);
    gtk_box_pack_start(GTK_BOX(right_side),score_label_static,FALSE,FALSE,3);
    
    score_hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(score_hbox);
    gtk_box_pack_start(GTK_BOX(right_side),score_hbox,FALSE,FALSE,3);
    
    player1.score_label = gtk_label_new("0");
    set_gtk_color_style(player1.score_label,0xffff,0,0);
    gtk_widget_show(player1.score_label);
    gtk_box_pack_start(GTK_BOX(score_hbox),player1.score_label,TRUE,FALSE,3);
    
    player2.score_label = gtk_label_new("0");
    set_gtk_color_style(player2.score_label,0xffff,0,0);
    if(game_type!=SINGLE_GAME)
        gtk_widget_show(player2.score_label);
    gtk_box_pack_start(GTK_BOX(score_hbox),player2.score_label,TRUE,FALSE,3);
    
    level_label_static = gtk_label_new("Level:");
    gtk_widget_show(level_label_static);
    gtk_box_pack_start(GTK_BOX(right_side),level_label_static,FALSE,FALSE,3);
    
    level_hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(level_hbox);
    gtk_box_pack_start(GTK_BOX(right_side),level_hbox,FALSE,FALSE,3);
        
    sprintf(dmmy,"%d",player1.current_level);
    player1.level_label = gtk_label_new(dmmy);
    set_gtk_color_style(player1.level_label,0,0,0xffff);
    gtk_widget_show(player1.level_label);
    gtk_box_pack_start(GTK_BOX(level_hbox),player1.level_label,TRUE,FALSE,3);
    
    player2.level_label = gtk_label_new(dmmy);
    set_gtk_color_style(player2.level_label,0,0,0xffff);
    if(game_type!=SINGLE_GAME)
        gtk_widget_show(player2.level_label);
    gtk_box_pack_start(GTK_BOX(level_hbox),player2.level_label,TRUE,FALSE,3);
    
    lines_label_static = gtk_label_new("Lines:");
    gtk_widget_show(lines_label_static);
    gtk_box_pack_start(GTK_BOX(right_side),lines_label_static,FALSE,FALSE,3);
    
    lines_hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(lines_hbox);
    gtk_box_pack_start(GTK_BOX(right_side),lines_hbox,FALSE,FALSE,3);
    
    player1.lines_label = gtk_label_new("0");
    //gtk_label_set_justify(GTK_LABEL(player1.lines_label),GTK_JUSTIFY_RIGHT);
    gtk_widget_show(player1.lines_label);
    gtk_box_pack_start(GTK_BOX(lines_hbox),player1.lines_label,TRUE,FALSE,3);
    
    player2.lines_label = gtk_label_new("0");
    if(game_type!=SINGLE_GAME)
        gtk_widget_show(player2.lines_label);
    gtk_box_pack_start(GTK_BOX(lines_hbox),player2.lines_label,TRUE,FALSE,3);
    
    //the game buttons
    //Start_stop
    Start_stop_button = gtk_button_new();
    gtk_widget_show(Start_stop_button);
    g_signal_connect ((gpointer) Start_stop_button, "clicked",
                G_CALLBACK (game_start_stop),
                NULL);
    Start_stop_button_label= gtk_label_new(start_stop_str[0]);
    box2 = label_box(right_side, Start_stop_button_label, 
            start_stop_str[0] );
    gtk_widget_show(box2);
    gtk_container_add (GTK_CONTAINER (Start_stop_button), box2);
    gtk_box_pack_start(GTK_BOX(right_side),Start_stop_button,FALSE,FALSE,3);
    GTK_WIDGET_SET_FLAGS(Start_stop_button, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(Start_stop_button);
    //Pause
    Pause_button = gtk_button_new();
    gtk_widget_show(Pause_button);
    g_signal_connect ((gpointer) Pause_button, "clicked",
                G_CALLBACK (game_set_pause_b), NULL);
    Pause_button_label = gtk_label_new(pause_str[0]);
    box1 = label_box(right_side, Pause_button_label, pause_str[0] );
    gtk_widget_show(box1);
    gtk_container_add (GTK_CONTAINER (Pause_button), box1);
    gtk_box_pack_start(GTK_BOX(right_side),Pause_button,FALSE,FALSE,3);
    GTK_WIDGET_SET_FLAGS(Pause_button, GTK_CAN_DEFAULT);
    gtk_widget_set_sensitive(Pause_button,FALSE);
    
    gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);
    
    /* Adding drawig area for opponent */
    // game_opp_border
    game_opp_border = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(game_opp_border),GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(h_box),game_opp_border,FALSE,FALSE,1);
    if(game_type!=SINGLE_GAME)
        gtk_widget_show(game_opp_border);
    
    // player2.game_area
    player2.event_box = gtk_event_box_new();
    gtk_widget_show(player2.event_box);
    gtk_container_add(GTK_CONTAINER(game_opp_border),player2.event_box);

    /* Set events, that we will receive(press, release and move mouse) */
    gtk_widget_set_events(player2.event_box, 
                GDK_BUTTON_PRESS_MASK | GDK_MOTION_NOTIFY | GDK_BUTTON_RELEASE);
    
    player2.game_area = gtk_drawing_area_new();
    gtk_widget_show(player2.game_area);
    gtk_drawing_area_size(GTK_DRAWING_AREA(player2.game_area),
                MAX_X*BLOCK_WIDTH,MAX_Y*BLOCK_HEIGHT);
    g_signal_connect ((gpointer) player2.game_area, "expose_event",
                G_CALLBACK (game_area_expose_event), NULL);
    /* widget can recive expose events -> work by default  */
    //gtk_widget_set_events(player2.next_block_area, GDK_EXPOSURE_MASK);
    gtk_container_add(GTK_CONTAINER(player2.event_box),player2.game_area);
   
    
    if(game_type == VS_COMPUTER) 
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu_vscomp_radio), TRUE); 
    else if (game_type == TWO_PLAYERS )
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu_twoplr_radio), TRUE);
        
    g_signal_connect ((gpointer) player1.event_box, "button_press_event",
                G_CALLBACK (game_area_pressed), NULL); 
    g_signal_connect ((gpointer) player1.event_box, "button_release_event",
                G_CALLBACK (game_area_released), NULL); 
    g_signal_connect ((gpointer) player1.event_box, "motion_notify_event",
                G_CALLBACK (game_area_move), NULL);
    
    g_signal_connect ((gpointer) player2.event_box, "button_press_event",
                G_CALLBACK (game_area_pressed), NULL); 
    g_signal_connect ((gpointer) player2.event_box, "button_release_event",
                G_CALLBACK (game_area_released), NULL); 
    g_signal_connect ((gpointer) player2.event_box, "motion_notify_event",
                G_CALLBACK (game_area_move), NULL);
   
    
    gtk_widget_show(GTK_WIDGET(main_window));
    
    // Block images...
    blocks_pixmap = gdk_pixmap_create_from_xpm_d(player1.game_area->window,
                            &mask,
                            NULL,
                            (gchar **)blocks_xpm);
    
    /* Main loop */
    gtk_main ();
    return 0;
}

