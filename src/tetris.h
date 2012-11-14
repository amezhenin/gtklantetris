#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <gtk/gtk.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* Game well size */
#define MAX_X 10
#define MAX_Y 18

/* A maximum of blocks in game well. We have four bricks in block. */
#define MAX_BLOCKS MAX_X*MAX_Y/4

/* Define size of brick */
#ifdef BIGBLOCKS
#define BLOCK_WIDTH 22
#define BLOCK_HEIGHT 22
#else
#ifdef BIGGERBLOCKS
#define BLOCK_WIDTH 33
#define BLOCK_HEIGHT 33
#else
#define BLOCK_WIDTH 15
#define BLOCK_HEIGHT 15
#endif
#endif



#define NUM_LEVELS 20
#define NUM_HIGHSCORE 10

// global variables
//#define MAEMO

/* Creating different interface for PC and MAEMO versions
 * for more comfortable usage
 */

#ifdef MAEMO
#include <hildon/hildon-program.h>
HildonWindow* main_window;
#else
GtkWidget* main_window;
#endif


/* We have different states of game */
static const int GAME_OVER    = 0;
static const int GAME_PAUSED  = 1;
static const int GAME_RUNNING = 2;
int game_state;

/* We have different types of game */
static const int SINGLE_GAME = 0;
static const int TWO_PLAYERS = 1;
static const int VS_COMPUTER = 2;
static const int LAN_GAME    = 3;
int game_type;

GdkPixmap *blocks_pixmap;


/* Number of different blocks */
static const int blocks          = 7;
/* Number of different frames for each block */
static const int block_frames[7]     = {1,2,2,2,4,4,4};
/* Score multiplier for each number of row  you collected at once*/
static const int lines_score[4]      = {40,100,300,1200};
/* block_data[][][][] contain information about different blocks:
 * 1) [7] - identify block, in classic tetris we have seven different
 *            blocks
 * 2) [4] - identify frame, each block has different number of 
 *            frames(see block_frames[])   
 * 3) [2] - "[0]": X-coordinate
 *          "[1]": Y-coordinate 
 * 4) [4] - coordinate of brick(part of block)
 * 
 *   Dimension 3) and 4) defines appearance of block by his id and 
 * frame id.
 * */
static const int block_data[7][4][2][4]  =
                {     /* 0 - O - blue*/ 
                    {
                        {{0,1,0,1},{0,0,1,1}}
                    },/* 1 - S - green*/
                    {
                        {{0,1,1,2},{1,1,0,0}},
                        {{0,0,1,1},{0,1,1,2}}
                    },/* 2 - Z - red*/
                    {
                        {{0,1,1,2},{0,0,1,1}},
                        {{1,1,0,0},{0,1,1,2}}
                    },/* 3 - 1 - white */
                    {
                        {{1,1,1,1},{0,1,2,3}},
                        {{0,1,2,3},{2,2,2,2}}
                    },/* 4 - J - violet */
                    {
                        {{1,1,1,2},{2,1,0,0}},
                        {{0,1,2,2},{1,1,1,2}},
                        {{0,1,1,1},{2,2,1,0}},
                        {{0,0,1,2},{0,1,1,1}}
                    },/* 5 - L - sky blue */
                    {
                        {{0,1,1,1},{0,0,1,2}},
                        {{0,1,2,2},{1,1,1,0}},
                        {{1,1,1,2},{0,1,2,2}},
                        {{0,0,1,2},{2,1,1,1}}
                    },/* 6 - T - yellow */
                    {
                        {{1,0,1,2},{0,1,1,1}},
                        {{2,1,1,1},{1,0,1,2}},
                        {{1,0,1,2},{2,1,1,1}},
                        {{0,1,1,1},{1,0,1,2}}
                    }
                };

/* This structure contains all information about player */
typedef struct PlrState{
    gint16 current_x;
    gint16 current_y;
    gint16 current_block;
    gint16 current_frame;
    gint32 current_score;
    gint16 current_level;
    gint16 current_lines;
    gint16 next_block;
    gint16 next_frame;
    gint8 virtual[MAX_Y][MAX_X];
    void* opp;
    GtkWidget *game_area;
    GtkWidget *next_block_area;
    GtkWidget *event_box;
    GtkWidget *score_label;
	GtkWidget *level_label;
	GtkWidget *lines_label;
    char* name;
    gint timer;
}PlrState;

/* Instance of structure PlrState */
/* First(main) player - always in left game area.
 * Controlled by user*/
PlrState player1;
/* Second player - always in right game area.
 * Controlled by second user or computer*/
PlrState player2;
/* Temporary player */
PlrState tmp_plr;
/* This player can move blocks by mouse */
PlrState* def_mouse_plr;

/* Variables used to manipulat blocks with mouse */
int mouse_hold;
int mouse_dx;
int mouse_dy;
int mouse_init;
int mouse_clicked;
gint mouse_timer;

/* This structure contains setting, that will be saved on exit, 
 * and  loaded on start of program */
struct Options
{
	/* game speed */
    int level;
    /* height and level of noise  */
    int noise_l;
    int noise_h;
    /* Show next block */
    gint shw_nxt;
    /* Current type of game */
    int game_type;
    /* who will control blocks by mouse */
    int mouse_plr;
    /* "Easy mode" for multiplayer game */
    int easy_mode;
}Options;

struct Options options;
struct Options tmp_opt;

struct item
{
    char name[10];
    long score;
    int level;
    int lines;
};

struct item highscore[NUM_HIGHSCORE];
char  highscore_file[100];

// tetris.c
void draw_block(int x,int y,int block,int frame,int clear, PlrState* plr, int next);
void to_virtual(PlrState* plr);
void from_virtual(PlrState* plr);
int is_brick_of_block(int x,int y, PlrState* plr);
int valid_position(int x,int y,int block,int frame, PlrState* plr);
void move_block(int x,int y,int f, PlrState* plr);
void add_row(PlrState* plr);
int check_lines(PlrState* plr);
int move_down(PlrState* plr);
void new_block(PlrState* plr);
void make_noise(int level,int height, PlrState* plr);
void game_init(PlrState* plr);
void drop_block(PlrState *plr);

/* *************** List of functions ************** */

// interface.c
void update_game_values();
int game_loop();
void game_over_init(PlrState *plr);
void game_set_pause();
void reset_timer(PlrState *plr);
gint game_area_pressed(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data);
gint game_area_released(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data);
gint game_area_move(GtkWidget       *widget,
                GdkEvent  *event,
                gpointer         user_data);               
            
// misc.c
void set_block(int x,int y,int color, PlrState* plr,int next);
int do_random(int max);
void add_submenu(gchar *name,GtkWidget *menu,GtkWidget *menu_bar,int right);
GtkWidget *add_menu_item(gchar *name,GtkSignalFunc func,gpointer data,gint state,GtkWidget *menu);
GtkWidget *add_menu_item_toggle(gchar *name,GtkSignalFunc func,gpointer data,gint state,GtkWidget *menu);
void set_gtk_color_style(GtkWidget *w, long red, long green,  long blue);
void get_opt_file(char *buf);
void get_hs_file(char *buf);
void get_def_path(char *buf);
GtkWidget *label_box (GtkWidget *parent, GtkWidget *label, gchar *label_text);

// highscore.c
void read_highscore();
void write_highscore();
//void show_highscore(int place);
void check_highscore();
int addto_highscore(char *name,long score, int level, int lines);

// network.c
int send_message(gint8 rows, gint8 disconnect);
int recv_message();
int create_game();
int connect_to_friend(const char *addr);
int accept_connection();
int close_connection();

//botlogic.h
void bot_step(PlrState *plr);

//dialogs.c
void show_about(GtkMenuItem     *menuitem,
        gpointer         user_data);
void show_help(GtkMenuItem     *menuitem,
           gpointer         user_data);
void show_highscore(int place);
void show_preferences(GtkMenuItem     *menuitem,
           gpointer         user_data);                  
#endif
