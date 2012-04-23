#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cube.h"
#include "draw.h"
#include "draw_3d.h"
#include "effect.h"
#include "gameoflife.h"

void *cube_updater (unsigned char rs232_cube[8][8]);

int main (int argc, char **argv)
{

	cube_init();

	pthread_t cube_thread;
	int iret, i, x;

	iret = pthread_create (&cube_thread, NULL, cube_updater, rs232_cube);


	while (1)
	{
		printf("Effect: sidewaves\n");
		sidewaves(2000,100);

		printf("Effect: ripples\n");
		ripples(2000,100);

		printf("Effect: linespin\n");
		linespin(2000,100);

		printf("Effect: sinelines\n");
		sinelines(2000,100);

		printf("Effect: spheremove\n");
		spheremove(1500,100);

		printf("Effect: fireworks\n");
		fireworks(7,50,1200);

        printf("Effect: gol_play\n");
        for (i=0; i<10; i++)
        {
            for (x=0; x<20; x++)
                setvoxel(rand()%4,rand()%4,rand()%4);

            gol_play(50,1000);
            
        }
	}

}

void *cube_updater (unsigned char rs232_cube[8][8])
{
    unsigned char pushcube[8][8];


	while (1)
    {
        memcpy(pushcube, rs232_cube, 64);
		cube_push(pushcube);
    }
}


