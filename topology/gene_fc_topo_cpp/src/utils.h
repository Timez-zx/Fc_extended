#ifndef UTILS_PRINT_H_
#define UTILS_PRINT_H_
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

void PrintVectorInt(const std::vector<int> &vec);

int GetHash(int src, int dst, int basic);

void GetValue(int hashValue, int basic, std::vector<int>& valueContain);

template <typename T>
void RemoveVecEle(std::vector<T> &vec, T &value){
    vec.erase(remove(vec.begin(), vec.end(), value), vec.end()); 
}

template <typename T>
bool FindVecEle(std::vector<T> &vec, T &value){
    return (std::find(vec.begin(), vec.end(), value) != vec.end()); 
}


#endif  // UTILS_PRINT_H_