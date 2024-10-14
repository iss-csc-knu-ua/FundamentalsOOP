#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>

#include <cassert>

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

    // Implementing the equality operator
    bool operator==(const Cell& other) const {
        return value == other.value;
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

TEST_CASE("compare vector of cells") {
    std::vector<Cell> vector1 { Cell(1), Cell(2), Cell(3)};
    std::vector<Cell> vector2 { Cell(1), Cell(2), Cell(3)};
    CHECK(vector1 == vector2);
}

// Enum to specify distance types
enum class DistanceType {
    Euclidean,
    Manhattan,
    Chebyshev,
};


class Region {
public:
    std::vector<std::pair<int, int>> coordinates;

    void addCell(int row, int col) {
        coordinates.emplace_back(row, col);
    }
};



class Grid {
private:
    int rows;
    int cols;
    std::vector<std::vector<Cell>> cells;

friend Grid convertRegionToGrid(const Grid& originalGrid, const Region& region);

public:
    Grid(int rows, int cols) : rows(rows), cols(cols) {
        // Initialize the 2D vector with Cell objects
        cells.resize(rows, std::vector<Cell>(cols));
    }

    bool isValidCoordinates(int row, int col) const {
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            return false;
        } 
        return true;
    }

    Cell& getCell(int row, int col) {
        // Add bounds checking
        if (!isValidCoordinates(row,col)) {
            throw std::out_of_range("Cell index out of range");
        }
        return cells[row][col];
    }

    const Cell& getCell(int row, int col) const {
        // Add bounds checking
        if (!isValidCoordinates(row,col)) {
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

    int getRows() const { return rows;}

    int getCols() const { return cols;}

    // Function to calculate the neighborhood based on distance type and distance
    std::vector<std::pair<int, int>>getNeighborhoodByDistance(int row, int col,
                                                DistanceType distanceType, int distance) const {
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

    std::vector<std::pair<int, int>> getNeighbors(int row, int col) const {
        return getNeighborhoodByDistance(row, col, DistanceType::Chebyshev, 1);
    }

    // returns true if next state is different from previous state, false if they are the same
    bool update() {
        std::vector<std::vector<Cell>> newCells = cells; // Copy current state

        for (int r = 0; r < cells.size(); ++r) {
            for (int c = 0; c < cells[0].size(); ++c) {
                int aliveNeighbors = 0;

                // Count alive neighbors using Manhattan distance
                auto neighborhood = getNeighbors(r, c);
                for (const auto& neighbor : neighborhood) {
                    if (cells[neighbor.first][neighbor.second].getValue() == 1) {
                        aliveNeighbors++;
                    }
                }

                // Apply Game of Life rules
                if (cells[r][c].getValue() == 1) {
                    // Cell is currently alive
                    if (aliveNeighbors < 3 || aliveNeighbors > 4) { // if this cell is alive, aliveNeighbors includes itself, so we add 1
                        newCells[r][c].setValue(0); // Die
                    }
                } else {
                    assert(cells[r][c].getValue() == 0); // all cells should be either 0 (dead) or 1 (alive)
                    // Cell is currently dead
                    if (aliveNeighbors == 3) {
                        newCells[r][c].setValue(1); // Become alive
                    }
                }
            }
        }
        if (cells == newCells) {
           return false;
        }

        cells = newCells; // Update to new state
        return true;
    }

    void fillGridWithRandomValues(const std::vector<int>& values, const std::vector<double>& probabilities) {
        // Check that probabilities sum to 1
        double totalProbability = 0.0;
        for (double prob : probabilities) {
            totalProbability += prob;
        }
        
        if (totalProbability != 1.0) {
            throw std::invalid_argument("Probabilities must sum to 1.");
        }

        // Create a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        // Create a cumulative distribution based on probabilities
        std::vector<double> cumulativeProbabilities(probabilities.size());
        cumulativeProbabilities[0] = probabilities[0];
        for (size_t i = 1; i < probabilities.size(); ++i) {
            cumulativeProbabilities[i] = cumulativeProbabilities[i - 1] + probabilities[i];
        }

        // Fill the grid
        for (auto& row : cells) {
            for (auto& cell : row) {
                double randomValue = dis(gen);
                int valueToSet = values.back(); // Default to last value
                
                for (size_t i = 0; i < cumulativeProbabilities.size(); ++i) {
                    if (randomValue <= cumulativeProbabilities[i]) {
                        valueToSet = values[i];
                        break;
                    }
                }

                cell.setValue(valueToSet);
            }
        }
    }

    void findRegions(int startX, int startY, std::vector<std::vector<bool>>& visited,
                     Region& region) {
        std::stack<std::pair<int, int>> stack;
        stack.push({startX, startY});
        visited[startX][startY] = true;
        region.addCell(startX, startY); // Add starting cell to the region

        while (!stack.empty()) {
            auto [x, y] = stack.top();
            stack.pop();

            for (const auto& neighbor : getNeighbors(x, y)) {
                int newX = neighbor.first;
                int newY = neighbor.second;

                if (cells[newX][newY].getValue() != 0 && !visited[newX][newY]) { // Check if the neighbor is alive (non-zero)
                    visited[newX][newY] = true;
                    region.addCell(newX, newY); // Add to the region
                    stack.push({newX, newY});
                }
            }
        }
    }

    std::vector<Region> getNonInteractingRegions() {
        std::vector<Region> regions;
        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (cells[i][j].getValue() != 0 && !visited[i][j]) { // Check if the cell is "alive" (non-zero value)
                    Region region;
                    findRegions(i, j, visited, region);
                    regions.push_back(region);
                }
            }
        }

        return regions;
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

    std::string gridToString() const {
        std::string result;
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                result += std::to_string(cell.getValue()) + " ";
            }
            result += "\n";
        }
        return result;
    }

    void printRegions(const std::vector<Region>& regions) {
        std::vector<std::vector<char>> regionGrid(rows, std::vector<char>(cols, '.'));

        char regionChar = 'A';
        for (const auto& region : regions) {
            for (const auto& cell : region.coordinates) {
                regionGrid[cell.first][cell.second] = regionChar; // Mark the region
            }
            regionChar++; // Move to the next character
        }

        // Output the region grid
        for (const auto& row : regionGrid) {
            for (const auto& cell : row) {
                std::cout << std::setw(2) << cell; // Format output for better visibility
            }
            std::cout << std::endl;
        }
    }


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

TEST_CASE("blinker has period of 2") {
    Grid grid(3, 3);

    // vertical
    grid.setCellValue(0, 1, 1);
    grid.setCellValue(1, 1, 1);
    grid.setCellValue(2, 1, 1);

    bool hasChanged = false;

    hasChanged = grid.update();
    CHECK(hasChanged); // changed

    // horizontal
    CHECK(grid.getCellValue(0,1) == 0);
    CHECK(grid.getCellValue(1,1) == 1);
    CHECK(grid.getCellValue(2,1) == 0);
    CHECK(grid.getCellValue(1,0) == 1);
    CHECK(grid.getCellValue(1,2) == 1); 

    hasChanged = grid.update();
    CHECK(hasChanged); // changed

    // back to vertical
    CHECK(grid.getCellValue(0,1) == 1);
    CHECK(grid.getCellValue(1,1) == 1);
    CHECK(grid.getCellValue(2,1) == 1);
    CHECK(grid.getCellValue(1,0) == 0);
    CHECK(grid.getCellValue(1,2) == 0);    

}

TEST_CASE("block is still life") {
    Grid grid(4, 4);

    // block without one cell
    grid.setCellValue(1, 1, 1);
    grid.setCellValue(1, 2, 1);
    grid.setCellValue(2, 1, 1);

    bool hasChanged = false;

    hasChanged = grid.update();
    CHECK(hasChanged); // changed

    // block
    CHECK(grid.getCellValue(1,1) == 1);
    CHECK(grid.getCellValue(2,1) == 1);
    CHECK(grid.getCellValue(1,2) == 1); 
    CHECK(grid.getCellValue(2,2) == 1); 

    hasChanged = grid.update();
    CHECK(! hasChanged); // no longer changing
  
    CHECK(grid.getCellValue(1,1) == 1);
    CHECK(grid.getCellValue(2,1) == 1);
    CHECK(grid.getCellValue(1,2) == 1); 
    CHECK(grid.getCellValue(2,2) == 1); 

}

TEST_CASE("Test Single Live Cell") {
    Grid grid(3, 3);
    grid.setCellValue(1, 1, 1); // Set a non-zero value
    auto regions = grid.getNonInteractingRegions();
    CHECK(regions.size() == 1);
    CHECK(regions[0].coordinates.size() == 1);
    CHECK(regions[0].coordinates[0] == std::make_pair(1, 1));
}

TEST_CASE("Test Two Separate Regions") {
    Grid grid(4, 4);
    grid.setCellValue(0, 1, 1);
    grid.setCellValue(0, 2, 2);
    grid.setCellValue(1, 1, 1);

    grid.setCellValue(3, 2, 3);
    grid.setCellValue(2, 3, 3);

    auto regions = grid.getNonInteractingRegions();
    CHECK(regions.size() == 2);
    CHECK(regions[0].coordinates.size() == 3); // First region (3 cells)
    CHECK(regions[1].coordinates.size() == 2); // Second region (2 cells)
    CHECK(regions[0].coordinates[0] == std::make_pair(0, 1));
    CHECK(regions[1].coordinates[0] == std::make_pair(2, 3));
}

TEST_CASE("Test Entire Grid is Alive") {
    Grid grid(2, 2);
    grid.setCellValue(0, 0, 1);
    grid.setCellValue(0, 1, 1);
    grid.setCellValue(1, 0, 1);
    grid.setCellValue(1, 1, 1);

    auto regions = grid.getNonInteractingRegions();
    CHECK(regions.size() == 1);
    CHECK(regions[0].coordinates.size() == 4); // All cells are "alive"
}

TEST_CASE("Test No Live Cells") {
    Grid grid(3, 3);
    auto regions = grid.getNonInteractingRegions();
    CHECK(regions.size() == 0); // No regions
}

Grid convertRegionToGrid(const Grid& originalGrid, const Region& region) {
    // Calculate the size of the new grid based on the region
    int maxRow = -1, maxCol = -1;
    int minRow = originalGrid.getRows(), minCol = originalGrid.getCols();
    if (region.coordinates.empty()) {
        return Grid(0,0); // empty grid - no cells
    }

    // Determine the bounding box of the region
    for (const auto& coord : region.coordinates) {
        if (!originalGrid.isValidCoordinates(coord.first, coord.second)) { continue; }
        minRow = std::min(minRow, coord.first);
        minCol = std::min(minCol, coord.second);
        maxRow = std::max(maxRow, coord.first);
        maxCol = std::max(maxCol, coord.second);
    }

    if (minRow > originalGrid.getRows()-1 || minCol > originalGrid.getCols()-1) {
        return Grid(0,0); // all cells in region outside borders of grid
    }

    // Create a new grid for the region
    Grid regionGrid(maxRow - minRow + 1, maxCol - minCol + 1);

    // Populate the new grid with cells from the original grid that are in the region
    for (const auto& coord : region.coordinates) {
        int row = coord.first - minRow; // Adjust row index
        int col = coord.second - minCol; // Adjust col index
        if (!regionGrid.isValidCoordinates(row, col)) {continue;}
        
        // Assuming originalGrid has valid cells for the given coordinates
        regionGrid.cells[row][col] = originalGrid.cells[coord.first][coord.second];
    }

    return regionGrid;
}

TEST_CASE("Test convertRegionToGrid function") {
    // Setup an original grid
    Grid originalGrid(4, 4);
    originalGrid.setCellValue(0, 0, 1);
    originalGrid.setCellValue(0, 1, 2);
    originalGrid.setCellValue(1, 0, 3);
    originalGrid.setCellValue(1, 1, 4);
    originalGrid.setCellValue(2, 1, 5);
    originalGrid.setCellValue(3, 0, 6);
    originalGrid.setCellValue(3, 3, 7);

    SUBCASE("Normal case with a region covering part of the grid") {
        Region region1;
        region1.addCell(0, 0);
        region1.addCell(1, 1);
        region1.addCell(2, 1);

        Grid regionGrid1 = convertRegionToGrid(originalGrid, region1);

        CHECK(regionGrid1.getRows() == 3);
        CHECK(regionGrid1.getCols() == 2);
        CHECK(regionGrid1.getCellValue(0, 0) == 1); // (0,0)
        CHECK(regionGrid1.getCellValue(1, 1) == 4); // (1,1)
        CHECK(regionGrid1.getCellValue(2, 1) == 5); // (2,1)
        // cells not in region get values 0, even if present in original grid
        CHECK(regionGrid1.getCellValue(1, 0) == 0);
        CHECK(originalGrid.getCellValue(1, 0) == 3);
    }

    SUBCASE("Edge case with empty region") {
        Region emptyRegion;
        Grid emptyGrid = convertRegionToGrid(originalGrid, emptyRegion);
        
        CHECK(emptyGrid.getRows() == 0);
        CHECK(emptyGrid.getCols() == 0);
    }

    SUBCASE("Region with cells at the edges of the grid") {
        Region region2;
        region2.addCell(0, 0);
        region2.addCell(3, 3);

        Grid regionGrid2 = convertRegionToGrid(originalGrid, region2);

        CHECK(regionGrid2.getRows() == 4);
        CHECK(regionGrid2.getCols() == 4);
        CHECK(regionGrid2.getCellValue(0, 0) == 1); // (0,0)
        CHECK(regionGrid2.getCellValue(3, 3) == 7); // (3,3)
    }

    SUBCASE("Region that is completely outside of the original grid") {
        Region region3;
        region3.addCell(5, 5);
        Grid regionGrid3 = convertRegionToGrid(originalGrid, region3);

        CHECK(regionGrid3.getRows() == 0);
        CHECK(regionGrid3.getCols() == 0);
    }

    SUBCASE("Region with some (but not all) cells outside of the original grid") {
        Region region;
        region.addCell(2, 1);
        region.addCell(5, 5);
        Grid regionGrid = convertRegionToGrid(originalGrid, region);

        CHECK(regionGrid.getRows() == 1);
        CHECK(regionGrid.getCols() == 1);
        CHECK(regionGrid.getCellValue(0, 0) == 5); // the only cell from region that is within original grid
    }
}

class GridStorage {
private:
    std::unordered_map<std::string, int> gridMap;

public:

    size_t size() const {
        return gridMap.size();
    }
    int& operator[](const Grid& grid) {
        return gridMap[grid.gridToString()];
    }

    // Adds a new grid or increments the count of an existing grid
    void addGrid(const Grid& grid) {
        std::string gridString = grid.gridToString();
        gridMap[gridString]++;
    }

    //Overload the << operator for writing to a stream
    friend std::ostream& operator<<(std::ostream& out, const GridStorage& storage) {
        for (const auto& entry : storage.gridMap) {
            out << entry.first; // Write the entire grid string; it always ends with \n 
            out << "count: " << entry.second << "\n"; // Store instance count
        }
        return out;
    }

    // Saves all unique grids to a file
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }

        file << *this;

        file.close();
    }

    // Overload the >> operator for reading from a stream
    friend std::istream& operator>>(std::istream& in_stream, GridStorage& storage) {
        std::string line;
        std::string currentGrid;

        while (std::getline(in_stream, line)) {
            if (line.find("count:") == 0) {
                int count = std::stoi(line.substr(7)); // Parse the count
                if (!currentGrid.empty()) {
                    storage.gridMap[currentGrid] = count; // Store the grid with its count
                }
                currentGrid.clear(); // Reset for the next grid
            } else {
                currentGrid += line+"\n"; // Append the grid line
            }
        }
        return in_stream;
    }

    // Loads grids from a file
    void loadFromFile(const std::string& filename, bool errorOnMissingFile=true) {
        std::ifstream file(filename);
        if (!file) {
            if (errorOnMissingFile) {
                std::cerr << "Error opening file for reading." << std::endl;
            }
            return;
        }

        file >> *this;


        file.close();
    }

    // Displays all unique grids and their counts
    void displayGrids() const {
        for (const auto& entry : gridMap) {
            std::cout << "Grid:\n" << entry.first << "\nCount: " << entry.second << std::endl;
        }
    }
};


TEST_CASE("GridStorage functionality") {
    GridStorage storage;
    Grid grid1(2,2); 
    grid1.setCellValue(0,1,1);
    grid1.setCellValue(1,0,1);
    Grid grid2(1,3);
    grid2.setCellValue(0,2,1);

    CHECK(storage.size()==0); // empty storage
    storage.addGrid(grid1);
    CHECK(storage.size()==1); 
    storage.addGrid(grid1); // Adding grid1 again
    CHECK(storage.size()==1); 
    storage.addGrid(grid2);

    SUBCASE("Adding grids and checking counts") {


    }

    SUBCASE("Saving and loading from file") {

        // Save to a temporary file
        const std::string filename = "test_grids.txt";
        storage.saveToFile(filename);

        // Create a new storage object to load the file
        GridStorage newStorage;
        newStorage.loadFromFile(filename);

        // Check loaded data
        CHECK(newStorage.size() == 2);
        CHECK(newStorage[grid1] == 2);
        CHECK(newStorage[grid2] == 1);

        // Clean up the temporary file
        std::remove(filename.c_str());
    }

    SUBCASE("Display function output") {

        std::ostringstream oss;
        std::streambuf* original = std::cout.rdbuf(oss.rdbuf());

        storage.displayGrids(); // Capture output to stringstream

        std::cout.rdbuf(original); // Restore original cout

        std::string output = oss.str();
        CHECK(output.find("Grid:") != std::string::npos);
        CHECK(output.find("Count: 2") != std::string::npos);
        CHECK(output.find("Count: 1") != std::string::npos);
        CHECK(output.find(grid1.gridToString()) != std::string::npos);
        CHECK(output.find(grid2.gridToString()) != std::string::npos);
    }

    // grid1 should have count 2, grid2 should have count 1
    CHECK(storage.size() == 2);
    CHECK(storage[grid1] == 2);
    CHECK(storage[grid2] == 1);
}


int main(int argc, char** argv) {
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests



    Grid grid(7, 7);
    
    // Set some values
    // grid.setCellValue(1, 2, 1);
    // grid.setCellValue(1, 3, 1);
    // grid.setCellValue(1, 4, 1);
    int regionsFound = 0;

    GridStorage storage;
    storage.loadFromFile("regions_found.txt",false); // no error when file is missing - just start with empty storage

    while (regionsFound < 2) {

        grid.fillGridWithRandomValues({0, 1}, {0.5, 0.5});

        // Print the grid
        // grid.printGrid();
        // std::cout<<"neighbors"<<std::endl;
        // auto neighborhood = grid.getNeighborhoodByDistance(2,4,DistanceType::Euclidean,2);
        // grid.printGridWithNeighborhood(neighborhood);

        auto regions = grid.getNonInteractingRegions();
        std::cout<<"Found regions: " << regions.size()<<std::endl;
        grid.printRegions(regions);


        std::string previousState;
        std::string stateBeforePrevious;
        
        for (int generation = 0; generation < 30; ++generation) {
            std::cout << "Generation " << generation << ":\n";
            grid.printGrid();
            bool hasChanged = grid.update();
            if (!hasChanged) {
                std::cout<<"simulation ended after " << generation << " steps"<<std::endl;
                break;
            }
            std::string currentState = grid.gridToString();
            
            // Check for repetition with period 2
            if (generation >= 2 && currentState == stateBeforePrevious) {
                std::cout << "Simulation ended due to repeating grid state after " << generation << " generations." << std::endl;
                break;
            }

            // Update states for the next iteration
            stateBeforePrevious = previousState;
            previousState = currentState;
        }

        regions = grid.getNonInteractingRegions();
        regionsFound = regions.size();
        std::cout<<"Found regions: " << regionsFound<<std::endl;
        grid.printRegions(regions);
        for (const auto& region: regions) {
            std::cout<<"Region grid"<<std::endl;
            Grid regionGrid = convertRegionToGrid(grid, region);
            storage.addGrid(regionGrid);
            regionGrid.printGrid();
            std::cout<<"Region grid"<<std::endl;
            regionGrid.update();
            regionGrid.printGrid();

        }
    }
    std::cout<<"regions found:"<<std::endl;
    storage.displayGrids();
    storage.saveToFile("regions_found.txt");

    return res;
}
