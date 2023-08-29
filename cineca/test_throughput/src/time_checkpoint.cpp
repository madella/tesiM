
#include <ctime>
#include <iostream>
#include <fstream>

int main(int argc,char** argv){
    if (argc < 2) {
        std::cout << "USAGE: pub/sub" << std::endl;
        return 1;
    }
    timespec start_time = {0, 0};
    std::string group=".";
    std::string transport = argv[1]; 
    if ("pub" == transport){
        group="pubs";
    }

    clock_gettime(CLOCK_REALTIME, &start_time);

    std::string filename= group + "/time.checpoint";
    std::cout << "start_s: " <<  start_time.tv_sec << " start_ns: " << start_time.tv_nsec << std::endl;
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << "start_s: " <<  start_time.tv_sec << " start_ns: " << start_time.tv_nsec << std::endl;
    }
}