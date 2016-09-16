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
	Property() :
		updateFunction([](int, int) {}),
		specialBehaviorFunction([](int, int) {}),
		color(UNKNOWN),
		highTemeperature(100000000000.0f),
		lowTemeperature(-273.0f),
		initTemperature(20),
		highTemperatureTransform(UNKNOWN),
		lowTemperatureTransform(UNKNOWN),
		weight(15),
		springiness(10),
		active(true),
		liquid(false),
		solid(true),
		noGravity(false)
	{};
	void(*updateFunction)(int, int);
	void(*specialBehaviorFunction)(int, int);
	unsigned int color;
	float highTemeperature;
	float lowTemeperature;
	float initTemperature;
	unsigned char highTemperatureTransform;
	unsigned char lowTemperatureTransform;
	unsigned char weight;
	unsigned char springiness;
	bool active;
	bool liquid;
	bool solid;
	bool noGravity;
	
};