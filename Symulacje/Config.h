#pragma once

#pragma once

#include "Structures.h"

#define XY(a,b) ((a)*mapSizeY+(b))
#define TEX_XY(a,b) ((b)*mapSizeX+(a))
#define IX(i,j) ((j)+(fluidBoxY+2)*(i))


#define IS_SLEEPING1(x,y) (cells[XY(x,y)].flags & 1)
#define IS_SLEEPING2(x,y) (cells[XY(x,y)].flags & (1<<(1)))
#define IS_LIQUID(x,y) (cells[XY(x,y)].flags & (1<<(2)))
#define IS_MOVING(x,y) (cells[XY(x,y)].flags & (1<<(3)))
#define IS_EMPTY(x,y) (cells[XY(x,y)].type == EMPTY)
#define TYPE(x,y) cells[XY(x,y)].type

#define SET_SLEEPING1(x,y) (cells[XY(x,y)].flags |= 1)
#define SET_SLEEPING2(x,y) (cells[XY(x,y)].flags |= (1 << 1))
#define SET_LIQUID(x,y) (cells[XY(x,y)].flags |= (1 << 2))
#define SET_MOVING(x,y) (cells[XY(x,y)].flags |= (1 << 3))

#define WAKE(x,y) (cells[XY(x,y)].flags &= ~3)
#define UNSET_LIQUID(x,y) (cells[XY(x,y)].flags &= ~(1 << 2))
#define UNSET_MOVING(x,y) (cells[XY(x,y)].flags &= ~(1 << 3))

#define LIQUID_DIR(x,y) (cells[XY(x,y)].flags & (1<<(5)))
#define LEFT_DIR(x,y) (cells[XY(x,y)].flags &= ~(1 << 5))
#define RIGHT_DIR(x,y) (cells[XY(x,y)].flags |= (1 << 5))
#define TOGGLE_DIR(x,y) (cells[XY(x,y)].flags ^= (1 << 5))

#define MAP "maps/mapa.png"
//#define MAP "maps/piasek.png"
//#define MAP "maps/woda.png"
//#define MAP "maps/pusta.png"
//#define MAP "maps/small.png"
//#define MAP "maps/test.png"

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
#define ALPHA 4278190080



//CONSTANTS
#define G 0.04f
#define SA 0.1f // 0.5 G
#define WA 0.2f
#define WR 10

#define FLUID_BOX_SCALE 10


#define lowNA SHRT_MIN 
#define highNA SHRT_MAX 
#define FLAME_LOW_TEMP 500

const Property properties[] = {

	//EMPTY 0
	{
		// updateFunction
		[](int, int) {},
		// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	0,
	// springiness
	0,
	// active
	false,
	// liquid
	true,
	// RGB 
	EMPTY_RGB
	},

	// WALL 1
	{
		// updateFunction
		[](int, int) {},
		// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	0,
	// springiness
	0,
	// active
	false,
	// liquid
	false,
	// RGB 
	WALL_RGB
	},

	// SAND 2
	{
		// updateFunction
		updateSand,
		// special Function
	[](int, int) {},
	// high temperature
	600,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	GLASS,
	// low temeprature transform
	255,
	// weight
	15,
	// springiness
	12,
	// active
	true,
	// liquid
	false,
	// RGB 
	SAND_RGB
	},

	// WATER 3
	{
		// updateFunction
		updateFluid,
		// special Function
	[](int, int) {},
	// high temperature
	100,
	// low temperature
	0,
	// init temperature 
	20,
	// high temeprature transform
	STEAM,
	// low temeprature transform
	ICE,
	// weight
	10,
	// springiness
	20,
	// active
	true,
	// liquid
	true,
	// RGB 
	WATER_RGB
	},

	// OLIVE 4
	{
		// updateFunction
		updateFluid,
		// special Function
	[](int, int) {},
	// high temperature
	300,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	FIRE_OIL,
	// low temeprature transform
	255,
	// weight
	8,
	// springiness
	3,
	// active
	true,
	// liquid
	true,
	// RGB 
	OLIVE_RGB
	},

	// FIREWORK 5
	{
		// updateFunction
		updateSand,
		// special Function
	[](int, int) {},
	// high temperature
	150,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	FIREWORK,
	// low temeprature transform
	255,
	// weight
	15,
	// springiness
	3,
	// active
	true,
	// liquid
	false,
	// RGB 
	FIREWORK_RGB
	},

	// DIRT 6
	{
		// updateFunction
		updateSand,
		// special Function
	[](int, int) {},
	// high temperature
	700,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	15,
	// springiness
	0,
	// active
	true,
	// liquid
	false,
	// RGB 
	DIRT_RGB
	},

	// FLAME 7
	{
		// updateFunction
		[](int, int) {},
		// special Function
	updateFlame,
	// high temperature
	highNA,
	// low temperature
	FLAME_LOW_TEMP,
	// init temperature 
	950,
	// high temeprature transform
	255,
	// low temeprature transform
	EMPTY,
	// weight
	0,
	// springiness
	3,
	// active
	true,
	// liquid
	true,
	// RGB 
	FLAME_RGB(0)
	},

	// RUBBER 8
	{
	// updateFunction
	updateSand,
	// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	10,
	// springiness
	80,
	// active
	true,
	// liquid
	false,
	// RGB 
	RUBBER_RGB
	},

	// ICE 9
	{
		// updateFunction
		updateSand,
		// special Function
	[](int, int) {},
	// high temperature
	0,
	// low temperature
	lowNA,
	// init temperature 
	-5,
	// high temeprature transform
	WATER,
	// low temeprature transform
	255,
	// weight
	11,
	// springiness
	2,
	// active
	true,
	// liquid
	false,
	// RGB 
	ICE_RGB
	},

	//STEAM 10
	{
		// updateFunction
		updateAir,
		// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	100,
	// init temperature 
	500,
	// high temeprature transform
	255,
	// low temeprature transform
	WATER,
	// weight
	0,
	// springiness
	10,
	// active
	true,
	// liquid
	true,
	// RGB 
	STEAM_RGB
	},

	// GLASS 11
	{
		// updateFunction
		updateSand,
		// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	20,
	// springiness
	3,
	// active
	true,
	// liquid
	false,
	// RGB 
	GLASS_RGB
	},

	// UNKNOWN 12
	{
		// updateFunction
		[](int, int) {},
		// special Function
	[](int, int) {},
	// high temperature
	highNA,
	// low temperature
	lowNA,
	// init temperature 
	20,
	// high temeprature transform
	255,
	// low temeprature transform
	255,
	// weight
	20,
	// springiness
	0,
	// active
	true,
	// liquid
	false,
	// RGB 
	UNKNOWN_RGB
	},

	// FIRE OIL 13
	{
	// updateFunction
	updateFluid,
	// special Function
	burn,
	// high temperature
	highNA,
	// low temperature
	OLIVE,
	// init temperature 
	301,
	// high temeprature transform
	FLAME,
	// low temeprature transform
	255,
	// weight
	8,
	// springiness
	3,
	// active
	true,
	// liquid
	true,
	// RGB 
	OLIVE_RGB
	},

};
