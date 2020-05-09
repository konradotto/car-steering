import sys, os
import fcntl


time_stamps = []
ground_steering_requests = []
calc_ground_steering_requests = []


def populate_data(lines):
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


if __name__ == '__main__':    
    populate_data(get_lines())
    print("tss:\n",time_stamps)
    print("gr0s:\n",ground_steering_requests)
    print("gr1s;\n",calc_ground_steering_requests)
    sys.exit()