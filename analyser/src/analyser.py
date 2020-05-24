import sys, os
import fcntl
import matplotlib.pyplot as plt
import numpy as np
    

time_stamps = []
ground_steering_requests = []
calc_ground_steering_requests = []
n = 0


def populate_data(lines):
    global n, calc_ground_steering_requests

    lines.pop(0) #csv header line
    for line in lines:
        strs = line.split(';')
        try:
            ts = int(strs[0])
            gr0 = float(strs[1])
            gr1 = float(strs[2])
        except ValueError:
            print("Error parsing value at line:\n", line)
            continue
        time_stamps.append(ts)
        ground_steering_requests.append(gr0)
        calc_ground_steering_requests.append(gr1)
    n = len(time_stamps)
    # calc_ground_steering_requests = [0] * n


def get_lines():
    fd = sys.stdin.fileno()
    fl = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)
    try:
        f = sys.stdin.read()
        lines = f.splitlines()
        if len(lines) > 0:
            return lines
        else:
            print("Empty file!")
    except:
        print('No input')
    sys.exit()

def plot():
    fig1 = plt.plot(time_stamps, ground_steering_requests, label='Ground Steering Request')
    fig2 = plt.plot(time_stamps, calc_ground_steering_requests, label='Calculated Ground Steering')
    # fig3 = plt.plot(time_stamps, [max(ground_steering_requests)]*len(time_stamps), label='max ground steering')
    # fig4 = plt.plot(time_stamps, [min(ground_steering_requests)]*len(time_stamps), label='min ground steering')
    plt.xlabel("Timestamp", fontsize=24)
    plt.ylabel("Angle [rad]", fontsize=24)
    plt.margins(x=0)

    plt.legend(loc=8, fontsize=16)
    plt.show()
    #fig1.show()
    #fig2.show()

def hist(num_bins=20):
    plt.hist(ground_steering_requests, num_bins, facecolor='blue', alpha=0.5)
    std = np.std(ground_steering_requests)
    mean = np.mean(ground_steering_requests)
    print(std, " ", mean)
    plt.yscale('log', nonposy='clip')
    plt.xlabel("Ground Steering Request [rad]", fontsize=24)
    plt.ylabel("Frequency (log scale)", fontsize=24)
    plt.show()

def calculate_performance(max_rel_error_allowed):
    relative_error = [None] * n
    for i in range(n):
        request = ground_steering_requests[i]
        calculated_request = calc_ground_steering_requests[i]
        error = abs(calculated_request - request)
        if request == 0:
            if error == 0:
                relative_error[i] = 0
            else:
                relative_error[i] = 1
        else:
            relative_error[i] = error/abs(request)

    nr_good_enough = sum([rel_error < max_rel_error_allowed for rel_error in relative_error])
    print(n)
    print(nr_good_enough)
    percentage_good_enough = nr_good_enough/n
    print(percentage_good_enough)




if __name__ == '__main__':    
    populate_data(get_lines())
    #print("tss:\n",time_stamps)
    #print("gr0s:\n",ground_steering_requests)
    #print("gr1s;\n",calc_ground_steering_requests)
    plot()
    calculate_performance(0.5)
    # hist(40)
    sys.exit()