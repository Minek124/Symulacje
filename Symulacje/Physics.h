#pragma once
#include <math.h>

bool updatePhysics(int srcX, int srcY) {
	int obstacleX = srcX;
	int obstacleY = srcY;
	int dstX = (int)(cells[XY(srcX, srcY)].velocityX) + srcX;
	int dstY = (int)(cells[XY(srcX, srcY)].velocityY) + srcY;

	SET_MOVING(srcX, srcY);

	// check for colision between source and destination position
	if (checkWay(obstacleX, obstacleY, dstX, dstY)) {
		moveCell(srcX, srcY, dstX, dstY);
	}
	else {
		if (IS_MOVING(obstacleX, obstacleY)) {  // colision in fly

			float av = (cells[XY(srcX, srcY)].velocityX + cells[XY(obstacleX, obstacleY)].velocityX) / 2;
			cells[XY(srcX, srcY)].velocityX = av;
			cells[XY(obstacleX, obstacleY)].velocityX = av;
			av = (cells[XY(srcX, srcY)].velocityY + cells[XY(obstacleX, obstacleY)].velocityY) / 2;
			cells[XY(srcX, srcY)].velocityY = av;
			cells[XY(obstacleX, obstacleY)].velocityY = av;

			//float angle = Random() * 3.14 - 1.5708;
			//cells[XY(srcX,srcY)].velocityX = (cells[XY(srcX,srcY)].velocityX * Cos(angle)) - (cells[XY(srcX,srcY)].velocityY * Sin(angle));
			//cells[XY(srcX,srcY)].velocityY = (cells[XY(srcX,srcY)].velocityX * Sin(angle)) + (cells[XY(srcX,srcY)].velocityY * Cos(angle));

			//cells[XY(obstacleX, obstacleY)].velocityX = (cells[XY(obstacleX, obstacleY)].velocityX * Cos(angle)) - (cells[XY(obstacleX, obstacleY)].velocityY * Sin(angle));
			//cells[XY(obstacleX, obstacleY)].velocityY = (cells[XY(obstacleX, obstacleY)].velocityX * Sin(angle)) + (cells[XY(obstacleX, obstacleY)].velocityY * Cos(angle));


			if (srcX != dstX || srcY != dstY) {
				moveCell(srcX, srcY, dstX, dstY);
			}
			else {
				if ( processed[XY(obstacleX, obstacleY)] || (cells[XY(obstacleX, obstacleY)].activeCellIndex == -1) ) {
					processed[XY(srcX, srcY)] = true;
					return true;
				}
				return false;
			}
		}
		else {
			if (IS_LIQUID(obstacleX, obstacleY) && !IS_MOVING(obstacleX, obstacleY)) {
				UNSET_MOVING(srcX, srcY);
				cells[XY(srcX, srcY)].velocityY = 0;
				cells[XY(srcX, srcY)].velocityX = 0;
				moveCell(srcX, srcY, dstX, dstY);
			}
			else {  // reflection
				float normalX = 0;
				float normalY = 0;
				for (int i = -3; i <= 3; i++) {
					for (int j = -3; j <= 3; j++) {
						if (TYPE(obstacleX + i, obstacleY + j) != EMPTY) {
							normalX -= i;
							normalY -= j;
						}
					}
				}
				float length = sqrt((normalX * normalX) + (normalY * normalY));
				if (length != 0) {
					normalX /= length;
					normalY /= length;
				}
				float projection = (cells[XY(srcX, srcY)].velocityX * normalX + cells[XY(srcX, srcY)].velocityY * normalY) * 2;

				// apply reflected velocity
				cells[XY(srcX, srcY)].velocityX -= projection * normalX;
				cells[XY(srcX, srcY)].velocityY -= projection * normalY;

				//cells[XY(obstacleX, obstacleY)].velocityX += ((100 - cells[XY(srcX,srcY)].springiness) / 100.0f) * cells[XY(srcX,srcY)].velocityX;
				//cells[XY(obstacleX, obstacleY)].velocityY += ((100 - cells[XY(srcX,srcY)].springiness) / 100.0f) * cells[XY(srcX,srcY)].velocityY;

				// apply friction
				cells[XY(srcX, srcY)].velocityX *= (properties[cells[XY(srcX, srcY)].type].springiness / 100.0f);
				cells[XY(srcX, srcY)].velocityY *= (properties[cells[XY(srcX, srcY)].type].springiness / 100.0f);

				if (srcX != dstX || srcY != dstY) {
					moveCell(srcX, srcY, dstX, dstY);
				}
				else {
					if (processed[XY(obstacleX, obstacleY)] || cells[XY(obstacleX, obstacleY)].activeCellIndex == -1) {
						UNSET_MOVING(srcX, srcY);
						processed[XY(srcX, srcY)] = true;
						return true;
					}
					return false;
				}
			}
		}
	}
	return true;
}