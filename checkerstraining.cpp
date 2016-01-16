/* Implementation of Checkers training with 6 ply and each board playing both teams.
Author: W. Duncan Fraser
Email: duncan@wduncanfraser.com

*/

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "lib/gameboard.h"
#include "lib/neuralnet.h"
#include "checkersabprune.h"

#define LAYERS 4
#define INPUT 32
#define HL1 40
#define HL2 10
#define OUTPUT 1
#define MUTATER 0.01

#define DEPTH 3

#define NETWORKCOUNT 30
#define NETWORKKEEP 10

#define STARTCYCLE 1
#define ENDCYCLE 1000

std::vector<int> scoreNetworks(std::vector<NeuralNet> networks)
{
	//Vector to hold win counts for networks
	std::vector<int> scores;
	scores.assign(networks.size(), 0);

	// Each board plays every other board as each team, storing total score for each neural network.
	#pragma omp parallel for firstprivate(networks) schedule(dynamic,1)
	for(unsigned int i = 0; i < networks.size(); i++)
	{
		for(unsigned int j = 0;  j < networks.size(); j++)
		{
			//Board used for training matches
			CheckersBoard training_board;
			training_board.initializeBoard();

			//Temp board used for AB Pruning
			CheckersBoard temp_board;

			//Transmission checkers board, used to translate board
			std::array<std::array<uint8_t, 8>, 8> transmit_board;

			//Bool to determine if game should continue
			bool continue_match = true;

			//Value of best move
			double best_move, temp_best_move = 0;

			//Move history for detecting cycles
			std::vector<std::array<std::array<uint8_t, 8>, 8>> move_History;
			unsigned int history_length;

			while (continue_match == true)
			{
				//i is red (0), j is black (1)
				//Red move
				//std::cout << "Red Move \n";

				if(training_board.generateMoves(0))
				{

					best_move = -std::numeric_limits<double>::infinity();

					//For each possible move, calculate Alpha Beta
					for(Move &row: training_board.getMoveList())
					{
						temp_board.setBoard(row.move_board);
						temp_best_move = CheckersABPrune(networks[i], temp_board, DEPTH, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 1, false, 0);
						
						if(temp_best_move > best_move)
						{
							best_move = temp_best_move;
							transmit_board = row.move_board;
						}	
					}
					//Make red move
					move_History.push_back(transmit_board);
					training_board.makeMove(transmit_board, 0);
					//training_board.displayCheckersBoard();
				}
				else
				{
					// Winner is black
					std::cout << "Black Wins. Network: " << j << " Opponent: " << i << ". \n";
					continue_match = false;
					scores[j] += 1;
					scores[i] -= 1;
					break;
				}

				//Black move
				//std::cout << "Black Move \n";

				if(training_board.generateMoves(1))
				{
					best_move = -std::numeric_limits<double>::infinity();

					//For each possible move, calculate Alpha Beta
					for(Move &row: training_board.getMoveList())
					{
						temp_board.setBoard(row.move_board);
						temp_best_move = CheckersABPrune(networks[j], temp_board, DEPTH, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 0, false, 1);
						
						if(temp_best_move > best_move)
						{
							best_move = temp_best_move;
							transmit_board = row.move_board;
						}	
					}
					//Make black move
					move_History.push_back(transmit_board);
					training_board.makeMove(transmit_board, 1);
					//training_board.displayCheckersBoard();
				}
				else
				{
					// Winner is red
					std::cout << "Red Wins. Network: " << i << " Opponent: " << j << ". \n";
					continue_match = false;
					scores[i] += 1;
					scores[j] -= 1;
					break;
				}

				//Cycle detection
				history_length = move_History.size();
				if(history_length >= 9)
				{
					if(((move_History[history_length-1] == move_History[history_length-5]) &&
						(move_History[history_length-1] == move_History[history_length-9])) ||
						(move_History.size() > 100))
					{
						std::cout << "Cycle detected, draw. Networks " << i << " and " << j << ".\n";
						continue_match = false;
						break;
					}
				}
			}
		}
	}

	return scores;
} 

int main()
{
	for(unsigned int n = STARTCYCLE; n <= ENDCYCLE; n++)
	{
		NeuralNet model_network(LAYERS, {INPUT,HL1,HL2,OUTPUT});

		std::vector<NeuralNet> training_networks(NETWORKCOUNT);
		std::vector<int> training_scores(NETWORKCOUNT);

		unsigned int export_count = 0;

		std::ifstream best_networks_in("generations"+std::to_string(DEPTH+1)+"ply"+std::to_string(NETWORKCOUNT)+"nets/lastbestnetworks.txt");

		std::cout << "Generation with " << NETWORKCOUNT << " Neural Networks.\n"
				<< "Total Games: " << NETWORKCOUNT * NETWORKCOUNT << std::endl;


		for(NeuralNet &row: training_networks)
		{
			row = model_network;
		}
			
		if(best_networks_in.is_open())
		{
			std::cout << "Starting generation " << n << ". Last best network file succesfully opened. \n";

			//Read kept networks from file
			for(unsigned int i = 0; i < NETWORKKEEP; i++)
			{
				training_networks[i].importWeights(best_networks_in);
			}
			for(unsigned int i = 0; i < NETWORKKEEP; i++)
			{
				training_networks[i+NETWORKKEEP] = training_networks[i];
				training_networks[i+NETWORKKEEP].mutate(MUTATER);
			}
			for(unsigned int i = 0; i < NETWORKKEEP; i++)
			{
				training_networks[i+(NETWORKKEEP*2)].initializeRandomWeights();
			}
		}
		else if(n == 1)
		{
			std::cout << "Starting first generation. Last best network file failed to open. Initializing random weights. \n";
			for(NeuralNet &row: training_networks)
			{
				row.initializeRandomWeights();
			}
		}
		else
		{
			std::cout << "Starting generation " << n << ". Last best network file failed to open. Ending training. \n";
			break;
		}

		training_scores = scoreNetworks(training_networks);
		
		for(unsigned int i = 0; i < training_scores.size(); i++)
		{
			std::cout << "Neural Network: " << i << ". Score: " << training_scores[i] << ".\n";
		}

		std::ofstream output_file("generations"+std::to_string(DEPTH+1)+"ply"+std::to_string(NETWORKCOUNT)+"nets/generation"+std::to_string(n)+".txt");
		std::ofstream best_networks_file("generations"+std::to_string(DEPTH+1)+"ply"+std::to_string(NETWORKCOUNT)+"nets/lastbestnetworks.txt", std::ofstream::out | std::ofstream::trunc);

		if(output_file.is_open())
		{
			for(unsigned int i = 0; i < training_scores.size(); i++)
			{
				output_file << "Neural Network: " << i << ". Score: " << training_scores[i] << ".\n";
			}
			output_file << std::endl;
			for(unsigned int i = 0; i < training_scores.size(); i++)
			{
				output_file << "Neural Network: " << i << ".\n";
				training_networks[i].exportWeights(output_file, true);
				output_file << std::endl;

			}

			output_file.close();
		}
		else 
		{
			std::cout << "Error opening output file. \n";
			break;
		}

		if (best_networks_file.is_open())
		{
			//Check down to lowest possible score, -NETWORKCOUNT*2
			for(int i = NETWORKCOUNT - 1; i > -NETWORKCOUNT*2; i--)
			{
				if(export_count >= NETWORKKEEP)
				{
					break;
				}
				for(unsigned int j = 0; j < NETWORKCOUNT; j++)
				{
					if(export_count >= NETWORKKEEP)
					{
						break;
					}
					if (training_scores[j] == i)
					{
						training_networks[j].exportWeights(best_networks_file, true);
						export_count += 1;
					}
				}
			}

			best_networks_file.close();
		}
		else
		{
			std::cout << "Error opening best networks file. \n";
			break;	
		}
	}
}
