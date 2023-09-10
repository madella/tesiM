import re,os,sys,glob
from collections import defaultdict

base_dir="./"
try: base_dir = sys.argv[1]
except: print("basedir not passed") 

pub_file_pattern = "pub*.data"
pub_matching_files = glob.glob(f"{base_dir}/{pub_file_pattern}")
sub_file_pattern = "sub*.data"
sub_matching_files = glob.glob(f"{base_dir}/{sub_file_pattern}")

protocols = ["udp"]
pub_pattern = r'cycles: (\d+) instruction: (\d+) start_ns: (\d+) end_ns: (\d+) start_s: (\d+) end_s: (\d+)'
sub_pattern = r'(\d+) received_s: (\d+) received_ns: (\d+)'
sent_data = defaultdict(list)
sent_data_time = defaultdict(list)
received_data = defaultdict(list)

# (17385992, 364797992) (63.0, 121102697.0) +0s
# 17386055.0 485900689.
pub_s = 17385992 
pubns = 364797992
sub_s = 17386055
subns = 485900689
def timespec_difference_without_skew(start_s, start_ns, end_s, end_ns):
    s_diff = end_s - start_s
    ns_diff = end_ns - start_ns 
    if ns_diff < 0:
        ns_diff += 1e9
        s_diff -= 1
    return s_diff, ns_diff
def timespec_difference(start_s_un, start_ns_un, end_s_un, end_ns_un):
    start_s,start_ns=timespec_difference_without_skew(pub_s,pubns,start_s_un,start_ns_un)
    end_s,end_ns=timespec_difference_without_skew(sub_s,subns, end_s_un, end_ns_un)
    s_diff = end_s - start_s
    ns_diff = end_ns - start_ns 
    if ns_diff < 0:
        ns_diff += 1e9
        s_diff -= 1
    return s_diff, ns_diff


for prots in protocols:
    # Pub part
    for file_path in pub_matching_files:
        with open(file_path, 'r') as file: 
                for line in file:
                    match = re.search(pub_pattern, line)
                    if match:
                        cycles, instruction, start_ns, end_ns, start_s, end_s = match.groups()
                        sent_data[prots].append((int(cycles), int(instruction), int(start_s), int(start_ns), int(end_s), int(end_ns)))
                        s_diff, ns_diff = timespec_difference_without_skew(int(start_s), int(start_ns) , int(end_s) ,int(end_ns) )
                        sent_data_time[prots].append((s_diff, ns_diff))             
    # Sub part
    for file_path in sub_matching_files:
        with open(file_path, 'r') as file: 
            for content in file:
                match = re.search(sub_pattern, content)
                if match:
                    index, received_s, received_ns = match.groups()
                    start_s,start_ns = ((sent_data[prots][int(index)][2], sent_data[prots][int(index)][3]))
                    s_diff, ns_diff = timespec_difference(start_s, start_ns , int(received_s), int(received_ns))
                    received_data[prots].append((s_diff, ns_diff))
                      

column_averages = defaultdict(lambda: [0, 0, 0, 0])

# Convert data to timespec
for protocol in protocols:
    nlen= len(received_data[protocol])
    for cycle_data in received_data[protocol]:
        column_averages[protocol][3] += cycle_data[0] * 1e9 + cycle_data[1]  # time in microseconds
    print(cycle_data[0],nlen,column_averages[protocol][3])
    column_averages[protocol][3] /= nlen

print(column_averages[protocol][3])
    

