import matplotlib.pyplot as plt
import datetime

f = open("rocket_launch_12_44_05.log")
data = []

for s in f:
    d = s.split('INFO')[1].replace('[', '').replace(']','').strip().split(',')
    parsed = [float(v) for v in d]
    if parsed[7] == 3 and parsed[8] == 3:
        data.append(parsed[2])


plt.plot(data)
plt.show()
