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
    prot1_list = []
    prot2_list = []
    metric_names = ['task-clock', 'cycles', 'instructions', 'seconds time elapsed', 'seconds user', 'seconds sys']
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
        prot1_file = f'data/{prot1}_partition_{X}.data'
        prot2_file = f'data/{prot2}_partition_{X}.data'
        prot2_list.append(prot2_file)    
        prot1_list.append(prot1_file)
    # Extract data from 'discovery' files
    for file_path in prot1_list:
        data = extract_data_from_file(file_path)
        for i, metric in enumerate(metric_names):
            metric_data_prot1[metric].append(data[i])
    # Extract data from 'discovery_server' files
    for file_path in prot2_list:
        data = extract_data_from_file(file_path)
        for i, metric in enumerate(metric_names):
            metric_data_prot2[metric].append(data[i])

    # Create a summary graph comparing 'discovery' and 'discovery_server' for each metric
    x_labels = [f'{i}' for i in range(1, len(prot1_list) + 1)]
    x_labels_2 = [f'{i}' for i in range(1, len(prot2_list) + 1)]

    for metric in metric_names:
        plt.figure()
        plt.plot(x_labels, metric_data_prot1[metric], label=prot1.replace('custom',''))
        plt.plot(x_labels_2, metric_data_prot2[metric], label=prot2.replace('custom',''))
        plt.xlabel('entities')
        plt.ylabel(metric)
        plt.title(f'{metric} comparison')
        plt.legend()
        plt.xticks(rotation=65)
        plt.tight_layout()
        plt.savefig(f'img/{prot1}_{prot2}__partition_{metric}.png')

if __name__ == "__main__":
    main()
