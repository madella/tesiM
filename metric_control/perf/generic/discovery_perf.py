import os
import re
import matplotlib.pyplot as plt

def extract_data_from_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    print(file_path)
    task_clock_match = re.search(r'([\d.]+,\d+)\s+msec task-clock', content)
    cycles_match = re.search(r'([\d.]+)\s+cycles', content)
    instructions_match = re.search(r'([\d.]+)\s+instructions', content)
    seconds_elapsed_match = re.search(r'(\d+,\d+)\s+seconds time elapsed', content)
    seconds_user_match = re.search(r'(\d+,\d+)\s+seconds user', content)
    seconds_sys_match = re.search(r'(\d+,\d+)\s+seconds sys', content)
    if task_clock_match:
        task_clock = float(task_clock_match.group(1).replace('.','').replace(',', '.'))
        print(task_clock)
    if cycles_match:
        cycles = int(cycles_match.group(1).replace('.',''))
    if instructions_match:
        instructions = int(instructions_match.group(1).replace('.',''))
    if seconds_elapsed_match:
        time_elapsed = float(seconds_elapsed_match.group(1).replace(',', '.'))
    if seconds_user_match:
       seconds_user = float(seconds_user_match.group(1).replace(',', '.'))
    if seconds_sys_match:
        seconds_sys = float(seconds_sys_match.group(1).replace(',', '.'))

    return task_clock, cycles, instructions, time_elapsed, seconds_user, seconds_sys

def main():
    discovery_files = []
    discovery_server_files = []
    metric_names = ['task-clock', 'cycles', 'instructions', 'seconds time elapsed', 'seconds user', 'seconds sys']
    metric_data_discovery = {metric: [] for metric in metric_names}
    metric_data_discovery_server = {metric: [] for metric in metric_names}
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
        simple_file = f'data/{prot1}_{X}.data'
        server_file = f'data/{prot2}_{X}.data'
        discovery_server_files.append(server_file)    
        discovery_files.append(simple_file)
    # Extract data from 'discovery' files
    for file_path in discovery_files:
        data = extract_data_from_file(file_path)
        for i, metric in enumerate(metric_names):
            metric_data_discovery[metric].append(data[i])
    # Extract data from 'discovery_server' files
    for file_path in discovery_server_files:
        data = extract_data_from_file(file_path)
        for i, metric in enumerate(metric_names):
            metric_data_discovery_server[metric].append(data[i])

    # Create a summary graph comparing 'discovery' and 'discovery_server' for each metric
    x_labels = [f'{i*10}' for i in range(1, len(discovery_files) + 1)]
    x_labels_server = [f'{i*10}' for i in range(1, len(discovery_server_files) + 1)]

    for metric in metric_names:
        plt.figure()
        plt.plot(x_labels, metric_data_discovery[metric], label=prot1)
        plt.plot(x_labels_server, metric_data_discovery_server[metric], label=prot2)
        plt.xlabel('entities')
        plt.ylabel(metric)
        plt.title(f'{metric} Comparison')
        plt.legend()
        plt.xticks(rotation=65)
        plt.tight_layout()
        plt.savefig(f'img/{metric}.png')

if __name__ == "__main__":
    main()
