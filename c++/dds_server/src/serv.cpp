
#include <sstream>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.h>

#include <fastrtps/utils/IPLocator.h>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

using namespace eprosima::fastdds;
using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/rtps/common/Locator.h>


class HPC_Node_Server
{
public:

    HPC_Node_Server()
    : mp_participant(nullptr)
    {
    }
    ~HPC_Node_Server()
    {
        DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
    }

    bool init(
            Locator server_address)
    {

        // Get default participant QoS
        DomainParticipantQos server_qos = PARTICIPANT_QOS_DEFAULT;

        // Set participant as SERVER
        server_qos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                DiscoveryProtocol_t::SERVER;
        server_qos.name("Participant_server");
        // Set SERVER's GUID prefix
        std::istringstream("44.53.00.5f.45.50.52.4f.53.49.4d.41") >> server_qos.wire_protocol().prefix;

        // Set SERVER's listening locator for PDP
        Locator_t locator;
        IPLocator::setIPv4(locator, 127, 0, 0, 1);
        locator.port = 11811;
        server_qos.wire_protocol().builtin.metatrafficUnicastLocatorList.push_back(locator);

        /* Add a remote serve to which this server will connect */
        // Set remote SERVER's GUID prefix
        RemoteServerAttributes remote_server_att;
        remote_server_att.ReadguidPrefix("44.53.01.5f.45.50.52.4f.53.49.4d.41");

        // Set remote SERVER's listening locator for PDP
        Locator_t remote_locator;
        IPLocator::setIPv4(remote_locator, 127, 0, 0, 1);
        remote_locator.port = 11812;
        remote_server_att.metatrafficUnicastLocatorList.push_back(remote_locator);

        // Add remote SERVER to SERVER's list of SERVERs
        server_qos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(remote_server_att);

        // Create SERVER
        DomainParticipant* server =
                DomainParticipantFactory::get_instance()->create_participant(0, server_qos);
        if (nullptr == server)
        {
            // Error
            return false;
        }
        return true;
    }

   void run()
    {
        std::cout << "Server running. Please press enter to stop the server" << std::endl;
        std::cin.ignore();
    }

private:

    eprosima::fastdds::dds::DomainParticipant* mp_participant;
};



int main(
        int argc,
        char** argv)
{
    std::cout << "Starting server." << std::endl;

    HPC_Node_Server* serv = new HPC_Node_Server();
    eprosima::fastdds::rtps::Locator server_address;
    //server_address.port = 1811;
    if(serv->init(server_address))
    {
        serv->run();
    }

    delete serv;
    return 0;
}

