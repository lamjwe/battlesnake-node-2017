#include "GameState.h"

#include <queue>
#include <algorithm>
#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <unordered_set>
#include <string>
#include <sstream>
#include "misc.h"
#include <sstream>


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

int calcHeuristic(GameState &gs) {
	int hungryVal = hungry(gs);
	int operEat = opertunisticEat(gs);
	int relArea = relativeAreas(gs);
	int ourAreaVal = ourArea(gs);
	int kills = enemyDeaths(gs);
	int win = gameLoss(gs);
	int loss = gameWin(gs);

	std::cout << hungryVal << ", " << operEat << ", " << relArea << ", " << ourAreaVal << ", " << kills << ", " << win << ", " << loss << std::endl;
	int value = hungryVal + operEat + relArea + ourAreaVal + kills + win + loss;

	return value;
}

GameState::GameState() :
	m_previousState(NULL),
	m_xSize(0),
	m_ySize(0),
	m_numSnakes(0),
	m_snakedDied(0)
{
}

//Will deep copy the game state, but will not update pathfindig etc.
GameState::GameState(const GameState &gs) :
m_previousState(&gs),
m_xSize(gs.m_xSize),
m_ySize(gs.m_xSize),
m_numSnakes(gs.m_numSnakes),
m_food(gs.m_food),
m_snakes(gs.m_snakes),
m_snakedDied(0)
{
}


GameState::~GameState()
{
}


GameState* GameState::buildFromCin(std::string input)
{
	
	std::stringstream ss;
	ss << input;
	GameState *gs = new GameState();
	ss  >> gs->m_xSize;
	ss  >> gs->m_ySize;

	for (int i = 0; i < gs->m_xSize; i++) {
		for (int j = 0; j < gs->m_ySize; j++) {
			gs->m_costs[i][j] = 1;
		}
	}

	for (int i = 0; i < MAX_SNAKES; i++) {
		gs->m_snakes[i] = Snake();
		gs->m_snakes[i].m_isAlive = false;
	}
	ss  >> gs->m_numSnakes;
	for (int s = 0; s < gs->m_numSnakes; s++) {
		int length;
		int health;
		ss  >> length >> health;
		gs->m_snakes[s].m_isAlive = true;
		gs->m_snakes[s].m_health = health;
		int x;
		int y;
		for (int l = 0; l < length; l++) {
			ss  >> x >> y;
			gs->m_snakes[s].m_snake.push_back(GridPoint(x, y));
			gs->m_costs[x][y] = INT_MAX;
		}
	}
	
	int numFood;
	ss  >> numFood;
	gs->m_food.clear();
	int x;
	int y;
	for (int f = 0; f < numFood; f++) {
		ss  >> x >> y;
		gs->m_food.push_back(GridPoint(x, y));
	}
	gs->updateSnakes();
	for (int s = 0; s < gs->m_numSnakes; s++) {
		gs->runDijkstra(s);
	}

	return gs;
}

std::vector<Direction> GameState::checkDirections(const int snakeNum) {
	std::vector<Direction> directions;
	const Snake &snake = m_snakes[snakeNum];
	if (!snake.m_isAlive) {
		directions.push_back(NONE);
		return directions;
	}
	const GridPoint &head = snake.m_snake[0];
	int x;
	int y;
	// right
	x = head.x + 1;
	y = head.y;
	if (x < m_xSize && (m_costs[x][y] < INT_MAX || m_ttl[x][y] == 1)) {
		directions.push_back(RIGHT);
	}
	// left
	x = head.x - 1;
	y = head.y;
	if (x >= 0 && (m_costs[x][y] < INT_MAX || m_ttl[x][y] == 1)) {
		directions.push_back(LEFT);
	}
	// down
	x = head.x;
	y = head.y + 1;
	if (y < m_ySize && (m_costs[x][y] < INT_MAX || m_ttl[x][y] == 1)) {
		directions.push_back(DOWN);
	}
	// up
	x = head.x;
	y = head.y - 1;
	if (y >= 0 && (m_costs[x][y] < INT_MAX || m_ttl[x][y] == 1)) {
		directions.push_back(UP);
	}

	if (directions.size() == 0) {
		directions.push_back(DIE);
	}

	return directions;
}

std::vector< std::vector<Direction>> GameState::pickMoves(const int snake) {
	std::vector< std::vector<Direction>> updatedMoves;

	const auto possibleMoves = checkDirections(snake);
	if (snake == MAX_SNAKES - 1 || snake == 0) {
		for (unsigned int i = 0; i < possibleMoves.size(); i++) {
			std::vector<Direction> newMove;
			newMove.resize(MAX_SNAKES, NONE);
			newMove[snake] = possibleMoves[i];
			updatedMoves.push_back(newMove);
		}
	}
	else {
		const auto otherMoves = pickMoves(snake + 1);
		for (unsigned int i = 0; i < otherMoves.size(); i++) {
			for (unsigned j = 0; j < possibleMoves.size(); j++) {
				std::vector<Direction> newMove = otherMoves[i];
				newMove[snake] = possibleMoves[j];
				updatedMoves.push_back(newMove);
			}
		}
	}
	return updatedMoves;
}

//Gives a list of all possible game states, either for our snake, or all enemy snakes together.
// ***CALLER MUST FREE RETURNED VECTOR***
std::vector< std::pair<GameState, Direction>> *GameState::getMoves(const bool ourSnake) 
{
	const std::vector< std::vector<Direction>> moveList = pickMoves(ourSnake ? 0 : 1);
	std::vector< std::pair<GameState, Direction>> *newMoves = new std::vector< std::pair<GameState, Direction>>();
	newMoves->reserve(moveList.size());

	for (unsigned int i = 0; i < moveList.size(); i++) {
		newMoves->emplace_back(std::pair<GameState,Direction>(*this, moveList[i][0]));
		GameState &gs = newMoves->back().first;
		gs.moveSnakes(moveList[i]);
		gs.updateSnakes();
		for (int j = 0; j < MAX_SNAKES; j++) {
			gs.runDijkstra(j);
		}
	}

	return newMoves;
}

int GameState::calculateValue()
{
	 return calcHeuristic(*this); 
}

bool GameState::gameOver()
{
	return gameWin(*this) || gameLoss(*this);
}

class DijkstraCompare {
public:
	bool operator()(const std::pair< int, GridPoint> a, const std::pair< int, GridPoint> b) {
		return a.first > b.first;
	}
};

inline int hashGridPoint(const GridPoint p) {
	return p.x << 8 | p.y;
}


void GameState::runDijkstra(const int snake)
{
	bool visited[(MAX_X << 8 | MAX_Y)+1] = { 0 };
	std::priority_queue< std::pair< int, GridPoint>,
		std::vector< std::pair< int, GridPoint>>,
		DijkstraCompare> queue;
	//std::unordered_set< int > set;

	for (int i = 0; i < MAX_X; i++) {
		for (int j = 0; j < MAX_Y; j++) {
			m_dijkstraCosts[snake][i][j] = INT_MAX;
			m_dijkstraPrev[snake][i][j] = GridPoint(255, 255);
			m_dijkstraNumMoves[snake][i][j] = INT_MAX;
		}
	}

	if (!m_snakes[snake].m_isAlive) {
		return;
	}

	GridPoint startPoint;
	startPoint = m_snakes[snake].m_snake[0];
	queue.push(std::pair< int, GridPoint>(0, startPoint));
	m_dijkstraCosts[snake][startPoint.x][startPoint.y] = 0;
	m_dijkstraNumMoves[snake][startPoint.x][startPoint.y] = 0;
	//set.insert(hashGridPoint(startPoint.start));

	while (!queue.empty()) {
		auto queueNode = queue.top();
		queue.pop();

		//Pop edge from u to v.
		int currentCost = queueNode.first;
		GridPoint currentPoint = queueNode.second;
		uint8_t x = currentPoint.x;
		uint8_t y = currentPoint.y;


		//Check if this is first visit to v.
		if (!visited[hashGridPoint(currentPoint)]) {
			visited[hashGridPoint(currentPoint)] = true;
			int moveNum = m_dijkstraNumMoves[snake][x][y];
			for (int d = 0; d < 4; d++) {
				uint8_t newX = x + directionsX[d];
				uint8_t newY = y + directionsY[d];
				GridPoint nextPoint(newX, newY);
				//set.count(hashGridPoint(newV)) == 0 &&
				if (newX < m_xSize&&newY < m_ySize) {
					//Valid point.
					int additionalCost = m_costs[newX][newY];
					if (additionalCost == INT_MAX && moveNum >= m_ttl[newX][newY]) {
						additionalCost = 1;
					}
					int newCost = ((additionalCost == INT_MAX) ? INT_MAX : (currentCost + additionalCost));
					if (!visited[hashGridPoint(nextPoint)] && newCost < m_dijkstraCosts[snake][newX][newY]) {
						m_dijkstraPrev[snake][newX][newY] = currentPoint;
						m_dijkstraCosts[snake][newX][newY] = newCost;
						m_dijkstraNumMoves[snake][newX][newY] = moveNum + 1;
						queue.push(std::pair< int, GridPoint>(newCost, nextPoint));
					}
				}
			}
		}
	}
}

bool GameState::partOfVoronoi(const int snake,const int turnFudgeFactor,const int x,const int y) {
	if (!m_snakes[snake].m_isAlive) {
		return false;
	}
	const int ourSnakeMoves = m_dijkstraNumMoves[snake][x][y];
	for (int i = 0; i < MAX_SNAKES; i++) {
		if (m_snakes[i].m_isAlive && i != snake) {
			if (m_dijkstraNumMoves[i][x][y] <= ourSnakeMoves + turnFudgeFactor) {
				return false;
			}
		}
	}
	return true;
}

int GameState::calcAccessibleArea(int snake, int turnFudgeFactor)
{
	bool visited[(MAX_X << 8 | MAX_Y) + 1] = { 0 };
	int area = 0;
	std::queue<GridPoint> queue;
	queue.push(m_snakes[snake].m_snake[0]);
	while (!queue.empty()) {
		const GridPoint p = queue.front();
		queue.pop();
		if(!visited[hashGridPoint(p)]) {
			visited[hashGridPoint(p)] = true;
			area++;
		}
		else {
			continue;
		}
		
		const uint8_t x = p.x;
		const uint8_t y = p.y;
		for (int d = 0; d < 4; d++) {
			uint8_t newX = x + directionsX[d];
			uint8_t newY = y + directionsY[d];
			if (newX < m_xSize&&newY < m_ySize) {
				if (m_dijkstraCosts[snake][newX][newY] != INT_MAX && partOfVoronoi(snake, turnFudgeFactor, newX, newY)) {
					GridPoint newPoint(newX, newY);
					if (!visited[hashGridPoint(newPoint)]) {
						queue.push(newPoint);
					}
				}
			}
		}
	}
	return area;
}

int GameState::distanceToClosestFood(const int snake)
{
	int bestFood = INT_MAX;
	GridPoint foodPoint(255,255);
	for (int i = 0; i < m_food.size(); i++) {
		const GridPoint &food = m_food[i];
		int cost = m_dijkstraCosts[snake][food.x][food.y];
		if (cost < bestFood) {
			bestFood = cost;
			foodPoint = GridPoint(food.x, food.y);
		}
	}
	return foodPoint.x == 255 ? INT_MAX : m_dijkstraNumMoves[snake][foodPoint.x][foodPoint.y];
}

int GameState::enemiesDiedThisTurn()
{
	return m_snakedDied - (m_snakes[0].m_isAlive ? 0 : 1);
}

int GameState::areWeDead()
{
	return !m_snakes[0].m_isAlive;
}

void GameState::printDijkstra(int snake)
{
	for (int y = 0; y < m_ySize; y++) {
		for (int x = 0; x < m_ySize; x++) {
			int deltaX = m_dijkstraPrev[snake][x][y].x - x;
			int deltaY = m_dijkstraPrev[snake][x][y].y - y;
			int cost = m_costs[x][y];
			wchar_t bg;
			wchar_t arrow;
			if (cost < 2) {
				bg = ' ';
			}
			else if (cost < 10) {
				bg = '1';
			}
			else if (cost < 100) {
				bg = '2';
			}
			else if (cost < 1000) {
				bg = '3';
			}
			else {
				bg = '4';
			}

			if (deltaY == -1) {
				arrow = '^';
			}
			else if (deltaY == 1) {
				arrow = 'v';
			}
			else if (deltaX == -1) {
				arrow = '<';
			}
			else if (deltaX == 1) {
				arrow = '>';
			}
			else {
				arrow = '*';
			}
			std::wcout << bg << arrow << bg;
		}
		std::wcout << std::endl;
	}
}

void GameState::printTTL()
{
	for (int y = 0; y < m_ySize; y++) {
		for (int x = 0; x < m_ySize; x++) {
			int ttl = m_ttl[x][y];
			std::wcout << (ttl > 9 || ttl < 0 ? "" : " ") << ttl << " ";
		}
		std::wcout << std::endl;
	}
}

void GameState::printMoves(int snake)
{
	for (int y = 0; y < m_ySize; y++) {
		for (int x = 0; x < m_ySize; x++) {
			int moves = m_dijkstraNumMoves[snake][x][y];
			std::wcout << (moves > 9 ? (moves > 99 ? "" : " ") : "  ") << moves;
		}
		std::wcout << std::endl;
	}
}

void GameState::printVoronoi()
{
	for (int y = 0; y < m_ySize; y++) {
		for (int x = 0; x < m_ySize; x++) {
			std::cout << " ";
			for (int s = 0; s < MAX_SNAKES; s++) {
				if (partOfVoronoi(s, 0, x, y)) {
					std::cout  << s;
				}
			}
			std::cout << ",";
		}
		std::wcout << std::endl;
	}
}



bool GameState::checkIfKilled(const int snake) {
	for (int i = 0; i < MAX_SNAKES; i++) {
		const Snake &otherSnake = m_snakes[i];
		if (otherSnake.m_isAlive && i != snake) {
			const GridPoint &head = m_snakes[snake].m_snake[0];
			for (unsigned int j = 0; j < otherSnake.m_snake.size(); j++) {
				const GridPoint &bodySegment = otherSnake.m_snake[j];
				if (bodySegment.x == head.x && bodySegment.y == head.y) {
					if (otherSnake.m_snake.size() >= m_snakes[snake].m_snake.size()) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void GameState::updateFood(Snake &snake) {
	const GridPoint &head = snake.m_snake[0];
	for (std::vector<GridPoint>::iterator food = m_food.begin(); food != m_food.end(); food++) {
		if (food->x == head.x && food->y == head.y) {
			food = m_food.erase(food);
			snake.m_health = 100;
			snake.m_snake.push_back(snake.m_snake.back());
		}
	}
}

void GameState::updateSnakes()
{
	bool nowDead[MAX_SNAKES] = { 0 };
	for (int i = 0; i < MAX_SNAKES; i++) {
		if (m_snakes[i].m_isAlive) {
			nowDead[i] = checkIfKilled(i);			
		}
	}
	for (int i = 0; i < MAX_SNAKES; i++) {
		if (nowDead[i]) {
			m_snakes[i].m_isAlive = false;
			m_snakedDied++;
		}
	}
	for (int i = 0; i < MAX_SNAKES; i++) {
		Snake &snake = m_snakes[i];
		if (snake.m_isAlive) {
			updateFood(snake);
		}
	}

	for (int i = 0; i < m_xSize; i++) {
		for (int j = 0; j < m_ySize; j++) {
			m_ttl[i][j] = -1;
			m_costs[i][j] = 1;
		}
	}
	for (int i = 0; i < MAX_SNAKES; i++) {
		const Snake &snake = m_snakes[i];
		if (snake.m_isAlive) {
			for (int j = (int)snake.m_snake.size()-1; j >=0 ; j--) {
				int x = snake.m_snake[j].x;
				int y = snake.m_snake[j].y;
				m_ttl[x][y] = snake.m_snake.size() - j;
				m_costs[x][y] = INT_MAX;
			}
		}
	}
}

void GameState::moveSnakes(std::vector<Direction> moves)
{
	for (int i = 0; i < MAX_SNAKES; i++) {
		Snake &snake = m_snakes[i];
		const Direction &move = moves[i];
		if (move != NONE && move != DIE) {
			const GridPoint &oldHead = snake.m_snake[0];
			GridPoint newHead;
			switch (move) {
			case UP:
				newHead = GridPoint(oldHead.x, oldHead.y - 1);
				break;
			case DOWN:
				newHead = GridPoint(oldHead.x, oldHead.y + 1);
				break;
			case RIGHT:
				newHead = GridPoint(oldHead.x+1, oldHead.y);
				break;
			case LEFT:
				newHead = GridPoint(oldHead.x-1, oldHead.y);
				break;
			default:
				newHead = GridPoint(oldHead.x, oldHead.y);
				break;
			}
			snake.m_snake.insert(snake.m_snake.begin(), newHead);
			snake.m_snake.pop_back();
		}
	}
}
