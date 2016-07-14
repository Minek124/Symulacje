#include <iostream>
#include <windows.h>
#include <GL/glut.h>
#include <Math.h>
#include <random>
#include <string>
#include "picopng.h"

#include "Config.h"
#include "Globals.h"
#include "Tools.h"
#include "Automation.h"
#include "Fluids.h"
#include "Physics.h"

#include <time.h>


using namespace std;

void preUpdate(int x, int y) {
	float temp = cells[XY(x, y)].temperature / 100;
	cells[XY(x, y)].temperature -= 20 * temp;

	cells[XY(x, y - 1)].temperature += 5 * temp;
	cells[XY(x - 1, y)].temperature += 5 * temp;
	cells[XY(x + 1, y)].temperature += 5 * temp;
	cells[XY(x, y + 1)].temperature += 5 * temp;

	UNSET_UPDATED(x, y);

}

void updateCell(int pos) {
	//if (cells[pos].flags & (1 << (1))) {  // don't update if sleeping
	//	processed[pos] = true;
	//	return;
	//}

	if ( (cells[pos].flags & (1 << (6))) )
		return;

	cells[pos].flags |= (1 << 6);

	Property prop = properties[cells[pos].type];
	int x = pos / mapSizeY;
	int y = pos % mapSizeY;

	
	// air movements
	int i = x / 10 + 1;
	int j = y / 10 + 1;
	if (prop.weight != 0) {
		cells[pos].velocityX += (u[IX(i, j)]) * 100 / prop.weight;
		cells[pos].velocityY += (v[IX(i, j)]) * 100 / prop.weight;
	}
	else {
		cells[pos].velocityX += (u[IX(i, j)]) * 100;
		cells[pos].velocityY += (v[IX(i, j)]) * 100;
	}
	//gravity
	cells[pos].velocityY += G * prop.weight;


	// high temperature transform
	if (cells[pos].temperature >= prop.highTemeperature) {
		transformCell(x, y, prop.highTemperatureTransform);
		return;
	}
	// low temperature transform
	if (cells[pos].temperature <= prop.lowTemeperature) {
		transformCell(x, y, prop.lowTemperatureTransform);
		return;
	}

	prop.specialBehaviorFunction(x, y);

	if (abs(cells[pos].velocityX) < 1 && abs(cells[pos].velocityY) < 1) {
		prop.updateFunction(x, y);
	}
	else {
		updatePhysics(x, y);
	}
}

void mainLoop() {
	if (paused)
		return;

	for (int i = 1; i < mapSizeX - 1; ++i)
		for (int j = 1; j < mapSizeY - 1; ++j)
			preUpdate(i, j);

	for (int i = 0; i < activeCellCount; i += 2)
		updateCell(activeCells[i]);

	for (int i = 1; i < activeCellCount; i += 2)
		updateCell(activeCells[i]);

	
	vel_step(u, v, u_prev, v_prev, 0.0f, 0.4f);
	dens_step(dens, dens_prev, u, v, 0.0f, 0.4f);

	memset(u_prev, 0, fluidBoxArraySize*sizeof(float));
	memset(v_prev, 0, fluidBoxArraySize*sizeof(float));
	memset(dens_prev, 0, fluidBoxArraySize*sizeof(float));

	if (!showDensity)
		updatePixelMap();
	else
		updatePixelMap2();

	glutPostRedisplay();
}

void initSimulation(char *path) {
	//load and decode image
	std::vector<unsigned char> buffer, image;
	loadFile(buffer, path);
	unsigned long w, h;
	int error = decodePNG(image, w, h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());
	if (error != 0) std::cout << "error: " << error << std::endl;
	if (image.size() > 4) std::cout << "width: " << w << " height: " << h << std::endl;

	mapSizeX = w;
	mapSizeY = h;

	pixels = new unsigned int[mapSizeY*mapSizeX]; // delete
	activeCells = new int[mapSizeY*mapSizeX]; //delete
	cells = new Cell[mapSizeY*mapSizeX]; //delete

	// test
	//fluidX = new float[mapSizeY*mapSizeX];
	//fluidY = new float[mapSizeY*mapSizeX];

	//fluids
	fluidBoxX = mapSizeX / FLUID_BOX_SCALE;
	fluidBoxY = mapSizeY / FLUID_BOX_SCALE;
	fluidBoxArraySize = (fluidBoxY + 2)*(fluidBoxX + 2);
	u = new float[fluidBoxArraySize];
	v = new float[fluidBoxArraySize];
	u_prev = new float[fluidBoxArraySize];
	v_prev = new float[fluidBoxArraySize];
	dens = new float[fluidBoxArraySize];
	dens_prev = new float[fluidBoxArraySize];
	clearFluidBox();

	activeCellCount = 0;

	timer = clock();
	fps = 0;

	for (int i = 0; i < 256; ++i) {
		randomBool[i] = (Random() < 0.5 ? true : false);
	}

	for (int y = 0; y < mapSizeY; y++) {
		for (int x = 0; x < mapSizeX; x++) {
			int xy = (y * mapSizeX * 4) + (x * 4);
			unsigned char r = image[xy];
			unsigned char g = image[xy + 1];
			unsigned char b = image[xy + 2];
			createCell(x, y, colorToType(r, g, b));
		}
	}

	updatePixelMap();
}

void display() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(mapSizeX, mapSizeY, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	++fps;
	double stop = (clock() - timer)*1.0 / CLOCKS_PER_SEC;
	if (stop > 1.0) {
		if (logging)
			cout << "fps:" << fps << endl;
		timer = clock();
		fps = 0;
	}

	glutSwapBuffers();
}

/* Callback handler for normal-key event */
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 8:     // backspace
		hardSpawn = !hardSpawn;
		break;
	case 27:     // ESC key
		exit(0);
		break;
	case 61:     // + =
		if (spawnRadius < 100) {
			spawnRadius += 5;
		}
		break;
	case 45:     // -
		if (spawnRadius) {
			spawnRadius -= 5;
		}
		break;
	case 48:     // 0
		spawnType = EMPTY;
		break;
	case 49:     // 1
		spawnType = WALL;
		break;
	case 50:     // 2
		spawnType = SAND;
		break;
	case 51:     // 3
		spawnType = WATER;
		break;
	case 52:     // 4
		spawnType = OLIVE;
		break;
	case 53:     // 5
		spawnType = FLAME;
		break;
	case 54:     // 6
		spawnType = RUBBER;
		break;
	case 55:     // 7
		spawnType = STEAM;
		break;
	case 56:     // 8
		spawnType = WALL;
		break;
	case 57:     // 9
		spawnType = WALL;
		break;
	case 98:     // b
		for (int i = x - spawnRadius; i <= x + spawnRadius; ++i) {
			for (int j = y - spawnRadius; j <= y + spawnRadius; ++j) {
				if (i >= 0 && i < mapSizeX && j >= 0 && j < mapSizeY) {
					if ((((j - y)*(j - y)) + ((i - x)*(i - x))) <= (spawnRadius*spawnRadius) && TYPE(i, j) != WALL) {
						cells[XY(i, j)].velocityX = (float)((i - x) * 100);
						cells[XY(i, j)].velocityY = (float)((j - y) * 100);
						SET_MOVING(i, j);
						wakeNearbyCells(i, j);
					}
				}
			}
		}
		break;
	case 99:     // c
		printCell(x, y);
		break;
	case 101: //  e
		for (int i = x - spawnRadius; i <= x + spawnRadius; ++i) {
			for (int j = y - spawnRadius; j <= y + spawnRadius; ++j) {
				if (i >= 0 && i < mapSizeX && j >= 0 && j < mapSizeY) {
					if ((((j - y)*(j - y)) + ((i - x)*(i - x))) <= (spawnRadius*spawnRadius) && (TYPE(i, j) == EMPTY || hardSpawn)) {
						if (Random() < 0.5) {
							createCell(i, j, spawnType);
							cells[XY(i, j)].velocityX = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
							cells[XY(i, j)].velocityY = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
						}
					}
				}
			}
		}
	break;
	case 102: //  f
	{
		float force = 3.0f;
		int i = (int)(x / FLUID_BOX_SCALE);
		int j = (int)(y / FLUID_BOX_SCALE);
		u_prev[IX(i, j)] = force * (x - ox);
		v_prev[IX(i, j)] = -force * (oy - y);
		ox = x;
		oy = y;
	}
	break;
	case 103: // g
	{
		float force = 5.0f;
		int i = (int)(x / FLUID_BOX_SCALE);
		int j = (int)(y / FLUID_BOX_SCALE);
		dens_prev[IX(i, j)] = 100.0f;
		ox = x;
		oy = y;
	}
	break;
	case 104: // h
	{
		int i = (int)(x / FLUID_BOX_SCALE);
		int j = (int)(y / FLUID_BOX_SCALE);
		float force = 100.0f;
		u_prev[IX(i - 1, j)] = -force;
		u_prev[IX(i + 1, j)] = force;
		v_prev[IX(i, j - 1)] = -force;
		v_prev[IX(i, j + 1)] = force;

		u_prev[IX(i - 1, j - 1)] = -force;
		v_prev[IX(i - 1, j - 1)] = -force;

		u_prev[IX(i + 1, j + 1)] = force;
		v_prev[IX(i + 1, j + 1)] = force;

		u_prev[IX(i - 1, j + 1)] = -force;
		v_prev[IX(i - 1, j + 1)] = force;

		u_prev[IX(i + 1, j - 1)] = force;
		v_prev[IX(i + 1, j - 1)] = -force;
	}
	break;
	case 108:     // l
		logging = !logging;
		break;
	case 110:     // n
		showDensity = !showDensity;
		break;
	case 112:     // p
		paused = !paused;
		break;
	case 113:     // q
		paused = !paused;
		break;
	case 114:     // r

		break;
	case 118:     // v
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY)
			createCell(x, y, spawnType);
		break;
	}
}

/* Callback handler for mouse event */
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = x - spawnRadius; i <= x + spawnRadius; ++i) {
			for (int j = y - spawnRadius; j <= y + spawnRadius; ++j) {
				if (i >= 0 && i < mapSizeX && j >= 0 && j < mapSizeY) {
					if ((((j - y)*(j - y)) + ((i - x)*(i - x))) <= (spawnRadius*spawnRadius) && (TYPE(i, j) == EMPTY || hardSpawn)) {
						if (Random() < 0.5) {
							createCell(i, j, spawnType);
							cells[XY(i, j)].velocityX = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
							cells[XY(i, j)].velocityY = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
						}
					}
				}
			}
		}
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		int r = 40;
		for (int i = x - r; i < x + r; ++i) {
			for (int j = y - r; j < y + r; ++j) {
				if (j >= 0 && j < mapSizeY && i >= 0 && i < mapSizeX) {
					if (TYPE(i, j) == EMPTY || hardSpawn) {
						if (Random() < 0.5) {
							createCell(i, j, spawnType);
						}
					}
				}
			}
		}
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		int r = 40;
		for (int i = y + r; i > y - r; --i) {
			for (int j = x - r; j < x + r; ++j) {
				if (i >= 0 && i < mapSizeY && j >= 0 && j < mapSizeX) {
					if (TYPE(j, i) == EMPTY || hardSpawn) {
						if (Random() < 0.5) {
							createCell(j, i, spawnType);
						}
					}
				}
			}
		}

	}
}

int main(int argc, char** argv) {

	initSimulation(MAP);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(mapSizeX, mapSizeY);
	glutInitWindowPosition(500, 70);
	glutCreateWindow("Symulacje");


	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(mainLoop);

	// upside down fix
	glViewport(0, 0, mapSizeX, mapSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, mapSizeX, 0, mapSizeY, 0.1, 1);
	glPixelZoom(1, -1);
	glRasterPos3f(0, (GLfloat)(mapSizeY - 1), -0.3f);

	glutMainLoop();

	return 0;
}