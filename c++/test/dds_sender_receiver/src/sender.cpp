
#include <stdio.h>

#include "pub.cpp"
#include "sub.cpp"
#include <chrono>


int main(int argc,char** argv){   
    if (argc < 4) {std::cout << "Please provide DOMAIN PARTITIONS TOPIC" << std::endl;return 1;}
    // Input handler
    int domain = atoi(argv[1]); 
    std::string inputString1 = argv[2]; 
    std::string inputString2 = argv[3];
    char* partition = const_cast<char*>(inputString1.c_str());
    char* topic = const_cast<char*>(inputString2.c_str());
    std::vector<int> atomic_time_vector; 
    std::vector<int> global_time_vector;  
    std::vector<int> cycles_vector; 

    int samples = 100;
    HelloWorldPublisher* publisher = new HelloWorldPublisher();
    HelloWorldSubscriber* subscriber = new HelloWorldSubscriber();
    auto pubtimestart = publisher->init(domain,partition,topic,"from_sender");
    auto subtimefinish = subscriber->init(domain,partition,const_cast<char*>("return"));
    if (pubtimestart && subtimefinish)
    {
        std::cout << "Started pub with " << domain << partition << topic << std::endl;

        for (size_t ii =0 ; ii< samples; ii++){
            auto beforerun = std::chrono::high_resolution_clock::now();
            auto metrics_start = publisher->run(static_cast<uint32_t>(1));
            auto afterpub = std::chrono::high_resolution_clock::now();
            auto metrics_end = subscriber->run();
            auto afterreceived = std::chrono::high_resolution_clock::now();

            auto start=metrics_start.sent_time;
            auto end=metrics_end.sent_time;

            auto send_time= std::chrono::duration_cast<std::chrono::microseconds>(afterpub-beforerun).count();
            auto receive_time= std::chrono::duration_cast<std::chrono::microseconds>(afterreceived-afterpub).count();            
            auto global_time= std::chrono::duration_cast<std::chrono::microseconds>(afterreceived-beforerun).count();
            auto amotic_time= std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
            atomic_time_vector.push_back(amotic_time);
            global_time_vector.push_back(global_time);
        }

    }

    delete subscriber;
    delete publisher;
    return 0;
};