#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "tetris.h"

//#define highscore_file "highscore.dat"

void read_highscore()
{
    FILE *fp;
    if((fp = fopen(highscore_file,"r")))
    {
        if(fread(&highscore,1,sizeof(highscore),fp)<sizeof(highscore))
    	{
    		printf("error while reading highscore file\n");
    	}
        fclose(fp);
    }
}

void write_highscore()
{
    FILE *fp;
    if(!(fp = fopen(highscore_file,"w")))
        return;
    if(fwrite(&highscore,1,sizeof(highscore),fp)<sizeof(highscore))
    {
    	printf("error while writing highscore file\n");
    }
    fclose(fp);
}


int addto_highscore(char *name,long score, int level, int lines)
{
	int place = -1;
	int temp,namelen;
	int added = FALSE;
	
	for(temp=NUM_HIGHSCORE-1;temp > -1;temp--)
	{
		if(score > highscore[temp].score || (highscore[temp].score == 0 && strlen(highscore[temp].name) == 0))
		{
			place = temp;
			added = TRUE;
		}
	}
	if(added)
	{
		for(temp=NUM_HIGHSCORE-1;temp > place;temp--)
			memcpy(&highscore[temp],&highscore[temp-1],sizeof(highscore[0]));
		namelen = strlen(name);
		if(namelen > 9)
			namelen = 9;
		memset(&highscore[place].name,0,sizeof(highscore[0].name));
		memcpy(&highscore[place].name,name,namelen);
		highscore[place].score = score;
		highscore[place].level = level;
		highscore[place].lines = lines;
	}
	return place+1;
}
