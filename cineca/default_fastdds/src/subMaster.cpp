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
 * @file HelloWorldPublisher.cpp
 *
 */
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <thread>
#include <inttypes.h>
#include "HelloWorldPubSubTypes.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <ctime>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/ioctl.h>
#include <fstream>
#include <unistd.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::rtps;

struct metrics_return{
    uint64_t cycles;
    uint64_t instructions;
    struct timespec start_time, end_time;
};

struct received_time_struct{
    timespec time;
    unsigned long index;
};

class HelloWorldPublisher
{
private:

    HelloWorld hello_;

    DomainParticipant* participant_;

    Publisher* publisher_;

    Topic* topic_;

    DataWriter* writer_;

    TypeSupport type_;

    metrics_return metrics_ ;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                // std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                //std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        std::atomic_int matched_;

    } listener_;

public:

    HelloWorldPublisher()
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , type_(new HelloWorldPubSubType())
    {
    }

    virtual ~HelloWorldPublisher()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    bool init(std::string transport,std::string partition,std::string ip, uint16_t port)
    {
        hello_.index(0);
        hello_.message("customTOPIC");
        // Init DPQos
        DomainParticipantQos participantQos;
        participantQos.name("transport_custom");
        // Set leaseDuration (for discovery)
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
        // Setting up the transport protocol
        if ("tcp" == transport){
            std::cout << "used tcp" << std::endl;
            participantQos.transport().use_builtin_transports = false;
            std::shared_ptr<TCPv4TransportDescriptor> descriptor = std::make_shared<TCPv4TransportDescriptor>();
            descriptor->sendBufferSize = 0;
            descriptor->receiveBufferSize = 0;
            descriptor->add_listener_port(port);
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



        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the publications Topic
        topic_ = participant_->create_topic("default", "HelloWorld", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Publisher

        PublisherQos publisherQos = PUBLISHER_QOS_DEFAULT; 
        publisherQos.partition().push_back(partition.c_str()); 
        publisher_ = participant_->create_publisher(publisherQos, nullptr);

        if (publisher_ == nullptr)
        {
            return false;
        }


        DataWriterQos wqos=DATAWRITER_QOS_DEFAULT;
        if ("tcp" == transport){
            // wqos.history().kind = KEEP_LAST_HISTORY_QOS;
            // wqos.history().depth = 30;
            // wqos.resource_limits().max_samples = 50;
            // wqos.resource_limits().allocated_samples = 20;
            // wqos.reliable_writer_qos().times.heartbeatPeriod.seconds = 2;
            // wqos.reliable_writer_qos().times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
            // wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        }
        wqos.publish_mode().kind= SYNCHRONOUS_PUBLISH_MODE; //  ensure sync mode
        writer_ = publisher_->create_datawriter(topic_, wqos, &listener_);

        if (writer_ == nullptr)
        {
            return false;
        }
        return true;
    }

    long long read_hw_counter(int fd) {
        long long count = 0;
        read(fd, &count, sizeof(long long));
        return count;
    }

    inline __attribute__((always_inline)) void read_tsc(uint64_t* tsc) {
        uint64_t a;
        uint64_t d;
        __asm__ volatile("rdtsc"                  : \
                         "=a" (a), "=d" (d));

        *tsc = (a | (d << 32));
    }
    
    //TODO:  clock_gettime con delle strutture "timespec".
    
    inline __attribute__((always_inline)) int get_pe(unsigned long long config){
            struct perf_event_attr pe;
            memset(&pe, 0, sizeof(struct perf_event_attr));
            pe.type = PERF_TYPE_HARDWARE;
            pe.config = PERF_COUNT_HW_REF_CPU_CYCLES; // TSC rdTSC ;TIME_STAMP_COUNTER? (CYCLE); REFERENCE CYCLE;  instruction; ABS time with  time.time()// Or any other desired event
            pe.size = sizeof(struct perf_event_attr);
            pe.disabled = 1;
            pe.exclude_kernel = 1;
            pe.exclude_hv = 1;
            int fd = syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
            if (fd == -1) { std::cout << "open event error; set kernel.perf_event_paranoid = -1" <<std::endl; return -1; }
            else{
                return fd;
            }
    }

    metrics_return get_metric(){
        return metrics_;
    }

    bool publish(uint32_t index)
    {
        if (listener_.matched_ > 0)
        {
            // std::cout << "size of message: %u" << sizeof(hello_); 
            hello_.index(index);
            uint64_t tsc_start(0),tsc_end(0);
            // Get PE syscall
            auto pe_instruction=get_pe(PERF_COUNT_HW_INSTRUCTIONS);
            // 
            ioctl(pe_instruction, PERF_EVENT_IOC_RESET, 0);
            clock_gettime(CLOCK_MONOTONIC, &metrics_.start_time);
            ioctl(pe_instruction, PERF_EVENT_IOC_ENABLE, 0);

            read_tsc(&tsc_start);
            writer_->write(&hello_);
            read_tsc(&tsc_end);
            
            ioctl(pe_instruction, PERF_EVENT_IOC_DISABLE, 0);
            clock_gettime(CLOCK_MONOTONIC, &metrics_.end_time);
            metrics_.instructions = read_hw_counter(pe_instruction);
            close(pe_instruction);
            metrics_.cycles=tsc_end-tsc_start;
            return true;
        }
        return false;
    }
};

HelloWorldPublisher* pubcallback;

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
                DataReader* reader
                ) override
        {
            SampleInfo info;
            if (reader->take_next_sample(&hello_, &info) == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    pubcallback->publish(hello_.index()); 
                    // clock_gettime(CLOCK_MONOTONIC,&received_time_.time);

                    // received_time_.index=hello_.index();
                    // received_vector_.push_back(received_time_);
                    // samples_++;
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
        }HelloWorldPublisher* pubcallback;

    }
    
    auto subscriber = new HelloWorldSubscriber();
    pubcallback = new HelloWorldPublisher();

    auto ret = pubcallback->init(transport,"ret",ip,port);
    auto ret2 = subscriber->init(transport,"publish",ip,port);


    if(ret && ret2)
    {
        save = subscriber->run(sleep);
    }
    
    delete subscriber;
    delete pubcallback;
    return 0;
}
