#ifndef EFFECT3D_H
#define EFFECT3D_H


void effect_rotate_random_pixels (int iterations, int delay, int pixels);

void linespin (int iterations, int delay);

void ripples (int iterations, int delay);
float distance2d (float x1, float y1, float x2, float y2);
float distance3d (float x1, float y1, float z1, float x2, float y2, float z2);


//typedef struct {float x; float y; float z;} vertex;

#endif
