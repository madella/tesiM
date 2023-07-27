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
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>


using namespace eprosima::fastdds::dds;

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
    bool init(int domain,char* partition,char* topic)
    {
        hello_.index(0);
        hello_.message("perf_metric");

        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");        
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

    //!Send a publication
    long long publish()
    {
        if (listener_.matched_ > 0)
        {
            hello_.index(hello_.index() + 1);
            struct perf_event_attr pe;
            memset(&pe, 0, sizeof(struct perf_event_attr));
            pe.type = PERF_TYPE_HARDWARE;
            pe.config = PERF_COUNT_HW_BUS_CYCLES; // Or any other desired event
            pe.size = sizeof(struct perf_event_attr);
            pe.disabled = 1;
            pe.exclude_kernel = 1;
            pe.exclude_hv = 1;
            int fd = syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
            if (fd == -1) { std::cout << "open event error" <<std::endl; }
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
            //HERE
            writer_->write(&hello_);
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long cycles = read_hw_counter(fd);
            close(fd);
            return cycles;
        }
        return -1;
    }

    //!Run the Publisher
    void run(
            uint32_t samples)
    {   
        uint32_t samples_sent = 0;
        while (samples_sent < samples)
        {

            auto ret=publish();

            if (ret != -1)
            {
                samples_sent++;
                std::cout << ret << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
};

int main(
        int argc,
        char** argv)
{
        if (argc < 4) {
        std::cout << "Please provide DOMAIN PARTITIONS TOPIC" << std::endl;
        return 1;
    }

    int domain = atoi(argv[1]); 
    std::string inputString1 = argv[2]; 
    std::string inputString2 = argv[3]; 
    // std::cout << "Starting subscriber with partition " << inputString <<std::endl;
    char* partition = const_cast<char*>(inputString1.c_str());
    char* topic = const_cast<char*>(inputString2.c_str());
    int samples = 10;

    HelloWorldPublisher* mypub = new HelloWorldPublisher();

    if(mypub->init(domain,partition,topic))
    {
        mypub->run(static_cast<uint32_t>(samples));
    }

    delete mypub;
    return 0;
}
