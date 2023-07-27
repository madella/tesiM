import os
import matplotlib.pyplot as plt

def read_data(file_path):
    with open(file_path, 'r') as file:
        return [int(line.strip()) for line in file]

def calculate_average(numbers):
    return sum(numbers) / len(numbers)

def main():
    methods = ['cht', 'chp', 'chd']
    data_folder = 'data'  # Replace this with the path to the folder containing your data files

    method_averages = {}
    std_average = {}
    std_average['std'] = {}
    for i in range(0, 101, 20): 
            file_path = os.path.join(data_folder, f'std_{i}_0.data')
            numbers = read_data(file_path)
            std_average['std'][i] = calculate_average(numbers)
            
    for method in methods:
        method_averages[method] = {}
        for i in range(0, 101, 20):  # Assuming the files have increments of 20 like in your example
            file_path = os.path.join(data_folder, f'{method}_10_{i}.data')
            numbers = read_data(file_path)
            method_averages[method][i] = calculate_average(numbers)
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))
                
    # Plotting the averages
    for method, averages in method_averages.items():
        percentages = list(averages.keys())
        avg_values = list(averages.values())
        ax2.plot(percentages, avg_values, marker='o', label=method.upper())
    # ax2.xlabel('#entities out of group')
    # ax2.ylabel('PERF_COUNT_HW_BUS_CYCLES on PUB write')
    # ax2.title('X same group - 10 same & X on different topic/partitions/domain')
    ax2.legend()
    ax2.grid(True)
    
    for method, averages in std_average.items():
        percentages = list(averages.keys())
        avg_values = list(averages.values())
        ax1.plot(percentages, avg_values, marker='o', label=method.upper(),color='red')
    # ax1.xlabel('#entities out of group')
    # ax1.ylabel('PERF_COUNT_HW_BUS_CYCLES on PUB write')
    # ax1.title('X same group - 10 same & X on different topic/partitions/domain')
    ax1.legend()
    ax1.grid(True)
    # ax1.text(50, 3400,f'Same group', ha='right', va='bottom',rotation=35 ,color='#00588C' )

    plt.savefig("img/pubblisher_cost_sub.png")

if __name__ == "__main__":
    main()
