import re,os,glob
from collections import defaultdict, Counter
import statistics

file_pattern = "timeshift*.data"
matching_files = glob.glob(f"./{file_pattern}")
n=len(matching_files)
pattern=r'Host (\w+)\s*, Rank (\d+): seconds (\d+), nanosecond (\d+)'
received_data =defaultdict(list)
hosts=[]
for file_path in matching_files:
    with open(file_path, 'r') as file: 
            for line in file:
                match = re.search(pattern, line)
                if match:
                    host,_,second,nanosecond = match.groups()
                    received_data[host].append((int(second), int(nanosecond)))
                    if host not in hosts :hosts.append(host)

# for host in hosts:
#     print(host, "len:", len(received_data[host]))

differences = []
differences_second =[]

for (sec1, ns1), (sec2, ns2) in zip(received_data[hosts[0]], received_data[hosts[1]]):
    diff_sec = sec2 - sec1
    diff_ns = ns2 - ns1
    if diff_ns < 0:
        diff_sec -= 1
        diff_ns += 1000000000
    differences.append((diff_ns/1e9))
    differences_second.append((diff_sec))
    
try: mode = statistics.mode(differences)
except:  
    c = Counter(differences)
    mode= c.most_common(1)[0][0]
# print(mode)
sec_diff= statistics.mean(differences_second)
#print(received_data[hosts[0]][0],(sec_diff,mode*1e9),"",end='')

sec=received_data[hosts[0]][0][0]+sec_diff

if received_data[hosts[0]][0][1]+mode*1e9 > 1e9: ns=received_data[hosts[0]][0][1]+mode*1e9-1e9;sec+=1#; print("+1s")
else: ns=received_data[hosts[0]][0][1]+mode*1e9#; print("+0s")
print(f'Difference {hosts[0]} - {hosts[1]} :{(sec_diff,mode*1e9)}\n{hosts[0]}: {received_data[hosts[0]][0]}\n{hosts[1]}: ({sec},{ns})')

if n == 3:
    differences = []
    differences_second = []
    for (sec1, ns1), (sec2, ns2) in zip(received_data[hosts[0]], received_data[hosts[2]]):
        diff_sec = sec2 - sec1
        diff_ns = ns2 - ns1
        if diff_ns < 0:
            diff_sec -= 1
            diff_ns += 1000000000
        differences.append((diff_ns/1e9))
        differences_second.append((diff_sec))
    import matplotlib.pyplot as plt
    import matplotlib.gridspec as gridspec

    fig = plt.figure(figsize=(12, 6))
    gs = gridspec.GridSpec(1, 3, width_ratios=[1, 3, 0.2])
    ax1 = plt.subplot(gs[0])
    ax1.boxplot([differences], labels=['Difference of ns'])
    ax1.set_title('Boxplot of Time Differences')
    ax1.set_ylabel('Time Difference')
    ax2 = plt.subplot(gs[1])
    ax2.plot(differences, marker='o', linestyle='-', color='b')
    ax2.set_title('Variation of Differences Between Nodes')
    ax2.set_xlabel('Data Point')
    ax2.set_ylabel('Nanoseconds Difference')
    ax2.grid(True)
    ax3 = plt.subplot(gs[2])
    ax3.axis('off')
    plt.tight_layout()
    plt.show()
    
    try: mode = statistics.mode(differences)
    except:  
        print("using Counter")
        c = Counter(differences)
        mode= c.most_common(1)[0][0]
    sec_diff= statistics.mean(differences_second)
    sec=received_data[hosts[0]][0][0]+sec_diff

    if received_data[hosts[0]][0][1]+mode*1e9 > 1e9: ns=received_data[hosts[0]][0][1]+mode*1e9-1e9;sec+=1#; print("+1s")
    else: ns=received_data[hosts[0]][0][1]+mode*1e9#; print("+0s")
    print(f'Difference {hosts[0]} - {hosts[2]} :{(sec_diff,mode*1e9)}\n{hosts[2]}: ({sec},{ns})\n')
