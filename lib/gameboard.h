/* Class definition of Checkers and Chess board.
Author: W. Duncan Fraser
Email: duncan@wduncanfraser.com

*/

//if-defs to prevent multiple inclusions.
#ifndef GAMEBOARD
#define GAMEBOARD

#define CHECKERS_RED_MASK 1
#define CHECKERS_BLACK_MASK 3
#define CHECKERS_TEAM_MASK 3
#define CHECKERS_KING_MASK 4
#define CHECKERS_PIECE_MASK 7
#define CHECKERS_NEUTRAL_MASK 5
#define CHESS_WHITE_MASK 1
#define CHESS_BLACK_MASK 17
#define CHESS_TEAM_MASK 17
#define CHESS_PIECE_MASK 15

//Check to make sure coordinates are within the bounds of a chess board.
inline bool withinBounds(const unsigned int vertical, const unsigned int horizontal);
// Return network translation for move
std::vector<double> getNetworkTranslation(const std::array<std::array<uint8_t, 8>, 8> &i_board, const bool color);
// Displays checkers board with formatting
void displayCheckersBoard(const std::array<std::array<uint8_t, 8>, 8> &i_board);

// Struct for holding possible moves
struct Move
{
	// Game board
	std::array<std::array<uint8_t, 8>, 8> move_board;
	// Origin of moved piece
	unsigned int v_pos_1, h_pos_1;
	// Destination of moved piece
	unsigned int v_pos_2, h_pos_2;
	// Attack count
	unsigned int attack;
	//Constructor
	// i_v_pos_capture and i_h_pos_capture are the coordinates of place captured. For all chess moves
	// and non-attack checkers moves this is the same as pos_2. For checkers jumps it is the position
	// of piece being jumped.
	Move(const std::array<std::array<uint8_t, 8>, 8> &i_board, 
		const unsigned int i_v_pos_1, const unsigned int i_h_pos_1, 
		const unsigned int i_v_pos_2, const unsigned int i_h_pos_2,
		const unsigned int i_v_pos_capture, const unsigned int i_h_pos_capture, 
		const unsigned int i_attack);
	Move();
};

//Gameboard class definition
//Using implicit constructors and destructor
class GameBoard
{
protected:
	// Game board
	std::array<std::array<uint8_t, 8>, 8> board;
	// Move list
	std::vector<Move> move_List;
	// Check validity of destination
	// 0 = empty destination
	// 1 = opponent piece
	// 2 = same team
	// 3 = out of bounds
	unsigned int destinationCheck(const uint8_t tMask, const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos);
	// Destination check based on input move struct
	unsigned int destinationCheck(const uint8_t tMask, const uint8_t oMask, const Move &i_move, const unsigned int v_pos, const unsigned int h_pos);
public:
	// Returns a copy of the current gameboard.
	std::array<std::array<uint8_t, 8>, 8> getBoard();
	// Returns a copy of the movelist
	std::vector<Move> getMoveList();
	// No error checking is required for size. Size is enforced by std::array template definition.
	// Needs to be moved into child classes as additional error checking is required for checkers.
	int setBoard(const std::array<std::array<uint8_t, 8>, 8> &i_board);
	// Display an ASCII representation of the board showing the numerical values for each cell
	void displayBoard();
	// Display each move board and the move coordinates.
	void displayMoves();
};


// Checkersboard using first bit as defining if there is a piece
// 0 = no piece, 1 = piece
// Second bit is team
// 0 = red, 1 = black
// Third bit is king
// 0 = not king, 1 = king
// Fourth bit is valid square
// 0 = valid, 1 = not valid
// Value: red piece = 1, black piece = 3, red king = 5, black king = 7
// Unoccupied valid square = 0, invalid square = 8.
// No other valid values.
class CheckersBoard : public GameBoard
{
private:
	// Bool to store whether there is an attack possible in the currently generated moves
	bool attack_Possible;
	// Depends on directionalMoveGenerate
	void pieceMoveGenerate(const uint8_t oMask, const Move &i_move, const int v_direction);
	// Check for available moves
	void directionalMoveGenerate(const uint8_t oMask, const Move &i_move, const int v_direction, const int h_direction, bool &move_popped);
public:
	// Default Constructor. Initializes a fresh board.
	CheckersBoard();
	// Initialize board. Sets up a fresh checkers board.
	void initializeBoard();
	// Generate all possible moves for specified color. 
	// red = 0, black = 1
	// Return value of 0 signifies no moves.
	bool generateMoves(const bool color);
	// Take a move by inputting new board and team making move.
	// red = 0, black = 1
	// Checks against valid moves, and returns 0 if move is not valid
	bool makeMove(const std::array<std::array<uint8_t, 8>, 8> &i_board, const bool color);
	// Displays checkers board with formatting
	void displayCheckersBoard();
};


// Chessboard using bit 1-4 to determine piece (1 determines if there is a piece)
// 0000 = no piece, 1100 = Pawn, 1010 = Knight, 1110 = Bishop, 1001 = Rook, 1101 = Queen, 1011 = King
// Fifth bit is team
// 0 = white, 1 = black
// Value: 	no piece = 0
//			white pawn = 3, white knight = 5, white bishop = 7, white rook = 9, white queen = 11, white king = 13
//			black pawn = 19, black knight = 21, black bishop = 23, black rook = 25, black queen = 27, black king = 29
class ChessBoard : public GameBoard
{
private:
	// Depends on directionalMoveGenerate
	void pieceMoveGenerate(const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos, const int v_direction, const unsigned int s_rank);
	// Called only by pieceMoveGenerate
	void directionalMoveGenerate(const uint8_t oMask, const unsigned int v_pos, const unsigned int h_pos, const int v_diff, const int h_diff, const bool repeat);
public:
	// Default Constructor. Initializes a fresh board.
	ChessBoard();
	// Initialize board. Sets up a fresh chess board.
	void initializeBoard();
	// Generate all possible moves for specified color. 
	// white = 0, black = 1
	// Return value of 0 signifies no moves.
	bool generateMoves(const bool color);	
}; 
#endif