#include "tetris.h"

/* make step from current position 
 * Brut force algorithm	*/
void bot_step_old(PlrState *plr)
{
    if(plr->current_y>3) drop_block(plr); 
    if(plr->current_y!=1) return;
    PlrState bf;
    
    int max_x,max_r,max_res;
    max_x = max_r = max_res = -256;
    int x,r,max;
    int i,j;
    int h,hh,d,dd,l;
    
    h=MAX_Y-1;
    d=0;
    for(i=0;i<MAX_Y;i++)
        for(j=0;j<MAX_X;j++) 
        {
            h = ( plr->virtual[i][j] && h>i) ? i : h;
            if(i>0   && !bf.virtual[i-1][j]    && plr->virtual[i-1][j]) d++;

        }
    bf.next_block_area = NULL;
    bf.game_area = NULL;
    
    /* rotations */
    for(r=0;r<block_frames[plr->current_block];r++)
    {
        /* x - coordinate */
        for(x=0;x<MAX_X;x++)
        {
            if(!valid_position(x, plr->current_y,plr->current_block,r,plr)) continue;
            /* copy structure */
            for(i=0;i<MAX_Y;i++)
                for(j=0;j<MAX_X;j++) 
                {
                    bf.virtual[i][j] = plr->virtual[i][j];
                }
            bf.current_lines = plr->current_lines;                 
            bf.current_frame = r;
            bf.current_x = x;
            bf.current_score = plr->current_score; 
            bf.current_level = plr->current_level; 
            bf.current_block = plr->current_block;   
            bf.current_y = plr->current_y;
            bf.next_block = plr->next_block;
            bf.next_frame = plr->next_frame;         

            /* calculations */
            drop_block(&bf);
            l = bf.current_lines - plr->current_lines;
            dd=0;
            hh=MAX_Y-1;
            for(i=0;i<MAX_Y;i++)
            for(j=0;j<MAX_X;j++) 
            {
                hh = ( bf.virtual[i][j] && hh>i) ? i : hh;
                if(i>0   && !bf.virtual[i-1][j]   && bf.virtual[i-1][j]) d++;
            }
            
            dd=d-dd;
            hh-=h;  
            max = 4*l + hh + 2*dd;
            if(max > max_res)
            {
                 max_x = x;
                 max_r = r;
                 max_res = max;
            }
             
        }
    }
    
    move_block(max_x - plr->current_x,0,max_r - plr->current_frame, plr);
    
    return;
}
/* *********************************************** */


/* ***************** D. DENISOV'S ALG. ****************** */

PlrState *tmp_comp_plr;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include "gameunit.h"

#define inf (int)1e9

#define H MAX_Y
#define W MAX_X

typedef gint8 TField[H * W];

#define N 7
int F[N][4][4][4], FL[N][4], FH[N][4][4];

int fig;
TField m, m2, mc;
int mh[H], mhc[H];

/* ********** Interface for this alg ********** */
void Set( TField m, int x, int y, gint8 f );

int Initialize()
{
	return tmp_comp_plr->current_block;
}

int Drop(int Rotation, int Shift)
{
	
	//if(tmp_comp_plr->current_block== 4||tmp_comp_plr->current_block==5
	//	||tmp_comp_plr->current_block==3 ||tmp_comp_plr->current_block==6)
	//		Shift++;
	//printf("int Rotation %d, int Shift %d\n", Rotation, Shift);
	int r = 0;
	switch (tmp_comp_plr->current_block)
	{
		case 0:
			r = 0;
			break;
		case 1:
			r = 1;
			break;
		case 2:
			r = 1;
			break;
		case 3:
			r = 0;
			break;
		case 4:
			r = 0;
			break;
		case 5:
			r = 2;
			break;
		case 6:
			r = 1;
			break;
	}
	//int i;
	//for (i=0;i<Rotation;i++)
	move_block(/*Shift-min_x-tmp_comp_plr->current_x*/0,0,r-tmp_comp_plr->current_frame-Rotation,tmp_comp_plr);
	
	int temp, min_x=8;

    for(temp=0;temp < 4;temp++)
        min_x = (block_data[tmp_comp_plr->current_block][tmp_comp_plr->current_frame][0][temp] < min_x) ? block_data[tmp_comp_plr->current_block][tmp_comp_plr->current_frame][0][temp] : min_x;
	
	move_block(Shift-min_x-tmp_comp_plr->current_x,0,0,tmp_comp_plr);
	return 0;
}
void GetField(int Who, TField Field)
{
	int temp_x,temp_y;
    for(temp_y=0;temp_y < MAX_Y;temp_y++)
        for(temp_x=0;temp_x < MAX_X;temp_x++)
        	Set(Field, temp_x,H - temp_y-1, tmp_comp_plr->virtual[temp_y][temp_x]); 
}

/* **** end of interface ***** */ 
int max(int a, int b)
{
	if(a>b) return a;
	return b;
}

gint8 Get( TField m, int x, int y )
{
  return m[(H - y - 1) * W + x];
}

void Set( TField m, int x, int y, gint8 f )
{
  m[(H - y - 1) * W + x] = f;
}

int MyDrop( int fig, int i, int x, int out_put )
{
  int h = 0, j, a, b, y;
  for (j = 0; j < FL[fig][i]; j++)
    h = max( mh[x + j] + FH[fig][i][j],h);
  if (h > H)
    return -inf;

  memcpy(mc, m, sizeof(m));
  for (a = 0; a < 4; a++)
    for (b = 0; b < 4; b++)
      if (F[fig][i][a][b])
        Set(mc, x + b, h - a - 1, 1);

 /* if (out_put)
  {
    for (a = H - 1; a >= 0; a--)
    {
      for (b = 0; b < W; b++)
        putc(Get(mc, b, a) ? '#' : '_', stdout);
      puts("");
    }
    puts("");
  }*/

  int str_num = 0;
  while (1)
  {
    for (a = 0; a < H; a++)
    {
      for (b = 0; b < W; b++)
        if (!Get(mc, b, a))
          break;
      if (b == W)
        break;
    }
    if (a == H)
      break;
    for (y = a; y + 1 < H; y++)
      for (b = 0; b < W; b++)
        Set(mc, b, y, Get(mc, b, y + 1));
    for (b = 0; b < W; b++)
      Set(mc, b, H - 1, 0);
    str_num++;
  }

  int maxh = 0;
  memset(mhc, 0, sizeof(mhc));
  for (a = 0; a < H; a++)
    for (b = 0; b < W; b++)
      if (Get(mc, b, a))
        maxh = max( a + 1,maxh), mhc[b] = max( a + 1, mhc[b]);
  
  int holl_num = 0;
  for (a = 0; a < H; a++)
    for (b = 0; b < W; b++)
      if (!Get(mc, b, a) && a + 1 < mhc[b])
        holl_num++;

  int dh = 0;
  for (b = 1; b < W; b++)
    dh += abs(mhc[b - 1] - mhc[b]);

  /*if (out_put)
  {
    printf("str_num=%d\n", str_num);
    printf("maxh=%d\n", maxh);
    printf("holl_num=%d\n", holl_num);
    printf("dh=%d\n", dh);
    puts("");
  }*/

  if (str_num == 4)
    return inf;
  if (str_num == 3)
    return inf / 2;

  int res = 0;

  if (str_num == 2)
    res += 150;
  res -= dh * 30;
  res -= h;
  res -= holl_num * 100;

  return res;
}

int den_bot_main()
{
  memset(F, 0, sizeof(F));
  /* Chenged order */
  F[3][0][0][0] = F[3][0][1][0] = F[3][0][2][0] = F[3][0][3][0] = 1;
  F[4][0][0][0] = F[4][0][1][0] = F[4][0][2][0] = F[4][0][0][1] = 1;
  F[5][0][0][0] = F[5][0][1][0] = F[5][0][2][0] = F[5][0][2][1] = 1;
  F[2][0][0][1] = F[2][0][1][0] = F[2][0][1][1] = F[2][0][2][0] = 1;
  F[1][0][0][0] = F[1][0][1][0] = F[1][0][1][1] = F[1][0][2][1] = 1;
  F[6][0][0][0] = F[6][0][1][0] = F[6][0][1][1] = F[6][0][2][0] = 1;
  F[0][0][0][0] = F[0][0][0][1] = F[0][0][1][0] = F[0][0][1][1] = 1;

  int i, j, x, y;
  for (i = 0; i < N; i++)
    for (j = 1; j < 4; j++)
    {
      for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
          F[i][j][y][x] = F[i][j - 1][x][3 - y];
      while (1)
      {
        for (y = 0; y < 4; y++)
          if (F[i][j][y][0])
            break;
        if (y == 4)
        {
          for (y = 0; y < 4; y++)
            for (x = 1; x < 4; x++)
              F[i][j][y][x - 1] = F[i][j][y][x];
          for (y = 0; y < 4; y++)
            F[i][j][y][3] = 0;
          continue;
        }
        for (x = 0; x < 4; x++)
          if (F[i][j][0][x])
            break;
        if (x == 4)
        {
          for (x = 0; x < 4; x++)
            for (y = 1; y < 4; y++)
              F[i][j][y - 1][x] = F[i][j][y][x];
          for (x = 0; x < 4; x++)
            F[i][j][3][x] = 0;
          continue;
        }
        break;
      }
    }
  memset(FL, 0, sizeof(FL));
  for (i = 0; i < N; i++)
    for (j = 0; j < 4; j++)
      for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
          if (F[i][j][y][x])
            FL[i][j] = max( x + 1, FL[i][j]);            
  for (i = 0; i < N; i++)
    for (j = 0; j < 4; j++)
      for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
          if (F[i][j][y][x])
            FH[i][j][x] = max( y + 1,FH[i][j][x] );

  fig = Initialize();
  while (1)
  {
    GetField(0, m);
    GetField(1, m2);
    memset(mh, 0, sizeof(mh));
    for (y = 0; y < H; y++)
      for (x = 0; x < W; x++)
        if (Get(m, x, y) && mh[x] < y + 1)
          mh[x] = y + 1;
   // for (x = 0; x < W; x++)
   //   fprintf(stderr, "%d ", mh[x]);
   // fprintf(stderr, "\n");
    
    int f, resI = 0, resX = 0, resFunc = -2 * inf;
    for (i = 0; i < 4; i++)
      for (x = 0; x + FL[fig][i] <= W; x++)
        if (resFunc < (f = MyDrop(fig, i, x, 0)))
          resFunc = f, resI = i, resX = x;
    MyDrop(fig, resI, resX, 1);
    //fprintf(stderr, "I=%d X=%d Min=%d\n", resI, resX, resFunc);
    fig = Drop(resI, resX);
    /* Meke only one step */
    return 0;
  }
  return 0;
}


void bot_step(PlrState *plr)
{
	if(plr->current_y>3) drop_block(plr); //3
    if(plr->current_y!=1) return;//1
	tmp_comp_plr = plr;
	den_bot_main();
		
}

