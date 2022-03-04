# import dependancies
import numpy as np
import pandas as pd

from sklearn.metrics import mean_squared_error, mean_absolute_error
from sklearn.model_selection import train_test_split, GridSearchCV, RandomizedSearchCV
from sklearn.multioutput import MultiOutputRegressor
from sklearn.svm import SVR
from sklearn.neighbors import KNeighborsRegressor

################################## Declared Functions ###############################################


def custom_scaler(dataframe):  # Function to Normalize the DataFrame
    '''
        We know the range that each of the categories vary in.
        So, this fuction will normalize the dataframe due to them.
        Gyro_x, Gyro_y, Gyro_z ==> -1500dps to 1500dps
        Acc_x, Acc_y, Acc_z ==> -40g to 40g
        Mag_x, Mag_y, Mag_z ==> -40uT to 40uT
    '''

    dataframe['Gyro_x'] = (dataframe['Gyro_x'] + 1500) / 3000
    dataframe['Gyro_y'] = (dataframe['Gyro_y'] + 1500) / 3000
    dataframe['Gyro_z'] = (dataframe['Gyro_z'] + 1500) / 3000

    dataframe['Acc_x'] = (dataframe['Acc_x'] + 40) / 80
    dataframe['Acc_y'] = (dataframe['Acc_y'] + 40) / 80
    dataframe['Acc_z'] = (dataframe['Acc_z'] + 40) / 80

    dataframe['Mag_x'] = (dataframe['Mag_x'] + 40) / 80
    dataframe['Mag_y'] = (dataframe['Mag_y'] + 40) / 80
    dataframe['Mag_z'] = (dataframe['Mag_z'] + 40) / 80

    return dataframe

# -----------------------------------------------------------------------------------


def sequence_creator(x, y, num_steps):  # Function to Create the Sequence

    n_vars = x.shape[1]
    df = pd.DataFrame(x)
    cols, names = list(), list()

    # prepare input sequence (t-n, ... t-1)
    for i in range(num_steps, -1, -1):
        cols.append(df.shift(i))

        if i == 0:
            names += [('in%d(t)' % (j+1)) for j in range(n_vars)]
        else:
            names += [('in%d(t-%d)' % (j+1, i)) for j in range(n_vars)]

    # put it all together
    out_x = pd.concat(cols, axis=1)
    out_x.columns = names

    out_x.dropna(inplace=True)

    # Prepare output
    out_y = pd.DataFrame(y)
    names = list()
    for i in range(out_y.shape[1]):
        names += [('out%d(t)' % (i+1))]

    out_y.columns = names
    out_y.drop(out_y.index[range(num_steps)], inplace=True)

    return out_x, out_y


############################ Import and Prepare Data #######################################
print('--------------------------------------------------------------------------------')
print('==> Importing Data...')
for i in range(1, 6):
    # read data
    data = pd.read_csv(f'../DataSet/IMU_Data_{i}.csv')

    # Normalize
    data = custom_scaler(data)

    # Split x and y Data and Create sequences
    data_x = data[['Gyro_x', 'Gyro_y', 'Gyro_z',
                   'Acc_x',  'Acc_y',  'Acc_z',
                   'Mag_x',  'Mag_y',  'Mag_z']].values.astype('float32')

    # data_y = data[['Quat_0', 'Quat_1', 'Quat_2', 'Quat_3']
    #               ].values.astype('float32')
    data_y = data[['Euler_x', 'Euler_y', 'Euler_z']].values.astype('float32')

    # Create the Sequence with Specified Time Lag
    num_steps = 60
    tmp_x, tmp_y = sequence_creator(data_x, data_y, num_steps=num_steps)

    # Conatenate the Feature Vectors
    if i == 1:
        x, y = tmp_x, tmp_y
    else:
        x = np.concatenate((x, tmp_x), axis=0)
        y = np.concatenate((y, tmp_y), axis=0)

# Log the Data Specs
print("\n\tTotal Input Data Points: ", x.shape[0])
print("\tTotal Output Data Points: ", y.shape[0])
print("\tInput Features Size: ", x.shape[1])
print("\tOutput Size: ", y.shape[1])
print('--------------------------------------------------------------------------------')

###################### Split Test and Train Data ###################################################
# Split the Data into Train and Test
print("==> Preparing the Train and Test Data...")

train_x, test_x, train_y, test_y = train_test_split(
    x, y, test_size=0.04, shuffle=True)

print('\tTrain Data Final Shape: ', train_x.shape)
print('\tTest Data Final Shape: ', test_x.shape)
print('--------------------------------------------------------------------------------')

##################################### SVM Model Definition ###############################################
print("==> Defining the SVM Model and Fit the Data...")

print("\tDefining the SVM Model...")
mdl = MultiOutputRegressor(SVR(gamma='scale'), n_jobs=4)

print("\tFitting the Data...")
mdl = mdl.fit(train_x, train_y)

print("\tPredicting the Test Data and Evaluating the Model...")
yhat = mdl.predict(test_x)

# Evaluate the Regressor
MSE = mean_squared_error(test_y, yhat)
print(f'MSE of the Prediction and the Real Data: {MSE}')

MAE = mean_absolute_error(test_y, yhat)
print(f'MAE of the Prediction and the Real Data: {MAE}')

print('--------------------------------------------------------------------------------')

##################################### KNN Model Definition ###############################################
print("==> Defining the KNN Model and Fit the Data...")

print("\tDefining the KNN Model...")

mdl = MultiOutputRegressor(KNeighborsRegressor(n_neighbors=5,
                                               weights='distance',
                                               algorithm='auto',
                                               leaf_size=30,
                                               p=1,
                                               metric='minkowski',
                                               metric_params=None,
                                               n_jobs=1),
                           n_jobs=4)

print("\tFitting the Data...")
mdl = mdl.fit(train_x, train_y)

print("\tPredicting the Test Data and Evaluating the Model...")
yhat = mdl.predict(test_x)

# Evaluate the Regressor
MSE = mean_squared_error(test_y, yhat)
print(f'MSE of the Prediction and the Real Data: {MSE}')

MAE = mean_absolute_error(test_y, yhat)
print(f'MAE of the Prediction and the Real Data: {MAE}')
print('--------------------------------------------------------------------------------')
