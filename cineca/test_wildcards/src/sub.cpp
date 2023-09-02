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
timespec received_time_;

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
                std::cout << "Partecipant exited" << this->count <<std::endl;

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
                    
                    clock_gettime(CLOCK_REALTIME,&received_time_);


                    received_vector_.push_back(received_time_);
                    samples_++;
                    // std::cout << "Message: " << hello_.message() << " after time: " << received_time_.tv_nsec << " RECEIVED." << std::endl;
                }
                else { std::cout << "message not valid" << std::endl;}
            }
        }
        HelloWorld hello_;
        std::atomic_int samples_;
        bool pubPresent;
        int count;
        timespec received_time_;
        std::vector<timespec> received_vector_; 

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

    virtual ~HelloWorldSubscriber()
    {
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
    bool init(std::string transport,std::string partition,std::string ip, uint16_t port)
    {
        DomainParticipantQos participantQos;
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
            std::cout << "using udp" << std::endl;
        }
        participantQos.name("transport_custom");
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = 
        
        eprosima::fastrtps::c_TimeInfinite;
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

        return true;
    }

    //!Run the Subscriber
    std::vector<timespec> run()
    {
        while(listener_.pubPresent)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(150));
        }
        return listener_.received_vector_;
    }
};


void printDataToFile(const std::string& filename, std::vector<timespec> vector_to_print ) {
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        // Print the data structure elements to the file
        for (const auto& element : vector_to_print) { // or dataDeque
            outputFile  << "received_s: " << element.tv_sec << " received_ns: " <<  element.tv_nsec << std::endl;
        }
        outputFile.close();
        std::cout << "Data has been written to the file: " << filename << std::endl;
    } else {
        std::cout << "Error opening the file: " << filename << std::endl;
    }
}


int main(
        int argc,
        char** argv)
{        
    if (argc < 4) {
            std::cout << "USAGE: partition transport #sub group tcp[ip,port] N" << std::endl;
            return 1;
    }
    std::string ip ;
    uint16_t port ;

    std::string partition(argv[1]);
    std::string transport = argv[2]; 
    std::string n_of_sub = argv[3]; 
    std::string group = argv[4]; 

    if ("tcp" == transport){
        if (argc < 7) {
             std::cout << "USAGE: partition transport #sub group tcp[ip,port] N" << std::endl;
            return 1;
        }

        char *end;
        intmax_t val = strtoimax(argv[6], &end, 10);        
        port = (uint16_t) val;
        ip = argv[5];
        }
    else{

        ip = "nullptr";
        port = port;
    }




    std::vector<timespec> save;
    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    if(mysub->init(transport,partition,ip,port))
    {
        save = mysub->run();
    }

    delete mysub;

    std::string filename= "group" + group +"/sub_" + transport + "_" + partition + "_" + n_of_sub +".data";
    printDataToFile(filename,save);

    return 0;
}
