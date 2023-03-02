#include "utils.h"



void PrintVectorInt(const std::vector<int> &vec) {
    std::ostream_iterator<int> it(std::cout, " ");
    std::copy(vec.begin(), vec.end(), it);
    std::cout << std::endl;
}