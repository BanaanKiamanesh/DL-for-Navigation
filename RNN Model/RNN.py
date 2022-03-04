#import dependancies
from math import sqrt
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split

from keras.models import Sequential
from keras.layers import Dense, LSTM, Dropout
from keras import backend as K
from keras.layers import Lambda
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau, ModelCheckpoint, TensorBoard

import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

################################## Declared Functions ##################################
# Function to Normalize the DataFrame


def custom_scaler(dataframe):
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

# -------------------------------------------------------------------------------

# Function to Create the Sequence


def sequence_creator(x, y, num_steps):

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

    data_y = data[['Quat_0', 'Quat_1', 'Quat_2', 'Quat_3']
                  ].values.astype('float32')
    # data_y = data[['Euler_x', 'Euler_y', 'Euler_z']].values.astype('float32')

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

###################### Split Test and Train and Prepare the 3D Data for LSTM ############################
# Split the Data into Train and Test
print("==> Preparing the Train and Test Data...")

print("\n\tDivision ...")
train_x, test_x, train_y, test_y = train_test_split(
    x, y, test_size=0.1, shuffle=True)

print("\tTrain Data Primary Shape:", train_x.shape, train_y.shape)
print("\tTest Data Primary Shape:", test_x.shape, test_y.shape)

print('\n\t\tReshaping Data into 3D form Desired by LSTM...')
print('\t\t\t ==> samples, timesteps, features')

train_x = train_x.reshape((train_x.shape[0],
                           num_steps + 1,
                           train_x.shape[1] // (num_steps + 1)))

test_x = test_x.reshape((test_x.shape[0],
                         num_steps + 1,
                         test_x.shape[1] // (num_steps + 1)))

print('\tTrain Data Final Shape: ', train_x.shape)
print('\tTest Data Final Shape: ', test_x.shape)
print('--------------------------------------------------------------------------------')

##################################### Model Definition ###############################################
print("==> Defining the Model...")

# custom optimizer
optim = Adam(learning_rate=0.001, beta_1=0.9, beta_2=0.999, decay=0.01)

model = Sequential()
model.add(LSTM(units=200,
               #    activation='sigmoid',
               return_sequences=True,
               input_shape=(num_steps + 1, train_x.shape[2])))

model.add(Dropout(0.25))

model.add(LSTM(units=200,
               #    activation='sigmoid',
               return_sequences=False))

model.add(Dropout(0.20))

model.add(Dense(units=train_y.shape[1],
                activation='relu'))

print('\t\tModel Summary:')
model.summary()

print("\n\tCompiling the Model...")
model.compile(optimizer=optim,
              loss='mae',
              metrics=['mse', 'mae'])

es = EarlyStopping(monitor='val_loss',
                   min_delta=1e-10,
                   patience=10,
                   verbose=1)

rlr = ReduceLROnPlateau(monitor='val_loss',
                        factor=0.01,
                        patience=10,
                        verbose=1)

mcp = ModelCheckpoint(filepath="test.h5",
                      monitor='val_loss',
                      verbose=1,
                      save_best_only=True,
                      save_weights_only=False)

tb = TensorBoard('./logs')

print('\t Fitting Data into the Model...')
history = model.fit(train_x, train_y,
                    epochs=100,
                    batch_size=10,
                    validation_data=(test_x, test_y),
                    verbose=2,
                    shuffle=False,
                    callbacks=[mcp, rlr])

print('\n--------------------------------------------------------------------------------')
#################################### Plot And Results ################################################
print('==> Plotting and Results...')
plt.figure(figsize=(12, 8))
plt.plot(history.history['loss'], label='Train Loss', linewidth=4)
plt.plot(history.history['val_loss'], label='Validation Loss', linewidth=4)
plt.grid("minor")
plt.legend()
plt.savefig('Res.png')

# Make Prediction
yhat = model.predict(test_x)

# calculate RMSE
rmse = sqrt(mean_squared_error(test_y, yhat))
print('\n\t\t ==> Test RMSE: %.3f' % rmse)
