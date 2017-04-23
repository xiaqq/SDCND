# Behaviorial Cloning Project

[![Udacity - Self-Driving Car NanoDegree](https://s3.amazonaws.com/udacity-sdc/github/shield-carnd.svg)](http://www.udacity.com/drive)

Overview
---
This repository contains starting files for the Behavioral Cloning Project.

In this project, I will use what deep neural networks and convolutional neural networks to clone driving behavior. I will train, validate and test a model using Keras. The model will output a steering angle to an autonomous vehicle (linear regression).

Udacity provided a simulator where one can steer a car around a track for data collection. Udacity also prepared a ready-made dataset for one to use it directly. I'll use those ready-made image data and steering angles to train a neural network and then use this model to drive the car autonomously around the track.

A detailed writeup of the project is provided (writeup.md).

To meet specifications, the project will require submitting five files:
* model.py (script used to create and train the model)
* drive.py (script to drive the car - feel free to modify this file)
* model.h5 (a trained Keras model)
* writeup.md (a report writeup file)
* video.mp4 (a video recording of the vehicle driving autonomously around the track for one full lap)

This README file describes how to output the video in the "Details About Files In This Directory" section.

The Project
---
The goals / steps of this project are the following:
* Design, train and validate a model that predicts a steering angle from image data
* Use the model to drive the vehicle autonomously around the first track in the simulator. The vehicle should remain on the road for an entire loop around the track.
* Summarize the results with a written report

### Dependencies
This lab requires:

* [CarND Term1 Starter Kit](https://github.com/udacity/CarND-Term1-Starter-Kit)

The lab enviroment can be created with CarND Term1 Starter Kit. Click [here](https://github.com/udacity/CarND-Term1-Starter-Kit/blob/master/README.md) for the details.

## Details About Files In This Directory

### `drive.py`

Usage of `drive.py` requires the saved the trained model model.h5. it can be used with drive.py using this command:

```sh
python drive.py model.h5
```

The above command will load the trained model and use the model to make predictions on individual images in real-time and send the predicted angle back to the server via a websocket connection. The drive.py files is modified to match the crop image size in the model.py.

To save a video recording, using this command:
```sh
python drive.py model.h5 run1
```
he fourth argument `run1` is the directory to save the images seen by the agent to. If the directory already exists it'll be overwritten.

Note: There is known local system's setting issue with replacing "," with "." when using drive.py. When this happens it can make predicted steering values clipped to max/min values. If this occurs, a known fix for this is to add "export LANG=en_US.utf8" to the bashrc file.

The image file name is a timestamp when the image image was seen. This information is used by `video.py` to create a chronological video of the agent driving.

### `video.py`

```sh
python video.py run1
```
The default FPS is 60.
Create a video based on images found in the `run1` directory. The name of the video will be name of the directory following by `'.mp4'`, so, in this case the video will be `run1.mp4`.
