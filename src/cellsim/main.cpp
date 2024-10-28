#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_COLORS_NONE
#include "../doctest.h"

#include "grid.h"
#include "grid_storage.h"

int main(int argc, char** argv) {
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests

    //return res;


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