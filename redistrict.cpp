/* File: redistrict.cpp
 * Names: Anika Mukherjee, Srishti Boral
 * Course: CS 106B
 * Description: This file uses recursive backtracking to check whether a
 * state's population grid can be split into equal-population rectangular
 * districts using only horizontal and vertical cuts (the HVS algorithm).
 * subgridSum, divideHorizontal, and divideVertical are helpers, and
 * redistrictHelper tries every possible cut on each rectangle.
 */

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "grid.h"
#include "gridgraphics.h"
#include "SimpleTest.h"
using namespace std;


/*
 * Returns the total population inside a rectangular range of the grid
 * by adding up the value at every location in the range.
 * Parameters: g (the population grid), range (the rectangle to add up)
 * Returns: the sum of all values in the range
 */
int subgridSum(Grid<int>& g, GridLocationRange range) {
    int sum = 0;
    for (GridLocation loc: range){
        sum += g[loc];
    }
    return sum;
}

/* Cuts a range with a horizontal line into a top part and a bottom part.
 * Both parts keep the same columns as the original range.
 * Parameters: range (the rectangle to cut), indexOfBottomMostRowInTopPart
 * (the last row in the top part), topPart and bottomPart (set to the two pieces)
 */
void divideHorizontal(GridLocationRange range, int indexOfBottomMostRowInTopPart, GridLocationRange& topPart, GridLocationRange& bottomPart) {
    GridLocation topStartLoc(range.startRow(), range.startCol());
    GridLocation topEndLoc(indexOfBottomMostRowInTopPart, range.endCol());
    GridLocation bottomStartLoc(indexOfBottomMostRowInTopPart + 1, range.startCol());
    GridLocation bottomEndLoc(range.endRow(), range.endCol());

    topPart = GridLocationRange (topStartLoc, topEndLoc);
    bottomPart = GridLocationRange(bottomStartLoc, bottomEndLoc);

}
/*
 * Cuts a range with a vertical line into a left side and a right side.
 * Both sides keep the same rows as the original range.
 * Parameters: range (the rectangle to cut), indexOfRightMostColInLeftSide
 * (the last column in the left side), leftSide and rightSide (set to the two pieces)
 */
void divideVertical(GridLocationRange range, int indexOfRightMostColInLeftSide, GridLocationRange& leftSide, GridLocationRange& rightSide) {
    GridLocation leftStartLoc(range.startRow(), range.startCol());
    GridLocation leftEndLoc(range.endRow(), indexOfRightMostColInLeftSide);
    GridLocation rightStartLoc(range.startRow() , indexOfRightMostColInLeftSide + 1);
    GridLocation rightEndLoc(range.endRow(), range.endCol());

    leftSide = GridLocationRange (leftStartLoc, leftEndLoc);
    rightSide = GridLocationRange (rightStartLoc, rightEndLoc);
}


/*
 * Recursive helper for canRedistrict. Returns whether a rectangle can be cut
 * into districts that each have exactly target people. If the rectangle's
 * population equals target, it is one district (success). If the population
 * is not a multiple of target, it can never be split evenly (failure).
 * Otherwise it tries every horizontal and vertical cut, and succeeds if both
 * pieces of some cut can be redistricted.
 *
 * Parameters: populationGrid (the population grid), range (the rectangle
 * being divided), target (the population every district must have)
 *
 * Returns: true if the rectangle can be split into districts, false otherwise
 */
bool redistrictHelper(Grid<int>& populationGrid, GridLocationRange range, int target){
    int population = subgridSum(populationGrid, range);
    if (population == target) {
        return true;
    }
    if (population % target != 0) {
        return false;
    }

    // horizontal cuts
    for (int r = range.startRow(); r < range.endRow(); r++) {
        GridLocationRange top, bottom;
        divideHorizontal(range, r, top, bottom);
        if (redistrictHelper(populationGrid, top, target) && redistrictHelper(populationGrid, bottom, target)) {
            return true;
        }
    }

    // vertical cuts
    for (int c = range.startCol(); c < range.endCol(); c++) {
        GridLocationRange left, right;
        divideVertical(range, c, left, right);
        if (redistrictHelper(populationGrid, left, target) && redistrictHelper(populationGrid, right, target)) {
            return true;
        }
    }
    return false;
}

/*
 * Returns whether the grid can be split into nDistricts rectangular districts
 * of equal population. It works out the target size for each district
 * (total population / nDistricts) and asks redistrictHelper whether the whole
 * grid can be split into districts of that size.
 *
 * Parameters: populationGrid (people living in each cell), nDistricts (how many districts to make)
 *
 * Returns: true if a valid redistricting was found, false otherwise
 * Assumes nDistricts > 0 and the total population divides evenly by it.
  */
bool canRedistrict(Grid<int>& populationGrid, int nDistricts) {
    GridLocationRange totalRange = populationGrid.locations();
    int target = subgridSum(populationGrid, totalRange) / nDistricts;
    return redistrictHelper(populationGrid, totalRange, target);
}


/* * * * * * Test Cases * * * * * */

STUDENT_TEST("subgridSum on a single cell") {
    Grid<int> grid = { {1, 2}, {3, 4} };
    EXPECT_EQUAL(subgridSum(grid, GridLocationRange(1, 1, 1, 1)), 4);
}

STUDENT_TEST("subgridSum on one row") {
    Grid<int> grid = { {1, 2}, {3, 4} };
    EXPECT_EQUAL(subgridSum(grid, GridLocationRange(0, 0, 0, 1)), 3);
}
STUDENT_TEST("divideHorizontal splits a range into top and bottom") {
    GridLocationRange range(0, 0, 3, 2);
    GridLocationRange top, bottom;
    divideHorizontal(range, 1, top, bottom);
    EXPECT_EQUAL(top.startLocation(), GridLocation(0, 0));
    EXPECT_EQUAL(top.endLocation(), GridLocation(1, 2));
    EXPECT_EQUAL(bottom.startLocation(), GridLocation(2, 0));
    EXPECT_EQUAL(bottom.endLocation(), GridLocation(3, 2));
}
STUDENT_TEST("divideVertical splits a range into left and right") {
    GridLocationRange range(0, 0, 2, 3);
    GridLocationRange left, right;
    divideVertical(range, 1, left, right);
    EXPECT_EQUAL(left.startLocation(), GridLocation(0, 0));
    EXPECT_EQUAL(left.endLocation(), GridLocation(2, 1));
    EXPECT_EQUAL(right.startLocation(), GridLocation(0, 2));
    EXPECT_EQUAL(right.endLocation(), GridLocation(2, 3));
}

STUDENT_TEST("divideHorizontal on a range that does not start at the top left") {
    GridLocationRange range(2, 1, 5, 3);
    GridLocationRange top, bottom;
    divideHorizontal(range, 3, top, bottom);
    EXPECT_EQUAL(top.endLocation(), GridLocation(3, 3));
    EXPECT_EQUAL(bottom.startLocation(), GridLocation(4, 1));
}

STUDENT_TEST("canRedistrict with 1 district") {
    Grid<int> grid = { {3, 7}, {6, 3} };
    EXPECT(canRedistrict(grid, 1));
}
STUDENT_TEST("canRedistrict where every cell is its own district") {
    Grid<int> grid = { {4, 4}, {4, 4} };
    EXPECT(canRedistrict(grid, 4));
}
STUDENT_TEST("canRedistrict on a row that cannot be split") {
    Grid<int> grid = { {1, 2, 3} };
    EXPECT(!canRedistrict(grid, 3));
}


// PROVIDED TESTS
PROVIDED_TEST("subgridSum sums values in grid range") {
    // This is an example test of helper function subgridSum
    Grid<int> grid = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 10, 10} };
    GridLocationRange all = grid.locations();
    GridLocationRange firstColumn = GridLocationRange(0, 0, 3, 0);
    GridLocationRange bottomCorner = GridLocationRange(3, 2, 3, 2);

    EXPECT_EQUAL(subgridSum(grid, firstColumn), 22);
    EXPECT_EQUAL(subgridSum(grid, bottomCorner), 10);
    EXPECT_EQUAL(subgridSum(grid, all), 75);
}

PROVIDED_TEST("nevada example from writeup can be divided into 4 districts") {
    Grid<int> nevada = { {20, 31, 20},
                         { 5, 19, 30},
                         {25, 25, 25} };

    initForPopulationGrid(nevada); // you can remove if you don't wish to animate this test case
    EXPECT(canRedistrict(nevada, 4));
    waitForCloseWindow();
}

PROVIDED_TEST("wyoming example from writeup cannot be divided into 2 districts") {
    Grid<int> wyoming = { {10, 15},
                          { 5, 10} };

    initForPopulationGrid(wyoming); // you can remove if you don't wish to animate this test case
    EXPECT(!canRedistrict(wyoming, 2));
    waitForCloseWindow();
}

PROVIDED_TEST("small example, 5 districts") {
    Grid<int> small = { {3, 7, 10},
                        {6, 3, 1},
                        {1, 10, 9} };
    initForPopulationGrid(small);  // you can remove if you don't wish to animate this test case
    EXPECT(canRedistrict(small, 5));
    waitForCloseWindow();
}

PROVIDED_TEST("medium example, 12 districts") {
    Grid<int> medium = { {10, 1,  2, 5, 1, 4},
                          {1, 1,  8, 2, 1, 7},
                          {9, 8, 10, 3, 3, 4},
                          {5, 2,  2, 8, 7, 3},
                          {2, 1,  1, 7, 1, 1} };
    initForPopulationGrid(medium);  // you can remove if you don't wish to animate this test case
    EXPECT(canRedistrict(medium, 12));
    waitForCloseWindow();
}

PROVIDED_TEST("large example can be divided into 25 districts") {
    Grid<int> large = { { 5,  2, 2,  1,  6, 4, 8, 2},
                        {10, 10, 2,  1,  3, 4, 2, 3},
                        { 1,  1, 4,  5,  1, 9, 1, 4},
                        { 9,  5, 5, 10, 10, 3, 3, 4},
                        { 3,  1, 1,  1,  2, 8, 6, 2},
                        { 2,  1, 1,  7,  3, 5, 1, 1},
                        { 1,  1, 2,  2,  7, 5, 1, 9},
                        { 4,  1, 2, 10,  1, 9, 5, 5} };
    initForPopulationGrid(large); // you can remove if you don't wish to animate this test case
    EXPECT(canRedistrict(large, 25));
    waitForCloseWindow();
}
