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
            
            method_averages[method][i] = calculate_average(numbers)

            
    # Plotting the averages
    plt.figure(figsize=(8, 6))
    avg_values_list=[]
    for method, averages in method_averages.items():
        avg_values_list.append(list(averages.values())[len(list(averages.values()))-1])
    bar_colors = ['red', 'green', 'magenta', 'orange']
    xlabels=['SAME-GRP','TOPIC-C','PARTITIONS-C','DOMAIN-C']
    plt.bar(xlabels, avg_values_list,color=bar_colors)
    plt.ylabel(f'PERF_COUNT_HW_BUS_CYCLES on 1 {ps}')
    plt.title('100 SAME-GRP vs 10 same - 100 out')
    plt.grid(True)
    plt.savefig(f"img/{ps}_cost_bar.png")

if __name__ == "__main__":
    main()
