#include <iostream>
#include <GL/glut.h>
#include <string>
#include <string.h>
#include <time.h>
#include <cmath>
#include <climits>

#include "Config.h"
#include "Globals.h"
#include "Tools.h"
#include "UpdateFunctions.h"
#include "Properties.h"
#include "Fluids.h"
#include "Physics.h"

using namespace std;

inline void preUpdate(int x, int y) {
	
#ifdef ENABLE_HEAT
	cells[XY(x, y)].temperature = (cells[XY(x, y - 1)].temperature + cells[XY(x - 1, y)].temperature + cells[XY(x + 1, y)].temperature + cells[XY(x, y + 1)].temperature) * 0.25f;
#endif

	UNSET_UPDATED(x, y);
}

void updateCell(int pos) {
	if ( (cells[pos].flags & (1 << (6))) ) // already updated
		return;
	cells[pos].flags |= (1 << 6); // set cell updated

	Property prop = properties[cells[pos].type];
	int x = pos / mapSizeY;
	int y = pos % mapSizeY;

	// high temperature transform
	if (cells[pos].temperature > prop.highTemeperature) {
		transformCell(x, y, prop.highTemperatureTransform);
		prop = properties[cells[pos].type];
	}
	// low temperature transform
	if (cells[pos].temperature < prop.lowTemeperature) {
		transformCell(x, y, prop.lowTemperatureTransform);
		prop = properties[cells[pos].type];
	}
	
	// air movements
	int i = x / 10 + 1;
	int j = y / 10 + 1;
	
	cells[pos].velX += (u[IX(i, j)]) * 100 / (prop.weight + 1);
	cells[pos].velY += (v[IX(i, j)]) * 100 / (prop.weight + 1);

	//gravity
	cells[pos].velY += G * prop.weight;

	prop.specialBehaviorFunction(x, y);

	if (abs(cells[pos].velX) < 1 && abs(cells[pos].velY) < 1) {
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

#ifdef ENABLE_FLUIDS
	vel_step(u, v, u_prev, v_prev, 0.0f, 0.4f);
	dens_step(dens, dens_prev, u, v, 0.0f, 0.4f);
	memset(u_prev, 0, fluidBoxArraySize*sizeof(float));
	memset(v_prev, 0, fluidBoxArraySize*sizeof(float));
	memset(dens_prev, 0, fluidBoxArraySize*sizeof(float));
#endif

	if (!showDensity)
		updatePixelMap();
	else
		updatePixelMap2();

	glutPostRedisplay();
}

void initSimulation(char *path) {
	//load and decode image
	loadProperties();
	
	mapSizeX = 600;
	mapSizeY = 600;

	pixels = new unsigned int[mapSizeY*mapSizeX]; // delete
	activeCells = new int[mapSizeY*mapSizeX]; //delete
	cells = new Cell[mapSizeY*mapSizeX]; //delete

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
	fluidObstacles = new bool[fluidBoxArraySize];
	clearFluidBox();

	activeCellCount = 0;

	timer = clock();
	fps = 0;

	for (int i = 0; i < 256; ++i) {
		randomBool[i] = (Random() < 0.5 ? true : false);
	}

	for (int i = 0; i < mapSizeX; i++) {
		for (int j = 0; j < mapSizeY; j++) {
			if (i < 4 || j < 4 || i > mapSizeX - 5 || j > mapSizeY - 5) {
				createCell(i, j, WALL);
			}
			else {
				createCell(i, j, EMPTY);
			}
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
		spawnType = POWDER;
		break;
	case 56:     // 8
		spawnType = STEAM;
		break;
	case 57:     // 9
		spawnType = ICE;
		break;
	case 97:     // a
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY) {
			createCell(x, y, spawnType);
			cells[XY(x, y)].velX = -10;
		}
		break;
	case 98:     // b
		for (int i = x - spawnRadius; i <= x + spawnRadius; ++i) {
			for (int j = y - spawnRadius; j <= y + spawnRadius; ++j) {
				if (i >= 0 && i < mapSizeX && j >= 0 && j < mapSizeY) {
					if ((((j - y)*(j - y)) + ((i - x)*(i - x))) <= (spawnRadius*spawnRadius) && TYPE(i, j) != WALL) {
						cells[XY(i, j)].velX = (float)((i - x) * 1);
						cells[XY(i, j)].velY = (float)((j - y) * 1);
						SET_MOVING(i, j);
					}
				}
			}
		}
		break;
	case 99:     // c
		printCell(x, y);
		break;
	case 100:     // d
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY) {
			createCell(x, y, spawnType);
			cells[XY(x, y)].velX = 10;
		}
		break;
	case 101: //  e
		for (int i = x - spawnRadius; i <= x + spawnRadius; ++i) {
			for (int j = y - spawnRadius; j <= y + spawnRadius; ++j) {
				if (i >= 0 && i < mapSizeX && j >= 0 && j < mapSizeY) {
					if ((((j - y)*(j - y)) + ((i - x)*(i - x))) <= (spawnRadius*spawnRadius) && (TYPE(i, j) == EMPTY || hardSpawn)) {
						createCell(i, j, spawnType);
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
		for (int i = 15; i < 35; i++) {
			for (int j = 0; j < 50; j++) {
				createCell(200 + i, 50 +  j, SAND);
			}
		}
		
		for (int i = 0; i < 50; i++) {
			createCell(200 + i, 100, WALL);
		}
		break;
	case 114:     // r

		break;
	case 115:     // s
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY) {
			createCell(x, y, spawnType);
			cells[XY(x, y)].velY = 10;
		}
		break;
	case 118:     // v
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY)
			createCell(x, y, spawnType);
		break;
	
	case 119:     // w
		if (x >= 0 && x < mapSizeX && y >= 0 && y < mapSizeY) {
			createCell(x, y, spawnType);
			cells[XY(x, y)].velY = -10;
		}
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
							cells[XY(i, j)].velX = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
							cells[XY(i, j)].velY = (Random() < 0.5 ? (Random() * 5.0f + 1.0f) : (-Random() * 5.0f - 1.0f));
						}
					}
				}
			}
		}
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
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

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
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
}

int main(int argc, char** argv) {

	initSimulation("mapa.png");

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