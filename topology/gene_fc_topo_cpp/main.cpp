#include <sys/time.h>
#include <iostream>
#include <unistd.h>
 
using namespace std;




int main(){
    struct timeval start, end;
    gettimeofday(&start, NULL);



    gettimeofday(&end, NULL);
    cout << "Time use: " << (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/double(1e6) << "s" << endl;
    return 0;
}
