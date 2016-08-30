#pragma once

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
		cells[XY(x, y)].velocityX += SA;
		moveCell(x, y, x + 1, targetY);
		return;
	}
	if (IS_LIQUID(x - 1, targetY)) {
		cells[XY(x, y)].velocityX -= SA;
		moveCell(x, y, x - 1, targetY);
		return;
	}
	if (IS_LIQUID(x + 1, targetY)) {
		cells[XY(x, y)].velocityX += SA;
		moveCell(x, y, x + 1, targetY);
		return;

	}
	UNSET_MOVING(x, y);
	cells[XY(x, y)].velocityY = 0;
	cells[XY(x, y)].velocityX = 0;
}

void updateFluid(int x, int y) {
	char weight = cells[XY(x, y)].weight;
	int dstY = y + 1;
	if (IS_LIQUID(x, dstY) && weight > cells[XY(x, dstY)].weight) {
		moveCell(x, y, x, dstY);
		return;
	}
	if (IS_LIQUID(x + 1, dstY) && randomBool[randomBoolCount++] && weight > cells[XY(x + 1, dstY)].weight) {
		RIGHT_DIR(x, y);
		cells[XY(x, y)].velocityX += WA;
		moveCell(x, y, x + 1, dstY);
		return;
	}
	if (IS_LIQUID(x - 1, dstY) && weight > cells[XY(x - 1, dstY)].weight) {
		LEFT_DIR(x, y);
		cells[XY(x, y)].velocityX -= WA;
		moveCell(x, y, x - 1, dstY);
		return;
	}
	if (IS_LIQUID(x + 1, dstY) && weight > cells[XY(x + 1, dstY)].weight) {
		RIGHT_DIR(x, y);
		cells[XY(x, y)].velocityX += WA;
		moveCell(x, y, x + 1, dstY);
		return;
	}

	// old water hack
	/*
	#define WR 10
	if (LIQUID_DIR(x, y)) {
		int i;
		char type = cells[XY(x, y)].type;
		for (i = 1; IS_LIQUID(x + i, y) && i < WR; ++i) {
			if (cells[XY(x + i, y)].type != type) {
				cells[XY(x, y)].velocityX += WA * i;
				moveCell(x, y, x + i, y);
				return;
			}
		}
		if (i < WR && IS_MOVING(x + i, y))
			cells[XY(x + i, y)].velocityX += WA * i;
		LEFT_DIR(x, y);
	}
	else {
		int i;
		char type = cells[XY(x, y)].type;
		for (i = 1; IS_LIQUID(x - i, y) && i < WR; ++i) {
			if (cells[XY(x - i, y)].type != type) {
				cells[XY(x, y)].velocityX -= WA * i;
				moveCell(x, y, x - i, y);
				return;
			}
		}
		if (i < WR && IS_MOVING(x - i, y))
			cells[XY(x - i, y)].velocityX -= WA * i;
		RIGHT_DIR(x, y);
	}*/
	
	if (LIQUID_DIR(x, y)) {
		if (IS_LIQUID(x + 1, y) && weight > cells[XY(x + 1, y)].weight) {
			cells[XY(x, y)].velocityX += WA;
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
		if (IS_LIQUID(x - 1, y) && weight > cells[XY(x - 1, y)].weight) {
			cells[XY(x, y)].velocityX -= WA;
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

	UNSET_MOVING(x, y);
	cells[XY(x, y)].velocityY = 0;
	cells[XY(x, y)].velocityX = 0;
}

#define FFF 50
void updateFlame(int x, int y) {
	if (TYPE(x, y - 1) == EMPTY) {
		cells[XY(x, y)].temperature -= FFF;
		cloneCell(x, y, x, y - 1);
	}
	else if (TYPE(x - 1, y - 1) == EMPTY) {
		cells[XY(x, y)].temperature -= FFF;
		cloneCell(x, y, x - 1, y - 1);
	}
	else if (TYPE(x + 1, y - 1) == EMPTY) {
		cells[XY(x, y)].temperature -= FFF;
		cloneCell(x, y, x + 1, y - 1);
	}
	else if (TYPE(x - 1, y) == EMPTY) {
		cells[XY(x, y)].temperature -= FFF;
		cloneCell(x, y, x - 1, y);
	}
	else if (TYPE(x + 1, y) == EMPTY) {
		cells[XY(x, y)].temperature -= FFF;
		cloneCell(x, y, x + 1, y);
	}
	int i = x / 10 + 1;
	int j = y / 10 + 1;
	v_prev[IX(i, j)] = -0.005f;
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