import os
import re
import matplotlib.pyplot as plt

def extract_data_from_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    print(file_path)
    task_clock_match = re.search(r'([\d.,]+)\s+msec task-clock', content)
    cycles_match = re.search(r'([\d.]+)\s+cycles', content)
    instructions_match = re.search(r'([\d.]+)\s+instructions', content)
    cache_references_match = re.search(r'([\d.]+)\s+cache-references', content)
    cache_misses_match = re.search(r'([\d.]+)\s+cache-misses', content)
    mem_loads_match = re.search(r'([\d.]+)\s+mem-loads', content)
    mem_stores_match = re.search(r'([\d.]+)\s+mem-stores', content)

    # Extracting the matched values and converting to appropriate data types
    if task_clock_match:
        task_clock = float(task_clock_match.group(1).replace('.', '').replace(',', '.'))
        
    if cycles_match:
        cycles = int(cycles_match.group(1).replace('.', ''))
    if instructions_match:
        instructions = int(instructions_match.group(1).replace('.', ''))
    if cache_references_match:
        cache_references = int(cache_references_match.group(1).replace('.', ''))
    if cache_misses_match:
        cache_misses = int(cache_misses_match.group(1).replace('.', ''))
    if mem_loads_match:
        mem_loads = int(mem_loads_match.group(1).replace('.', ''))
    if mem_stores_match:
        mem_stores = int(mem_stores_match.group(1).replace('.', ''))


    return task_clock, cycles, instructions, cache_references, cache_misses, mem_loads,mem_stores

def main():
    prot1_list = []
    prot2_list = []
    metric_names = ['task-clock', 'cycles', 'instructions','cache-references','cache-misses','mem-loads','mem-stores']
    metric_data_prot1 = {metric: [] for metric in metric_names}
    metric_data_prot2 = {metric: [] for metric in metric_names}
    try:
        ranges=int(os.environ['RANG'])
        granularity=int(os.environ['GRAN'])
        prot1=os.environ['PROT1']
        prot2=os.environ['PROT2']
        print(type(ranges),type(granularity))
    except:
        print("ENV NOT FOUND")
        exit(1)
    # Find all files in the 'perf' folder
    for X in range(1, ranges + 1, granularity):
        prot1_file = f'data/{prot1}_{X}.data'
        prot2_file = f'data/{prot2}_{X}.data'
        prot2_list.append(prot2_file)    
        prot1_list.append(prot1_file)
        
    for file_path in prot1_list:
        data = extract_data_from_file(file_path)
        for i, metric in enumerate(metric_names):
            metric_data_prot1[metric].append(data[i])
            
    for file_path in prot2_list:
        data = extract_data_from_file(file_path)
        print(data)
        for i, metric in enumerate(metric_names):
            metric_data_prot2[metric].append(data[i])

    # Create a summary graph comparing 'discovery' and 'discovery_server' for each metric
    print(len(prot2_list))
    if len(prot2_list) == 1:
        x_labels = 'udp'
        x_labels_2 = 'shared_mem'
    else:
        x_labels = [f'{i*10}' for i in range(1, len(prot1_list) + 1)]
        x_labels_2 = [f'{i*10}' for i in range(1, len(prot2_list) + 1)]

    for metric in metric_names:
        plt.figure()
        plt.plot(x_labels, metric_data_prot1[metric])
        plt.plot(x_labels_2, metric_data_prot2[metric])
        plt.xlabel('entities')
        plt.ylabel(metric)
        plt.title(f'{metric} comparison')
        plt.xticks(rotation=0)
        plt.tight_layout()
        plt.savefig(f'img/{prot1}_{prot2}_{metric}.png')
    print("Check coherence {} equal to {} ".format(x_labels_2,prot2))

if __name__ == "__main__":
    main()
