import numpy as np
from utils import createInitialCmd, createLambdaFunc, computeFromLambda
import serial
import serial.tools.list_ports
import time
import struct
import sys

# functions connnect to the serial port
def connect_to_serial_port(port, baud_rate=115200, timeout=1):
    try:
        # Create a serial connection
        ser = serial.Serial(port, baud_rate, timeout=timeout)
        print(f"Connected to {port} at {baud_rate} baud rate.")
        return ser
    except serial.SerialException as e:
        print(f"Error: {e}")
        return None

## function send the frame to the board
def sendFrame(board, frame):
    for i in range(4):
        for j in range(4):
            value = frame[i, j].astype(np.int8)
            packed_data = struct.pack('b', value)
            board.write(packed_data)
            # Serves as back and forth. Waits for OK from Arduino
            data = board.readline()
  
  
class Control:
    def __init__(self, interpolation = False, _TIMECONST = 0.1):
        self.interpolation = interpolation
        self._TIMECONST = _TIMECONST
        
    def __del__(self):
        if self.board:
            self.board.close()
        
    def changeFile(self, name):
        self.Magnet, self.time_step = createInitialCmd(fileName = name)
        self.frame_ori = self.Magnet.copy()
        self.total_time = runtime
        self.sequence_duration = self.Magnet.shape[0] * self.time_step
      
    def connect2Board(self, port):
        ## This function is to connect to the board
        # ports = serial.tools.list_ports.comports()
        # for port, desc, hwid in sorted(ports):
        #     print(f"{port}: {desc} [{hwid}]")
        self.board = connect_to_serial_port(port)
    
    
    def warmup(self):
        start_time = time.time()
        while True:
            ready_cmd = "READY\r"
            print("try to establish")
            self.board.write(ready_cmd.encode('utf-8'))
            data = self.board.readline()
            data_str = data.decode('utf-8').strip()
            if data_str == "ACK":
                print("Connection established")
                break
            # discard the buffer
        self.board.readline()
        self.board.readline()
        # tell the board we are ready for experiments
        ready_cmd = "LV_A\r"
        self.board.write(ready_cmd.encode('utf-8'))
        data = self.board.readline()
        data_str = data.decode('utf-8').strip()
        if data_str == "ACK":
            print("start streaming")
            
    def run(self, runtime):
        # if interpolation is off
        if not self.interpolation:
            stamp = 0
            curr_stamp = -1
            start_time = time.time()
            elapsed_time = time.time() - start_time
            while elapsed_time < runtime:
                
                if elapsed_time < self._TIMECONST * self.time_step:
                    frame = np.zeros_like(self.frame_ori[0], dtype = np.int8)
                else:
                    stamp = (elapsed_time - self._TIMECONST * self.time_step)/self.time_step
                    stamp = int(stamp)
                    frame = self.frame_ori[stamp]
                    
                if curr_stamp < stamp:
                    frame = frame.astype(np.int8)
                    print(frame)
                    sendFrame(self.board, frame)   
                    rec = self.board.readline().decode('utf-8').strip()
                    print("Received: " + rec)
                    curr_stamp = stamp
                    
                elapsed_time = time.time() - start_time
             
            return

        # if interpolation is on
        Magnet_func = createLambdaFunc(control.Magnet, self.time_step, runtime)
        start_time = time.time()
        while time.time() - start_time < runtime:
            frame = computeFromLambda(Magnet_func, 
                                      t = time.time() - start_time,
                                      timelenght = self.sequence_duration)
                
            frame = frame.astype(np.int8)
            print(frame)
            sendFrame(self.board, frame)


if __name__ == '__main__':
    #if (len(sys.argv) != 4
    #    or not sys.argv[1].endswith(".txt") 
    #    or 0 > int(sys.argv[2])
    #    or sys.argv[3] not in ['y', 'n']) :
    #    print("Error: Incorrect use of command line, use... \n$" 
    #    + "python3 " + sys.argv[0] 
    #    + " [file_name] [Arduino_COM_port_#] [interpolation (y/n)]")
    #    sys.exit()
        
    #fileName = sys.argv[1]
    #port = 'COM' + str(sys.argv[2])
    
    fileNames = ['test.txt']#, './testFiles/test_M_path.txt']
    port = 'COM3'
    runtimes = [10]#, 20]
    interp = 'y'

    control = Control(interpolation = interp == 'y')
    control.connect2Board(port)
    control.warmup()
    
    for name, runtime in zip(fileNames, runtimes):
        control.changeFile(name)
        control.run(runtime)
        
    del control
