/*
 * TODO: remove and replace this file header comment
 * You will edit and turn in this file.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "grid.h"
#include "gridgraphics.h"
#include "SimpleTest.h"
using namespace std;


/* TODO: Design and implement helpers
 * We offer some suggested helper prototypes. You are free to
 * to modify and extend on these suggestions as you see fit.
 *
 * NOTE: the prototype of required function canRedistrict must not be changed.
 */
int subgridSum(Grid<int>& g, GridLocationRange range) {
    int sum = 0;
    for (GridLocation loc: range){
        sum += g[loc];
    }
    return sum;
}

void divideHorizontal(GridLocationRange range, int indexOfBottomMostRowInTopPart, GridLocationRange& topPart, GridLocationRange& bottomPart) {
    GridLocation topStartLoc(range.startRow(), range.startCol());
    GridLocation topEndLoc(indexOfBottomMostRowInTopPart, range.endCol());
    GridLocation bottomStartLoc(indexOfBottomMostRowInTopPart + 1, range.startCol());
    GridLocation bottomEndLoc(range.endRow(), range.endCol());

    topPart = GridLocationRange (topStartLoc, topEndLoc);
    bottomPart = GridLocationRange(bottomStartLoc, bottomEndLoc);

}

void divideVertical(GridLocationRange range, int indexOfRightMostColInLeftSide, GridLocationRange& leftSide, GridLocationRange& rightSide) {
    GridLocation leftStartLoc(range.startRow(), range.startCol());
    GridLocation leftEndLoc(range.endRow(), indexOfRightMostColInLeftSide);
    GridLocation rightStartLoc(range.startRow() , indexOfRightMostColInLeftSide + 1);
    GridLocation rightEndLoc(range.endRow(), range.endCol());

    leftSide = GridLocationRange (leftStartLoc, leftEndLoc);
    rightSide = GridLocationRange (rightStartLoc, rightEndLoc);
}


/*
 * TODO: Replace this comment with a descriptive function
 * header comment.
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


bool canRedistrict(Grid<int>& populationGrid, int nDistricts) {
    GridLocationRange totalRange = populationGrid.locations();
    int target = subgridSum(populationGrid, totalRange) / nDistricts;
    return redistrictHelper(populationGrid, totalRange, target);
}


/* * * * * * Test Cases * * * * * */

// STUDENT TESTS
STUDENT_TEST("divideHorizontal splits ranges correctly") {
    GridLocationRange top, bottom;

    // Case 1: full 4x3 range, split after row 1
    GridLocationRange range1(0, 0, 3, 2);
    divideHorizontal(range1, 1, top, bottom);
    EXPECT_EQUAL(top.startLocation(), GridLocation(0, 0));
    EXPECT_EQUAL(top.endLocation(), GridLocation(1, 2));
    EXPECT_EQUAL(bottom.startLocation(), GridLocation(2, 0));
    EXPECT_EQUAL(bottom.endLocation(), GridLocation(3, 2));

    // Case 2: split at the last valid row (bottom part is one row)
    divideHorizontal(range1, 2, top, bottom);
    EXPECT_EQUAL(top.endLocation(), GridLocation(2, 2));
    EXPECT_EQUAL(bottom.startLocation(), GridLocation(3, 0));
    EXPECT_EQUAL(bottom.numRows(), 1);

    // Case 3: offset range (rows 2-5, cols 1-3), split after row 3
    GridLocationRange range2(2, 1, 5, 3);
    divideHorizontal(range2, 3, top, bottom);
    EXPECT_EQUAL(top.startLocation(), GridLocation(2, 1));
    EXPECT_EQUAL(top.endLocation(), GridLocation(3, 3));
    EXPECT_EQUAL(bottom.startLocation(), GridLocation(4, 1));
    EXPECT_EQUAL(bottom.endLocation(), GridLocation(5, 3));

    // Sanity check: the two pieces together cover the original
    EXPECT_EQUAL(top.size() + bottom.size(), range2.size());
}

STUDENT_TEST("divideVertical splits ranges correctly") {
    GridLocationRange left, right;

    // Case 1: full 3x4 range, split after column 1
    GridLocationRange range1(0, 0, 2, 3);
    divideVertical(range1, 1, left, right);
    EXPECT_EQUAL(left.startLocation(), GridLocation(0, 0));
    EXPECT_EQUAL(left.endLocation(), GridLocation(2, 1));
    EXPECT_EQUAL(right.startLocation(), GridLocation(0, 2));
    EXPECT_EQUAL(right.endLocation(), GridLocation(2, 3));

    // Case 2: split at the first column (left part is one column)
    divideVertical(range1, 0, left, right);
    EXPECT_EQUAL(left.endLocation(), GridLocation(2, 0));
    EXPECT_EQUAL(left.numCols(), 1);
    EXPECT_EQUAL(right.startLocation(), GridLocation(0, 1));

    // Case 3: split at the last valid column (right part is one column)
    divideVertical(range1, 2, left, right);
    EXPECT_EQUAL(left.endLocation(), GridLocation(2, 2));
    EXPECT_EQUAL(right.startLocation(), GridLocation(0, 3));
    EXPECT_EQUAL(right.numCols(), 1);

    // Case 4: offset range (rows 1-3, cols 2-5), split after column 3
    GridLocationRange range2(1, 2, 3, 5);
    divideVertical(range2, 3, left, right);
    EXPECT_EQUAL(left.startLocation(), GridLocation(1, 2));
    EXPECT_EQUAL(left.endLocation(), GridLocation(3, 3));
    EXPECT_EQUAL(right.startLocation(), GridLocation(1, 4));
    EXPECT_EQUAL(right.endLocation(), GridLocation(3, 5));

    // Sanity check: the two pieces together cover the original
    EXPECT_EQUAL(left.size() + right.size(), range2.size());
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
