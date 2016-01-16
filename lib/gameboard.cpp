#include <iostream>
#include <array>
#include <vector>
#include <cstdint>

#include "gameboard.h"

inline bool withinBounds(const unsigned int vertical, const unsigned int horizontal)
{
	return ((vertical <= 7) && (vertical >= 0) && (horizontal <= 7) && (horizontal >= 0));
}

std::vector<double> getNetworkTranslation(const std::array<std::array<uint8_t, 8>, 8> &i_board, const bool color)
{
	std::vector<double> network_translation(32);
	//Always sends to neural net as if player was red, rotates for symmetry to help specialize neural network.

	if (color)
	{
		// If black, rotate board for symmetry so that network always has the same perspective
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 4; j++)
			{				
				if((i_board[7-i][7-((j*2) + (i%2))] & CHECKERS_TEAM_MASK) == CHECKERS_BLACK_MASK)
				{
					network_translation[j + (4 * i)] = 	i_board[7-i][7-((j*2) + (i%2))] & CHECKERS_NEUTRAL_MASK;
				}
				else
				{
					network_translation[j + (4 * i)] = 	-(i_board[7-i][7-((j*2) + (i%2))] & CHECKERS_NEUTRAL_MASK);
				}
				//std::cout << (int)network_translation[j + (4 * i)] << ",";
			}
		}
	}
	else
	{
		// If red, process normally
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				if((i_board[i][(j*2) + (i%2)] & CHECKERS_TEAM_MASK) == CHECKERS_RED_MASK)
				{
					network_translation[j + (4 * i)] = 	i_board[i][(j*2) + (i%2)] & CHECKERS_NEUTRAL_MASK;
				}
				else
				{
					network_translation[j + (4 * i)] = 	-(i_board[i][(j*2) + (i%2)] & CHECKERS_NEUTRAL_MASK);
				}
				//std::cout << (int)network_translation[j + (4 * i)] << ",";
			}
		}
	}
	//std::cout << std::endl;

	return network_translation;
}

void displayCheckersBoard(const std::array<std::array<uint8_t, 8>, 8> &i_board)
{
	std::cout << std::endl;
	for(int i = 7; i >= 0; i--)
	{
		for(unsigned int j = 0; j < 8; j++)
		{
			if(i_board[i][j] < 10)
			{
				std::cout << " ";
			}
			if ((int)i_board[i][j] == 1)
			{
				std::cout << "r";	
			}
			else if ((int)i_board[i][j] == 5)
			{
				std::cout << "R";	
			}
			else if ((int)i_board[i][j] == 3)
			{
				std::cout << "b";	
			}
			else if ((int)i_board[i][j] == 7)
			{
				std::cout << "B";	
			}
			else if ((int)i_board[i][j] == 0)
			{
				std::cout << "_";	
			}
			else if ((int)i_board[i][j] == 8)
			{
				std::cout << " ";	
			}
			else
			{
				std::cout << (int)i_board[i][j];
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

Move::Move(const std::array<std::array<uint8_t, 8>, 8> &i_board, 
		const unsigned int i_v_pos_1, const unsigned int i_h_pos_1, 
		const unsigned int i_v_pos_2, const unsigned int i_h_pos_2,
		const unsigned int i_v_pos_capture, const unsigned int i_h_pos_capture, 
		const unsigned int i_attack):
		move_board(i_board), v_pos_1(i_v_pos_1), h_pos_1(i_h_pos_1),
		v_pos_2(i_v_pos_2), h_pos_2(i_h_pos_2), attack(i_attack)
{
	move_board[i_v_pos_capture][i_h_pos_capture] = 0;
	move_board[v_pos_2][h_pos_2] = move_board[v_pos_1][h_pos_1];
	move_board[v_pos_1][h_pos_1] = 0;
}

Move::Move()
{

}

unsigned int GameBoard::destinationCheck(const uint8_t tMask, const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos)
{
	if (withinBounds(v_pos, h_pos))
	{
		if((board[v_pos][h_pos] & tMask) == oMask)
		{	
			// Square has opponent piece
			return 1;
		}
		else if (board[v_pos][h_pos] == 0)
		{
			// Square is empty
			return 0;
		}
		else
		{
			// Square is same team
			return 2;
		}
	}
	else
	{
		// Square is out of bounds
		return 3;
	}
}

unsigned int GameBoard::destinationCheck(const uint8_t tMask, const uint8_t oMask, const Move &i_move, const unsigned int v_pos, const unsigned int h_pos)
{
	if (withinBounds(v_pos, h_pos))
	{
		if((i_move.move_board[v_pos][h_pos] & tMask) == oMask)
		{	
			// Square has opponent piece
			return 1;
		}
		else if (i_move.move_board[v_pos][h_pos] == 0)
		{
			// Square is empty
			return 0;
		}
		else
		{
			// Square is same team
			return 2;
		}
	}
	else
	{
		// Square is out of bounds
		return 3;
	}
}

std::array<std::array<uint8_t, 8>, 8> GameBoard::getBoard()
{
	//Return a copy of board
	return board;
}

std::vector<Move> GameBoard::getMoveList()
{
	return move_List;
}

int GameBoard::setBoard(const std::array<std::array<uint8_t, 8>, 8> &i_board)
{
	board = i_board;
	return 0;
}

void GameBoard::displayBoard()
{
	std::cout << std::endl;
	for(int i = 7; i >= 0; i--)
	{
		for(unsigned int j = 0; j < 8; j++)
		{
			if(board[i][j] < 10)
			{
				std::cout << " ";
			}
			std::cout << (int)board[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void GameBoard::displayMoves()
{
	for (Move &row: move_List)
	{	
		displayCheckersBoard(row.move_board);	
		std::cout << row.v_pos_1 << "," << row.h_pos_1 << "," << row.v_pos_2 << "," << row.h_pos_2 << "," << row.attack <<std::endl;
	}
	std::cout << "Total available moves: " << move_List.size() << std::endl;
	std::cout << std::endl;
}


//Default Constructor. Initializes a fresh board.
CheckersBoard::CheckersBoard()
{
	//Board is being initialized in reverse of actual representation, as bottom left was set to be 0,0
	board= {{
		{{1, 8, 1, 8, 1, 8, 1, 8}},
		{{8, 1, 8, 1, 8, 1 ,8 ,1}},
		{{1, 8, 1, 8, 1, 8, 1, 8}},
		{{8, 0, 8, 0, 8, 0, 8 ,0}},
		{{0, 8, 0, 8, 0, 8, 0 ,8}},
		{{8, 3, 8, 3, 8, 3, 8, 3}},
		{{3, 8, 3, 8, 3, 8, 3, 8}},
		{{8, 3, 8, 3, 8, 3, 8, 3}}
	}};
}

void CheckersBoard::initializeBoard()
{
	//Board is being initialized in reverse of actual representation, as bottom left was set to be 0,0
	board= {{
		{{1, 8, 1, 8, 1, 8, 1, 8}},
		{{8, 1, 8, 1, 8, 1 ,8 ,1}},
		{{1, 8, 1, 8, 1, 8, 1, 8}},
		{{8, 0, 8, 0, 8, 0, 8 ,0}},
		{{0, 8, 0, 8, 0, 8, 0 ,8}},
		{{8, 3, 8, 3, 8, 3, 8, 3}},
		{{3, 8, 3, 8, 3, 8, 3, 8}},
		{{8, 3, 8, 3, 8, 3, 8, 3}}
	}};
}

bool CheckersBoard::generateMoves(const bool color)
{
	//Wipe any existing moves
	move_List.clear();
	attack_Possible = false;
	//Set bit mask and direction modifier
	uint8_t color_mask, opponent_mask;
	int v_direction;
	if (color)
	{
		color_mask = CHECKERS_BLACK_MASK;
		opponent_mask = CHECKERS_RED_MASK;
		v_direction = -1;
	}
	else
	{
		color_mask = CHECKERS_RED_MASK;
		opponent_mask = CHECKERS_BLACK_MASK;
		v_direction = 1;
	}

	for(unsigned int i = 0; i < 8; i++)
	{
		for(unsigned int j = 0; j < 8; j++)
		{
			if((board[i][j] & CHECKERS_TEAM_MASK) == color_mask)
			{
				Move temp_move;
				temp_move.move_board = board;
				temp_move.v_pos_2 = i;
				temp_move.h_pos_2 = j;
				temp_move.attack = 0;
				pieceMoveGenerate(opponent_mask, temp_move, v_direction);
			}
		}
	}
	// Remove all non attack moves if an attack is posisble. Rules of checkers dictate attack must me made if possible.
	if(attack_Possible == true)
	{
		for(int i = move_List.size() - 1; i >= 0; i--)
		{
			if(move_List[i].attack <= 0)
			{
				move_List.erase(move_List.begin() + i);
			}
		}
	}

	//Promote kings as necessary
	for(int i = move_List.size() - 1; i >= 0; i--)
	{
	//Check if destination is enemy rank, and promote to king
		if(((move_List[i].v_pos_2 == 7) && (color == 0)) || ((move_List[i].v_pos_2 == 0) && (color == 1)))
		{
			move_List[i].move_board[move_List[i].v_pos_2][move_List[i].h_pos_2] = move_List[i].move_board[move_List[i].v_pos_2][move_List[i].h_pos_2] | CHECKERS_KING_MASK;
		}
	}

	// Return true if move_List is empty
	return !move_List.empty();
}

void CheckersBoard::pieceMoveGenerate(const uint8_t oMask, const Move &i_move, const int v_direction)
{
	// Bool to determine if move has been popped. Used during move generation to force double jumps.
	bool move_popped = false;

	directionalMoveGenerate(oMask, i_move, v_direction, -1, move_popped);
	directionalMoveGenerate(oMask, i_move, v_direction, 1, move_popped);

	//Determine if king
	if((i_move.move_board[i_move.v_pos_2][i_move.h_pos_2] & CHECKERS_KING_MASK) == CHECKERS_KING_MASK)
	{
		directionalMoveGenerate(oMask, i_move, -v_direction, -1, move_popped);
		directionalMoveGenerate(oMask, i_move, -v_direction, 1, move_popped);
	}	
}

void CheckersBoard::directionalMoveGenerate(const uint8_t oMask, const Move &i_move, const int v_direction, const int h_direction, bool &move_popped)
{
	// If space is empty and attack is not already possible
	if((destinationCheck(CHECKERS_TEAM_MASK, oMask, i_move, i_move.v_pos_2 + v_direction, i_move.h_pos_2 + h_direction) == 0) && (i_move.attack == 0))
	{
		move_List.push_back(Move(i_move.move_board, i_move.v_pos_2, i_move.h_pos_2, i_move.v_pos_2 + v_direction, i_move.h_pos_2 + h_direction, i_move.v_pos_2 + v_direction, i_move.h_pos_2 + h_direction, 0));
	}
	// If space is opponent, and next space is empty
	else if ((destinationCheck(CHECKERS_TEAM_MASK, oMask, i_move, i_move.v_pos_2 + v_direction, i_move.h_pos_2 + h_direction) == 1) && 
			(destinationCheck(CHECKERS_TEAM_MASK, oMask, i_move, i_move.v_pos_2 + 2*v_direction, i_move.h_pos_2 + 2*h_direction) == 0))
	{
		attack_Possible = true;
		// If double jump is possible, remove move from the stack if it has not already been
		if((i_move.attack > 0) && (move_popped == false))
		{
			move_List.pop_back();
			move_popped = true;
		}
		Move temp_move(i_move.move_board, i_move.v_pos_2, i_move.h_pos_2, i_move.v_pos_2 + 2*v_direction, i_move.h_pos_2 + 2*h_direction, i_move.v_pos_2 + v_direction, i_move.h_pos_2 + h_direction, i_move.attack + 1);
		move_List.push_back(temp_move);
		// v_direction can be used as it will be +v_direction for any non-king pieces
		pieceMoveGenerate(oMask, temp_move, v_direction);
	}	
}

bool CheckersBoard::makeMove(const std::array<std::array<uint8_t, 8>, 8> &i_board, const bool color)
{
	//Bool to determine if move is valid
	bool validMove = false;
	//Generate moves
	generateMoves(color);

	for(Move &possible_move: move_List)
	{
		if(i_board == possible_move.move_board)
		{
			validMove = true;
			board = i_board;
		}
	}

	return validMove;	
}

void CheckersBoard::displayCheckersBoard()
{
	std::cout << std::endl;
	for(int i = 7; i >= 0; i--)
	{
		for(unsigned int j = 0; j < 8; j++)
		{
			if(board[i][j] < 10)
			{
				std::cout << " ";
			}
			if ((int)board[i][j] == 1)
			{
				std::cout << "r";	
			}
			else if ((int)board[i][j] == 5)
			{
				std::cout << "R";	
			}
			else if ((int)board[i][j] == 3)
			{
				std::cout << "b";	
			}
			else if ((int)board[i][j] == 7)
			{
				std::cout << "B";	
			}
			else if ((int)board[i][j] == 0)
			{
				std::cout << "_";	
			}
			else if ((int)board[i][j] == 8)
			{
				std::cout << " ";	
			}
			else
			{
				std::cout << (int)board[i][j];
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

//Default Constructor. Initializes a fresh board.
ChessBoard::ChessBoard()
{
	//Board is being initialized in reverse of actual representation, as bottom left was set to be 0,0
	board= {{
		{{9, 5, 7, 11, 13, 7, 5, 9}},
		{{3, 3, 3, 3, 3, 3 ,3 ,3}},
		{{0, 0, 0, 0, 0, 0, 0, 0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0, 0}},
		{{19, 19, 19, 19, 19, 19, 19, 19}},
		{{25, 21, 23, 27, 29, 23, 21, 25}}
	}};
}

void ChessBoard::initializeBoard()
{
	//Board is being initialized in reverse of actual representation, as bottom left was set to be 0,0
	board= {{
		{{9, 5, 7, 11, 13, 7, 5, 9}},
		{{3, 3, 3, 3, 3, 3 ,3 ,3}},
		{{0, 0, 0, 0, 0, 0, 0, 0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0, 0}},
		{{19, 19, 19, 19, 19, 19, 19, 19}},
		{{25, 21, 23, 27, 29, 23, 21, 25}}
	}};
}

bool ChessBoard::generateMoves(const bool color)
{
	//Wipe any existing moves
	move_List.clear();
	//Set bit mask and direction modifier
	uint8_t color_mask, opponent_mask;
	int v_direction;
	unsigned int starting_rank;
	if (color)
	{
		color_mask = CHESS_BLACK_MASK;
		opponent_mask = CHESS_WHITE_MASK;
		v_direction = -1;
		starting_rank = 6;
	}
	else
	{
		color_mask = CHESS_WHITE_MASK;
		opponent_mask = CHESS_BLACK_MASK;
		v_direction = 1;
		starting_rank = 1;
	}

	for(unsigned int i = 0; i < 8; i++)
	{
		for(unsigned int j = 0; j < 8; j++)
		{
			if((board[i][j] & CHESS_TEAM_MASK) == color_mask)
			{
				pieceMoveGenerate(opponent_mask, i, j, v_direction, starting_rank);
			}
		
		}
	}

	// Return true if move_List is empty
	return !move_List.empty();
}

void ChessBoard::pieceMoveGenerate(const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos, const int v_direction, const unsigned int s_rank)
{
	// Check if pawn. Pawn has a unique ruleset, so not using directionalMoveGenerate
	if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 3)
	{
		// Attack left
		if(destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + v_direction, h_pos - 1) == 1)
		{
			move_List.push_back(Move(board, v_pos, h_pos, (v_pos + v_direction), (h_pos - 1), (v_pos + v_direction), (h_pos - 1), 1));
		}
		// Forward one
		if(destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + v_direction, h_pos) == 0)
		{
			move_List.push_back(Move(board, v_pos, h_pos, (v_pos + v_direction), h_pos, (v_pos + v_direction), h_pos, 0));
		}
		// Forward two
		if(v_pos == s_rank)
		{
			if((destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + v_direction, h_pos) == 0) &&
				(destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + 2*v_direction, h_pos) == 0))
			{
				move_List.push_back(Move(board, v_pos, h_pos, (v_pos + 2*v_direction), h_pos, (v_pos + 2*v_direction), h_pos, 0));
			}
		}
		// Attack right
		if(destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + v_direction, h_pos + 1) == 1)
		{
			move_List.push_back(Move(board, v_pos, h_pos, (v_pos + v_direction), (h_pos + 1), (v_pos + v_direction), (h_pos + 1), 1));
		}
	}
	//Check if knight
	else if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 5)
	{
		// Forward-left
		directionalMoveGenerate(oMask, v_pos, h_pos, 2, -1, false);
		// Forward-right
		directionalMoveGenerate(oMask, v_pos, h_pos, 2, 1, false);
		// Left-forward
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, -2, false);
		// Left-back
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, -2, false);
		// Right-forward
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 2, false);
		// Right-back
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 2, false);
		// Back-left
		directionalMoveGenerate(oMask, v_pos, h_pos, -2, -1, false);
		// Back-right
		directionalMoveGenerate(oMask, v_pos, h_pos, -2, 1, false);		
	}
	//Check if bishop
	else if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 7)
	{
		// Forward Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, -1, true);
		// Forward right
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 1, true);
		// Back right
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 1, true);
		// Back left
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, -1, true);
	}
	//Check if rook
	else if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 9)
	{
		// Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, -1, true);
		// Forward
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 0, true);
		// Right
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, 1, true);
		// Back
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 0, true);
	}
	//Check if queen
	else if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 11)
	{
		// Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, -1, true);
		// Forward Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, -1, true);
		// Forward
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 0, true);
		// Forward right
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 1, true);
		// Right
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, 1, true);
		// Back right
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 1, true);
		// Back
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 0, true);
		// Back left
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, -1, true);
	}
	//Check if king
	else if ((board[v_pos][h_pos] & CHESS_PIECE_MASK) == 13)
	{
		// Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, -1, false);
		// Forward Left
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, -1, false);
		// Forward
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 0, false);
		// Forward right
		directionalMoveGenerate(oMask, v_pos, h_pos, 1, 1, false);
		// Right
		directionalMoveGenerate(oMask, v_pos, h_pos, 0, 1, false);
		// Back right
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 1, false);
		// Back
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, 0, false);
		// Back left
		directionalMoveGenerate(oMask, v_pos, h_pos, -1, -1, false);
	}
}

void ChessBoard::directionalMoveGenerate(const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos, const int v_diff, const int h_diff, const bool repeat)
{
	//Iterate through moves, pushing possible moves to the move_List.
	for (unsigned int i = 1; destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + (i * v_diff), h_pos + (i * h_diff)) < 2; i++)
	{
		move_List.push_back(Move(board, v_pos, h_pos, v_pos + (i * v_diff), h_pos + (i * h_diff), v_pos + (i * v_diff), h_pos + (i * h_diff), destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + (i * v_diff), h_pos + (i * h_diff))));
		//Stop looking if space was not empty
		if ((destinationCheck(CHESS_TEAM_MASK, oMask, v_pos + (i * v_diff), h_pos + (i * h_diff)) != 0) || (repeat == false))
		{
			break;
		}
	};
}