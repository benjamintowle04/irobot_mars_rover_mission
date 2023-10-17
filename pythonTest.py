import socket
import time 
import tkinter as tk
from tkinter import filedialog
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import io
import threading
import sys

keyinput = None
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
        time.sleep(50)
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
    global keyinput
    while True:
        command = input()
        if command == "+":
            disconnect(robot)
            sys.exit()
        else:
            sender(client, command)

def plotgraph(xVals, yVals, data, ax):
    with io.StringIO(data) as f:
        for line in f:
            if line.startswith('/'):
                #print(data)
                print("seen")
            else:
                x, y = line.strip().split()
                newX = (float(x)*3.14)/180.0
                xVals.append(newX)
                yVals.append(y)
                ax.scatter(xVals, yVals)
                




    

def main():
    xVals = []
    yVals = []
    xVals.append(0)
    yVals.append(0)
    robot = connect()
    data = None
    fig, ax = plt.subplots(subplot_kw = {'projection': 'polar'})

    input_thread = threading.Thread(target=keyboard_input, args=(robot, ))
    input_thread.daemon = True
    input_thread.start()

    global keyinput
    while True:
        print("enter data\n")
        while data == None:
            data = receive(robot)
            print("if 1\n")
            if not(data == None):
                print("Data ", data)
                print("starting graph")
                ax.set_title('plots')
                plotgraph(xVals, yVals, data, ax)
                plt.show(block=False)
                plt.draw()
                plt.pause(2)
                print("graph done")
                print("keys ", keyinput)
                keyinput = None
                
                xVals.clear()
                yVals.clear()
            data = None
            print("here")    

if __name__ == '__main__':
    main()



        





    