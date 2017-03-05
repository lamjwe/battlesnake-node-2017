#pragma once

#include "misc.h"
#include <vector>

enum Direction;
struct GridPoint;

class Snake
{
public:
	Snake();
	~Snake();

	void moveSnake(Direction dir) {};

	int m_id;
	int m_health;
	bool m_isAlive;
	std::vector<GridPoint> m_snake;
	std::vector<int> m_ttl;

};

