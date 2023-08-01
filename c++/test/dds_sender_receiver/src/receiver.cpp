
#include <stdio.h>

#include "pub.cpp"
#include "sub.cpp"


int main(
        int argc,
        char** argv)
{   
    time_t start,finish;
    if (argc < 4) {std::cout << "Please provide DOMAIN PARTITIONS TOPIC" << std::endl;return 1;}
    int domain = atoi(argv[1]); 
    std::string inputString1 = argv[2]; 
    std::string inputString2 = argv[3];
    char* partition = const_cast<char*>(inputString1.c_str());
    char* topic = const_cast<char*>(inputString2.c_str());
    // starting a pub
    int samples = 100;
    HelloWorldPublisher* mypub = new HelloWorldPublisher();
    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    auto retpub = mypub->init(domain,partition,const_cast<char*>("return"),"from_receiver");
    auto retsub = mysub->init(domain,partition,topic);
    if (retpub && retsub)
    {
        for (size_t ii =0 ; ii< samples; ii++){
            std::chrono::high_resolution_clock::time_point finish = mysub->run();
            std::cout << "R] recived" << std::endl; 
            std::chrono::high_resolution_clock::time_point start = mypub->run(static_cast<uint32_t>(0));
            std::cout << "R] sent" << std::endl;

        }
    }
    delete mysub;
    delete mypub;
    return 0;
};