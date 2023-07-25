import os
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os
def plot_packets(data):
    x_values = [first[0] for first in data]
    server = [first[1] for first in data]
    distributed = [first[2] for first in data]
    fig, ax = plt.subplots()
    ax.plot(x_values, server, color='red', label='Discovery Server')
    ax.plot(x_values, distributed, color='blue', label='Distributed')
    ax.set_xlabel('X-axis')
    ax.set_ylabel('Y-axis')
    ax.set_title('Graphs showing ')
    ax.legend()
    #plt.show()
    plt.savefig('discovery_graph.png')
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
    ranges=os.environ['RANG']
    granularity=os.environ['GRAN']
    print("Check env variable: {}, {}".format(ranges,granularity))
    for X in range(1, ranges + 1, granularity):
        simple_file = f'pcapng/simple_{X}.pcapng'
        server_file = f'pcapng/discovery_server_{X}.pcapng'

        if not os.path.isfile(simple_file) or not os.path.isfile(server_file):
            print(f'Cannot find files {simple_file} or {server_file}')
            continue

        server = count_packets(server_file)
        distributed =  count_packets(simple_file)
        data.append([X,server,distributed])

    print(data)
    plot_packets(data)
