/* Implementation of Checkers network client.
Author: W. Duncan Fraser
Email: duncan@wduncanfraser.com

*/

#include <iostream>
#include <array>
#include <vector>

#include "lib/socket.h" /* osl/socket.h */

#include "lib/gameboard.h"
#include "lib/neuralnet.h"
#include "checkersabprune.h"

#define STATUS_CHANGE 1
#define STATUS_GAME_OVER 3
#define STATUS_ILLEGAL_MOVE 5
#define STATUS_CORRUPT_BOARD 7

#define LAYERS 4
#define INPUT 32
#define HL1 40
#define HL2 10
#define OUTPUT 1
#define SERVER_PORT 42069

#define DEPTH 7

int main()
{
	// Server IP Address
	skt_ip_t server_IP = {{172, 20, 229, 207}};

	// Buffers for socket
	char ready_message [] = "Ready to Play";
	char transmit_message [65];

	// Create socket for game session
	SOCKET session = skt_connect(server_IP, SERVER_PORT, 5);

	if (session == -1)
	{
		std::cout << "ERROR: Socket failed to connect. Socket output: " << session << std::endl;
	}
	else
	{
		//Define board for match
		CheckersBoard match_board;
		//Temp board
		CheckersBoard temp_board;

		std::ifstream best_networks_in("6plynetwork2.txt");

		if(!best_networks_in.is_open())
		{
			std::cout << "Failed to open file\n";
			return 1;
		}

		NeuralNet match_network(LAYERS, {INPUT,HL1,HL2,OUTPUT});
		match_network.importWeights(best_networks_in);



		//Value of best move
		double best_move, temp_best_move = 0;

		//Transmission checkers board, used to translate board
		std::array<std::array<uint8_t, 8>, 8> transmit_board;
		//Status byte
		uint8_t status_byte;

		//Bool to determine color
		bool color = 0;
		//Bool to determine if first move
		bool first_move = true;
		//Bool to determine if game should continue
		bool continue_match = true;
		//Integer to store status messages from socket connections
		int socket_status;
		//Bool for checking if move made was valid
		bool valid_move = true;

		//Send ready message to server
		skt_sendN(session, &ready_message, 13);

		//Move history
		std::vector<std::array<std::array<uint8_t, 8>, 8>> move_History;
		unsigned int history_length;
		
		while (continue_match == true)
		{
			//Wait for data to be ready to be recieved
			skt_select1(session, 50);
			//Recieve game board message
			socket_status = skt_recvN(session, &transmit_message, 65);
			if(socket_status != 0)
			{
				std::cout << "Error recieving data from server. Return statement: " << socket_status << std::endl;
				continue_match = false;
				break;
			}

			//Check status byte
			status_byte = (transmit_message[0] - 48);

			//Translate recieved buffer to board
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					transmit_board[i][j] = ((transmit_message[(i*8)+j+1] - 48));
				}
			}

			//Determine status byte
			if((status_byte & STATUS_CHANGE) == STATUS_CHANGE)
			{
				std::cout << "Status Changed: ";

				if(status_byte == STATUS_GAME_OVER)
				{
					std::cout << "Game Over.";
				}
				else if(status_byte == STATUS_ILLEGAL_MOVE)
				{
					std::cout << "Illegal Move.";
				}
				else if(status_byte == STATUS_CORRUPT_BOARD)
				{
					std::cout << "Corrupt Board.";
				}
				std::cout << std::endl;

				continue_match = false;
				break;
			}
			
			if(first_move)
			{
				first_move = false;

				// If base board, color is red
				if(transmit_board == match_board.getBoard())
				{
					color = 0;
					std::cout << "Color Red." << std::endl;
				}
				// Else black
				else
				{
					color = 1;
					std::cout << "Color Black." << std::endl;

					valid_move = match_board.makeMove(transmit_board, !color);
					std::cout << "Opponent's move (red): " << std::endl;
				}
			}
			else
			{
				valid_move = match_board.makeMove(transmit_board, !color);

				if(color == 0)
				{
					std::cout << "Opponent's move (black): " << std::endl;
				}
				else
				{
					std::cout << "Opponent's move (red): " << std::endl;
				}
			}
			move_History.push_back(transmit_board);
			displayCheckersBoard(transmit_board);

			if(valid_move)
			{
				//Get move list. If no moves possible, game over
				if(match_board.generateMoves(color))
				{
					best_move = -std::numeric_limits<double>::infinity();

					//For each possible move, calculate Alpha Beta

					for(Move &row: match_board.getMoveList())
					{
						temp_board.setBoard(row.move_board);
						temp_best_move = CheckersABPrune(match_network, temp_board, DEPTH, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), !color, false, color);
						
						if(temp_best_move > best_move)
						{

							best_move = temp_best_move;
							transmit_board = row.move_board;
						}	
					}

					move_History.push_back(transmit_board);
					match_board.makeMove(transmit_board, color);

					if(color == 0)
					{
						std::cout << "Your move (red): " << std::endl;
					}
					else
					{
						std::cout << "Your move (black): " << std::endl;
					}

					match_board.displayCheckersBoard();


					//Check if moves are possible for opponent and set status accordingly
					if(match_board.generateMoves(!color))
					{
						transmit_message[0] = '0';
					}
					else
					{
						//No moves possible for opponent. Send game over and end match.
						transmit_message[0] = STATUS_GAME_OVER + 48;

						std::cout << "Game over. Game won, opponent has no moves possible." << std::endl;

						continue_match = false;	
					}

					//Cycle detection
					history_length = move_History.size();
					if(history_length >= 10)
					{
						if(((move_History[history_length-1] == move_History[history_length-5]) &&
							(move_History[history_length-1] == move_History[history_length-9])) ||
							(move_History.size() > 200))
						{
							continue_match = false;
							transmit_message[0] = STATUS_GAME_OVER + 48;
							std::cout << "Game over. Cycle" << std::endl;
						}
					}

					// Convert board to message format
					for(int i = 0; i < 8; i++)
					{
						for(int j = 0; j < 8; j++)
						{
							transmit_message[(8 * i) + j + 1] = (transmit_board[i][j] + 48);
						}
					} 
				}
				else
				{
					transmit_message[0] = STATUS_GAME_OVER + 48;

					std::cout << "Game over. Game lost, no moves possible." << std::endl;
					
					continue_match = false;
				}
			}
			else
			{
				//Inform server of illegal move
				transmit_message[0] = STATUS_ILLEGAL_MOVE + 48;

				std::cout << "Opponent made illegal move." << std::endl;
				
				continue_match = false;
			}

			// Send game message to server
			socket_status = skt_sendN(session, &transmit_message, 65);
			if(socket_status != 0)
			{
				std::cout << "Error sending data to server. Return statement: " << socket_status << std::endl;
				continue_match = false;
				break;
			}	
		} 
	}
}