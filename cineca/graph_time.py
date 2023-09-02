import re
import matplotlib.pyplot as plt
from collections import defaultdict
import os
# Step 1: Read the data from the .data file and extract the relevant information
protocols = ["upd", "udpM", "tcp", "shm"]
data_pattern = r'cycles: (\d+) instruction: (\d+) start_ns: (\d+) end_ns: (\d+) start_s: (\d+) end_s: (\d+)'
sub_pattern = r'received_s: (\d+) received_ns: (\d+)'
protocols = ["udp", "udpM", "tcp", "shm"]
protocol_data = defaultdict(list)
received_data =defaultdict(list)
# data_folder = 'data_singleshot/'
data_folder="data/transport/100shotv5/"


def timespec_difference(start_s, start_ns, end_s, end_ns):
    s_diff = end_s - start_s
    ns_diff = end_ns - start_ns
    if ns_diff < 0:
        ns_diff += 1000000000
        s_diff -= 1
    return s_diff, ns_diff
count=0
count_normal=0


for i in protocols: 
    file_path = os.path.join(data_folder, f'pub_{i}_0.data')
    # Open all publisher
    with open(file_path, 'r') as file:
        for line in file:
            match = re.search(data_pattern, line)
            if match:
                cycles, instruction, start_ns, end_ns, start_s, end_s = match.groups()
                protocol_data[i].append((int(cycles), int(instruction), int(start_s), int(start_ns), int(end_s), int(end_ns)))
    # Open all subscriber
    for j in range(46):
        file_path = os.path.join(data_folder, f'sub_{i}_{j}.data')
        try:
            count_normal+=1
            with open(file_path, 'r') as file:
                line_number=0
                for line in file:
                    match = re.search(sub_pattern, line)
                    if match:
                        received_s, received_ns = match.groups()
                        try:
                            start_s,start_ns = ((protocol_data[i][line_number][2], protocol_data[i][line_number][3]))
                            s_diff, ns_diff = timespec_difference(start_s, start_ns , int(received_s), int(received_ns))
                            if s_diff == -1: count+=1 # TODO:remove
                            received_data[i].append((s_diff, ns_diff))
                        except Exception as e: 
                            continue
                    line_number+=1
        except Exception as e:
            continue
print(count,count_normal)

# Collapse time data in difference of end - starts
for protocol in protocol_data:
    for i in range(len(protocol_data[protocol])):
        cycle_data = protocol_data[protocol][i]
        s_diff, ns_diff = timespec_difference(cycle_data[2], cycle_data[3], cycle_data[4], cycle_data[5])
        protocol_data[protocol][i] = cycle_data[:2] + (s_diff,ns_diff)


column_averages = defaultdict(lambda: [0, 0, 0, 0])

# Convert data to timespec
for protocol in received_data:
    nlen= len(received_data[protocol])
    print(protocol)
    for cycle_data in received_data[protocol]:
        column_averages[protocol][3] += cycle_data[0]  + cycle_data[1] / 1e9 # time in microseconds
        if protocol == "tcp":  column_averages[protocol][3] =0
    column_averages[protocol][3] /= nlen


num_entries = defaultdict(int)

for protocol in protocol_data:
    for cycle_data in protocol_data[protocol]:
        column_averages[protocol][0] += cycle_data[0] / 1e9 # cycles
        column_averages[protocol][1] += cycle_data[1] / 1e9 # instruction
        column_averages[protocol][2] += cycle_data[2] + cycle_data[3] / 1e9  # time in nanoseconds
        num_entries[protocol] += 1


for protocol in column_averages:
    num_entries_p = num_entries[protocol]
    column_averages[protocol][0] /= num_entries_p
    column_averages[protocol][1] /= num_entries_p
    column_averages[protocol][2] /= num_entries_p

# Step 4: Plot the differences between protocols in a graph
x_labels = ["cycles", "instruction", "time SEND(ns)","time RECIVE(ns)"]
plt.figure(figsize=(10, 6))
for i, protocol in enumerate(protocols):
    y_values = column_averages[protocol]
    plt.bar([x + i * 0.2 for x in range(4)], y_values, width=0.2, label=protocol)

plt.xlabel("Metrics")
plt.ylabel("Average Values")
plt.xticks([0.2, 1.2, 2.2,3.2], x_labels)
plt.legend()
plt.title("Average Metrics Comparison between Protocols")
plt.grid(axis='y', linestyle=':')
#plt.show()
plt.savefig("multiple_nodes.png")
