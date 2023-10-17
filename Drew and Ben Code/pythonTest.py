import socket
import time 
import tkinter as tk
from tkinter import filedialog
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import io
import threading
import sys
import numpy as np

cnt = 0

def connect():
    print('Begin connection')
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ROBOT_IP = "192.168.1.1"
    ROBOT_PORT = 288
    client.connect((ROBOT_IP, ROBOT_PORT))

    print('Socket connected to ' + ROBOT_IP)
    return client

def disconnect(client):
    client.close()
    print('disconnected')
    return

def receive(client):
    try: 
        data = client.recv(8192)
        data = data.decode('ascii')
        if not data:
            print("no data recieved")
            time.sleep(20)
    except:
        pass
    return data

def sender(client, DataToSend):
    DataToSend = DataToSend.encode('ascii')
    client.sendall(DataToSend)


def key_press(event):
    command = None
    if event.keysm == 'w':
        command ="MOVE_FORWARD"
    elif event.keysm == 'a':
        command = "TURN_LEFT_45"
    elif event.keysm == 'd':
        command = "TURN_RIGHT_45"
    elif event.keysm == 'q':
        command = "TURN_LEFT_1"
    elif event.keysm == 'e':
        command = "TURN_RIGHT_1d"
    elif event.keysm == 's':
        command = "STOP"
    elif event.keysm == 't':
        command = "SCAN"

    if command:
        sender(robot, command)

def keyboard_input(client, block= False):
    while True:
        command = input()
        if command == "+":
            sys.exit()
            disconnect(robot)
        else:
            sender(client, command)

def plotgraph(xVals, yVals, zVals, data, ax):
    global cnt
    ax.clear()
    with io.StringIO(data) as newData:
        for line in newData:
            x, y, z = data.split()
            print("x: ", x)
            print("y: ", y)
            print("z: ", z)
            xVals.append(int(x))
            yVals.append(float(y))
            zVals.append(float(z))
            cnt+=1
    
    
    print("zVals: ", str(zVals))
    print("xVals: ", str(xVals))
    print("yVals: ", str(yVals))
    for i in range(len(zVals)):
        ax.bar3d(xVals[int(i)], yVals[int(i)], 0, dx=zVals[int(i)], dy = 4, dz = 4)

    ax.xaxis.set_ticks(np.arange(0,190, 20.0))
    ax.yaxis.set_ticks(np.arange(0, 60, 5))
    plt.show(block=False)
    plt.draw()
    plt.pause(2)

    # # for i in zVals:
    # #     if i == 0:
    # #         ax.clear()
    # #     if zVals[i] < 9.0:
    # #         ax.scatter(xVals, yVals, marker = 'D', color='red')
    # #         plt.show(block=False)
    # #         plt.draw()
    # #         plt.pause(2)
    # #     else:
    # #         ax.scatter(xVals, yVals, marker='o', color='blue')
    # #         plt.show(block=False)
    # #         plt.draw()
    # #         plt.pause(2)
    # var = ['D' if x < 4 else 'o' for x in zVals] 
    # print("var: ", str(var))
    # print("zVals: ", str(zVals))
    # print("xVals: ", str(xVals))
    # print("yVals: ", str(yVals))
    # for s, a, b in zip(var, xVals, yVals):
    #     ax.scatter(a, b, marker = s)
    #     ax.text(a, b, str(zVals))
    # plt.show(block=False)
    # plt.draw()
    # plt.pause(2) 
    # xVals.clear()
    # yVals.clear()
    # zVals.clear()
    # # xVals.append(0)
    # # yVals.append(0)
    # # zVals.append(0)          

    
    print("graph done")



    

def main():
    global cnt
    xVals = [] 
    yVals = []
    zVals = []
    # xVals.append(0)
    # yVals.append(0)
    # zVals.append(0)
    robot = connect()
    data = None
    fig = plt.figure(figsize=(8,8))
    ax = fig.add_subplot(111, projection='3d')
    ax.view_init(-140, 90)
    

    input_thread = threading.Thread(target=keyboard_input, args=(robot, ))
    input_thread.daemon = True
    input_thread.start()

    while True:
        print("enter data\n")
        while data == None:
            data = receive(robot)
            if not(data == None):
                print("Data:\n ", data)
                if data.startswith('/'):
                    print("seen")
                else:
                    plotgraph(xVals, yVals, zVals, data, ax)
                    print("cnt: ", str(cnt))
            else:
                print(data)
            data = None   

if __name__ == '__main__':
    main()



        





    