#include <iostream>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_COLORS_NONE
#include "../doctest.h"

class Cell {
public:
    Cell(int value = 0) : value(value) {}

    int getValue() const {
        return value;
    }

    void setValue(int newValue) {
        value = newValue;
    }

private:
    int value;
};

TEST_CASE("empty cell has value 0") {
    Cell cell;
    CHECK(cell.getValue() == 0);
}

TEST_CASE("cell constructor sets value") {
    Cell cell(5);  // Initialize cell with a value of 5
    CHECK(cell.getValue() == 5);  // Initial value should be 5
}

TEST_CASE("Set value changes value of cell") {
    Cell cell;  // Default constructor
    CHECK(cell.getValue() == 0);  // Default value should be 0

    cell.setValue(10);
    CHECK(cell.getValue() == 10);  // Value should be updated to 10
}

class Grid {
public:
    Grid(int rows, int cols) : rows(rows), cols(cols) {
        // Initialize the 2D vector with Cell objects
        cells.resize(rows, std::vector<Cell>(cols));
    }

    Cell& getCell(int row, int col) {
        // Add bounds checking
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw std::out_of_range("Cell index out of range");
        }
        return cells[row][col];
    }

    const Cell& getCell(int row, int col) const {
        // Add bounds checking
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw std::out_of_range("Cell index out of range");
        }
        return cells[row][col];
    }

    void setCellValue(int row, int col, int value) {
        getCell(row, col).setValue(value);
    }

    int getCellValue(int row, int col) const {
        return getCell(row, col).getValue();
    }

    void printGrid() const {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                std::cout << cell.getValue() << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    int rows;
    int cols;
    std::vector<std::vector<Cell>> cells;
};

TEST_CASE("Grid initialization and value setting") {
    Grid grid(3, 3);  // Create a 3x3 grid

    // Initially, all cells should have the default value of 0
    CHECK(grid.getCellValue(0, 0) == 0);
    CHECK(grid.getCellValue(1, 1) == 0);
    CHECK(grid.getCellValue(2, 2) == 0);
    CHECK(grid.getCellValue(1, 2) == 0);
}

TEST_CASE("Setting and getting cell values") {
    Grid grid(3, 3);

    grid.setCellValue(0, 0, 1);
    grid.setCellValue(1, 1, 2);
    grid.setCellValue(2, 2, 3);

    CHECK(grid.getCellValue(0, 0) == 1);
    CHECK(grid.getCellValue(1, 1) == 2);
    CHECK(grid.getCellValue(2, 2) == 3);
    CHECK(grid.getCellValue(1, 2) == 0);
}

TEST_CASE("Boundary checks") {
    Grid grid(3, 3);

    // Valid cell accesses
    CHECK_NOTHROW(grid.getCell(0, 0));
    CHECK_NOTHROW(grid.getCell(2, 2));

    // Invalid cell accesses (should throw std::out_of_range)
    CHECK_THROWS_AS(grid.getCell(3, 3), std::out_of_range);
    CHECK_THROWS_AS(grid.getCell(-1, 0), std::out_of_range);
    CHECK_THROWS_AS(grid.getCell(0, 3), std::out_of_range);
    CHECK_THROWS_AS(grid.getCell(3, 0), std::out_of_range);
}

int main(int argc, char** argv) {
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests



    Grid grid(3, 3);
    
    // Set some values
    grid.setCellValue(0, 0, 1);
    grid.setCellValue(1, 1, 2);
    grid.setCellValue(2, 2, 3);

    // Print the grid
    grid.printGrid();

    return res;
}
