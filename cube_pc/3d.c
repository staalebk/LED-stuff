#include "draw.h"
#include "draw_3d.h"
#include "3d.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void linespin (int iterations, int delay)
{
	float top_x, top_y, top_z, bot_x, bot_y, bot_z, sin_base;
	float center_x, center_y;

	center_x = 4;
	center_y = 4;

	int i, z;
	for (i=0;i<iterations;i++)
	{

		//printf("Sin base %f \n",sin_base);

		for (z = 0; z < 8; z++)
		{

		sin_base = (float)i/50 + (float)z/(10+(7*sin((float)i/200)));

		top_x = center_x + sin(sin_base)*5;
		top_y = center_x + cos(sin_base)*5;
		//top_z = center_x + cos(sin_base/100)*2.5;

		bot_x = center_x + sin(sin_base+3.14)*10;
		bot_y = center_x + cos(sin_base+3.14)*10;
		//bot_z = 7-top_z;
		
		bot_z = z;
		top_z = z;

		//setvoxel((int) top_x, (int) top_y, 7);
		//setvoxel((int) bot_x, (int) bot_y, 0);

		//printf("P1: %i %i %i P2: %i %i %i \n", (int) top_x, (int) top_y, 7, (int) bot_x, (int) bot_y, 0);

		//line_3d((int) top_x, (int) top_y, (int) top_z, (int) bot_x, (int) bot_y, (int) bot_z);
		line_3d((int) top_z, (int) top_x, (int) top_y, (int) bot_z, (int) bot_x, (int) bot_y);
		}

		delay_ms(120);
		fill(0x00);
	}

}

void sinelines (int iterations, int delay)
{
	int i,x;

	float left, right, sine_base, x_dividor,ripple_height;

	for (i=0; i<iterations; i++)
	{
		for (x=0; x<8 ;x++)
		{
			x_dividor = 2 + sin((float)i/100)+1;
			ripple_height = 3 + (sin((float)i/200)+1)*6;

			sine_base = (float) i/40 + (float) x/x_dividor;

			left = 4 + sin(sine_base)*ripple_height;
			right = 4 + cos(sine_base)*ripple_height;
			right = 7-left;

			//printf("%i %i \n", (int) left, (int) right);

			line_3d(0-3, x, (int) left, 7+3, x, (int) right);
			//line_3d((int) right, 7, x);
		}
	
	delay_ms(delay);
	fill(0x00);
	}
}

// Display a sine wave running out from the center of the cube.
void ripples (int iterations, int delay)
{
	float origin_x, origin_y, distance, height, ripple_interval;
	int x,y,i;

	fill(0x00);

	for (i=0;i<iterations;i++)
	{
		for (x=0;x<8;x++)
		{
			for (y=0;y<8;y++)
			{
				distance = distance2d(3.5,3.5,x,y)/9.899495*8;
				//distance = distance2d(3.5,3.5,x,y);
				ripple_interval =1.3;
				height = 4+sin(distance/ripple_interval+(float) i/50)*4;

				setvoxel(x,y,(int) height);	
			}
		}
		delay_ms(delay);
		fill(0x00);
	}
}

void sidewaves (int iterations, int delay)
{
	float origin_x, origin_y, distance, height, ripple_interval;
	int x,y,i;

	fill(0x00);

	for (i=0;i<iterations;i++)
	{

		origin_x = 3.5+sin((float)i/500)*4;
		origin_y = 3.5+cos((float)i/500)*4;
		
		for (x=0;x<8;x++)
		{
			for (y=0;y<8;y++)
			{
				distance = distance2d(origin_x,origin_y,x,y)/9.899495*8;
				ripple_interval =2;
				height = 4+sin(distance/ripple_interval+(float) i/50)*3.6;

				setvoxel(x,y,(int) height);
				setvoxel(x,y,(int) height);
					
			}
		}

		delay_ms(delay);
		fill(0x00);
	}
}

void spheremove (int iterations, int delay)
{
	
	fill(0x00);

	float origin_x, origin_y, origin_z, distance, diameter;

	origin_x = 0;
	origin_y = 3.5;
	origin_z = 3.5;

	diameter = 3;

	int x, y, z, i;

	for (i=0; i<iterations; i++)
	{
		origin_x = 3.5+sin((float)i/50)*2.5;
		origin_y = 3.5+cos((float)i/50)*2.5;
		origin_z = 3.5+cos((float)i/30)*2;

		diameter = 2+sin((float)i/150);

		for (x=0; x<8; x++)
		{
			for (y=0; y<8; y++)
			{
				for (z=0; z<8; z++)
				{
					distance = distance3d(x,y,z, origin_x, origin_y, origin_z);
					//printf("Distance: %f \n", distance);

					if (distance>diameter && distance<diameter+1)
					{
						setvoxel(x,y,z);
					}
				}
			}
		}

		delay_ms(delay);
		fill(0x00);
	}

}

void fireworks (int iterations, int n, int delay)
{
	fill(0x00);

	int i,f,e;

	float origin_x = 3;
	float origin_y = 3;
	float origin_z = 3;

	int rand_y, rand_x, rand_z;

	float slowrate, gravity;

	// Particles and their position, x,y,z and their movement, dx, dy, dz
	float particles[n][6];

	for (i=0; i<iterations; i++)
	{

		origin_x = rand()%4;
		origin_y = rand()%4;
		origin_z = rand()%2;
		origin_z +=5;
        origin_x +=2;
        origin_y +=2;

		// shoot a particle up in the air
		for (e=0;e<origin_z;e++)
		{
			setvoxel(origin_x,origin_y,e);
			delay_ms(600+500*e);
			fill(0x00);
		}

		// Fill particle array
		for (f=0; f<n; f++)
		{
			// Position
			particles[f][0] = origin_x;
			particles[f][1] = origin_y;
			particles[f][2] = origin_z;
			
			rand_x = rand()%200;
			rand_y = rand()%200;
			rand_z = rand()%200;

			// Movement
			particles[f][3] = 1-(float)rand_x/100; // dx
			particles[f][4] = 1-(float)rand_y/100; // dy
			particles[f][5] = 1-(float)rand_z/100; // dz
		}

		// explode
		for (e=0; e<25; e++)
		{
			slowrate = 1+tan((e+0.1)/20)*10;
			
			gravity = tan((e+0.1)/20)/2;

			for (f=0; f<n; f++)
			{
				particles[f][0] += particles[f][3]/slowrate;
				particles[f][1] += particles[f][4]/slowrate;
				particles[f][2] += particles[f][5]/slowrate;
				particles[f][2] -= gravity;

				setvoxel(particles[f][0],particles[f][1],particles[f][2]);


			}

			delay_ms(delay);
			fill(0x00);
		}

	}

}

void effect_rotate_random_pixels (int iterations, int delay, int pixels)
{
	vertex points[pixels];
	vertex rotated[pixels];

	float fy, fx, fz;
	int x,y,z;
	int i,p;

	float rot_x = 0;
	float rot_y = 0;
	float rot_z = 0;
	vertex cube_center = {3.5, 3.5, 3.5};

	for (i=0; i<pixels; i++)
	{
		x = rand()%1200-200;	
		y = rand()%1200-200;	
		z = rand()%1200-200;
		fx = (float)x/100;
		fy = (float)y/100;
		fz = (float)z/100;

		points[i].x = fx;
		points[i].y = fy;
		points[i].z = fz;

		setvoxel((int)points[i].x, (int)points[i].y, (int)points[i].z);
		delay_ms(100);
	}
	delay_ms(10000);

	for (i=0; i<iterations; i++)
	{
		rot_x = (float)i/75;
		rot_y = (float)i/150;
		rot_z = (float)i/200;

		for (p=0; p<pixels; p++)
		{
			rotated[p] = point_rotate_around_point (points[p], cube_center, rot_x, rot_y, rot_z);
		}

		fill(0x00);
		for (p=0; p<pixels; p++)
		{
			setvoxel((int)rotated[p].x, (int)rotated[p].y, (int)rotated[p].z);
		}

		delay_ms(delay);
	}

	fill(0x00);
}


float distance2d (float x1, float y1, float x2, float y2)
{	
	float dist;
	dist = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

	return dist;
}

float distance3d (float x1, float y1, float z1, float x2, float y2, float z2)
{	
	float dist;
	dist = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));

	return dist;
}




