import websocket
import threading
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.animation as animation
import queue
import numpy as np

xqueue = queue.Queue()
yqueue = queue.Queue()

socket = ""
plt.style.use('grayscale')
#192.168.137.34
# Function to receive WebSocket data
def receive_data():
    global Timestamp  # Declare Timestamp as a global variable
    Timestamp = 0  # Initialize Timestamp
    while True:
        Sensor_Readings = ws.recv()
        # Process the received data
        # Split the string by commas
        Readings_buffer = Sensor_Readings.split(',')
        Timestamp_arr = []
        Readings_arr = []
        for i in range(0, len(Readings_buffer) - 1, 2):
            Timestamp = int(Readings_buffer[i + 1]) + Timestamp  # Accumulate timestamps
            Timestamp_arr.append(Timestamp)
            Readings_arr.append(float(Readings_buffer[i]))

        # Sort the data based on Timestamp_arr
        sorted_indices = sorted(range(len(Timestamp_arr)), key=lambda k: Timestamp_arr[k])
        Timestamp_arr = [Timestamp_arr[i] for i in sorted_indices]
        Readings_arr = [Readings_arr[i] for i in sorted_indices]

        # Split into chunks of 5 elements
        Timestamp_chunks = [Timestamp_arr[i:i+3] for i in range(0, len(Timestamp_arr), 3)]
        Readings_chunks = [Readings_arr[i:i+3] for i in range(0, len(Readings_arr), 3)]

        # Put the chunks into the queues
        for t_chunk, r_chunk in zip(Timestamp_chunks, Readings_chunks):
            xqueue.put(t_chunk)
            yqueue.put(r_chunk)

    ws.close()


while (True):
    socket =  input("Socket")
    socket = "ws://" + socket + "/ws"
    print("Socket is: " + socket)
    break;

ws = websocket.WebSocket()
ws.connect(socket)
# Create and start threads for WebSocket data reception and GUI update
receive_thread = threading.Thread(target=receive_data)
receive_thread.start()

# Use Python lists to accumulate data efficiently
x_list = []
y_list = []
counter = 1
xlim = 0

def init():
    line.set_data([], [])
    return line,


def animate(i):
    global x_list, y_list, counter, xlim

    # Check if the queues have values
    if not xqueue.empty() and not yqueue.empty():
        # Accumulate data in Python lists
        x_data = xqueue.get()
        y_data = yqueue.get()

        # Extend the lists with new data
        x_list.extend(x_data)
        y_list.extend(y_data)

        # Keep only the last 100000 elements
        if len(x_list) > 5000:
            x_list = x_list[-5000:]
            y_list = y_list[-5000:]

        # Check if the last element of x_list exceeds the current xlim
        if len(x_list) > 0 and x_list[-1] > xlim:
            counter += 1
            new_xlim = counter * 258 * 50
            ax.set_xlim(xlim, new_xlim)
            xlim = new_xlim



    # Update the plot with blitting
    line.set_data(x_list, y_list)

    ax.relim()
    ax.autoscale_view()

    return line,


fig, ax = plt.subplots()
line, = ax.plot([], [])
ax.set_ylim(-1, 3)
ax.set_xlim(0, 258 * 50)
#add legends to the plot
ax.set_xlabel('Time(miliseconds)')
ax.set_ylabel('Data')
ax.set_title('Realtime Accelerometer data in Gs')
ax.legend(['Acceleration in Gs'], loc='upper left')
ani = animation.FuncAnimation(fig, animate, init_func=init, frames=1, blit=True, interval=1)
plt.show()

#
# #Set up plot and axis
# plt.ion()
# fig = plt.figure()
# ax = fig.add_subplot(111)
# x = np.array([])
# y = np.array([])
#
# plt.ylim(-3,3)
# plt.xlim(0,1000000)
#
# line, = ax.plot(x,y)
#
# #add legends to the plot
# ax.set_xlabel('Time(microseconds)')
# ax.set_ylabel('Data')
# ax.set_title('Realtime Accelerometer data in Gs')
# ax.legend(['Acceleration in Gs'], loc='upper left')
#
# while True:
#     x = np.append(x, xqueue.get())
#     y = np.append(y, yqueue.get())
#     if len(x) > 100:
#         x = x[-100:]
#         y = y[-100:]
#     print(x)
#     print(y)
#     line.set_ydata(y)
#     line.set_xdata(x)
#     plt.draw()
#     plt.pause(0.001)
#
#
# # Optionally, you can join the threads to wait for their completion
# # receive_thread.join()
# # gui_thread.join()
