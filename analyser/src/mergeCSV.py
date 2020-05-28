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
    f = open("../../mergedReports.csv","w")
    f.write("time_stamp;ground_steering_0;ground_steering_1;ground_steering_prev;\n")
    count = len(time_stamps) if len(previous_ground_steering_requests) > len(time_stamps) else len(previous_ground_steering_requests)
    previous_off = count if len(previous_ground_steering_requests) - len(ground_steering_requests) > 0 else 0
    off = count if len(ground_steering_requests) - len(previous_ground_steering_requests) > 0 else 0
    for i in range(0, count):
        f.write("{};{};{};{};\n".format(time_stamps[i+off],ground_steering_requests[i+off],calc_ground_steering_requests[i+off],previous_ground_steering_requests[i+previous_off]))
        i += 1
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
    new_csv = get_lines(sys.argv[1])
    old_csv = get_lines(sys.argv[2])
    populate_data(new_csv)
    populate_prev_data(old_csv)
    mergeCSV()
    sys.exit()
