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
    try:
        ps=os.environ['PS']
    except:
        print("ENV NOT FOUND")
        exit(1)
    method_averages = {}

    method_averages['std'] = {}
    for i in range(0, 101, 20): 
            file_path = os.path.join(data_folder, f'std_{i}_0.data')
            numbers = read_data(file_path)
            method_averages['std'][i] = calculate_average(numbers)
            
    for method in methods:
        method_averages[method] = {}
        for i in range(0, 101, 20):  # Assuming the files have increments of 20 like in your example
            file_path = os.path.join(data_folder, f'{method}_10_{i}.data')
            numbers = read_data(file_path)
            print(numbers)
            method_averages[method][i] = calculate_average(numbers)
            
    # Plotting the averages
    plt.figure(figsize=(8, 6))
    bar_colors = ['red', 'green', 'magenta', 'orange']
    for (method, averages),color in zip(method_averages.items(),bar_colors):
        percentages = list(averages.keys())
        avg_values = list(averages.values())
        plt.plot(percentages, avg_values, marker='o', label=method.upper().replace('STD','SAME-GRP'),color=color)

    plt.text(50, 3400,f'Same group', ha='right', va='bottom',rotation=35 ,color='red' )
    plt.xlabel('#entities out of group')
    plt.ylabel('PERF_COUNT_HW_BUS_CYCLES on PUB write')
    plt.title('same group - different topic/partitions/domain BASE(10)')
    plt.legend()
    plt.grid(True)
    plt.savefig(f"img/{ps}_cost.png")

if __name__ == "__main__":
    main()
