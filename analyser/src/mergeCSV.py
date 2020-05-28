import sys, os

time_stamps = []
ground_steering_requests = []
calc_ground_steering_requests = []
previous_ground_steering_requests = []

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


def populate_prev_data(lines):
    lines.pop(0)
    for line in lines:
        strs = line.split(';')
        try:
            pgr=(strs[2])
        except ValueError:
            print("Error parsing value at line:\n", line)
            continue
        previous_ground_steering_requests.append(pgr)

def mergeCSV():
    f = open("somename.csv","w")
    f.write("time_stamp;ground_steering_0;ground_steering_1;ground_steering_prev;\n")
    i = 0
    for ts in time_stamps:
        f.write("{};{};{};{};\n".format(ts,ground_steering_requests[i],calc_ground_steering_requests[i],previous_ground_steering_requests[i]))
        i++
    f.close()


def get_lines(file):
    f = open(file, 'r')
    lines = f.readlines()
    if len(lines) > 0:
        return lines
    else:
        print("Empty file!")
    f.close()

if __name__ == '__main__':
    populate_data(get_lines(argv[1]))
    populate_prev_data(get_lines(argv[2]))
    mergeCSV()
    sys.exit()
