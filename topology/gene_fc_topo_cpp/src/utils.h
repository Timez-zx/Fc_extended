#ifndef UTILS_PRINT_H_
#define UTILS_PRINT_H_
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

void PrintVectorInt(const std::vector<int> &vec);

template <typename T>
void RemoveVecEle(std::vector<T> &vec, T value){
    vec.erase(remove(vec.begin(), vec.end(), value), vec.end()); 
}


#endif  // UTILS_PRINT_H_