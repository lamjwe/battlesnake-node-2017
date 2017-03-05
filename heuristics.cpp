// heuristics.cpp : Defines the entry point for the console application.
//

#define NOMINMAX

#include "GameState.h"
#include "heuristics.h"
//#include "SnakeHeuristics.h"
#include "daniel_timer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>
const int MIN_VAL = -10000000;
const int MAX_VAL =  10000000;


// void test() {
	// Timer t;
	// t.start();
	// GameState *gs;
	// gs = GameState::buildFromCin();
	// unsigned long time = t.millisecondsPassed();
	// std::cout << time << std::endl;
	// gs->printDijkstra(0);
	// gs->printTTL();
	// gs->printMoves(0);
	// for (int i = 0; i < 4; i++) {
		// std::cout << gs->calcAccessibleArea(i, 0) << std::endl;
	// }
	// gs->printVoronoi();
	// t.start();
	// //Gets the game states for us.
	// volatile auto results = gs->getMoves(true);
	// time = t.millisecondsPassed();
	// //Gets the game states for enemy moves.
	// volatile auto results2 = gs->getMoves(false);
	// unsigned long time2 = t.millisecondsPassed();
	// std::cout << time << " " << time2 << std::endl;
	// results->at(0).first.printDijkstra(0);
	// std::cout << "---------------" << std::endl;
	// for (int i = 0; i < results2->size(); i++) {
		// std::cout << results2->at(i).first.calculateValue() << std::endl;
		// results2->at(i).first.printDijkstra(0);
		// std::cout << std::endl;
	// }
	// std::cout << "---------------" << std::endl;
	// auto results3 = results2->back().first.getMoves(false);
	// for (int i = 0; i < results3->size(); i++) {
		// std::cout << results3->at(i).first.calculateValue() << std::endl;
		// results3->at(i).first.printDijkstra(0);
		// std::cout << std::endl;
	// }

	// delete results;
	// delete results2;
// }

int* abNegamax(GameState& G, int max_depth, int current_depth, int alpha, int beta, bool us)
{
	int* scoreandmove = new int[2];

	if (G.gameOver() || current_depth == max_depth)
	{
		scoreandmove[0] = G.calculateValue();
		scoreandmove[1] = -1;
		return scoreandmove;
	}

	int* best_score = &scoreandmove[0];
	int* best_move = &scoreandmove[1];


	*best_score = MIN_VAL;
	*best_move = -1;
	std::vector< std::pair<GameState, Direction>> *possible_moves = G.getMoves(us);

	int* recursed_score_move;
	int current_score;

	for (int move = 0; move < possible_moves->size(); move++)
	{
		recursed_score_move = abNegamax(possible_moves->at(move).first, max_depth, current_depth + 1, -beta, -(std::max(alpha, (*best_score))), !us);
		current_score = -recursed_score_move[0];

		if (current_score >(*best_score))
		{
			*best_score = current_score;
			*best_move = (int)possible_moves->at(move).second;
		}
		delete[] recursed_score_move;
		if (*best_score >= beta)
		{
			//cout<<"best score is "<<*best_score<<" which is higher than beta="<<beta<<" so no need to search node "<<possible_moves[1].get_id()<<endl;
			return scoreandmove;
		}
	}
	return scoreandmove;
}


int run(std::string input)
{
	GameState *gs;
	gs = GameState::buildFromCin(input);
	int *results = abNegamax(*gs, 2, 0, MIN_VAL, MAX_VAL, true);
	int d = (Direction)results[1];
	delete gs;
	delete results;
	
	return d;
}

int main() {
	std::string input;
	std::string line;
	while (std::getline(std::cin, line))
	{
		input.append(" ");
		input.append(line);
	}
	run(input);
}

