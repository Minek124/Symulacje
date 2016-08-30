#pragma once

struct Cell {
	Cell() : activeCellIndex(-1) {};
	int activeCellIndex;
	float velocityX;
	float velocityY;
	float temperature;
	unsigned char type;
	unsigned char weight;
	unsigned char springiness;
	unsigned char flags;
};

struct Property {
	void(*updateFunction)(int, int);
	void(*specialBehaviorFunction)(int, int);
	short highTemeperature;
	short lowTemeperature;
	short initTemperature;
	unsigned char highTemperatureTransform;
	unsigned char lowTemperatureTransform;
	unsigned char weight;
	unsigned char springiness;
	bool active;
	bool liquid;
	bool solid;
	unsigned int RGB;
};


void updateFluid(int x, int y);
void updateSand(int x, int y);
void updateFlame(int x, int y);
void updateAir(int x, int y);
void burn(int x, int y);
void updateCell(int pos);

inline float Random() {
	return ((float)rand() / (RAND_MAX));
}