#pragma once

#define ENABLE_FLUIDS
#define ENABLE_HEAT
#define ENABLE_WATER_HACK

#define EMPTY 0
#define WALL 1
#define SAND 2
#define WATER 3
#define OLIVE 4
#define FIREWORK 5
#define DIRT 6
#define FLAME 7
#define RUBBER 8
#define ICE 9
#define STEAM 10
#define GLASS 11
#define UNKNOWN 12
#define FIRE_OIL 13
#define POWDER 14
#define FIELD1 15
#define TOTAL_PARTICLES 16

#define EMPTY_RGB 0x000000
#define WALL_RGB 0xFFFFFF 
#define SAND_RGB 0x00FFFF
#define WATER_RGB 0xFF0000
#define OLIVE_RGB 0x008080
#define FLAME_RGB(n) ((12582911 >> (n)) + 4278190080)
#define FIREWORK_RGB 0x9F773B
#define RUBBER_RGB 0x00FF00
#define DIRT_RGB 0x003366
#define ICE_RGB 0xFFCC99
#define STEAM_RGB 0xFFFFCC
#define GLASS_RGB 0xFFFF66
#define UNKNOWN_RGB 0x660066
#define POWDER_RGB 0x666666
#define ALPHA 4278190080

//CONSTANTS
#define G 0.03f
#define SA 0.1f // 0.5 G
#define WA 0.4f

#define FLUID_BOX_SCALE 10

#include "Structures.h"

#define XY(a,b) ((a)*mapSizeY+(b))
#define TEX_XY(a,b) ((b)*mapSizeX+(a))
#define IX(i,j) ((j)+(fluidBoxY+2)*(i))

#define IS_SLEEPING1(x,y) (cells[XY(x,y)].flags & 1)
#define IS_SLEEPING2(x,y) (cells[XY(x,y)].flags & (1<<(1)))
#define IS_LIQUID(x,y) (cells[XY(x,y)].flags & (1<<(2)))
#define IS_MOVING(x,y) (cells[XY(x,y)].flags & (1<<(3)))
#define IS_SOLID(x,y) (cells[XY(x,y)].flags & (1<<(4)))
#define IS_UPDATED(x,y) (cells[XY(x,y)].flags & (1<<(6)))

#define IS_EMPTY(x,y) (cells[XY(x,y)].type == EMPTY)
#define IS_ACTIVE(x,y) (cells[XY(x, y)].activeCellIndex != -1)
#define TYPE(x,y) cells[XY(x,y)].type

#define SET_SLEEPING1(x,y) (cells[XY(x,y)].flags |= 1)
#define SET_SLEEPING2(x,y) (cells[XY(x,y)].flags |= (1 << 1))
#define SET_LIQUID(x,y) (cells[XY(x,y)].flags |= (1 << 2))
#define SET_MOVING(x,y) (cells[XY(x,y)].flags |= (1 << 3))
#define SET_SOLID(x,y) (cells[XY(x,y)].flags |= (1 << 4))
#define SET_UPDATED(x,y) (cells[XY(x,y)].flags |= (1 << 6))

#define WAKE(x,y) (cells[XY(x,y)].flags &= ~3)
#define UNSET_LIQUID(x,y) (cells[XY(x,y)].flags &= ~(1 << 2))
#define UNSET_MOVING(x,y) (cells[XY(x,y)].flags &= ~(1 << 3))
#define UNSET_UPDATED(x,y) (cells[XY(x,y)].flags &= ~(1 << 6))

#define LIQUID_DIR(x,y) (cells[XY(x,y)].flags & (1<<(5)))
#define LEFT_DIR(x,y) (cells[XY(x,y)].flags &= ~(1 << 5))
#define RIGHT_DIR(x,y) (cells[XY(x,y)].flags |= (1 << 5))
#define TOGGLE_DIR(x,y) (cells[XY(x,y)].flags ^= (1 << 5))