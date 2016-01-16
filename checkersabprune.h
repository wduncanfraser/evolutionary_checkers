/* Header for alpha beta pruning for checkers client
Author: W. Duncan Fraser
Email: duncan@wduncanfraser.com

*/
#include <iostream>
#include <array>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

#include "lib/gameboard.h"
#include "lib/neuralnet.h"

//if-defs to prevent multiple inclusions.
#ifndef CHECKERSABPRUNE
#define CHECKERSABPRUNE

double CheckersABPrune(NeuralNet &network, CheckersBoard &generation_board, const int depth, double alpha, double beta, const bool move_color, const bool maxplayer, const bool player_color)
{
	//std::cout << "Recursive call \n";
	generation_board.generateMoves(move_color);
	std::vector<Move> current_move_list = generation_board.getMoveList();
	//std::cout<<depth<<std::endl;

	if ((depth <= 0) || (current_move_list.size() <=0 ))
	{
		std::vector<double> network_translation = getNetworkTranslation(generation_board.getBoard(), player_color);

		network.feedForward(network_translation);
		network_translation.front() = network.getOutput().front();
		//std::cout<< network_translation.front() << std::endl;
		return network_translation.front();
	}

	if(maxplayer)
	{
		for(Move &i: current_move_list)
		{
			generation_board.setBoard(i.move_board);
			alpha = std::max(alpha, CheckersABPrune(network, generation_board, depth-1, alpha, beta, !move_color, false, player_color));
			if(beta <= alpha)
			{
				break;
			}
		}
		//std::cout << "Alpha: " << alpha << std::endl;
		return alpha;
	}
	else
	{
		for(Move &i: current_move_list)
		{
			generation_board.setBoard(i.move_board);
			beta = std::min(beta, CheckersABPrune(network, generation_board, depth-1, alpha, beta, !move_color, true, player_color));
			if(beta <= alpha)
			{
				break;
			}
		}
		//std::cout << "Beta: " << alpha << std::endl;
		return beta;
	}
}

#endif