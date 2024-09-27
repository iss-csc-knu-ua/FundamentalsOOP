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

// Enum to specify distance types
enum class DistanceType {
    Euclidean,
    Manhattan,
    Chebyshev,
};



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

// Function to calculate the neighborhood based on distance type and distance
std::vector<std::pair<int, int>>getNeighborhoodByDistance(int row, int col,
                                             DistanceType distanceType, int distance) {
    std::vector<std::pair<int, int>> neighborhood;

    for (int r = -distance; r <= distance; ++r) {
        for (int c = -distance; c <= distance; ++c) {
            int newRow = row + r;
            int newCol = col + c;

            // Check if the new position is within bounds
            if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols) {
                continue;
            }

            // Calculate the distance from the center cell to the current cell
            double d = 0.0;

            if (distanceType == DistanceType::Euclidean) {
                d = std::sqrt(r * r + c * c);
            } else if (distanceType == DistanceType::Manhattan) {
                d = std::abs(r) + std::abs(c);
            } else if (distanceType == DistanceType::Chebyshev) {
                d = std::max(std::abs(r), std::abs(c));
            }

            // If the distance is within the specified range, add to neighborhood
            if (d <= distance) {
                neighborhood.emplace_back(newRow, newCol);
            }
        }
    }

    return neighborhood;
}



    void printGrid() const {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                std::cout << cell.getValue() << " ";
            }
            std::cout << std::endl;
        }
    }

    // Function to print the grid, marking the neighborhood cells by their coordinates
void printGridWithNeighborhood(const std::vector<std::pair<int, int>>& neighborhoodCoords,
                                char mark = '*') {
    // Create a set of coordinates for the neighborhood cells for quick lookup
    std::set<std::pair<int, int>> neighborhoodSet(neighborhoodCoords.begin(), neighborhoodCoords.end());

    // Print the grid
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            // Check if the current cell is in the neighborhood
            if (neighborhoodSet.find({static_cast<int>(r), static_cast<int>(c)}) != neighborhoodSet.end()) {
                std::cout << mark; // Mark the neighborhood cell
            } else {
                std::cout << cells[r][c].getValue(); // Print the normal cell value
            }
        }
        std::cout << std::endl; // New line after each row
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

TEST_CASE("Get reference to cell object") {
    Grid grid(3, 3);
    CHECK(grid.getCellValue(0,0) == 0);
    grid.getCell(0,0).setValue(123); // set cell value in result of getCell, it should modify value in grid
    CHECK(grid.getCellValue(0,0) == 123);

}

// Helper function to check if a given cell is in the neighborhood
bool isCellInNeighborhood(const std::vector<std::pair<int, int>>& neighborhood, int row, int col) {
    return std::find(neighborhood.begin(), neighborhood.end(), std::make_pair(row, col)) != neighborhood.end();
}

TEST_CASE("getNeighborhoodByDistance function tests") {
    Grid grid(4, 4);
    grid.setCellValue(1,1,1);

    // Test Euclidean distance with distance 1
    auto euclideanNeighbors = grid.getNeighborhoodByDistance(1, 2, DistanceType::Euclidean, 1);
    CHECK(euclideanNeighbors.size() == 5);
    CHECK(isCellInNeighborhood(euclideanNeighbors, 1, 2)); // same cell
    CHECK(isCellInNeighborhood(euclideanNeighbors, 0, 2));
    CHECK(isCellInNeighborhood(euclideanNeighbors, 2, 2));
    CHECK(isCellInNeighborhood(euclideanNeighbors, 1, 1));    
    CHECK(isCellInNeighborhood(euclideanNeighbors, 1, 3));
    CHECK(!isCellInNeighborhood(euclideanNeighbors, 0, 0)); // Not included

    // Test Manhattan distance with distance 1
    auto manhattanNeighbors = grid.getNeighborhoodByDistance(1, 2, DistanceType::Manhattan, 1);
    CHECK(manhattanNeighbors == euclideanNeighbors); // for distance 1, Manhattan and Euclidean should be the same


    // Test Chebyshev distance with distance 1
    auto chebyshevNeighbors = grid.getNeighborhoodByDistance(1, 2, DistanceType::Chebyshev, 1);
    CHECK(chebyshevNeighbors.size() == 9);
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 1, 2)); // same cell

    CHECK(isCellInNeighborhood(chebyshevNeighbors, 0, 2));
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 2, 2));
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 1, 1));    
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 1, 3));

    CHECK(isCellInNeighborhood(chebyshevNeighbors, 0, 1));
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 0, 3));
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 2, 1));    
    CHECK(isCellInNeighborhood(chebyshevNeighbors, 2, 3));

    CHECK(!isCellInNeighborhood(chebyshevNeighbors, 0, 0)); // Not included
}

int main(int argc, char** argv) {
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests



    Grid grid(7, 7);
    
    // Set some values
    grid.setCellValue(0, 0, 1);
    grid.setCellValue(1, 1, 2);
    grid.setCellValue(2, 2, 3);

    // Print the grid
    grid.printGrid();
    std::cout<<"neighbors"<<std::endl;
    auto neighborhood = grid.getNeighborhoodByDistance(2,4,DistanceType::Euclidean,2);
    grid.printGridWithNeighborhood(neighborhood);

    return res;
}
