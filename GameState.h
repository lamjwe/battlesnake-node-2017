#pragma once


#include <vector>
#include "Snake.h"
#include <string>
#include "misc.h"
//#include "SnakeHeuristics.h"
#include <iostream>

class GameState
{
public:
	GameState();
	static GameState *buildFromCin(std::string);
	GameState(const GameState &gs);
	~GameState();

	std::vector< std::pair<GameState, Direction>> * getMoves(const bool ourSnake);
	int calculateValue();
	bool gameOver();

	int calcAccessibleArea(int snake, int turnFudgeFactor);
	int distanceToClosestFood(int snake);
	int enemiesDiedThisTurn();
	int areWeDead();
	int numSnakes() { return m_numSnakes; }
	const Snake getSnake(int i) { return m_snakes[i]; }

	void printDijkstra(int snake);
	void printTTL();
	void printMoves(int snake);

	void printVoronoi();

	//void addSnakeTest(int snake);


private:
	int m_xSize = MAX_X;
	int m_ySize = MAX_Y;
	const GameState *m_previousState;

	int m_numSnakes = MAX_SNAKES;
	std::vector<Snake> m_snakes = std::vector<Snake>(MAX_SNAKES);
	int m_ttl[MAX_X][MAX_Y];

	std::vector<GridPoint> m_food;

	int m_costs[MAX_X][MAX_Y];
	GridPoint m_dijkstraPrev[MAX_SNAKES][MAX_X][MAX_Y];
	int m_dijkstraNumMoves[MAX_SNAKES][MAX_X][MAX_Y];
	int m_dijkstraCosts[MAX_SNAKES][MAX_X][MAX_Y];

	int m_snakedDied;

	void runDijkstra(const int snake);

	bool partOfVoronoi(const int snake,const int turnFudgeFactor, const int x,  const int y);

	bool checkIfKilled(const int snake);
	void updateFood(Snake &snake);
	void updateSnakes();
	void moveSnakes(std::vector<Direction> moves);

	std::vector<Direction> checkDirections(const int snakeNum);
	std::vector< std::vector<Direction>> pickMoves(const int snake);	
};
