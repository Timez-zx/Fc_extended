#ifndef UTILS_PRINT_H_
#define UTILS_PRINT_H_
#include <iostream>
#include <algorithm>
#include <vector>

void PrintVectorInt(const std::vector<int> &vec) {
    std::ostream_iterator<int> it(std::cout, " ");
    std::copy(vec.begin(), vec.end(), it);
    std::cout << std::endl;
}


void RemoveVecEle(std::vector<int> &vec, int value) {
    vec.erase(remove(vec.begin(), vec.end(), value), vec.end()); 
}


#endif  // UTILS_PRINT_H_