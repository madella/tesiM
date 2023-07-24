# Tesi Magistrale FastDDS-REGALE
##### Legenda:
`
🟢 CHECKED
🟡 TO-CHEKC-FURTHER
🟣 TODO
🔴 NOT-DOABLE
⚫ NEED-CHOICE
`
## TODOs:
- 🟢 **Can the same app be subscribed to different domains?**
    Yes, I tested this morning and a single subscriber node (pure FastDDS, not using ROS) is able to subscribe (and consequently publish) to two different topics on two different DomainIDs.

- 🟢**How does ROS2 define topics and data types?**
 
    ROS2 has its own extensive library of supported type_msg. For example: ```geometry_msgs/msg/Accel.msg, geometry_msgs/msg/AccelStamped.msg, geometry_msgs/msg/AccelWithCovariance.msg, geometry_msgs/msg/AccelWithCovarianceStamped.msg, geometry_msgs/msg/Inertia.msg, geometry_msgs/msg/InertiaStamped.msg, geometry_msgs/msg/Point.msg, geometry_msgs/msg/Point32.msg, geometry_msgs/msg/PointStamped.msg, geometry_msgs/msg/Polygon.msg, geometry_msgs/msg/PolygonStamped.msg, geometry_msgs/msg/Pose.msg``` and so on. This is just for the geometry_msgs package. Additionally, under the rwm_implementation, in my understanding, everything is basically serialized/deserialized.

- 🟢⚫ **Verify communication overhead as the number of subscribers increases.**

    From the call, it seems that a distributed approach is preferable rather than having a server for the discovery phase. This approach does not have overhead issues once the network is online and all nodes are connected. However, regarding the discovery phase, it is widely discussed that a distributed approach creates significant problems as the number of nodes increases: "as the number of exchanged packets increases significantly as new nodes are added." So, every time a node is added with a large number of nodes (>500), it will definitely stress the network. 
    
    650 nodes (SDP: Distributed; DS 1/2: Server v1/2 ).We are able to use also DS2.0: ![IMAGE](image.png)


- 🟢⚫ **Regarding verifying which entities are connected on a specific topic at a specific moment.**

    I heard there was talk of checking if there was an entity x on a topic else y would take over. Verifying which entities is listening (both publishers and subscribers) to a topic is a non-trivial process. When testing in ROS2 (and this is why i believe it is also not possible in FastDDS), it is not possible to know at runtime who is connected to a topic at a FastDDS level (such as NodeManager instead of JobManager, Countdown, EAR, etc.). However, it is possible to know the number of connected nodes (#sub #pub #totsub_ever_connected #totpub_ever_connected and maybe only the UID), but identification is not performed. ROS2 offers a higher-level tool that provides a graph view of which nodes are publishers and subscribers to a topic, and I think this is done using a higher-level event-driven structure (ROS's structure).

    Based on what I've thought, it would be possible to work around this concept in at least these ways:

  - Identifying publishers (subscribers cannot be identified) in the messages they send. When messages are exchanged, a structure is defined to be exchanged, and this structure can include a field like <char[] id = "JobManagerX">.
  - Having multiple topics (e.g., JobManagerTopic, etc.) where our physical node subscribes to multiple topics. It would be possible for the node to know if there are connected subscribers to those topics and gather information from where at least one subscriber is present.

    PS: It is not possible with default rules to know which Partecipant is online on certain topic, and for example it is not possible to know for a publisher if ther's another publisher on the same Domain&Topic. You can only know how many Partecipants of the opposite type are connected. But it is possible for certain entuty to sub and pub on different topic, to know if someone is online or not (dds_simpubsub).

- 🟢🟡 **How to manage real-time pull messages?**

    I'm not sure if I understood correctly.

- 🟢 **Need to check where wildcards are defined. Are these on topic or in partitions? How we can leverage partitions to create hierarchies of topics.** Wildcards are defined only in partitions. Pretty simple, user for example perm_1 perm_2 perm_x and perm*  (dds_partitions)

- 🟢🟡 **Understand the flexibilities introduced by domains and partitions in topic based communications?**

- 🟢 **Evaluate the cost of #publish #subscribers.**
    Inside the "traffic_control" folder, there are some scripts that allow testing only on the discovery part without any use of ROS. Once the network of nodes is configured, there is no need to check how many packets are exchanged, as it depends on how they have been programmed rather than FastDDS itself. The next step is to assess the performance impact of connected nodes.

---

## Mail from Lluis Alonso

We have done a first implementation of message exchange between two of our tools and we are happy to report that the basic functionality of a "get" is working as intended. It is very much a proof of concept, but it has allowed us to see a few things that we would like to comment about the current in-progress API:

- 🟢 **We should change it to either void* or char*, depending on what is easier to implement on the library layer.**
    void* is not possible with FastDDS std, char* is doable with or with string, or with sequence<char>.

- 🟢 **With the current implementation, every topic needs to be found on the "types" file. As we discussed on the meeting, there should only be two or three different types, and what would change is only the topic itself. The naming of the topic and the type name should be decoupled so we can easily create types with data only available at runtime (like the job id). If it was not possible to separate the naming of the type with the topic name, we could approach this with a small tool that would run before starting the entity that will use those types and would generate the proper name for the topic/type combination.**
    It depends on what you want to do: If you simply want to disconnect the type name from the topic name, it is possible, provided that the data type is already instantiated and structured (IDL). For the topic, we can give it any name we want, given a certain data type. On the other hand, if we want to keep a specific topic and change the data type sent on that topic, it is not possible. Also, it is not possible to generate custom data types according to our needs (it is possible, but it requires studying, see also: https://fast-dds.docs.eprosima.com/en/latest/fastdds/dynamic_types/dynamic_types.html).
- 🟣 **A few error codes would need to be defined and used by the REGALE library. It would make for a better experience debugging and error-correcting. For example, when trying to create a publisher object a bit more information on why that is failing would save a lot of time when implementing our own publisher/subscribers. Likewise, if there was a problem when publishing data (is even possible and/or detectable?) an error describing why could be useful.**
  -  As mentioned earlier, all errors should throw error codes and provide information about the error, but the current implementation sometimes crashes when something is not being used correctly. It's not the end of the world since you would need to recompile anyway if you have an error on your code, but it is much cleaner.

- 🟢🟡 **We have run into errors when using '/' in the topic name. An easy alternative is to use underscores but we are open to any suggestions that make the names more readable and that do not cause errors.**
    I cannot repeat this error, as seen in dds_customtopicname i can start topic with 'test/one'.

- 🟣🟡 **We have found a couple of issues when naming objects with very basic nomenclature (for example, nm_init or nm_get_data) due to conflicts with existing symbols. We should take into account all projects' naming schemes so we avoid this issue (this could probably be avoided by using "regale_" at the beginning, but it was worth mentioning).**
  
- 🟣🟡**REGALE library and all its dependencies will have to be installed on any node that needs to run it. This is easily solvable by installing it on a shared folder, but it is worth mentioning.**

- 🟡 **We have run into issues when using the shared memory and TCP transport methods. Due to the way FastDDS is installed, we believe it may be caused by privilege discrepancies with the foonathan_memory library, but we have not been able to fix it yet. It would be helpful to show where in the process the error can be found to reduce the time searching for it. We were able to send messages through UDP though we do not know why.**

