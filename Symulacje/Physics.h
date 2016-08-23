#pragma once

void updatePhysics(int srcX, int srcY) {
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
			
			if (!IS_UPDATED(obstacleX, obstacleY) && IS_ACTIVE(obstacleX, obstacleY)) {
				updateCell(XY(obstacleX, obstacleY));
				updatePhysics(srcX, srcY);
				return;
			}

			float av = (cells[XY(srcX, srcY)].velocityX + cells[XY(obstacleX, obstacleY)].velocityX) / 2;
			cells[XY(srcX, srcY)].velocityX = av;
			cells[XY(obstacleX, obstacleY)].velocityX = av;
			av = (cells[XY(srcX, srcY)].velocityY + cells[XY(obstacleX, obstacleY)].velocityY) / 2;
			cells[XY(srcX, srcY)].velocityY = av;
			cells[XY(obstacleX, obstacleY)].velocityY = av;
			
			if (srcX != dstX || srcY != dstY) 
				moveCell(srcX, srcY, dstX, dstY);
		}
		else {
			// Water colision
			if (IS_LIQUID(obstacleX, obstacleY)) {
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
						if (IS_SOLID(obstacleX + i, obstacleY + j)) {
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

				// apply friction
				cells[XY(srcX, srcY)].velocityX *= (cells[XY(srcX, srcY)].springiness / 100.0f);
				cells[XY(srcX, srcY)].velocityY *= (cells[XY(srcX, srcY)].springiness / 100.0f);

				if (srcX != dstX || srcY != dstY) {
					moveCell(srcX, srcY, dstX, dstY);
					if ((dstY - srcY) >= 0)
						properties[cells[XY(dstX, dstY)].type].updateFunction(dstX, dstY);
				}
				else
					UNSET_MOVING(srcX, srcY);
			}
		}
	}
}