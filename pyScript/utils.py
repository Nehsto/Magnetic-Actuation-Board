import numpy as np
import re
import math
from scipy.interpolate import interp1d


def extract_numbers(string):
    pattern = r'-?\d+\.?\d*'
    matches = re.findall(pattern, string)
    numbers = [float(match) for match in matches]

    return  numbers


def createInitialCmd(fileName):
    with open(fileName, 'r') as file:
        lines = file.readlines()
    Magnet = []
    for line in lines:
        numbers = extract_numbers(line)
        if "time_step" in line:
            time_step = numbers[0]
        elif "row_number" in line:
            row_number = int(numbers[0])
        elif "col_number" in line:
            col_number = int(numbers[0])
        else:
            if len(Magnet) == 0:
                tempM = np.zeros((row_number, col_number))
            else:
                tempM = Magnet[-1].copy()
                
            index = int(numbers[0])
            if index == len(Magnet):
                numbers = numbers[1:]
                for i in range(0, len(numbers), 3):
                    idx_row = int(numbers[i]) - 1
                    idx_col = int(numbers[i+1]) -1
                    value = int(numbers[i+2])
                    # print("values" , numbers[i+2], value)
                    tempM[idx_row, idx_col] = value
                Magnet.append(tempM)
            else:
                # come here
                while len(Magnet) < index:
                    Magnet.append(tempM)
                numbers = numbers[1:]
                tempM = Magnet[-1].copy()
                for i in range(0, len(numbers), 3):
                    idx_row = int(numbers[i]) - 1
                    idx_col = int(numbers[i+1]) -1
                    value = int(numbers[i+2])
                    tempM[idx_row, idx_col] = value
                Magnet.append(tempM)




    Magnet = np.asarray(Magnet).reshape(-1, row_number, col_number)
    return Magnet, time_step

def createLambdaFunc(Magnet, time_step, total_time):
    num_rows = Magnet[0].shape[0]
    num_cols = Magnet[0].shape[1]

    # Two extra timesteps, 
    # one from 0 to 1st_timestep, 
    # and one from last_timestep to idle (0 intensity)
    # X = np.linspace(0, total_time, 2 + Magnet.shape[0]) 
    # Y = np.concatenate((np.zeros((1, num_rows, num_cols)), 
    #                     Magnet, 
    #                     (np.zeros((1, num_rows, num_cols)))))
    
    # No extra timesteps, 
    rep_time = Magnet.shape[0]*time_step
    repetitions = math.ceil(total_time/rep_time)
    X = np.linspace(0, rep_time*repetitions - 1, Magnet.shape[0]*repetitions)
    Y = Magnet
    Y = np.tile(Y, [repetitions, 1, 1])

    array_of_lambdas = []

    for i in range(num_cols):
        temp_array  = []
        for j in range(num_cols):
            interp_func =  interp1d(X, Y[:, i, j], kind = 'linear')
            temp_array.append((interp_func))
        array_of_lambdas.append(temp_array)

    return array_of_lambdas

def computeFromLambda(lambda_funcs, t, timelenght):
    num_rows = len(lambda_funcs)
    num_cols = len(lambda_funcs[0])
    frames = np.zeros((num_rows, num_cols), dtype = np.int16)

    for i in range(num_rows):
        for j in range(num_cols):
            frames[i, j] = lambda_funcs[i][j](t % timelenght)

    return frames
