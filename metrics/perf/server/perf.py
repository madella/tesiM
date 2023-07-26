import os
import re
import matplotlib.pyplot as plt

def extract_perf_data(file):
    with open(file, 'r') as f:
        data = {}
        data_str=f.read()
        task_clock_match = re.search(r'(\d+,\d+)\s+msec task-clock', data_str)
        cycles_match = re.search(r'(\d+)\s+cycles', data_str)
        instructions_match = re.search(r'(\d+)\s+instructions', data_str)
        seconds_elapsed_match = re.search(r'(\d+,\d+)\s+seconds time elapsed', data_str)
        seconds_user_match = re.search(r'(\d+,\d+)\s+seconds user', data_str)
        seconds_sys_match = re.search(r'(\d+,\d+)\s+seconds sys', data_str)
        if task_clock_match:
            data['task-clock'] = float(task_clock_match.group(1).replace(',', '.'))
        if cycles_match:
            data['cycles'] = int(cycles_match.group(1))
        if instructions_match:
            data['instructions'] = int(instructions_match.group(1))
        if seconds_elapsed_match:
            data['seconds time elapsed'] = float(seconds_elapsed_match.group(1).replace(',', '.'))
        if seconds_user_match:
            data['seconds user'] = float(seconds_user_match.group(1).replace(',', '.'))
        if seconds_sys_match:
            data['seconds sys'] = float(seconds_sys_match.group(1).replace(',', '.'))
    return data

def plot_perf_data(data, filename):
    events = list(data.keys())
    values = list(data.values())

    plt.barh(events, values)
    plt.legend(events[:])
    plt.xlabel('Values')
    plt.ylabel('Events')
    plt.title('Performance Counter Stats')
    plt.tight_layout()
    plt.savefig(filename)
    #plt.show()

def main():
    folder_path = "./"  # Update this with the correct path to your folder
    output_path = "perf_plots/"  # Folder to save the generated graphs

    if not os.path.exists(output_path):
        os.makedirs(output_path)

    for filename in os.listdir(folder_path):
        if filename.endswith(".data"):
            file_path = os.path.join(folder_path, filename)
            data = extract_perf_data(file_path)
            print(data)
            plot_filename = os.path.join(output_path, filename.replace('.data', '.png'))
            plot_perf_data(data, plot_filename)

if __name__ == "__main__":
    main()
