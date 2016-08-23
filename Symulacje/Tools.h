#pragma once

void updatePixelMap() {
	for (int i = 0; i < mapSizeY; i++) {
		for (int j = 0; j < mapSizeX; j++) {
			//if (cells[XY(j, i)].type != FLAME)
				pixels[TEX_XY(j, i)] = properties[cells[XY(j, i)].type].RGB | ALPHA;
			/*else {
				pixels[TEX_XY(j, i)] = FLAME_RGB(2 * (5 - ((int)((cells[XY(j, i)].temperature - FLAME_LOW_TEMP)) / 100))) | ALPHA;
			}*/
		}
	}
}

// update air flow on pixelmap
void updatePixelMap2() {
	for (int i = 0; i < mapSizeX; i++) {
		for (int j = 0; j < mapSizeY; j++) {
			pixels[TEX_XY(i, j)] = properties[cells[XY(i, j)].type].RGB | ALPHA;
		}
	}
	for (int i = 1; i <= fluidBoxX; i++) {
		for (int j = 1; j <= fluidBoxY; j++) {
			unsigned int d = (unsigned int)(dens[IX(i, j)] * 255);
			if (d > 255) {
				d = 255;
			}
			for (int x = 0; x < FLUID_BOX_SCALE; ++x)
				for (int y = 0; y < FLUID_BOX_SCALE; ++y)
					pixels[TEX_XY((i - 1)*FLUID_BOX_SCALE + x, (j - 1)*FLUID_BOX_SCALE + y)] |= d;
		}
	}
}

// debug cell
void printCell(int x, int y) {
	std::string cell = "cell[" + std::to_string(x) + "][" + std::to_string(y) + "]";
	std::string type = " type:" + std::to_string(((int)cells[XY(x, y)].type));
	std::string sleep = (IS_SLEEPING2(x, y) ? (IS_SLEEPING1(x, y) ? ", SLEEPING+" : ", SLEEPING") : (IS_SLEEPING1(x, y) ? ", NOT_SLEEPING+" : ", NOT_SLEEPING"));
	std::string liquid = (IS_LIQUID(x, y) ? (LIQUID_DIR(x, y) ? ", LIQUID_RIGHT" : ", LIQUID_LEFT") : ", NOT_LIQUID");
	std::string solid = (IS_SOLID(x, y) ? ", SOLID" : ", NOT_SOLID");
	std::string falling = (IS_MOVING(x, y) ? ", FALLING" : ", NOT_FALLING");
	std::string speed = ", velocity: [" + std::to_string(cells[XY(x, y)].velocityX) + ", " + std::to_string(cells[XY(x, y)].velocityY) + "]" + ((abs(cells[XY(x, y)].velocityX) >= 1 || abs(cells[XY(x, y)].velocityY) >= 1) ? "PHYSIC" : "AUTO");
	std::string active = (cells[XY(x, y)].activeCellIndex != -1 ? ", ACTIVE: " + std::to_string(activeCells[cells[XY(x, y)].activeCellIndex]) : ", NOT_ACTIVE");
	std::string temperature = " temp:" + std::to_string(cells[XY(x, y)].temperature);
	std::string proc = ( IS_UPDATED(x,y) ? " proc:" : " not_proc");
	std::string s = cell + type + speed + sleep + liquid + solid + falling + temperature + active + proc;
	std::cout << s << std::endl;
}


// used in loading map from image
unsigned int colorToType(unsigned char r, unsigned char g, unsigned char b) {
	if (r == 0 && g == 0 && b == 0) {
		return EMPTY;
	}
	if (r == 255 && g == 255 && b == 255) {
		return WALL;
	}
	if (r > 230 && g > 230 && b < 10) {
		return SAND;
	}
	if (r < 20 && g < 20 && b > 230) {
		return WATER;
	}
	std::cout << "UNKNOWN COLOR TYPE: R: " << (int)(r) << " G: " << (int)(g) << " B: " << (int)(b);
	return UNKNOWN;
}

// swap cells
void moveCell(int srcX, int srcY, int dstX, int dstY) {

	Cell tmp = cells[XY(srcX, srcY)];
	cells[XY(srcX, srcY)] = cells[XY(dstX, dstY)];
	cells[XY(dstX, dstY)] = tmp;

	if (cells[XY(srcX, srcY)].activeCellIndex != -1) {
		activeCells[cells[XY(srcX, srcY)].activeCellIndex] = XY(srcX, srcY);
	}
	if (cells[XY(dstX, dstY)].activeCellIndex != -1) {
		activeCells[cells[XY(dstX, dstY)].activeCellIndex] = XY(dstX, dstY);
	}
}

void cloneCell(int srcX, int srcY, int dstX, int dstY) {
	cells[XY(dstX, dstY)] = cells[XY(srcX, srcY)];
	if (cells[XY(dstX, dstY)].activeCellIndex != -1) {
		activeCells[activeCellCount] = XY(dstX, dstY);
		cells[XY(dstX, dstY)].activeCellIndex = activeCellCount;
		++activeCellCount;
	}
}

void createCell(int x, int y, unsigned int type) {
	// delete from active cells array if it is there
	if (cells[XY(x, y)].activeCellIndex != -1) {
		--activeCellCount;
		cells[activeCells[activeCellCount]].activeCellIndex = cells[XY(x, y)].activeCellIndex;
		activeCells[cells[XY(x, y)].activeCellIndex] = activeCells[activeCellCount];
		cells[XY(x, y)].activeCellIndex = -1;
	}

	// cell properties
	Property prop = properties[type];

	cells[XY(x, y)].velocityY = 0;
	cells[XY(x, y)].velocityX = 0;
	cells[XY(x, y)].temperature = prop.initTemperature;
	cells[XY(x, y)].type = type;
	cells[XY(x, y)].weight = prop.weight;
	cells[XY(x, y)].springiness = prop.springiness;
	cells[XY(x, y)].flags = 0;

	if (properties[type].active) {
		activeCells[activeCellCount] = XY(x, y);
		cells[XY(x, y)].activeCellIndex = activeCellCount;
		++activeCellCount;
	}

	if (properties[type].liquid) {
		SET_LIQUID(x, y);
		if (Random() < 0.5)
			TOGGLE_DIR(x, y);
	}

	if (properties[type].solid) {
		SET_SOLID(x, y);
	}
	
}

void transformCell(int x, int y, unsigned char type) {
	float velX = cells[XY(x, y)].velocityX;
	float velY = cells[XY(x, y)].velocityY;
	float temperature = cells[XY(x, y)].temperature;

	createCell(x, y, type);

	cells[XY(x, y)].velocityX = velX;
	cells[XY(x, y)].velocityY = velY;
	cells[XY(x, y)].temperature = temperature;
}

inline void destroyCell(int x, int y) {
	createCell(x, y, EMPTY);
}

// Bresenham's line algorithm for colision detection
bool checkWay(int& startX, int& startY, int& endX, int& endY) {
	int deltax = abs((int)(endX - startX));
	int deltay = abs((int)(endY - startY));
	int x = startX;
	int y = startY;
	int xinc1, xinc2, yinc1, yinc2;
	// Determine whether x and y is increasing or decreasing 
	if (endX >= startX) { // The x-values are increasing     
		xinc1 = 1;
		xinc2 = 1;
	}
	else { // The x-values are decreasing
		xinc1 = -1;
		xinc2 = -1;
	}
	if (endY >= startY) { // The y-values are increasing
		yinc1 = 1;
		yinc2 = 1;
	}
	else { // The y-values are decreasing
		yinc1 = -1;
		yinc2 = -1;
	}
	int den, num, numadd, numpixels;
	if (deltax >= deltay) { // There is at least one x-value for every y-value
		xinc1 = 0; // Don't change the x when numerator >= denominator
		yinc2 = 0; // Don't change the y for every iteration
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; // There are more x-values than y-values
	}
	else { // There is at least one y-value for every x-value
		xinc2 = 0;// Don't change the x for every iteration
		yinc1 = 0; // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; // There are more y-values than x-values
	}
	int prevX = startX;
	int prevY = startY;

	//zero iteration
	num += numadd;
	if (num >= den) {
		num -= den;
		x += xinc1;
		y += yinc1;
	}
	x += xinc2;
	y += yinc2;

	for (int curpixel = 1; curpixel <= numpixels; curpixel++) {
		if (IS_SOLID(x, y)) {
			endX = prevX;
			endY = prevY;
			startX = x;
			startY = y;
			return false;
		}

		prevX = x;
		prevY = y;

		num += numadd; // Increase the numerator by the top of the fraction

		if (num >= den) {  // Check if numerator >= denominator
			num -= den; // Calculate the new numerator value
			x += xinc1; // Change the x as appropriate
			y += yinc1; // Change the y as appropriate
		}

		x += xinc2; // Change the x as appropriate
		y += yinc2; // Change the y as appropriate
	}
	return true; // nothing was found
}