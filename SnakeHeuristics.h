#pragma once

#include "GameState.h"

class GameState;

namespace SnakeHeuristics
{
	int calcHeuristic(const GameState &gs);

	int hungry(const GameState& gs);

	int opertunisticEat(const GameState &gs);

	int relativeAreas(const GameState& gs);

	int ourArea(const GameState& gs);

	int enemyDeaths(const GameState& gs);

	int gameLoss(const GameState& gs);

	int gameWin(const GameState& gs);
}

