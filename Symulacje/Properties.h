#pragma once

void loadProperties() {
	
	properties = new Property[TOTAL_PARTICLES];
	unsigned char type;

	type = EMPTY;
	properties[type].weight = 0;
	properties[type].springiness = 0;
	properties[type].active = false;
	properties[type].liquid = true;
	properties[type].solid = false;
	properties[type].color = EMPTY_RGB;

	type = WALL;
	properties[type].weight = 20;
	properties[type].springiness = 0;
	properties[type].active = false;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = WALL_RGB;

	type = SAND;
	properties[type].updateFunction = updateSand;
	properties[type].highTemeperature = 600;
	properties[type].highTemperatureTransform = GLASS;
	properties[type].weight = 20;
	properties[type].springiness = 12;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = SAND_RGB;

	type = WATER;
	properties[type].updateFunction = updateFluid;
	properties[type].highTemeperature = 130;
	properties[type].lowTemeperature = -1;
	properties[type].initTemperature = 20;
	properties[type].highTemperatureTransform = STEAM;
	properties[type].lowTemperatureTransform = ICE;
	properties[type].weight = 10;
	properties[type].springiness = 1;
	properties[type].active = true;
	properties[type].liquid = true;
	properties[type].solid = true;
	properties[type].color = WATER_RGB;

	type = OLIVE;
	properties[type].updateFunction = updateFluid;
	properties[type].highTemeperature = 300;
	properties[type].highTemperatureTransform = FIRE_OIL;
	properties[type].weight = 9;
	properties[type].springiness = 3;
	properties[type].active = true;
	properties[type].liquid = true;
	properties[type].solid = true;
	properties[type].color = OLIVE_RGB;

	type = FIREWORK;
	properties[type].updateFunction = updateSand;
	properties[type].highTemeperature = 150;
	properties[type].highTemperatureTransform = FLAME;
	properties[type].weight = 15;
	properties[type].springiness = 3;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = FIREWORK_RGB;

	type = DIRT;
	properties[type].updateFunction = updateSand;
	properties[type].weight = 15;
	properties[type].springiness = 0;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = DIRT_RGB;

	type = FLAME;
	properties[type].specialBehaviorFunction = updateFlame;
	properties[type].lowTemeperature = 500;
	properties[type].initTemperature = 950;
	properties[type].lowTemperatureTransform = EMPTY;
	properties[type].weight = 0;
	properties[type].springiness = 0;
	properties[type].active = true;
	properties[type].liquid = true;
	properties[type].solid = false;
	properties[type].color = FLAME_RGB(5);
	properties[type].noGravity = true;

	type = RUBBER;
	properties[type].updateFunction = updateSand;
	properties[type].weight = 15;
	properties[type].springiness = 90;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = RUBBER_RGB;

	type = ICE;
	properties[type].updateFunction = updateSand;
	properties[type].highTemeperature = 1;
	properties[type].initTemperature = -10;
	properties[type].highTemperatureTransform = WATER;
	properties[type].weight = 10;
	properties[type].springiness = 1;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = ICE_RGB;

	type = STEAM;
	properties[type].updateFunction = updateAir;
	properties[type].lowTemeperature = 70;
	properties[type].initTemperature = 400;
	properties[type].lowTemperatureTransform = WATER;
	properties[type].weight = 0;
	properties[type].springiness = 40;
	properties[type].active = true;
	properties[type].liquid = true;
	properties[type].solid = false;
	properties[type].color = STEAM_RGB;
	properties[type].noGravity = true;

	type = GLASS;
	properties[type].updateFunction = updateSand;
	properties[type].weight = 20;
	properties[type].springiness = 0;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = GLASS_RGB;

	type = FIRE_OIL;
	properties[type].updateFunction = updateFluid;
	properties[type].specialBehaviorFunction = burn;
	properties[type].weight = 9;
	properties[type].springiness = 3;
	properties[type].active = true;
	properties[type].liquid = true;
	properties[type].solid = true;
	properties[type].color = OLIVE_RGB;

	type = UNKNOWN;
	properties[type].weight = 20;
	properties[type].springiness = 0;
	properties[type].active = false;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = UNKNOWN_RGB;

	type = POWDER;
	properties[type].updateFunction = updateSand;
	properties[type].weight = 1;
	properties[type].springiness = 1;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = true;
	properties[type].color = POWDER_RGB;

	type = FIELD1;
	properties[type].specialBehaviorFunction = attractAll;
	properties[type].weight = 0;
	properties[type].springiness = 0;
	properties[type].active = true;
	properties[type].liquid = false;
	properties[type].solid = false;
	properties[type].color = UNKNOWN_RGB;
	properties[type].noGravity = true;
}