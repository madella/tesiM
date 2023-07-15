#include "pub.cpp"
#include "sub.cpp"

int main(
        int argc,
        char** argv)
{
    int samples = 1000;
    
    
    HelloWorldPublisher* mypub = new HelloWorldPublisher();
    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    
    if(mypub->init(1) && mysub->init(0))
    {   
        for (size_t ii =0 ; ii< samples; ii++){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            auto hello_rcvd = mysub->run(samples);
            std::cout << "sub index:" << hello_rcvd.index() << std::endl;

            if (!hello_rcvd.message().empty()){
                mypub->run(static_cast<uint32_t>(samples),hello_rcvd);
            }
            else{
                std::cout << "sub received nullptr" << std::endl;
            }
        }

    }



    delete mysub;
    return 0;
}