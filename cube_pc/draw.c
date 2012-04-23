#include "draw.h"
#include "draw_3d.h"
#include <string.h>


void setvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		cube[z][y] |= (1 << x);
    
}

void tmpsetvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		fb[z][y] |= (1 << x);
}

void clrvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		cube[z][y] &= ~(1 << x);
}

void tmpclrvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
		fb[z][y] &= ~(1 << x);
}

// This function validates that we are drawing inside the cube.
unsigned char inrange(int x, int y, int z)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8 && z >= 0 && z < 8)
	{
		return 0x01;
	} else
	{
		// One of the coordinates was outside the cube.
		return 0x00;
	}
}

// Get the current status of a voxel
unsigned char getvoxel(int x, int y, int z)
{
	if (inrange(x,y,z))
	{
		if (cube[z][y] & (1 << x))
		{
			return 0x01;
		} else
		{
			return 0x00;
		}
	} else
	{
		return 0x00;
	}
}

void altervoxel(int x, int y, int z, int state)
{
	if (state == 1)
	{
		setvoxel(x,y,z);
	} else
	{
		clrvoxel(x,y,z);
	}
}

// Flip the state of a voxel.
// If the voxel is 1, its turned into a 0, and vice versa.
void flpvoxel(int x, int y, int z)
{
	if (inrange(x, y, z))
		cube[z][y] ^= (1 << x);
}

// Makes sure x1 is alwas smaller than x2
void argorder(int ix1, int ix2, int *ox1, int *ox2)
{
	if (ix1>ix2)
	{
		int tmp;
		tmp = ix1;
		ix1= ix2;
		ix2 = tmp;
	}
	*ox1 = ix1;
	*ox2 = ix2;
}

void setplane_z (int z)
{
	int i;
	if (z>=0 && z<8)
	{
		for (i=0;i<8;i++)
			cube[z][i] = 0xff;
	}
}


void clrplane_z (int z)
{
	int i;
	if (z>=0 && z<8)
	{
		for (i=0;i<8;i++)
			cube[z][i] = 0x00;
	}
}

void setplane_x (int x)
{
	int z;
	int y;
	if (x>=0 && x<8)
	{
		for (z=0;z<8;z++)
		{
			for (y=0;y<8;y++)
			{
				cube[z][y] |= (1 << x);
			}
		}
	}
}

void clrplane_x (int x)
{
	int z;
	int y;
	if (x>=0 && x<8)
	{
		for (z=0;z<8;z++)
		{
			for (y=0;y<8;y++)
			{
				cube[z][y] &= ~(1 << x);
			}
		}
	}
}

void setplane_y (int y)
{
	int z;
	if (y>=0 && y<8)
	{
		for (z=0;z<8;z++)
			cube[z][y] = 0xff;
	} 
}

void clrplane_y (int y)
{
	int z;
	if (y>=0 && y<8)
	{
		for (z=0;z<8;z++)
			cube[z][y] = 0x00; 
	}
}


void fill (unsigned char pattern)
{
	int z;
	int y;
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
		{
			cube[z][y] = pattern;
		}
	}
}

void tmpfill (unsigned char pattern)
{
	int z;
	int y;
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
		{
			fb[z][y] = pattern;
		}
	}
}


void box_filled(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;

	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	for (iz=z1;iz<=z2;iz++)
	{
		for (iy=y1;iy<=y2;iy++)
		{
			cube[iz][iy] |= byteline(x1,x2);
		}
	}

}

void box_walls(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;
	
	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	for (iz=z1;iz<=z2;iz++)
	{
		for (iy=y1;iy<=y2;iy++)
		{	
			if (iy == y1 || iy == y2 || iz == z1 || iz == z2)
			{
				cube[iz][iy] = byteline(x1,x2);
			} else
			{
				cube[iz][iy] |= ((0x01 << x1) | (0x01 << x2));
			}
		}
	}

}


void box_wireframe(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int iy;
	int iz;

	argorder(x1, x2, &x1, &x2);
	argorder(y1, y2, &y1, &y2);
	argorder(z1, z2, &z1, &z2);

	// Lines along X axis
	cube[z1][y1] = byteline(x1,x2);
	cube[z1][y2] = byteline(x1,x2);
	cube[z2][y1] = byteline(x1,x2);
	cube[z2][y2] = byteline(x1,x2);

	// Lines along Y axis
	for (iy=y1;iy<=y2;iy++)
	{
		setvoxel(x1,iy,z1);
		setvoxel(x1,iy,z2);
		setvoxel(x2,iy,z1);
		setvoxel(x2,iy,z2);
	}

	// Lines along Z axis
	for (iz=z1;iz<=z2;iz++)
	{
		setvoxel(x1,y1,iz);
		setvoxel(x1,y2,iz);
		setvoxel(x2,y1,iz);
		setvoxel(x2,y2,iz);
	}

}

// Returns a byte with a row of 1's drawn in it.
// byteline(2,5) gives 0b00111100
char byteline (int start, int end)
{
	return ((0xff<<start) & ~(0xff<<(end+1)));
}

char flipbyte (char byte)
{
	char flop = 0x00;

	flop = (flop & 0b11111110) | (0b00000001 & (byte >> 7));
	flop = (flop & 0b11111101) | (0b00000010 & (byte >> 5));
	flop = (flop & 0b11111011) | (0b00000100 & (byte >> 3));
	flop = (flop & 0b11110111) | (0b00001000 & (byte >> 1));
	flop = (flop & 0b11101111) | (0b00010000 & (byte << 1));
	flop = (flop & 0b11011111) | (0b00100000 & (byte << 3));
	flop = (flop & 0b10111111) | (0b01000000 & (byte << 5));
	flop = (flop & 0b01111111) | (0b10000000 & (byte << 7));
	return flop;
}

void line(int x1, int y1, int z1, int x2, int y2, int z2)
{
	float xy;	// how many voxels do we move on the y axis for each step on the x axis
	float xz;	// how many voxels do we move on the y axis for each step on the x axis 
	unsigned char x,y,z;
	unsigned char lasty,lastz;

	// We always want to draw the line from x=0 to x=7.
	// If x1 is bigget than x2, we need to flip all the values.
	if (x1>x2)
	{
		int tmp;
		tmp = x2; x2 = x1; x1 = tmp;
		tmp = y2; y2 = y1; y1 = tmp;
		tmp = z2; z2 = z1; z1 = tmp;
	}

	
	if (y1>y2)
	{
		xy = (float)(y1-y2)/(float)(x2-x1);
		lasty = y2;
	} else
	{
		xy = (float)(y2-y1)/(float)(x2-x1);
		lasty = y1;
	}

	if (z1>z2)
	{
		xz = (float)(z1-z2)/(float)(x2-x1);
		lastz = z2;
	} else
	{
		xz = (float)(z2-z1)/(float)(x2-x1);
		lastz = z1;
	}



	for (x = x1; x<=x2;x++)
	{
		y = (xy*(x-x1))+y1;
		z = (xz*(x-x1))+z1;
		setvoxel(x,y,z);
	}
	
}
	
void delay_ms(int x)
{
    memcpy(rs232_cube, cube, 64);
	usleep(x*40);
}

// Copies the contents of fb (temp cube buffer) into the rendering buffer
void tmp2cube (void)
{	
	int y, z;
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
		{
			cube[z][y] = fb[z][y];
		}
	}
}

void shift (char axis, int direction)
{
	int i, x ,y;
	int ii, iii;
	int state;

	for (i = 0; i < 8; i++)
	{
		if (direction == -1)
		{
			ii = i;
		} else
		{
			ii = (7-i);
		}	
	
	
		for (x = 0; x < 8; x++)
		{
			for (y = 0; y < 8; y++)
			{
				if (direction == -1)
				{
					iii = ii+1;
				} else
				{
					iii = ii-1;
				}
				
				if (axis == AXIS_Z)
				{
					state = getvoxel(x,y,iii);
					altervoxel(x,y,ii,state);
				}
				
				if (axis == AXIS_Y)
				{
					state = getvoxel(x,iii,y);
					altervoxel(x,ii,y,state);
				}
				
				if (axis == AXIS_X)
				{
					state = getvoxel(iii,y,x);
					altervoxel(ii,x,y,state);
				}
			}
		}
	}
	
	if (direction == -1)
	{
		i = 7;
	} else
	{
		i = 0;
	}	
	
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			if (axis == AXIS_Z)
				clrvoxel(x,y,i);
				
			if (axis == AXIS_Y)
				clrvoxel(x,i,y);
			
			if (axis == AXIS_X)
				clrvoxel(i,y,x);
		}
	}
}



void line_3d (int x1, int y1, int z1, int x2, int y2, int z2)
{
	int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc,
	err_1, err_2, dx2, dy2, dz2;
	int pixel[3];
	pixel[0] = x1;
	pixel[1] = y1;
	pixel[2] = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;
	if ((l >= m) && (l >= n)) {
	err_1 = dy2 - l;
	err_2 = dz2 - l;
	for (i = 0; i < l; i++) {
	//PUT_PIXEL(pixel);
	setvoxel(pixel[0],pixel[1],pixel[2]);
	//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
	if (err_1 > 0) {
	pixel[1] += y_inc;
	err_1 -= dx2;
	}
	if (err_2 > 0) {
	pixel[2] += z_inc;
	err_2 -= dx2;
	}
	err_1 += dy2;
	err_2 += dz2;
	pixel[0] += x_inc;
	}
	} else if ((m >= l) && (m >= n)) {
	err_1 = dx2 - m;
	err_2 = dz2 - m;
	for (i = 0; i < m; i++) {
	//PUT_PIXEL(pixel);
	setvoxel(pixel[0],pixel[1],pixel[2]);
	//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
	if (err_1 > 0) {
	pixel[0] += x_inc;
	err_1 -= dy2;
	}
	if (err_2 > 0) {
	pixel[2] += z_inc;
	err_2 -= dy2;
	}
	err_1 += dx2;
	err_2 += dz2;
	pixel[1] += y_inc;
	}
	} else {
	err_1 = dy2 - n;
	err_2 = dx2 - n;
	for (i = 0; i < n; i++) {
	setvoxel(pixel[0],pixel[1],pixel[2]);
	//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
	//PUT_PIXEL(pixel);
	if (err_1 > 0) {
	pixel[1] += y_inc;
	err_1 -= dz2;
	}
	if (err_2 > 0) {
	pixel[0] += x_inc;
	err_2 -= dz2;
	}
	err_1 += dy2;
	err_2 += dx2;
	pixel[2] += z_inc;
	}
	}
	setvoxel(pixel[0],pixel[1],pixel[2]);
	//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
	//PUT_PIXEL(pixel);
}


void line_3d_float (vertex point1, vertex point2)
{
	float x1, y1, z1, x2, y2, z2;

	x1 = point1.x;
	y1 = point1.y;
	z1 = point1.z;
	x2 = point2.x;
	y2 = point2.y;
	z2 = point2.z;


	float i;
	float dx, dy, dz, l, m, n, x_inc, y_inc, z_inc,
	err_1, err_2, dx2, dy2, dz2;
	float pixel[3];
	pixel[0] = x1;
	pixel[1] = y1;
	pixel[2] = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l*l;
	dy2 = m*m;
	dz2 = n*n;
	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			//PUT_PIXEL(pixel);
			setvoxel((int)pixel[0],(int)pixel[1],(int)pixel[2]);
			//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
			if (err_1 > 0) {
				pixel[1] += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				pixel[2] += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			pixel[0] += x_inc;
		}
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			//PUT_PIXEL(pixel);
			//setvoxel(pixel[0]/scale,pixel[1]/scale,pixel[2]/scale);
			setvoxel((int)pixel[0],(int)pixel[1],(int)pixel[2]);
			//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
			if (err_1 > 0) {
				pixel[0] += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				pixel[2] += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			pixel[1] += y_inc;
		}
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			//setvoxel(pixel[0]/scale,pixel[1]/scale,pixel[2]/scale);
			setvoxel((int)pixel[0],(int)pixel[1],(int)pixel[2]);
			//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
			//PUT_PIXEL(pixel);
			if (err_1 > 0) {
				pixel[1] += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				pixel[0] += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			pixel[2] += z_inc;
		}
	}
	//setvoxel(pixel[0]/scale,pixel[1]/scale,pixel[2]/scale);
			setvoxel((int)pixel[0],(int)pixel[1],(int)pixel[2]);
	//printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
	//PUT_PIXEL(pixel);
}

// Flip the cube 180 degrees along the x axis
void mirror_x (void)
{
    unsigned char buffer[8][8];
    unsigned char y,z;

    memcpy(buffer, cube, 64); // copy the current cube into a buffer.

    fill(0x00);

    for (z=0; z<8; z++)
    {
        for (y=0; y<8; y++)
        {
            cube[z][y] = flipbyte(buffer[z][y]);
        }
    }
}
// Flip the cube 180 degrees along the y axis.
void mirror_y (void)
{
    unsigned char buffer[8][8];
    unsigned char x,y,z;

    memcpy(buffer, cube, 64); // copy the current cube into a buffer.

    fill(0x00);
    for (z=0; z<8; z++)
    {
        for (y=0; y<8; y++)
        {
            for (x=0; x<8; x++)
            {
                if (buffer[z][y] & (0x01 << x))
                    setvoxel(x,7-y,z);
            }
        }
    }

}
// flip the cube 180 degrees along the z axis
void mirror_z (void)
{
    unsigned char buffer[8][8];
    unsigned char z, y;

    memcpy(buffer, cube, 64); // copy the current cube into a buffer.

    for (y=0; y<8; y++)
    {
        for (z=0; z<8; z++)
        {
            cube[7-z][y] = buffer[z][y];
        }
    }
}




