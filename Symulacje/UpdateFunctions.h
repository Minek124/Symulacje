#pragma once

void updateCell(int);

void burn(int x, int y) {
	if (TYPE(x, y - 1) == EMPTY) {
		createCell(x, y, FLAME);
	}
}

void updateSand(int x, int y) {
	int targetY = y + 1;
	if (IS_LIQUID(x, targetY)) {
		moveCell(x, y, x, targetY);
		return;
	}
	if (randomBool[randomBoolCount++] && IS_LIQUID(x + 1, targetY)) {
		cells[XY(x, y)].velX += SA;
		moveCell(x, y, x + 1, targetY);
		return;
	}
	if (IS_LIQUID(x - 1, targetY)) {
		cells[XY(x, y)].velX -= SA;
		moveCell(x, y, x - 1, targetY);
		return;
	}
	if (IS_LIQUID(x + 1, targetY)) {
		cells[XY(x, y)].velX += SA;
		moveCell(x, y, x + 1, targetY);
		return;

	}
	UNSET_MOVING(x, y);
	cells[XY(x, y)].velY = 0;
	cells[XY(x, y)].velX = 0;
}

void updateFluid(int x, int y) {
	char weight = properties[cells[XY(x, y)].type].weight;
	int dstY = y + 1;
	if (IS_LIQUID(x, dstY) && weight > properties[cells[XY(x, dstY)].type].weight) {
		moveCell(x, y, x, dstY);
		return;
	}
	if (IS_LIQUID(x + 1, dstY) && randomBool[randomBoolCount++] && weight > properties[cells[XY(x + 1, dstY)].type].weight) {
		RIGHT_DIR(x, y);
		cells[XY(x, y)].velX += WA;
		moveCell(x, y, x + 1, dstY);
		return;
	}
	if (IS_LIQUID(x - 1, dstY) && weight > properties[cells[XY(x - 1, dstY)].type].weight) {
		LEFT_DIR(x, y);
		cells[XY(x, y)].velX -= WA;
		moveCell(x, y, x - 1, dstY);
		return;
	}
	if (IS_LIQUID(x + 1, dstY) && weight > properties[cells[XY(x + 1, dstY)].type].weight) {
		RIGHT_DIR(x, y);
		cells[XY(x, y)].velX += WA;
		moveCell(x, y, x + 1, dstY);
		return;
	}

#ifdef ENABLE_WATER_HACK
	#define WR 10
	if (LIQUID_DIR(x, y)) {
		int i;
		char type = cells[XY(x, y)].type;
		for (i = 1; IS_LIQUID(x + i, y) && i < WR; ++i) {
			if (cells[XY(x + i, y)].type != type) {
				cells[XY(x, y)].velX += WA * i;
				moveCell(x, y, x + i, y);
				return;
			}
		}
		if (i < WR && IS_MOVING(x + i, y))
			cells[XY(x + i, y)].velX += WA * i;
		LEFT_DIR(x, y);
	}
	else {
		int i;
		char type = cells[XY(x, y)].type;
		for (i = 1; IS_LIQUID(x - i, y) && i < WR; ++i) {
			if (cells[XY(x - i, y)].type != type) {
				cells[XY(x, y)].velX -= WA * i;
				moveCell(x, y, x - i, y);
				return;
			}
		}
		if (i < WR && IS_MOVING(x - i, y))
			cells[XY(x - i, y)].velX -= WA * i;
		RIGHT_DIR(x, y);
	}

#else
	if (LIQUID_DIR(x, y)) {
		if (IS_LIQUID(x + 1, y) && weight > properties[cells[XY(x + 1, y)].type].weight) {
			cells[XY(x, y)].velX += WA;
			moveCell(x, y, x + 1, y);
			return;
		}
		else {
			if (!IS_UPDATED(x + 1, y) && IS_ACTIVE(x + 1, y)) {
				RIGHT_DIR(x + 1, y);
				updateCell(XY(x + 1, y));
				updateFluid(x, y);
				return;
			}
		}
		LEFT_DIR(x, y);
	}
	else {
		if (IS_LIQUID(x - 1, y) && weight > properties[cells[XY(x - 1, y)].type].weight) {
			cells[XY(x, y)].velX -= WA;
			moveCell(x, y, x - 1, y);
			return;
		}
		else {
			if (!IS_UPDATED(x - 1, y) && IS_ACTIVE(x - 1, y)) {
				LEFT_DIR(x - 1, y);
				updateCell(XY(x - 1, y));
				updateFluid(x, y);
				return;
			}
		}
		RIGHT_DIR(x, y);
	}
#endif

	UNSET_MOVING(x, y);
	cells[XY(x, y)].velY = 0;
	cells[XY(x, y)].velX = 0;
}

void updateFlame(int x, int y) {
	if (cells[XY(x, y)].other >= 20) {
		destroyCell(x, y);
		return;
	}
	cells[XY(x, y)].other += 1;
	if (TYPE(x, y - 1) == EMPTY) {
		cloneCell(x, y, x, y - 1);
	}
	else if (TYPE(x - 1, y - 1) == EMPTY) {
		cloneCell(x, y, x - 1, y - 1);
	}
	else if (TYPE(x + 1, y - 1) == EMPTY) {
		cloneCell(x, y, x + 1, y - 1);
	}
	else if (TYPE(x - 1, y) == EMPTY) {
		cloneCell(x, y, x - 1, y);
	}
	else if (TYPE(x + 1, y) == EMPTY) {
		cloneCell(x, y, x + 1, y);
	}
	int i = x / 10 + 1;
	int j = y / 10 + 1;
	v_prev[IX(i, j)] = -0.001f;
	dens_prev[IX(i, j)] += 0.01f;
}

void updateAir(int x, int y) {
	switch (rand() % 5) {
	case 0:
		if (IS_EMPTY(x, y - 1)) {
			moveCell(x, y, x, y - 1);
		}
		break;
	case 1:
		if (IS_EMPTY(x - 1, y - 1)) {
			moveCell(x, y, x - 1, y - 1);
		}
		break;
	case 2:
		if (IS_EMPTY(x + 1, y - 1)) {
			moveCell(x, y, x + 1, y - 1);
		}
		break;
	case 3:
		if (IS_EMPTY(x + 1, y)) {
			moveCell(x, y, x + 1, y);
		}
		break;
	case 4:
		if (IS_EMPTY(x - 1, y)) {
			moveCell(x, y, x - 1, y);
		}
		break;
	}
}

void attractAll(int x, int y) {
	

	int r = 50;
	float force = 200.0f;

	for (int i = x - r; i <= x + r; i++) {
		for (int j = y - r; j <= y + r; j++) {
			if (i != x && j != y) {
				float radius =(float) (sqrt(((x - i) * (x - i)) + ((y - j) * (y - j))));
				cells[XY(i, j)].velX += (force * (x - i) / abs((x - i))) / radius;
				cells[XY(i, j)].velY += (force * (y - j) / abs((y - j))) / radius;
			}
		}
	}

	cells[XY(x, y)].velX = 0;
	cells[XY(x, y)].velY = 0;
}