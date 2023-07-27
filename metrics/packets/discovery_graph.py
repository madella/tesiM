import os
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os
def plot_packets(data,prot1,prot2):
    x_values = [first[0] for first in data]
    protc1 = [first[1] for first in data]
    protc2 = [first[2] for first in data]
    fig, ax = plt.subplots()
    ax.plot(x_values, protc1, color='red', label=prot1.replace('custom',''))
    ax.plot(x_values, protc2, color='blue', label=prot2.replace('custom',''))
    ax.set_xlabel('entities')
    ax.set_ylabel('packets')
    ax.set_title('comparing')
    ax.legend()
    plt.savefig(f'img/{prot1}_{prot2}.png')
    plt.close()

def count_packets(filename):
    print('Processing {}'.format(filename))
    command = 'tshark -r {} -Y'.format(filename)
    command += ' "rtps && ('
    command += ' (rtps.sm.rdEntityId == 0x000002c2) ||'
    command += ' (rtps.sm.rdEntityId == 0x000002c7) ||'
    command += ' (rtps.sm.rdEntityId == 0x000003c2) ||'
    command += ' (rtps.sm.rdEntityId == 0x000003c7) ||'
    command += ' (rtps.sm.rdEntityId == 0x000004c2) ||'
    command += ' (rtps.sm.rdEntityId == 0x000004c7) ||'
    command += ' (rtps.sm.rdEntityId == 0x000100c2) ||'
    command += ' (rtps.sm.rdEntityId == 0x000100c7) ||'
    command += ' (rtps.sm.rdEntityId == 0x000200C2) ||'
    command += ' (rtps.sm.rdEntityId == 0x000200C7) ||'
    command += ' (rtps.sm.rdEntityId == 0x000201C3) ||'
    command += ' (rtps.sm.rdEntityId == 0x000201C4))" | wc -l'

    res = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    n_packets = int(res.communicate()[0].decode().rstrip())
    return n_packets

if __name__ == '__main__':
    data = []
    try:
        ranges=int(os.environ['RANG'])
        granularity=int(os.environ['GRAN'])
        prot1=os.environ['PROT1']
        prot2=os.environ['PROT2']
        print(type(ranges),type(granularity))
    except:
        print("ENV NOT FOUND")
        exit(1)
    for X in range(1, ranges + 1, granularity):
        prot1_file = f'data/{prot1}_{X}.data'
        prot2_file = f'data/{prot2}_{X}.data'
        if not os.path.isfile(prot1_file) or not os.path.isfile(prot2_file):
            print(f'Cannot find files {prot1_file} or {prot2_file}')
            continue

        prot_c1 =  count_packets(prot1_file)
        prot_c2 = count_packets(prot2_file)
        data.append([X,prot_c1,prot_c2])

    print(data)
    plot_packets(data,prot1=prot1,prot2=prot2)
