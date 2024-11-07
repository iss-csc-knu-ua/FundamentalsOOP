#pragma once
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