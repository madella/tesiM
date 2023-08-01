# General
- [ ] Documentation of ROS2 impl.
- [ ] Compare RWM_IMPL and Stack FastDDS, if cost-benefit is ok
  - [ ] Try with rwm tests
  - [ ] implement libraries and use it from custom c++
- [x] Show which actor are present on specific topic
- [x] Vedere che HW ha E4 : lscpu , lshw 
- [x] Try shared auto shm_transport 
- [ ] get-set function prototype
- [ ] Realtime message (only qos?)
- [ ] CHeck tcp, udp, shmem
- [ ] Dynamic types  
- [ ] [Progetto di ricerca](./research_project/TODO.md)

# August
Baically 4 scheme
- [ ] Google-doc skeleton of entities and protagonist envolved in HPC communication with type, request etc.
  - [x] TESTcpp create a 4 entities scheme (sender - 1 pub 1 sub on callback; receiver 1sub 1 pub for callback)
- [ ] Perform metric in which we record difference on atomic (shared_mem udp_multicas udp_unicast tcp?)
- [ ] Is using wildcard partition costly?
  - [ ] Design a test to prove that ( maybe wit changin partition dinamically in order to see if is increasing)
    - [ ] 1 dummy for discovery
    - [ ] 1 pubisher with dynamic wildcard partitions
    - [ ] n subscriber with fixe interconnected partitions
- [ ] measure LOAD CORE - avg LATENCY per message- **THROUGHPUT** (maybe after?)

## minor
- [ ] remove the first data (that contain discovery latency) from measured metrics
- [ ] store data out in a file, not in output (and manage that)
- [ ] chechk how to store the metrics isntead of printing on stdout?
- [x] check how to change partition at runtime (on dds_partitions)
- [ ] same process qos ingore


# Ask to prof.
## Andrea
- [x] Nelle prossime settimane devo essere piuttosto autonomo. Come procedo? up
- [x] Implementazione dei protocolli -> pub.cpp atomc
- [x] Devo monitorare altre metriche oltre a quelle già fatte? pub.cpp atomic
## Federico
- [x] Va bene il branch atomic che ho fatto
- [x] Ho accesso a Cineca, come fare per fare i test? -> call on _?_



LOAD CORE - avg LATENCY per message- THROUGHPUT