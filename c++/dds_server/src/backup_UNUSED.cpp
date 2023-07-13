
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

namespace eprosima {
namespace fastdds {
namespace dds {
class DomainParticipant;
} // namespace dds
} // namespace fastdds
} // namespace eprosima

class HPC_Node_Server_Backup
{
public:

    HPC_Node_Server_Backup()
    : mp_participant(nullptr)
    {
    }
    ~HPC_Node_Server_Backup()
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
                DiscoveryProtocol_t::BACKUP;
        server_qos.name("Participant_server_backup");
        // Set SERVER's GUID prefix
        std::istringstream("44.49.53.43.53.45.52.56.45.52.5F.31") >> server_qos.wire_protocol().prefix;

        // Set SERVER's listening locator for PDP
        Locator_t locator;
        IPLocator::setIPv4(locator, 127, 0, 0, 1);
        locator.port = 11813;
        server_qos.wire_protocol().builtin.metatrafficUnicastLocatorList.push_back(locator);

        /* Add a remote serve to which this server will connect */
        // Set remote SERVER's GUID prefix
        // RemoteServerAttributes remote_server_att;
        // remote_server_att.ReadguidPrefix("44.49.53.43.53.45.52.56.45.52.5F.31");
        // Locator_t remote_locator;
        // IPLocator::setIPv4(remote_locator, 127, 0, 0, 1);
        // remote_locator.port = 11814;
        // remote_server_att.metatrafficUnicastLocatorList.push_back(remote_locator);
        // server_qos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(remote_server_att);

        RemoteServerAttributes main_server;
        main_server.ReadguidPrefix("44.53.01.5f.45.50.52.4f.53.49.4d.41");
        Locator_t main_locator;
        IPLocator::setIPv4(main_locator, 127, 0, 0, 1);
        main_locator.port = 11812;
        main_server.metatrafficUnicastLocatorList.push_back(main_locator);
        server_qos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(main_server);


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
    std::cout << "Starting server Backup." << std::endl;

    HPC_Node_Server_Backup* serv_backup = new HPC_Node_Server_Backup();
    eprosima::fastdds::rtps::Locator server_address;
    //server_address.port = 1811;
    if(serv_backup->init(server_address))
    {
        serv_backup->run();
    }

    delete serv_backup;
    return 0;
}

