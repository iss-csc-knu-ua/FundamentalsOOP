#pragma once

#include <unordered_map>
#include <sstream>
#include <fstream>

#include "../doctest.h"

#include "grid.h"


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



TEST_CASE("GridStorage operations using streams") {
    GridStorage storage;

    // Creating grids using setCellValue
    Grid grid1(3, 3);
    grid1.setCellValue(0, 0, 0);
    grid1.setCellValue(0, 1, 1);
    grid1.setCellValue(0, 2, 0);
    grid1.setCellValue(1, 0, 1);
    grid1.setCellValue(1, 1, 1);
    grid1.setCellValue(1, 2, 1);
    grid1.setCellValue(2, 0, 0);
    grid1.setCellValue(2, 1, 0);
    grid1.setCellValue(2, 2, 1);

    Grid grid2(3, 3);
    grid2.setCellValue(0, 0, 1);
    grid2.setCellValue(0, 1, 0);
    grid2.setCellValue(0, 2, 1);
    grid2.setCellValue(1, 0, 0);
    grid2.setCellValue(1, 1, 1);
    grid2.setCellValue(1, 2, 0);
    grid2.setCellValue(2, 0, 1);
    grid2.setCellValue(2, 1, 1);
    grid2.setCellValue(2, 2, 0);

    CHECK(storage.size() == 0);

    // Adding first grid
    storage.addGrid(grid1);
    CHECK(storage.size() == 1);
    CHECK(storage[grid1] == 1);

    // Adding the same grid again
    storage.addGrid(grid1);
    CHECK(storage.size() == 1);
    CHECK(storage[grid1] == 2);

    // Adding second grid
    storage.addGrid(grid2);
    CHECK(storage.size() == 2);
    CHECK(storage[grid2] == 1);

    // Testing the output operator <<
    std::ostringstream outStream;
    outStream << storage;

    std::ostringstream compareResults;
    bool isInitialOrder = compareStrings( outStream.str(),
                                        "0 1 0 \n1 1 1 \n0 0 1 \ncount: 2\n"
                                        "1 0 1 \n0 1 0 \n1 1 0 \ncount: 1\n",
                                        compareResults);                                   
    bool isReverseOrder = compareStrings( outStream.str(),
                                        "1 0 1 \n0 1 0 \n1 1 0 \ncount: 1\n"
                                        "0 1 0 \n1 1 1 \n0 0 1 \ncount: 2\n",
                                        compareResults);
    bool isValid = isInitialOrder || isReverseOrder;  

    if (! isValid) {
        std::cout<<compareResults.str();
        std::cout.flush(); // to be sure that contents are displayed, because we don't need std::endl here
    }                                        

    CHECK(isValid);

    // Testing the input operator >>
    std::istringstream inStream("0 1 0 \n1 1 1 \n0 0 1 \ncount: 3\n"
                                 "1 0 1 \n0 1 0 \n1 1 0 \ncount: 1\n");
    GridStorage newStorage;
    inStream >> newStorage;

    CHECK(newStorage.size() == 2);
    CHECK(newStorage[grid1] == 3);
    CHECK(newStorage[grid2] == 1);

}