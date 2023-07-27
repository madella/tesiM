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
                //std::cout << "Subscriber matched.  " << this->count << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                this->count-=1;
                // std::cout << "Subscriber unmatched.  " << this->count << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
            }
            if (this->count == 0){
                // std::cout << "0 pubs remained." << std::endl;
                this->goon=false;
                return;
            }
        }
        long long read_hw_counter(int fd) {
            long long count = 0;
            read(fd, &count, sizeof(long long));
            return count;
        }
        void on_data_available(
                DataReader* reader) override
        {
            SampleInfo info;

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
            auto ret = reader->take_next_sample(&hello_, &info);
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long cycles = read_hw_counter(fd);
            close(fd);
            if (ret == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    samples_++;
                    std::cout << cycles << std::endl;
                }
            }
        }

        HelloWorld hello_;

        std::atomic_int samples_;
        
        bool goon;

        int count;

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
    bool init(int domain,char* partitions,char* topic)
    {
        DomainParticipantQos participantQos;
        participantQos.name("perf_metric");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(domain, participantQos);
        listener_.goon=true;
        listener_.count=0;
        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);
        topic_ = participant_->create_topic(topic, "HelloWorld", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr){return false;}
        SubscriberQos subscriberQos = SUBSCRIBER_QOS_DEFAULT;

        subscriberQos.partition().push_back(partitions);  //SET PARTITION PASSED IN argv
       
        subscriber_ = participant_->create_subscriber(subscriberQos, nullptr);
        if (subscriber_ == nullptr)
        {return false;}

        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);
        if (reader_ == nullptr)
        {return false;}

        return true;
    }

    //!Run the Subscriber
    void run(uint32_t samples)
    {
        while(listener_.samples_ < samples && listener_.goon)
        {
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
    int samples = 1000;

    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    if(mysub->init(domain,partition,topic))
    {
        mysub->run(static_cast<uint32_t>(samples));
    }

    delete mysub;
    return 0;
}
