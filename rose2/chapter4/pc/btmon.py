import serial
import csv
import matplotlib.pyplot as plt
from statistics import mean

plot_x =[]
plot_volt =[]
plot_amp =[]

fig = plt.figure(tight_layout=True)
ax1 = fig.add_subplot(211, title='current', ylabel='mA')
ax2 = fig.add_subplot(212, title='voltage', ylabel='V')
lines1, = ax1.plot(plot_x,plot_amp)
lines2, = ax2.plot(plot_x,plot_volt)

try:
    ser = serial.Serial('/dev/tty.M5StickC-BT-ESP32SPP', 115200)
    while True:
        line = ser.readline()
        tmp_data = line.decode(encoding='utf-8').strip()
        data = []
        data.append(tmp_data.replace(" ",""))
        for row in csv.reader(data, quoting=csv.QUOTE_NONNUMERIC):
            if len(plot_x) == 100:
                print(mean(plot_amp))
                plot_x.pop(0)
                plot_volt.pop(0)
                plot_amp.pop(0)
            plot_x.append(row[0]/1000000)
            plot_volt.append(row[1])
            plot_amp.append(row[4])
            print(row)
        
        ax1.set_xlim(min(plot_x), max(plot_x))
        ax1.set_ylim(0, max(plot_amp) + 10)
        lines1.set_data(plot_x,plot_amp)
        
        ax2.set_xlim(min(plot_x), max(plot_x))
        ax2.set_ylim(0, max(plot_volt) + 0.5)
        lines2.set_data(plot_x,plot_volt)

        plt.pause(.01)

except KeyboardInterrupt:
    ser.close()
except Exception as e:
    print(e)
