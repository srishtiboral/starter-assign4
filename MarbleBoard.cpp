/*
 * File: MarbleBoard.cpp
 * Names: Anika Mukherjee, Srishti Boral
 * Course: CS 106B
 * Description: This file implements the MarbleBoard class for Marble
 * Solitaire. findPossibleMoves finds every legal jump on the current board,
 * and makeMove and undoMove update the board when a jump is made or taken
 * back. The solver in marbles.cpp uses these three functions for its
 * choose-explore-unchoose search.
 */

#include "MarbleBoard.h"
#include "set.h"
#include "SimpleTest.h"
using namespace std;

/*
 * Returns every legal move on the current board. It loops over every square,
 * and for each square with a marble it checks all four directions. A jump is
 * legal if the landing square is in bounds and empty and the square in between
 * has a marble. Corner squares are MARBLE_INVALID, so they never count as empty.
 * Returns: a Vector of all legal moves, in no particular order
 */

Vector<Move> MarbleBoard::findPossibleMoves() const {
    Vector<Move> moves;
    for (int row = 0; row < _grid.numRows(); row++) {
        for (int col = 0; col < _grid.numCols(); col++) {
            GridLocation initialLoc(row, col);
            if (_grid[initialLoc] == MARBLE_OCCUPIED) {
                if (_grid.inBounds(row + 2, col) && _grid[row + 1][col] == MARBLE_OCCUPIED && _grid[row + 2][col] == MARBLE_EMPTY) { // down loc
                    moves.add( {initialLoc, GridLocation(row + 2, col)} );
                }
                if (_grid.inBounds(row, col + 2) &&_grid[row][col + 1] == MARBLE_OCCUPIED && _grid[row][col + 2] == MARBLE_EMPTY) { // right loc
                    moves.add( {initialLoc, GridLocation(row, col + 2)} );
                }
                if (_grid.inBounds(row - 2, col) && _grid[row - 1][col] == MARBLE_OCCUPIED && _grid[row - 2][col] == MARBLE_EMPTY) { // up loc
                    moves.add( {initialLoc, GridLocation(row - 2, col)} );
                }
                if (_grid.inBounds(row, col - 2) && _grid[row][col - 1] == MARBLE_OCCUPIED && _grid[row][col - 2] == MARBLE_EMPTY) { // left loc
                    moves.add( {initialLoc, GridLocation(row, col - 2)} );
                }
            }
        }
    }
    return moves;
}

/* Makes a jump. The marble at start moves to end, and the marble in between
 * (the midpoint of start and end) is removed. Updates the marble count.
 * Parameter: move (the jump to make, assumed to be legal)
 */

void MarbleBoard::makeMove(Move move) {
    GridLocation middleLoc((move.start.row + move.end.row) / 2,
                           (move.start.col + move.end.col) / 2); // grid location of middle
    _grid[move.end] = MARBLE_OCCUPIED;
    _grid[middleLoc] = MARBLE_EMPTY;
    _grid[move.start] = MARBLE_EMPTY;
    _numMarbles--;
}

/*
 * Takes back a jump. The marble at end goes back to start, and the marble
 * in between is put back. Updates the marble count.
 * Parameter: move (the jump to undo, assumed to be the last move made)
 */
void MarbleBoard::undoMove(Move move) {
    GridLocation middleLoc((move.start.row + move.end.row) / 2,
                           (move.start.col + move.end.col) / 2); // grid location of middle
    _grid[move.end] = MARBLE_EMPTY;
    _grid[middleLoc] = MARBLE_OCCUPIED;
    _grid[move.start] = MARBLE_OCCUPIED;
    _numMarbles++;
}

/* The functions below have already been written for you! */

MarbleBoard::MarbleBoard() : MarbleBoard(fullBoard()) {}   // default constructor creates the starting board

MarbleBoard::MarbleBoard(const Grid<Marble>& grid) {
    _grid = grid;
    _numMarbles = countMarbles(grid);
}

int MarbleBoard::numMarbles() const {
    return _numMarbles;
}

bool MarbleBoard::isSolved() const {
    return _numMarbles == 1;
}

Marble MarbleBoard::at(GridLocation loc) const {
    return _grid[loc];
}

BoardEncoding MarbleBoard::encode() const {
    return compressBoard(_grid);
}

Grid<Marble> MarbleBoard::toGrid() const {
    return _grid;
}

string MarbleBoard::toString() const {
    return boardToString(_grid);
}


/* * * * * * Test Cases * * * * * */

/*
 * Helper for the tests below: returns the moves as a Set, so that the tests
 * do not depend on the order in which findPossibleMoves returns its moves.
 */
static Set<Move> toSet(const Vector<Move>& moves) {
    Set<Move> result;
    for (const Move& move : moves) {
        result.add(move);
    }
    return result;
}

PROVIDED_TEST("MarbleBoard::findPossibleMoves: a lone marble has no moves") {
    MarbleBoard board(loadBoard("marbles/boards/solved.txt"));
    EXPECT_EQUAL(board.findPossibleMoves().size(), 0);
}

PROVIDED_TEST("MarbleBoard::findPossibleMoves: two marbles side by side can jump either way") {
    MarbleBoard board(boardFromStrings({
        "XX---XX",
        "XX---XX",
        "-------",
        "-OO----",
        "-------",
        "XX---XX",
        "XX---XX"
    }));
    Vector<Move> moves = board.findPossibleMoves();
    Set<Move> expected = { { {3, 1}, {3, 3} },     // left marble jumps right
                          { {3, 2}, {3, 0} } };   // right marble jumps left
    EXPECT_EQUAL(moves.size(), 2);
    EXPECT_EQUAL(toSet(moves), expected);
}

PROVIDED_TEST("MarbleBoard::findPossibleMoves: no jumping over empty holes, diagonally, off the board, or into corners") {
    // marbles with empty holes between them
    MarbleBoard spaced(boardFromStrings({
        "XXO-OXX",
        "XX---XX",
        "O-O-O-O",
        "-------",
        "O-O-O-O",
        "XX---XX",
        "XXO-OXX"
    }));
    EXPECT_EQUAL(spaced.findPossibleMoves().size(), 0);

    // marbles touching only diagonally
    MarbleBoard diagonals(loadBoard("marbles/boards/no-diagonals.txt"));
    EXPECT_EQUAL(diagonals.findPossibleMoves().size(), 0);

    // the only jumps in this line of marbles would land off the board or in a corner
    MarbleBoard edge(loadBoard("marbles/boards/off-the-edge.txt"));
    EXPECT_EQUAL(edge.findPossibleMoves().size(), 0);
}

PROVIDED_TEST("MarbleBoard::findPossibleMoves: the full board has exactly four moves, all into the center") {
    MarbleBoard board;
    Vector<Move> moves = board.findPossibleMoves();
    Set<Move> expected = { { {1, 3}, {3, 3} }, { {5, 3}, {3, 3} },
                          { {3, 1}, {3, 3} }, { {3, 5}, {3, 3} } };
    EXPECT_EQUAL(moves.size(), 4);
    EXPECT_EQUAL(toSet(moves), expected);
}

PROVIDED_TEST("MarbleBoard::makeMove and undoMove: jump down into the center, then undo") {
    MarbleBoard board;
    Move move = { {1, 3}, {3, 3} };
    board.makeMove(move);
    EXPECT_EQUAL(board.at({1, 3}), MARBLE_EMPTY);       // jumping marble left its start
    EXPECT_EQUAL(board.at({2, 3}), MARBLE_EMPTY);       // jumped-over marble is gone
    EXPECT_EQUAL(board.at({3, 3}), MARBLE_OCCUPIED);    // jumping marble landed
    EXPECT_EQUAL(board.numMarbles(), 31);

    board.undoMove(move);
    EXPECT_EQUAL(board, MarbleBoard());
    EXPECT_EQUAL(board.numMarbles(), 32);
}

PROVIDED_TEST("MarbleBoard::makeMove and undoMove: jump to the left, then undo") {
    MarbleBoard board(loadBoard("marbles/boards/one-jump.txt"));
    MarbleBoard original = board;
    Move move = { {3, 2}, {3, 0} };
    board.makeMove(move);
    EXPECT_EQUAL(board, MarbleBoard(boardFromStrings({
                            "XX---XX",
                            "XX---XX",
                            "-------",
                            "O------",
                            "-------",
                            "XX---XX",
                            "XX---XX"
                        })));
    EXPECT(board.isSolved());
    board.undoMove(move);
    EXPECT_EQUAL(board, original);
    EXPECT_EQUAL(board.numMarbles(), 2);
}

//STUDENT TESTS
STUDENT_TEST("MarbleBoard::findPossibleMoves: one marble not in the center has no moves") {
    MarbleBoard board(loadBoard("marbles/boards/off-center.txt"));
    EXPECT_EQUAL(board.findPossibleMoves().size(), 0);
}

STUDENT_TEST("MarbleBoard::findPossibleMoves: small plus sign has 4 moves, all by the center marble") {
    MarbleBoard board(loadBoard("marbles/boards/five-plus.txt"));
    EXPECT_EQUAL(board.findPossibleMoves().size(), 4);
}
STUDENT_TEST("MarbleBoard::findPossibleMoves: three-step board has 4 possible first moves") {
    MarbleBoard board(loadBoard("marbles/boards/three-step.txt"));
    EXPECT_EQUAL(board.findPossibleMoves().size(), 4);
}
STUDENT_TEST("MarbleBoard::makeMove and undoMove: jump to the right, then undo") {
    MarbleBoard board(loadBoard("marbles/boards/one-jump.txt"));
    MarbleBoard original = board;
    Move move = { {3, 1}, {3, 3} };
    board.makeMove(move);
    EXPECT(board.isSolved());
    EXPECT_EQUAL(board.at({3, 3}), MARBLE_OCCUPIED);
    board.undoMove(move);
    EXPECT_EQUAL(board, original);
}