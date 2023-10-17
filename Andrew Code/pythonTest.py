import socket
import time 
import tkinter as tk
from tkinter import filedialog
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import io
import threading
import sys



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
    with io.StringIO(data) as f:
        for line in f:
            x, y, z = line.strip().split()
            newX = (float(x)*3.14)/180.0
            newZ = float(z)
            xVals.append(newX)
            yVals.append(y)
            zVals.append(newZ)
        for i in range(len(zVals)):
            if i == 0:
                ax.clear()
            if zVals[i] < 9.0:
                ax.scatter(xVals, yVals, marker = 'D', color='red')
                plt.show(block=False)
                plt.draw()
                plt.pause(2)
            else:
                ax.scatter(xVals, yVals, marker='o', color='blue')
                plt.show(block=False)
                plt.draw()
                plt.pause(2)
        print("graph done")      
        xVals.clear()
        yVals.clear()
        zVals.clear()
        xVals.append(0)
        yVals.append(0)
        zVals.append(0)          




    

def main():
    xVals = []
    yVals = []
    zVals = []
    xVals.append(0)
    yVals.append(0)
    zVals.append(0.0)
    robot = connect()
    data = None
    fig, ax = plt.subplots(subplot_kw = {'projection': 'polar'})
    ax.set_title('plots')
    
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
            else:
                print(data)
            data = None   

if __name__ == '__main__':
    main()



        





    