#pragma once

struct Cell {
	Cell() : activeCellIndex(-1) {};
	int activeCellIndex;
	float velX;
	float velY;
	float temperature;
	unsigned char type;
	unsigned char flags;
	unsigned char other;
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