#include <iostream>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_NONE
#include "doctest.h"

// Function to swap two elements
void swap(std::vector<int>& arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Function to partition the array
int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high]; // Choose the last element as pivot
    int i = low - 1; // Index of the smaller element

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr, i, j);
        }
    }
    swap(arr, i + 1, high);
    return i + 1;
}

// QuickSort function
void quickSort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high); // Partitioning index

        quickSort(arr, low, pi - 1); // Recursively sort the left sub-array
        quickSort(arr, pi + 1, high); // Recursively sort the right sub-array
    }
}

// Function to perform selection sort
void selectionSort(std::vector<int>& arr) {
    int n = arr.size();
    
    // One by one move boundary of unsorted subarray
    for (int i = 0; i < n - 1; ++i) {
        // Find the minimum element in unsorted array
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        
        // Swap the found minimum element with the first element of the unsorted subarray
        if (minIndex != i) {
            std::swap(arr[i], arr[minIndex]);
        }
    }
}

// Helper function to print the array
void printArray(const std::vector<int>& arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

int main_manual() {
    std::vector<int> arr = {10, 7, 8, 9, 1, 5};
    
    std::cout << "Original array: ";
    printArray(arr);
    
    quickSort(arr, 0, arr.size() - 1);
    
    std::cout << "Sorted array: ";
    printArray(arr);
    
    return 0;
}

TEST_CASE("Test sorting for known array") {
    std::vector<int> arr = {10, 7, 8, 9, 1, 5};
    SUBCASE("quicksort") {
        quickSort(arr, 0, arr.size() - 1);
    }
    SUBCASE("selection sort") {
        selectionSort(arr);
    }
    CHECK(arr == (std::vector<int>){1, 5, 7, 8, 9, 10 });
}

TEST_CASE("Test sorting for repeated values") {
    std::vector<int> arr = {1, 1, 1, 9, 1, 1};
    quickSort(arr, 0, arr.size() - 1);
    CHECK(arr == (std::vector<int>){1, 1, 1, 1, 1, 9 });
}