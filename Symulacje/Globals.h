#pragma once

// current randrom boolean
unsigned char randomBoolCount = 0;

// active cell array size
int activeCellCount;

// cell array size X
int mapSizeX;

// cell array size Y
int mapSizeY;

// array of random booleans
bool randomBool[256];

// pixel map used in drawing
unsigned int *pixels;

// array of active cells, almost all cells without EMPTY and WALL
int *activeCells;

// cell array
Cell *cells;

//fluids
int ox;
int oy;
int fluidBoxX;
int fluidBoxY;
int fluidBoxArraySize;
float *u;
float *v;
float *u_prev;
float *v_prev;
float *dens;
float *dens_prev;

//float *fluidX;
//float *fluidY;

// helper variables, not used in simulation
unsigned int spawnType = 2;
int spawnRadius = 15;
bool hardSpawn = false;
bool paused = false;
bool logging = false;
bool showDensity = false;
long timer;
int fps;
