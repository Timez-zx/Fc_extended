#include "utils.h"

int GetHash(int src, int dst, int basic){
    return src*basic+dst;
}

void GetValue(int hashValue, int basic, std::vector<int>& valueContain){
    valueContain[0] = hashValue/basic;
    valueContain[1] = hashValue%basic;
}


void PrintVectorInt(const std::vector<int> &vec) {
    std::ostream_iterator<int> it(std::cout, " ");
    std::copy(vec.begin(), vec.end(), it);
    std::cout << std::endl;
}