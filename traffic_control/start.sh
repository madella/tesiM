#!/bin/bash
pwd
export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"
pwd
sudo bash fastdds_generate_discovery_packages.bash /opt/ros/humble/local_setup.bash
echo "
Il risultato dell'operazione è $?
"
sudo bash fastdds_generate_discovery_packages.bash /opt/ros/humble/local_setup.bash SERVER
echo "
Il risultato dell'operazione è $?
"

python3 discovery_packets.py
