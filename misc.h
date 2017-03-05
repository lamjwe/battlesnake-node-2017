#pragma once

#include <stdint.h>
#include "misc.h"
#include <limits.h>

#define MAX_X 40
#define MAX_Y 40
#define MAX_SNAKES 10

enum Direction { NONE=0, UP, DOWN, RIGHT, LEFT, DIE };
const int8_t directionsX[] = { -1,0,1,0 };
const int8_t directionsY[] = { 0,1,0,-1 };

struct GridPoint {
public:
	GridPoint() : x(0), y(0) {}
	GridPoint(uint8_t newX, uint8_t newY) : x(newX), y(newY) {}
	uint8_t x = 0;
	uint8_t y = 0;
};