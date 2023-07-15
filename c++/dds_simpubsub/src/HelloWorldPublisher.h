#ifndef HELLOWROLDPUB
#define HELLOWORLDPUB

#include "HelloWorldPubSubTypes.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

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

        PubListener();
        ~PubListener();

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info)

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

    virtual ~HelloWorldPublisher(){
    }

    //!Initialize the publisher
    bool init(int domainID)
    {    }

    //!Send a publication
    bool publish()
    {    }

    //!Run the Publisher
    void run(uint32_t samples)
    {    }
};

#endif