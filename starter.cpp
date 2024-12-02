#include <algorithm>
#include <iostream>
#include <vector>

// color code constants https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
// backgrounds
const std::string BG_BLACK = "\033[40m";
const std::string BG_WHITE = "\033[100m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
// colors
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
// reset colors
const std::string CLEAR = "\033[0m\033[49m";
const std::string RESET = "\033[2J\033[H";

// ascii representation of chess pieces
const std::string ROOK = "r ";
const std::string KNIGHT = "n ";
const std::string BISHOP = "b ";
const std::string QUEEN = "q ";
const std::string KING = "k ";
const std::string PAWN = "p ";
const std::string BK_ROOK = "R ";
const std::string BK_KNIGHT = "n ";
const std::string BK_BISHOP = "B ";
const std::string BK_QUEEN = "Q ";
const std::string BK_KING = "K ";
const std::string BK_PAWN = "P ";
const std::string EMPTY = ". ";
// unicode representation of chess pieces
// add an extra space after unicode characters because windows
// renders these at 1.5 size which causes overlapping
const std::string UNI_ROOK = "♜ ";
const std::string UNI_KNIGHT = "♞ ";
const std::string UNI_BISHOP = "♝ ";
const std::string UNI_QUEEN = "♛ ";
const std::string UNI_KING = "♚ ";
const std::string UNI_PAWN = "♟ ";
const std::string UNI_BK_ROOK = "♖ ";
const std::string UNI_BK_KNIGHT = "♘ ";
const std::string UNI_BK_BISHOP = "♗ ";
const std::string UNI_BK_QUEEN = "♕ ";
const std::string UNI_BK_KING = "♔ ";
const std::string UNI_BK_PAWN = "♙ ";
const std::string UNI_EMPTY = ". ";

// Position represents a coordinate position on the chessboard
struct Position {
  int x;
  int y;
  Position(int x, int y) : x(x), y(y) {}
};

// IGamePiece represents a generic chess piece
// all chess pieces inherit from this class
class IGamePiece {
public:
  // isWhite is a boolean to track what team the piece belongs to
  bool isWhite = false;
  // position represents the piece coordinates on the game board
  // this position will be updated by main() after every board update
  Position position = Position(0, 0);
  // returns the name of the gamePiece
  virtual std::string getName() = 0;
  // returns a string representing the playing piece
  virtual std::string render() = 0;
  // returns a vector of positions where the piece could move
  virtual std::vector<Position> getPotentialMoves() = 0;
  virtual ~IGamePiece() = default;
};

// BoardManager is a globally avalble object to hold information about the boardgame state
class BoardManager {
private:
  std::vector<std::vector<IGamePiece *>> board; // 2D vector grid to represent the chessboard

public:
  // prepareBoard creates a chessboard data structure
  //  implement this *after* all gamePiece declarations so that they can be referenced
  void prepareBoard();

  // getAtPosition returns a pointer to the IGamePiece located at the specified position on the board
  IGamePiece *const getAtPosition(int row, int col) {
    return board[row][col];
  }

  // renderBoard prints a colored grid to the terminal representing a chessboard with pieces
  // this also adds highlights for the cursor, selected pieces, and potential moves when applicable
  void renderBoard(Position cursor, IGamePiece *selectedPiece, std::vector<Position> moves) {
    int rows = board.size();
    // set `cols` (columns) is zero if there are no rows, prevents crash with empty boards.
    int cols = rows == 0 ? 0 : board[0].size();
    // iterate through all positions on the chessboard printing to the console
    for (int col = 0; col < cols; col++) {
      for (int row = 0; row < rows; row++) {
        for (auto &move : moves) { // if a position is in the potential move set, highlight positions in red
          if (move.x == row && move.y == col) {
            std::cout << BG_RED;
            break;
          }
        }
        if (selectedPiece != nullptr) { // if a piece is currently selected, highlight it in green
          if (selectedPiece->position.x == row && selectedPiece->position.y == col)
            std::cout << BG_GREEN;
        }
        bool highlighted = (row == cursor.x && col == cursor.y);
        if (highlighted) // use white background to create highlight effect to represent the cursor
          std::cout << BG_WHITE;
        if (board[row][col] == nullptr) { // print empty space characters
          std::cout << EMPTY;
        } else {
          // print icon for piece at current position
          std::cout << board[row][col]->render();
          // update piece position data to match current board position
          board[row][col]->position = Position(row, col);
        }
        std::cout << CLEAR << BG_BLACK; // reset text and background colors back to default
      }
      std::cout << '\r' << std::endl; // begin next row
    }
  }

  // movePiece moves an IGamePiece to a new position on the board
  // returns true on success
  bool movePiece(IGamePiece *piece, Position target) {
    // check that move is valid (by checking that the move is in the PotentialMoves list)
    bool isValidMove = false;
    for (auto &move : piece->getPotentialMoves()) {
      if (move.x == target.x && move.y == target.y) {
        isValidMove = true;
        break;
      }
    }
    if (!isValidMove)
      return false;
    // delete existing piece at new position if present
    if (board[target.x][target.y] != nullptr) {
      delete board[target.x][target.y];
      board[target.x][target.y] = nullptr;
    }
    // perform move
    Position oldPos = piece->position;
    board[target.x][target.y] = piece;   // update pointer at new board position to point to gamePiece
    board[oldPos.x][oldPos.y] = nullptr; // delete reference to piece at original board position
    return true;
  }

  // you may want to add more helper functionality here, such as checking if the a position is valid on the board
};

// declare global static boardManager for easy access anywhere
// so all gamePiece implemenentations can access this object
static BoardManager boardManager;

// represents a imaginary piece called a "Plusser"
// this is an example demonstrating how you could implement a real chess piece
class Plusser : public IGamePiece {
public:
  std::string getName() override {
    std::string color = isWhite ? "White " : "Black ";
    return color + "Plusser";
  }
  std::string render() override {
    if (isWhite) {
      return "⚔ "; // use a different symbol for each color
    } else {
      return "+ ";
    }
  }
  virtual std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    // add potential moves
    moves.push_back(Position(position.x + 1, position.y));
    moves.push_back(Position(position.x - 1, position.y));
    moves.push_back(Position(position.x, position.y + 1));
    moves.push_back(Position(position.x, position.y - 1));
    // does not have logic to prevent moves attacking own pieces
    // does not have any logic to prevent moves jumping off the board... could cause a crash.
    return moves;
  }
};

// Implement prepareBoard *after* declaring all pieces so they can be referenced here and placed on the board
void BoardManager::prepareBoard() {
  // create an 8x8 chessboard defaulting to null pointers of IGamePiece objects
  board = std::vector<std::vector<IGamePiece *>>(8, std::vector<IGamePiece *>(8, nullptr));
  // TODO add pieces to the board here
  board[4][4] = new Plusser(); // example imaginary piece (defaults to black)
  board[3][2] = new Plusser(); // example imaginary piece (white)
  board[3][2]->isWhite = true;
}

// you shouldnt need to modify main(), but you are free to change it if you want
int main() {
  system("stty raw -echo"); // Disable line buffering and echo
  printf("\033[?25l");      // hide the cursor
  printf("\033[?1049h");    // use alternate screen buffer
  // populate terminal before starting...
  std::cout << BG_BLACK << RESET; // Clear screen and use dark background
  printf("Controls: Arrow Keys, Space or Enter to Select ('q' to quit)\n\r");
  boardManager.prepareBoard();      // populate chessboard
  Position cursor = Position(0, 0); // start cursor in top left corner

  IGamePiece *selectedPiece = nullptr; // reference to the currently selected gamePiece, start deselected
  std::vector<Position> moves;         // vector of potential moves for the selectedPiece

  boardManager.renderBoard(cursor, selectedPiece, moves); // print the initial board to the console

  while (true) {             // user interaction loop
    std::string status = ""; // reset status text after any interaction
    char c = std::cin.get(); // wait for keyboard input
    // ... after user enters a key.
    std::cout << BG_BLACK << RESET;                                             // Clear screen and use dark background
    printf("Controls: Arrow Keys, Space or Enter to Select ('q' to quit)\n\r"); // print help text at top
    // process user input
    if (c == '\033') { // if control characterwas pressed, we need to capture the following control characters and
                       // process them
      char seq1 = std::cin.get();
      char seq2 = std::cin.get();
      if (seq1 == '[') { // control character may be an arrow if sequence starts with `\033[`
        switch (seq2) {  // if arrow key was pressed, move cursor around the board
        case 'A':        // up arrow
          cursor.y = std::clamp(cursor.y - 1, 0, 7);
          break;
        case 'B': // down arrow
          cursor.y = std::clamp(cursor.y + 1, 0, 7);
          break;
        case 'C': // right arrow
          cursor.x = std::clamp(cursor.x + 1, 0, 7);
          break;
        case 'D': // left arrow
          cursor.x = std::clamp(cursor.x - 1, 0, 7);
          break;
        }
        if (selectedPiece != nullptr) // if a piece is currently selected, add its name to the status message
          status += selectedPiece->getName() + " selected";
      }
    } else if (c == 'q' || c == 3) { // quit on 'q' or ctrl+c
      printf("\033[?25h");           // show the cursor
      printf("\033[?1049l");         // Restore the main screen buffer
      system("stty sane");           // Restore terminal settings
      printf("Exiting...\n");
      break;
    } else if (c == ' ') {                                 // "select" (space or return key pressed)
      if (selectedPiece != nullptr) {                      // if a piece is currently selected
        if (boardManager.movePiece(selectedPiece, cursor)) // attempt move if a potential move position is selected
          status += "Moved " + selectedPiece->getName();
        else
          status += "Deselected";
        selectedPiece = nullptr; // clear the selectedPiece and the potential moves list after any selection is made
        moves = std::vector<Position>();
      } else { // if a piece is not currently selected
        if (boardManager.getAtPosition(cursor.x, cursor.y) ==
            nullptr) { // do nothing but update status message if an empty space is selected
          status += "Empty space selected";
        } else { // set the selectedPiece reference and update the potentialmoves list for the piece
          selectedPiece = boardManager.getAtPosition(cursor.x, cursor.y);
          moves = selectedPiece->getPotentialMoves();
          status += selectedPiece->getName() + " selected";
        }
      }
    }
    boardManager.renderBoard(cursor, selectedPiece, moves); // render the board and print the status message
    std::cout << status;
  }
}
