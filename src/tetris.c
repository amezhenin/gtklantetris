#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "tetris.h"

/* Draw block on GTK drawing area */
void draw_block(int x,int y,int block,int frame,int clear,PlrState* plr,int next)
{
    if(plr->game_area == NULL || plr->next_block_area == NULL) return;
    int temp;
    if (next)
        gdk_draw_rectangle(plr->next_block_area->window,
               plr->next_block_area->style->black_gc,
               TRUE, 0, 0,
               plr->next_block_area->allocation.width,
               plr->next_block_area->allocation.height);
    for(temp=0;temp < 4;temp++)
        set_block(x+block_data[block][frame][0][temp],
            y+block_data[block][frame][1][temp],
            (clear ? 0 : block+1), plr, next);
}

/* Add current block to game well */
void to_virtual(PlrState* plr)
{
    int temp;
    for(temp=0;temp < 4;temp++)
        if(plr->current_y+block_data[plr->current_block][plr->current_frame][1][temp] >=0 )
            plr->virtual[plr->current_y+block_data[plr->current_block][plr->current_frame][1][temp]]
                [plr->current_x+block_data[plr->current_block][plr->current_frame][0][temp]] = plr->current_block+1;    
}

/* Redraw information from plr->virtual[][] to GTK drawing area */
void from_virtual(PlrState* plr)
{
    if(plr->game_area == NULL) return;
    
    int temp_x,temp_y;
    for(temp_y=0;temp_y < MAX_Y;temp_y++)
        for(temp_x=0;temp_x < MAX_X;temp_x++)
            set_block(temp_x,temp_y,plr->virtual[temp_y][temp_x],plr,FALSE);
}

int is_brick_of_block(int x,int y, PlrState* plr)
{
    int temp;
    //return TRUE; 
    //printf("%i %i\n",x,y);
    for(temp=0;temp < 4;temp++)
        if(abs(plr->current_y+block_data[plr->current_block][plr->current_frame][1][temp]-y) < 2 &&
           abs(plr->current_x+block_data[plr->current_block][plr->current_frame][0][temp]-x) < 2)
               return TRUE; 
    return FALSE;
}

/* Check: valid position of block or not */
int valid_position(int x,int y,int block,int frame, PlrState* plr)
{
    int temp, max=0, mcol=0;

    for(temp=0;temp < 4;temp++)
        max = (block_data[block][frame][1][temp] > max) ? block_data[block][frame][1][temp] : max; 

    for(temp=0;temp < 4;temp++)
    {
        /* Validate coordinates */
        if( x+block_data[block][frame][0][temp] < 0        ||
            x+block_data[block][frame][0][temp] > MAX_X-1  ||
            y+block_data[block][frame][1][temp] < -max     ||
            y+block_data[block][frame][1][temp] > MAX_Y-1  )
                 return FALSE;
        /* Calculate number of negative coordinates */
        if(y+block_data[block][frame][1][temp] < 0 ) 
        { 
            mcol++;
            continue;
        }
        /* Validate free space in game well */
        if(plr->virtual[y+block_data[block][frame][1][temp]]
                       [x+block_data[block][frame][0][temp]] != 0)
                return FALSE;
          
    }   
    if(mcol==4) return FALSE;        
    return TRUE;
}

/* Move and flip current_block if position is valid or do nothing */
void move_block(int x,int y,int f, PlrState* plr)
{

    
    int last_frame = plr->current_frame;
    int last_block = plr->current_block;

    if(f != 0)
        plr->current_frame = (block_frames[plr->current_block]+(plr->current_frame+f))%block_frames[plr->current_block];
    if(valid_position(plr->current_x+x,plr->current_y+y,plr->current_block,plr->current_frame,plr))
    {
        /* Clear last position */
        draw_block(plr->current_x,plr->current_y,last_block,last_frame,TRUE, plr,FALSE);
        plr->current_x += x;
        plr->current_y += y;
        /* Draw block on new position */
        draw_block(plr->current_x,plr->current_y,plr->current_block,plr->current_frame,FALSE,plr,FALSE);
    }
    else
    {
        plr->current_block = last_block;
        plr->current_frame = last_frame;
    }
}

/* Drop current_block to bottom of game well and calculate score bonus */
void drop_block(PlrState *plr)
{
    int dropbonus=0;
    while(move_down(plr)) dropbonus++;
    plr->current_score += dropbonus*(plr->current_level+1);
}

/* Adds one row of "noise" in game well */
void add_row(PlrState* plr)
{
    move_block(0,-1,0,plr);
    int j;
    for(j=1;j<MAX_Y;j++) 
        memcpy( plr->virtual[j-1], plr->virtual[j] ,sizeof(plr->virtual[0]));
    make_noise(9,1,plr);
    from_virtual(plr);
    move_block(0, 0 ,0,plr);
}

/* Check game well for collected lines */
int check_lines(PlrState* plr)
{
    int temp_x,temp_y,temp,line,lines=0;
    for(temp_y=0;temp_y < MAX_Y;temp_y++)
    {
        line = TRUE;
        for(temp_x=0;temp_x < MAX_X;temp_x++)
            if(plr->virtual[temp_y][temp_x] == 0)
                line = FALSE;
        if(line)
        {
            lines++;
            for(temp=temp_y;temp > 0;temp--)
              //  for(j=0;j<MAX_X;j++) 
                //    plr->virtual[temp][j]=plr->virtual[temp-1][j];
                memcpy(plr->virtual[temp],plr->virtual[temp-1],sizeof(plr->virtual[0]));
            
            //for(j=0;j<MAX_X;j++) plr->virtual[0][j]=0;    
            memset(plr->virtual[0],0,sizeof(plr->virtual[0]));
            /* Animation for deleting line */
            usleep(50000); // tweak this?
           
            from_virtual(plr);
        }
    }
    //printf("lines %d\n",lines);
    /*This is bug =(. We have to check that stcucture is not fake */
    if(plr->game_area != NULL && plr->next_block_area != NULL)
    {
        /* Adding new line to opponent, if needed */
        if(game_type == VS_COMPUTER || game_type == TWO_PLAYERS)
        {
            int i,l;
            l=(options.easy_mode==TRUE) ? lines-1 : lines;
            
            for(i=0; i<l; i++)  
                add_row((PlrState*)plr->opp);
        }
        else if (game_type == LAN_GAME)
            send_message(lines,0);         
    }        
    return lines;
}

/* Move current_block at one position lower */
int move_down(PlrState* plr)
{
    int lines;

    if(!valid_position(plr->current_x,plr->current_y+1,plr->current_block,plr->current_frame,plr))
    {
        to_virtual(plr);
        lines = check_lines(plr);
        if(lines > 0)
        {
            from_virtual(plr);
            plr->current_lines += lines;
            if((int)plr->current_lines/10 > plr->current_level)
                plr->current_level = (int)plr->current_lines/10;
            if(plr->current_level > NUM_LEVELS -1)
                plr->current_level = NUM_LEVELS -1;
            plr->current_score += lines_score[lines-1]*(plr->current_level+1);
            update_game_values();
        }
        new_block(plr);
        move_block(0,0,0,plr);
        return FALSE;
    }
    else
    {
        move_block(0,1,0,plr);
        return TRUE;
    }
}

/* Create new block and put it on game well */
void new_block(PlrState* plr)
{
 
    if (def_mouse_plr == plr) mouse_hold = FALSE;
        
    plr->current_block = plr->next_block;
    plr->current_frame = plr->next_frame;
    
    plr->next_block = do_random(blocks);
    plr->next_frame = do_random(block_frames[plr->next_block]);
    plr->current_x = (int)(MAX_X/2)-1;


    /* hack to make the block start at top.. */
    int temp, max=0;
    for(temp=0;temp < 4;temp++)
        max = (block_data[plr->current_block][plr->current_frame][1][temp] > max) ? block_data[plr->current_block][plr->current_frame][1][temp] : max; 
    
    plr->current_y = -max;
    
    if(!valid_position(plr->current_x,plr->current_y,plr->current_block,plr->current_frame, plr))
    {
        /* This is bug =(. We have to check that stcucture is not fake */
        if (plr->game_area != NULL)  game_over_init(plr);
        return;
    }
        
    if(options.shw_nxt)
    {
        draw_block(0,0,plr->current_block,plr->current_frame,TRUE, plr,TRUE);
        draw_block(0,0,plr->next_block,plr->next_frame,FALSE, plr,TRUE);
    }
    /* This is bug =(. We have to check that stcucture is not fake */
    if (plr->game_area != NULL)  reset_timer(plr);
}

/* Adds rows (in number of _height_) with bricks to bottom of game well.
 * This makes game more difficulte. */
void make_noise(int level,int height, PlrState* plr)
{
    int x,y;

    if(!level || !height)
        return;
    
    for(y=MAX_Y-height;y<MAX_Y;y++)
    {
        for(x=0;x<MAX_X;x++)
            plr->virtual[y][x] = do_random(blocks)+1;
        for(x=0;x<MAX_X-level;x++)
            plr->virtual[y][do_random(MAX_X)] = 0;
    }
}

/* Start new game and init. variables */
void game_init(PlrState* plr)
{   
    game_state = GAME_RUNNING;
    plr->current_score = 0;
    plr->current_level = options.level;
    plr->current_lines = 0;

    memset(plr->virtual,0,sizeof(plr->virtual));    

    new_block(plr);

    move_block(0,0,0,plr);
}

