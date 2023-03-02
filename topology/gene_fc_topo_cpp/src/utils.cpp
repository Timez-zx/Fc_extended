#include "utils.h"



void PrintVectorInt(const std::vector<int> &vec) {
    std::ostream_iterator<int> it(std::cout, " ");
    std::copy(vec.begin(), vec.end(), it);
    std::cout << std::endl;
}


void RemoveVecEle(std::vector<int> &vec, int value) {
    vec.erase(remove(vec.begin(), vec.end(), value), vec.end()); 
}