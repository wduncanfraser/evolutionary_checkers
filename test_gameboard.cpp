#include <iostream>
#include <array>
#include <vector>
#include <cstdint>

//Test for GameBoard
#include "lib/gameboard.h"

int main()
{
	// Create test boards
	CheckersBoard test1;
	ChessBoard test2;

	// Checkers tests
	std::cout << "Checkers Board Test" << std::endl;
	test1.displayBoard();
	test1.generateMoves(0);
	test1.displayMoves();

	std::array<std::array<uint8_t, 8>, 8> testInput1 = {{
		{{1, 0, 1, 0, 1, 0, 1, 0}},
		{{0, 1, 0, 1, 0, 1 ,0 ,1}},
		{{1, 0, 1, 0, 0, 0, 1, 0}},
		{{0, 0, 0, 1, 0, 0, 0 ,0}},
		{{0, 0, 3, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 3, 0, 3, 0, 3}},
		{{3, 0, 3, 0, 3, 0, 3, 0}},
		{{0, 3, 0, 3, 0, 3, 0, 3}}
	}};

	std::array<std::array<uint8_t, 8>, 8> testInput2 = {{
		{{1, 0, 1, 0, 1, 0, 1, 0}},
		{{0, 1, 0, 1, 0, 1 ,0 ,1}},
		{{1, 0, 1, 0, 0, 0, 1, 0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 1, 0, 3, 0, 3, 0, 3}},
		{{3, 0, 3, 0, 3, 0, 3, 0}},
		{{0, 3, 0, 3, 0, 3, 0, 3}}
	}};

	test1.setBoard(testInput1);
	test1.displayBoard();
	test1.generateMoves(0);
	test1.displayMoves();
	if (test1.makeMove(testInput2, 0))
	{
		std::cout << "Valid Move." << std::endl;
	}
	else
	{
		std::cout << "Invalid Move." << std::endl;
	}
	test1.displayBoard();
	test1.generateMoves(1);
	test1.displayMoves();


	std::array<std::array<uint8_t, 8>, 8> testInput3 = {{
		{{1, 0, 1, 0, 0, 0, 1, 0}},
		{{0, 1, 0, 1, 0, 1 ,0 ,1}},
		{{1, 0, 0, 0, 0, 0, 1, 0}},
		{{0, 1, 0, 1, 0, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0 ,0}},
		{{0, 1, 0, 3, 0, 3, 0, 3}},
		{{3, 0, 7, 0, 3, 0, 3, 0}},
		{{0, 3, 0, 3, 0, 3, 0, 3}}
	}};
	test1.setBoard(testInput3);
	test1.displayBoard();
	test1.generateMoves(1);
	test1.displayMoves();

	//Chess tests
	/*
	std::cout << "Chess Board Test" << std::endl;
	test2.displayBoard();
	test2.generateMoves(1);
	test2.displayMoves();

	testInput1 = {{
		{{9, 5, 7, 11, 13, 7, 0, 9}},
		{{3, 3, 0, 0, 3, 0 ,3 ,3}},
		{{0, 0, 0, 3, 0, 5, 0, 0}},
		{{0, 0, 3, 0, 0, 3, 0 ,0}},
		{{0, 0, 19, 0, 19, 0, 0 ,0}},
		{{0, 0, 0, 0, 0, 0, 0, 19}},
		{{19, 19, 0, 19, 0, 19, 19, 0}},
		{{25, 21, 23, 27, 29, 23, 21, 25}}
	}};

	test2.setBoard(testInput1);
	test2.displayBoard();
	test2.generateMoves(0);
	test2.displayMoves();*/
}