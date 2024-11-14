#include <vector>

#include "cell.h"


enum class DistanceType {
    Euclidean,
    Manhattan,
    Chebyshev
};

class NeighborhoodCalculator {
public:
    // Constructor to accept grid dimensions (could be extended for different grids)
    NeighborhoodCalculator(int rows, int cols) : rows(rows), cols(cols) {}

    // Function to calculate the neighborhood based on distance type and distance
    std::vector<std::pair<int, int>> getNeighborhoodByDistance(int row, int col, 
                                                                DistanceType distanceType, 
                                                                int distance) const {
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

    // A simpler interface for getting immediate neighbors (Chebyshev distance 1)
    std::vector<std::pair<int, int>> getNeighbors(int row, int col) const {
        return getNeighborhoodByDistance(row, col, DistanceType::Chebyshev, 1);
    }

private:
    int rows;
    int cols;
};

class Updater {
public:
    Updater(NeighborhoodCalculator& neighborhoodCalculator)
        : neighborhoodCalculator(neighborhoodCalculator) {}
        
    std::vector<std::vector<Cell>> update(const std::vector<std::vector<Cell>> cells) {
        std::vector<std::vector<Cell>> newCells = cells; // Copy current state

        for (int r = 0; r < cells.size(); ++r) {
            for (int c = 0; c < cells[0].size(); ++c) {
                int aliveNeighbors = 0;

                // Count alive neighbors using Manhattan distance
                auto neighborhood = neighborhoodCalculator.getNeighbors(r, c);
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

        return newCells;       
    }
private:
    NeighborhoodCalculator& neighborhoodCalculator;

};