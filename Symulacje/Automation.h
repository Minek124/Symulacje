#pragma once

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

void burn(int x, int y) {
	if (TYPE(x, y - 1) == EMPTY) {
		createCell(x, y, FLAME);
	}
}

void updateSand(int x, int y) {
	int targetY = y + 1;
	//SET_MOVING(x, y);
	if (IS_LIQUID(x, targetY)) {
		moveCell(x, y, x, targetY);
		return;
	}
	if (randomBool[randomBoolCount++] && IS_LIQUID(x + 1, targetY)) {
		cells[XY(x, y)].velocityX += SA;
		if (randomBool[randomBoolCount++]) {
			moveCell(x, y, x + 1, targetY);
		}
		return;
	}
	if (IS_LIQUID(x - 1, targetY)) {
		cells[XY(x, y)].velocityX -= SA;
		if (randomBool[randomBoolCount++]) {
			moveCell(x, y, x - 1, targetY);
		}
		return;
	}
	if (IS_LIQUID(x + 1, targetY)) {
		cells[XY(x, y)].velocityX += SA;
		if (randomBool[randomBoolCount++]) {
			moveCell(x, y, x + 1, targetY);
		}
		return;

	}
	if (IS_SLEEPING1(x, y))
		SET_SLEEPING2(x, y);
	SET_SLEEPING1(x, y);
	UNSET_MOVING(x, y);
	cells[XY(x, y)].velocityY = 0;
	cells[XY(x, y)].velocityX = 0;
}

void updateFluid(int x, int y) {
	char weight = properties[cells[XY(x, y)].type].weight;
	int destinationY = y + 1;
	if (IS_LIQUID(x, destinationY) && weight > properties[cells[XY(x, destinationY)].type].weight) {
		moveCell(x, y, x, destinationY);
		return;
	}
	if (IS_LIQUID(x + 1, destinationY) && randomBool[randomBoolCount++] && weight > properties[cells[XY(x + 1, destinationY)].type].weight) {
		if (randomBool[randomBoolCount++]) {
			RIGHT_DIR(x, y);
			cells[XY(x, y)].velocityX += WA;
			moveCell(x, y, x + 1, destinationY);
			return;
		}
	}
	if (IS_LIQUID(x - 1, destinationY) && weight > properties[cells[XY(x - 1, destinationY)].type].weight) {
		if (randomBool[randomBoolCount++]) {
			LEFT_DIR(x, y);
			cells[XY(x, y)].velocityX -= WA;
			moveCell(x, y, x - 1, destinationY);
			return;
		}
	}
	if (IS_LIQUID(x + 1, destinationY) && weight > properties[cells[XY(x + 1, destinationY)].type].weight) {
		if (randomBool[randomBoolCount++]) {
			RIGHT_DIR(x, y);
			cells[XY(x, y)].velocityX += WA;
			moveCell(x, y, x + 1, destinationY);
			return;
		}
	}
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
	}

	if (IS_SLEEPING1(x, y)) {
		SET_SLEEPING2(x, y);
	}
	SET_SLEEPING1(x, y);
	UNSET_MOVING(x, y);
	//cells[XY(x, y)].velocityY = 0;
	//cells[XY(x, y)].velocityX = 0;
}

void updateAir(int x, int y) {
	SET_MOVING(x, y);
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
	UNSET_MOVING(x, y);
}