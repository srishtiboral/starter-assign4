/*
 * File: marbles.cpp
 * -----------------
 * Names: Anika Mukherjee, Srishti Boral
 * Course: CS 106B
 * Description: This file creates a recursive backtracking solver for the game
 * Marble Solitaire. solvePuzzle() takes a MarbleBoard and finds a sequence of
 * jumps that solves the board (aka leaves the board with only one marble remaining
 * while following the rules of the moves from the game).
 * solveHelper() does majority of the work to solve the Puzzle, using the choose-explore-unchoose
 * pattern to recursively solve each board that is passed into it.
 */

#include <iostream>
#include "backtracking.h"
#include "hashset.h"
#include "set.h"
#include "SimpleTest.h"
using namespace std;

/*
 * This function recursively tries all legal moves using the choose-explore-unchoose method,
 * and skips boards if they've already been visited before.
 * Returns: true if a sequence of moves leaves one marble on the board, false otherwise.
 * Parameters: board (current state of the board), moveHistory (moves made so far), and
 *              visited (boards already explored).
 */

bool solveHelper(MarbleBoard& board, Vector<Move>& moveHistory, Set<BoardEncoding>& visited) {
    if (board.isSolved()) {
        return true;
    } else if (visited.contains(board.encode())) {
        return false;
    } else {
        visited.add(board.encode());
        Vector<Move> possMoves = board.findPossibleMoves();
        for (Move move : possMoves) {
            board.makeMove(move);
            moveHistory.add(move);
            if (solveHelper(board, moveHistory, visited)) {
                return true;
            }
            board.undoMove(move);
            moveHistory.remove(moveHistory.size() - 1);
        }
        return false;
    }
}

/*
 * This function sets up a set of visited boards and recursively calls solveHelper to
 * search for a winning sequence of moves.
 * Returns: true if the board is solved, false otherwise
 * Parameters: board (starting state of the board) and moveHistory (filled in with the winning moves, in order).
 *
 */

bool solvePuzzle(MarbleBoard& board, Vector<Move>& moveHistory) {
    Set<BoardEncoding> visited;
    return solveHelper(board, moveHistory, visited);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("solvePuzzle: one marble in the center is already solved") {
    MarbleBoard board(loadBoard("marbles/boards/solved.txt"));
    Vector<Move> moves;
    EXPECT(solvePuzzle(board, moves));
    EXPECT_EQUAL(moves.size(), 0);
}

PROVIDED_TEST("solvePuzzle: two marbles are solved with one jump") {
    Grid<Marble> original = loadBoard("marbles/boards/one-jump.txt");
    MarbleBoard board(original);
    Vector<Move> moves;
    EXPECT(solvePuzzle(board, moves));
    EXPECT_EQUAL(moves.size(), 1);
    EXPECT_EQUAL(checkSolution(original, moves), "");
}


PROVIDED_TEST("solvePuzzle: an unsolvable board returns false and is left unchanged") {
    MarbleBoard board(loadBoard("marbles/boards/five-plus.txt"));
    MarbleBoard original = board;
    Vector<Move> moves;
    EXPECT(!solvePuzzle(board, moves));
    EXPECT_EQUAL(board, original);
    EXPECT_EQUAL(board.numMarbles(), original.numMarbles());
}

PROVIDED_TEST("solvePuzzle: solvable mid-size boards") {
    for (string name : { "cross", "seven-step", "plus", "double-cross"}) {
        Grid<Marble> original = loadBoard("marbles/boards/" + name + ".txt");
        MarbleBoard board(original);
        Vector<Move> moves;
        EXPECT(solvePuzzle(board, moves));
        EXPECT_EQUAL(checkSolution(original, moves), "");
    }
}

PROVIDED_TEST("solvePuzzle: mid-size unsolvable board") {
    MarbleBoard board(loadBoard("marbles/boards/tangle.txt"));
    Vector<Move> moves;
    EXPECT(!solvePuzzle(board, moves));
}

PROVIDED_TEST("solvePuzzle: the full 32-marble board") {
    MarbleBoard board;
    Vector<Move> moves;
    bool solved = false;
    TIME_OPERATION(32, solved = solvePuzzle(board, moves));
    EXPECT(solved);
    EXPECT_EQUAL(checkSolution(fullBoard(), moves), "");
}

PROVIDED_TEST("solvePuzzle: unsolvable 15-marble board (needs the explored-boards optimization)") {
    // A solver that does not skip boards it has already explored will take
    // a very, very long time on this test.
    MarbleBoard board(loadBoard("marbles/boards/letter-l.txt"));
    Vector<Move> moves;
    bool solved = true;
    TIME_OPERATION(15, solved = solvePuzzle(board, moves));
    EXPECT(!solved);
}

// STUDENT TESTS

STUDENT_TEST("solvePuzzle: one marble not in the center is already solved") {
    MarbleBoard board(loadBoard("marbles/boards/off-center.txt"));
    Vector<Move> moves;
    EXPECT(solvePuzzle(board, moves));
    EXPECT_EQUAL(moves.size(), 0);
}
STUDENT_TEST("solvePuzzle: three-step board needs backtracking and takes 3 moves") {
    Grid<Marble> original = loadBoard("marbles/boards/three-step.txt");
    MarbleBoard board(original);
    Vector<Move> moves;
    EXPECT(solvePuzzle(board, moves));
    EXPECT_EQUAL(moves.size(), 3);
    EXPECT_EQUAL(checkSolution(original, moves), "");
}
STUDENT_TEST("solvePuzzle: boards where no jump is possible cannot be solved") {
    for (string name : {"no-diagonals", "off-the-edge"}) {
        MarbleBoard board(loadBoard("marbles/boards/" + name + ".txt"));
        Vector<Move> moves;
        EXPECT(!solvePuzzle(board, moves));
    }
}
STUDENT_TEST("solvePuzzle: more solvable boards from the boards folder") {
    for (string name : {"letter-i", "pyramid", "half-done"}) {
        Grid<Marble> original = loadBoard("marbles/boards/" + name + ".txt");
        MarbleBoard board(original);
        Vector<Move> moves;
        EXPECT(solvePuzzle(board, moves));
        EXPECT_EQUAL(checkSolution(original, moves), "");
    }
}


