import matplotlib.image as mpimg
import numpy as np
import cv2
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten,Lambda
from keras.activations import relu
from keras.layers.convolutional import Convolution2D
from keras.optimizers import Adam
from keras.regularizers import l2
import tensorflow as tf
import pandas
import random
from random import randrange
from sklearn.utils import shuffle
from sklearn.model_selection import train_test_split



import math
##read all the necessary samples: left, center and right camera
colnames = ['center', 'left', 'right', 'steering', 'throttle', 'brake', 'speed']
data = pandas.read_csv('driving_log.csv', skiprows=[0], names=colnames)
center = data.center.tolist()
left = data.left.tolist()
right = data.right.tolist()
angles = data.steering.tolist()

center, angles = shuffle(center, angles)
center, X_valid, angles, y_valid = train_test_split(center, angles, test_size = 0.20, random_state = 100)

data_left,data_right,data_straight = [], [], []
angle_right,angle_left,angle_straight = [], [], []

print (len(angles))
for i,angle in enumerate(angles):
    #Positive angle is turning from Left -> Right. Negative is turning from Right -> Left#
    if angle < -0.1:
        data_left.append(center[i])
        angle_left.append(angle)
    elif angle > 0.1:
        data_right.append(center[i])
        angle_right.append(angle)
    else: ## -0.1<= angle<=0.1
        data_straight.append(center[i])
        angle_straight.append(angle)


## ADD RECOVERY Data ##
## Inspired by the discussion with my Mentor AN NGUYEN ##
#  The number of sample differences between straight,left and right and generate a random list for left and right recovery

left_addition = len(data_straight) - len(data_left)
right_addition = len(data_straight) - len(data_right)

left_recovery = random.sample(range(math.ceil(len(center))), left_addition) #indice_L
right_recovery = random.sample(range(math.ceil(len(center))), right_addition)

# Filter angle less than -0.15 and add right camera images into driving left list, minus an adjustment angle #
for i in left_recovery:
    if angles[i] < -0.1:
        data_left.append(right[i])
        angle_left.append(angles[i] - 0.265)

# Filter angle more than 0.15 and add left camera images into driving right list, add an adjustment angle #
for i in right_recovery:
    if angles[i] > 0.1:
        data_right.append(left[i])
        angle_right.append(angles[i] + 0.265)


####Generate Training data!!#####
X_train = data_straight + data_left + data_right
y_train = np.float32(angle_straight + angle_left + angle_right)


def crop_resize(image):
    cropped = cv2.resize(image[60:140,:], (64,64))
    return cropped

def augment_brightness_camera_images(image):
    image1 = cv2.cvtColor(image,cv2.COLOR_RGB2HSV) #hue, saturation, and brightness
    random_bright = .25+np.random.uniform()
    image1[:,:,2] = image1[:,:,2]*random_bright
    image1 = cv2.cvtColor(image1,cv2.COLOR_HSV2RGB)
    return image1

def generator_data(data, angle, batch_size):
    batch_train = np.zeros((batch_size, 64, 64, 3), dtype = np.float32)
    batch_angle = np.zeros((batch_size,), dtype = np.float32)
    while True:
        data, angle = shuffle(data, angle)
        for i in range(batch_size):

            random_pick = randrange(len(data))
            batch_train[i] = crop_resize(mpimg.imread(data[random_pick].strip()))
            batch_train[i] = augment_brightness_camera_images(batch_train[i])
            batch_angle[i] = angle[random_pick]*(np.random.uniform(0.9,1.1))

            #Flip random images#
            flip_prob = np.random.random()
            if flip_prob > 0.5:
                batch_train[i] = np.fliplr(batch_train[i])
                batch_angle[i] = -batch_angle[i]

        yield batch_train, batch_angle
        
### PART 3: TRAINING ###

data_generator = generator_data(X_train,y_train,128)
valid_generator = generator_data(X_valid, y_valid, 128)

# Training Architecture: inspired by NVIDIA architecture #

model = Sequential()
input_shape=(64,64,3)
model.add(Lambda(lambda x: (x /255.0)-0.5, input_shape=input_shape))
model.add(Convolution2D(24, 5, 5, border_mode='valid', subsample =(2,2), W_regularizer = l2(0.001)))
model.add(Activation('relu'))
model.add(Convolution2D(36, 5, 5, border_mode='valid', subsample =(2,2), W_regularizer = l2(0.001)))
model.add(Activation('relu'))
model.add(Convolution2D(48, 5, 5, border_mode='valid', subsample = (2,2), W_regularizer = l2(0.001)))
model.add(Activation('relu'))
model.add(Convolution2D(64, 3, 3, border_mode='same', subsample = (2,2), W_regularizer = l2(0.001)))
model.add(Activation('relu'))
model.add(Flatten())
model.add(Dense(100))
model.add(Dense(50))
model.add(Dense(10))
model.add(Dense(1))
model.compile(optimizer= 'adam', loss='mse')
model.summary()
model.fit_generator(data_generator, samples_per_epoch = math.ceil(len(X_train)), nb_epoch=25, validation_data = valid_generator, nb_val_samples = len(X_valid))
model.save('model.h5')
print('Done Training')