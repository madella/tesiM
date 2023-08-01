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

#include "HelloWorldPubSubTypes.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>

#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::rtps;

class HelloWorldPublisher
{
private:

    HelloWorld hello_;

    DomainParticipant* participant_;

    Publisher* publisher_;

    Topic* topic_;

    DataWriter* writer_;

    TypeSupport type_;

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
                //std::cout << "Publisher matched." << std::endl;
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

    //!Initialize the publisher
    bool init(int domain,char* partition,char* topic, std::string message)
    {
        hello_.index(0);
        hello_.message(message);
        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");        
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration_announcementperiod = Duration_t(3, 2);
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;

        if (false)
        {
            participantQos.wire_protocol().builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
            participantQos.wire_protocol().builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
            participantQos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
            participantQos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
            participantQos.transport().use_builtin_transports = false;
            auto sm_transport = std::make_shared<SharedMemTransportDescriptor>();
            sm_transport->segment_size(2 * 1024 * 1024);
            participantQos.transport().user_transports.push_back(sm_transport);
        }
        participant_ = DomainParticipantFactory::get_instance()->create_participant(domain, participantQos);
        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type

        type_.register_type(participant_);

        // Create the publications Topic
        topic_ = participant_->create_topic(topic, "HelloWorld", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }
        PublisherQos publisherQos = PUBLISHER_QOS_DEFAULT; 
        publisherQos.partition().push_back(partition);  
        // Create the Publisher
        publisher_ = participant_->create_publisher(publisherQos, nullptr);

        if (publisher_ == nullptr)
        {
            return false;
        }


        // Create the DataWriter
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &listener_);

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
    struct metrics_return{
        unsigned long long cycles;
        unsigned long long instructions;
        std::chrono::high_resolution_clock::time_point sent_time;
    } metrics ;

    int get_pe(unsigned long long config){
            struct perf_event_attr pe;
            memset(&pe, 0, sizeof(struct perf_event_attr));
            pe.type = PERF_TYPE_HARDWARE;
            pe.config = PERF_COUNT_HW_REF_CPU_CYCLES; // TSC TIME_STAMP_COUNT? (CYCLE); REFERENCE CYCLE;  instruction; ABS time with  time.time()// Or any other desired event
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

    metrics_return publish()
    {
        if (listener_.matched_ > 0)
        {
            hello_.index(hello_.index() + 1);
             
            auto pe_cycles=get_pe(PERF_COUNT_HW_REF_CPU_CYCLES);
            auto pe_instruction=get_pe(PERF_COUNT_HW_INSTRUCTIONS);
            ioctl(pe_instruction, PERF_EVENT_IOC_RESET, 0);
            ioctl(pe_cycles, PERF_EVENT_IOC_RESET, 0);
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            ioctl(pe_instruction, PERF_EVENT_IOC_ENABLE, 0);
            ioctl(pe_cycles, PERF_EVENT_IOC_ENABLE, 0);
            // Start monitoring
            writer_->write(&hello_);
            // Stop monitoring
            ioctl(pe_cycles, PERF_EVENT_IOC_DISABLE, 0);
            ioctl(pe_instruction, PERF_EVENT_IOC_DISABLE, 0);
            unsigned long long cycles = read_hw_counter(pe_cycles);
            unsigned long instructions = read_hw_counter(pe_cycles);
            close(pe_cycles);
            close(pe_instruction);
            // Return result to print out
            metrics.cycles=cycles;
            metrics.instructions=instructions;
            metrics.sent_time= start;
            return metrics;
        }
        metrics.cycles=-1;
        metrics.instructions=-1;
        time_t dummytime(0);
        metrics.sent_time = std::chrono::high_resolution_clock::from_time_t(dummytime);
        return metrics;
    }

    metrics_return run(uint32_t samples)
    {   
        bool sent(false);
        while (!sent){
            auto ret=publish();
            if (ret.cycles != -1){
                // std::cout << ret.cycles << std::endl;
                sent=true;
            } 
        }
        return metrics;
    }
};
