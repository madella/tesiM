// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorldSubscriber.cpp
 *
 */

#include "HelloWorldPubSubTypes.h"

#include <inttypes.h>
#include <functional>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <ctime>
#include <fstream>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::rtps;
struct received_time_struct{
    timespec time;
    unsigned long index;
};

class HelloWorldSubscriber
{
private:

    DomainParticipant* participant_;

    Subscriber* subscriber_;

    DataReader* reader_;

    Topic* topic_;

    TypeSupport type_;

    class SubListener : public DataReaderListener
    {
    public:

        SubListener()
            : samples_(0)
        {
        }

        ~SubListener() override
        {
        }
        void on_subscription_matched(
                DataReader*,
                const SubscriptionMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                this->count+=1;
            }
            else if (info.current_count_change == -1)
            {
                this->count-=1;
                //std::cout << "Publisher exited" << this->count <<std::endl;

            }
            if (this->count == 0){
                this->pubPresent=false;
                return;
            }
        }
        
        void on_data_available(
                DataReader* reader) override
        {
            SampleInfo info;
            if (reader->take_next_sample(&hello_, &info) == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    
                    clock_gettime(CLOCK_MONOTONIC,&received_time_.time);

                    received_time_.index=hello_.index();
                    received_vector_.push_back(received_time_);
                    samples_++;
                    //std::cout << "RECEIVED" << std::endl;
                }
            }
        }
        HelloWorld hello_;
        std::atomic_int samples_;
        bool pubPresent;
        int count;
        received_time_struct received_time_;
        std::vector<received_time_struct> received_vector_; 

    } listener_;

public:

    HelloWorldSubscriber()
        : participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , reader_(nullptr)
        , type_(new HelloWorldPubSubType())
    {
    }

    virtual ~HelloWorldSubscriber(){
        if (reader_ != nullptr)
        {
            subscriber_->delete_datareader(reader_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        if (subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the subscriber
    bool init(std::string transport,std::string partition,std::string ip, uint16_t port){
        DomainParticipantQos participantQos;
        participantQos.name("transport_custom");
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;

        if ("tcp" == transport){
            std::cout << "used tcp" << std::endl;
            Locator initial_peer_locator;
            initial_peer_locator.kind = LOCATOR_KIND_TCPv4;
            std::shared_ptr<TCPv4TransportDescriptor> descriptor = std::make_shared<TCPv4TransportDescriptor>();
            IPLocator::setIPv4(initial_peer_locator, ip);
            initial_peer_locator.port = port;
            participantQos.wire_protocol().builtin.initialPeersList.push_back(initial_peer_locator);
            participantQos.transport().use_builtin_transports = false;
            participantQos.transport().user_transports.push_back(descriptor);
        }
        else if ("udpM" == transport)
        {
            std::cout << "using udp-multicast" << std::endl;
        }
        else if ("shm" == transport)
        {
            std::cout << "using sharedMemory" << std::endl;
            participantQos.transport().use_builtin_transports = false;
            auto sm_transport = std::make_shared<SharedMemTransportDescriptor>();
            sm_transport->segment_size(2 * 1024 * 1024);
            participantQos.transport().user_transports.push_back(sm_transport);
        }
        else {
            //std::cout << "using udp" << std::endl;
        }

        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }


        listener_.pubPresent=true;
        listener_.count=0;

        // Register the Type
        type_.register_type(participant_);

        // Create the subscriptions Topic
        topic_ = participant_->create_topic("default", "HelloWorld", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Subscriber
        SubscriberQos subscriberQos = SUBSCRIBER_QOS_DEFAULT;

        subscriberQos.partition().push_back(partition.c_str());  //SET PARTITION PASSED IN argv
       
        subscriber_ = participant_->create_subscriber(subscriberQos, nullptr);

        if (subscriber_ == nullptr)
        {
            return false;
        }

        auto dr_qos = DATAREADER_QOS_DEFAULT;
        if ("udpM" == transport){
            eprosima::fastrtps::rtps::Locator_t new_multicast_locator;
            eprosima::fastrtps::rtps::IPLocator::setIPv4(new_multicast_locator, "239.255.0.4");
            new_multicast_locator.port = 7900;
            dr_qos.endpoint().multicast_locator_list.push_back(new_multicast_locator);
        }
        else if ("tcp" == transport)
        {
            //ONLY QOS setting, not really useful to tcp
            // dr_qos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
            // dr_qos.history().depth = 30;
            // dr_qos.resource_limits().max_samples = 50;
            // dr_qos.resource_limits().allocated_samples = 20;
            // dr_qos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
            // dr_qos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
        }
        
        reader_ = subscriber_->create_datareader(topic_, dr_qos, &listener_);

        if (reader_ == nullptr)
        {
            return false;
        }

        return true; }

    std::vector<received_time_struct> run(uint32_t sleep_time)
    {
        while(listener_.pubPresent)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_time));
        }
        return listener_.received_vector_; 
    }
};


uint64_t timespec_diff_nsec(const struct timespec& start, const struct timespec& end) {
    return static_cast<uint64_t>((end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec));
}


void avg_calculator(std::vector<received_time_struct> metric_vector,uint32_t samples){

    uint64_t total_time_diff = 0;
    auto last(metric_vector[1]);
    for (const auto& metric : metric_vector) {
        // Calculate time difference in nanoseconds
        uint64_t time_diff = timespec_diff_nsec(last.time, metric.time);
        total_time_diff += time_diff;
        //std::cout << "  " <<time_diff;
        last=metric;
    }
    size_t num_elements = metric_vector.size();
    if (((num_elements / samples) * 100)== 0 ){
        std::cout << "Numelements seems 0:  " << num_elements << std::endl;
    }
    double avg_time_diff = static_cast<double>(total_time_diff) / num_elements;
    // Print the averages to stdout
    std::cout << "Average Time Difference (ns): " << avg_time_diff / 1e9 << " receiving rate: " << ((num_elements / samples) * 100)<< "% " << std::endl;

}

void printDataToFile(const std::string& filename, std::vector<received_time_struct> vector_to_print ) {
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        // Print the data structure elements to the file
        for (const auto& element : vector_to_print) { // or dataDeque
            outputFile  << element.index << " received_s: " << element.time.tv_sec << " received_ns: " <<  element.time.tv_nsec << std::endl;
        }
        outputFile.close();
        //std::cout << "Data has been written to the file: " << filename << std::endl;
    } else {
        std::cout << "Error opening the file: " << filename << std::endl;
    }
}


int main(int argc, char** argv){        


    std::string usage("USAGE: transport sleep_time(nanoseconds) n_of_message partition #sub base/ tcp[ip,port]");
    std::string ip("127.0.0.1");
    uint16_t    port(5100);
    char *port_tmp;
    std::vector<received_time_struct> save;

    if (argc < 7) {
            std::cout << usage << std::endl;
            return 1;
    }

    std::string transport = argv[1]; 
    uint32_t sleep = std::stoi(argv[2]);
    uint32_t samples = std::stoi(argv[3]);

    std::string partition(argv[4]);
    std::string n_of_sub = argv[5]; 
    std::string base= argv[6];

    if ("tcp" == transport){
        if (argc < 8) {
            std::cout << "Using default port and ip" << std::endl;
        }
        else{
            ip = argv[7];
            intmax_t val = strtoimax(argv[8], &port_tmp, 10);        
            port = (uint16_t) val;
        }
    }
    
    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    auto ret = mysub->init(transport,partition,ip,port);
    if(ret)
    {
        save = mysub->run(sleep);
    }
    
    if ("part*" == partition){partition="part+";} // For not messing windows    
    avg_calculator(save,samples);
    std::string filename= base + "sub_" + transport + "_" + partition + "_" + n_of_sub +".data";
    printDataToFile(filename,save);

    delete mysub;
    return 0;
}
