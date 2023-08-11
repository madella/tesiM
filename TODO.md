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
- [ ] Dynamic types  
- [ ] [Progetto di ricerca](./research_project/TODO.md)

# August
Baically 4 things
- [ ] Google-doc skeleton of entities and protagonist envolved in HPC communication with type, request etc.
  - [x] TESTcpp create a 4 entities scheme (sender - 1 pub 1 sub on callback; receiver 1sub 1 pub for callback)
- [ ] Perform metric in which we record difference on atomic (shared_mem udp_multicas udp_unicast tcp?)
  - [x] do custom dds_on specific implementation
    - [x] tcp (some problems on different nodes)
    - [x] udp
    - [x] upd-multicast
    - [x] memshared
- [ ] Is using wildcard partition costly?
  - [x] Design a test to prove that ( maybe wit changin partition dinamically in order to see if is increasing)
    - [x] 1 dummy for discovery
    - [x] 1 pubisher with dynamic wildcard partitions
    - [x] n subscriber with fixe interconnected partitions
    - [x] An idea should be:
      - [x] 1 (N1) pub 48(N2) sub all interconnected partition with udp (DUMMY)
      - [x] 1 (N1) pub parition group* 48(N3) sub partition group0, then try to divide in multiple group the group0. For example 48/4 group0-3, then 48/8 group0-5 etc
- [ ] Study the regale lib and implement your version
- [ ] measure LOAD CORE - avg LATENCY per message- **THROUGHPUT** (maybe after?)

---

- [ ] dummy monitoring (federico)
  - [ ] + articolato (servirebbe anche un broker mqtt dummy) bridge; 
    - [ ] fastdds subscriba a tutti i topic e li manda al broker
    - [ ] è necessario poter ricostruire le informazioni di chi invia le info (lato dds)
  - [ ] guarda il derivarable
- [ ] dummy jobmanager (francesco)


## minor
- [x] remove the first data (that contain discovery latency) from measured metrics
- [x] store data out in a file, not in std-output (and manage that)
- [x] check how to change partition at runtime (on dds_partitions)
- [ ] same process qos ingore
- [x] asyncronous-sincronous mode
- [x] implement Federico's metrics
- [ ] more than 10 messages for latency & THROUGHPUT
- [ ] tcp on different galileo100's nodes

# Ask to prof.
## Andrea
- [x] Nelle prossime settimane devo essere piuttosto autonomo. Come procedo? up
- [x] Implementazione dei protocolli -> pub.cpp atomc
- [x] Devo monitorare altre metriche oltre a quelle già fatte? pub.cpp atomic
## Federico
- [x] Va bene il branch atomic che ho fatto
- [x] Ho accesso a Cineca, come fare per fare i test? -> call on _?_
- [x] devo studiare la parte di julita e luis? (not now)
  - [ ] list instead of uint32
  - [ ] tcp
  - [ ] asy-sync mode?
  - [ ] get-set
- [ ]improve atomic time empiricamente o idealmente?




LOAD CORE - avg LATENCY per message- THROUGHPUT