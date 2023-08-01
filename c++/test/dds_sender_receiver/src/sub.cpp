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
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::rtps;

class HelloWorldSubscriber
{
private:
    DomainParticipant *participant_;

    Subscriber *subscriber_;

    DataReader *reader_;

    Topic *topic_;

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
            DataReader *,
            const SubscriptionMatchedStatus &info) override
        {
            if (info.current_count_change == 1)
            {
                this->count += 1;
                // std::cout << "Subscriber matched.  " << this->count << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                this->count -= 1;
                // std::cout << "Subscriber unmatched.  " << this->count << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                          << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
            }
            if (this->count == 0)
            {
                // std::cout << "0 pubs remained." << std::endl;
                this->goon = false;
                return;
            }
        }

        void on_data_available(
            DataReader *reader) override
        {
            SampleInfo info;

            auto ret = reader->take_next_sample(&hello_, &info);
            // std::cout << hello_.message() << " " << hello_.index() << std::endl;
            this->received_time = std::chrono::high_resolution_clock::now();
            this->goon = false;
        }

        HelloWorld hello_;

        std::atomic_int samples_;

        bool goon;

        std::chrono::high_resolution_clock::time_point received_time;

        int count;

    } listener_;

public:
    HelloWorldSubscriber()
        : participant_(nullptr), subscriber_(nullptr), topic_(nullptr), reader_(nullptr), type_(new HelloWorldPubSubType())
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

    //! Initialize the subscriber
    bool init(int domain, char *partitions, char *topic)
    {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");        
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
        topic_ = participant_->create_topic(topic, "HelloWorld", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr)
        {
            return false;
        }
        SubscriberQos subscriberQos = SUBSCRIBER_QOS_DEFAULT;

        subscriberQos.partition().push_back(partitions); // SET PARTITION PASSED IN argv

        subscriber_ = participant_->create_subscriber(subscriberQos, nullptr);
        if (subscriber_ == nullptr)
        {
            return false;
        }

        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);
        if (reader_ == nullptr)
        {
            return false;
        }

        return true;
    }

    //! Run the Subscriber
    std::chrono::high_resolution_clock::time_point run()
    {
        listener_.goon = true;
        while (listener_.goon)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        return std::chrono::high_resolution_clock::time_point(listener_.received_time);
    }
};