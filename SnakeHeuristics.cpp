#pragma once

#include "GameState.h"
#include "SnakeHeuristics.h"
#include <iostream>

namespace SnakeHeuristics
{
	//	SnakeHeuristics();
	//	~SnakeHeuristics();

	int calcHeuristic(GameState &gs) {
		int hungry = SnakeHeuristics::hungry(gs);
		int operEat = SnakeHeuristics::opertunisticEat(gs);
		int relArea = SnakeHeuristics::relativeAreas(gs);
		int ourArea = SnakeHeuristics::ourArea(gs);
		int kills = SnakeHeuristics::enemyDeaths(gs);
		int win = SnakeHeuristics::gameLoss(gs);
		int loss = SnakeHeuristics::gameWin(gs);

		std::cout << hungry << ", " << operEat << ", " << relArea << ", " << ourArea << ", " << kills << ", " << win << ", " << loss << std::endl;
		int value = hungry + operEat + relArea + ourArea + kills + win + loss;

		return value;
	}

	int hungry(GameState& gs) {
		Snake us = gs.getSnake(0);
		int distanceToFood = gs.distanceToClosestFood(0);
		int foodDelta = us.m_health - distanceToFood;
		if (foodDelta > 20) {
			return 0;
		}
		else {
			return foodDelta * (foodDelta < 5 ? 100 : 20);
		}
	}

	int opertunisticEat(GameState &gs) {
		return (gs.distanceToClosestFood(0) < 2) ? 200 : 0;
	}

	int relativeAreas(GameState& gs)
	{
		int ourArea = gs.calcAccessibleArea(0, 0);
		double averageEnemyArea = 0;
		for (int i = 1; i < gs.numSnakes(); i++) {
			averageEnemyArea += gs.calcAccessibleArea(i, 0);
		}
		averageEnemyArea /= (gs.numSnakes());
		std::cout << "Ours:" << ourArea << " Num Enemies:" << gs.numSnakes() << "Total area:" << averageEnemyArea*gs.numSnakes() << " Average area:" << averageEnemyArea << std::endl;
		return (int)(ourArea - averageEnemyArea) * 100;
	}

	int ourArea(GameState& gs) {
		return gs.calcAccessibleArea(0, 0) * 5;
	}

	int enemyDeaths(GameState& gs)
	{
		return gs.enemiesDiedThisTurn() * 100;
	}

	int gameLoss(GameState& gs)
	{
		return gs.areWeDead() ? -1000000 : 0;
	}

	int gameWin(GameState& gs)
	{
		return (!(gs.areWeDead()) && gs.numSnakes() == 0) ? 1000000 : 0;
	}
}

