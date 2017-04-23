#**Behavioral Cloning Project**

## The Goals

The goals / steps of this project are the following:
* Build, a convolution neural network in Keras that predicts steering angles from images
* Train and validate the model with a training and validation set
* Test that the model successfully drives around track one without leaving the road
* Summarize the results with a written report


[//]: # (Image References)

[image1]: ./examples/placeholder.png "Model Visualization"
[image2]: ./examples/placeholder.png "Grayscaling"
[image3]: ./examples/placeholder_small.png "Recovery Image"
[image4]: ./examples/placeholder_small.png "Recovery Image"
[image5]: ./examples/placeholder_small.png "Recovery Image"
[image6]: ./examples/placeholder_small.png "Normal Image"
[image7]: ./examples/placeholder_small.png "Flipped Image"

## Rubric Points
###Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/432/view) individually and describe how I addressed each point in my implementation.  

---
###Files Submitted and Code Quality

####1. Submission includes all required files and can be used to run the simulator in autonomous mode

My project includes the following files:
* model.py containing the script to create and train the model
* drive.py for driving the car in autonomous mode
* model.h5 containing a trained convolution neural network
* writeup.md summarizing the results

####2. Submission includes functional code
Using the Udacity provided simulator and my altered drive.py file, the car can be driven autonomously around the track by executing
```sh
python3 drive.py model.h5
```

####3. Submission source code

The model.py file contains the code for training and saving the convolution neural network. The file shows the pipeline I used for training and validating the model, and it contains comments to explain how the code works.
```sh
python3 model.py model.py
```


###Model Architecture and Training Strategy

####1. An appropriate model architecture has been employed

My network model consists of a convolution neural network with 5x5 filter sizes for the first 3 convolution layers and 3x3 for the last convolution layer.
The depths between 24 and 64. There are 4 fully connected layers.

The model includes RELU layers to introduce nonlinearity, and the data is normalized in the model using a Keras lambda layer.

####2. Attempts to reduce overfitting in the model

The model was trained and validated on different data sets to ensure that the model was not overfitting (code line 29). The model was tested by running it through the simulator and ensuring that the vehicle could stay on the track.

####3. Model parameter tuning

The model used an Adam optimizer, so the learning rate was not tuned manually.

####4. Appropriate training data

Training data was chosen to keep the vehicle driving on the road. I used a combination of center lane driving, recovering from the left and right sides of the road. I also cropped the image to reduce the computation effort.
![center camera](examples/center_camera.png)
*Example image from center camera*
![left camera](examples/left_camera.png)
*Image from left camera*
![right camera](examples/right_camera.png)
*Image from right camera*
![flipped center camera](examples/flipped_center_camera.png)
*Image of flipped center camera*
![cropped image](examples/crop_60_140.png)
*Cropped center camera image*


For details about how I created the training data, see the next section.

###Model Architecture and Training Strategy

####1. Solution Design Approach

The overall strategy for deriving a model architecture was to create a balanced dataset and using the center driving image and recovery images to keep the car on track.

My first step was to use a convolution neural network model similar to LeNet. I though this model might be appropriate, since I have used it pretty successfully for my Traffic Sign project.

I started with only center images without any preprocessing to see if the training and verification with simulator work flow works at all. At the first trial with the simulator the car went for a about 5sec autonomously and then fell off the track. Nevertheless I know the work flow is fine. I split my images and steering angle data into a training and validation set. I noticed my first model had a low mean squared error on the training set but a high mean squared error on the validation set, which indicates a overfitting. To combat the overfitting, I used the method learned from the lecture to add left camera and right camera images with angle correction as my recovery image to extend my datasets. I flipped the center images to enlarge the training set as well and used more complex Nvidia Network. After starting training the network with around 32000 images, I noticed it takes extreme long time to finish the training process. The trained model though worked much better than first trial, despite there is one curve the car always drive off on the ledges. After this I decided to look for more neat and elegant way instead of this "hard" way to collect data. I started to study how to use generator in python for saving computer memory resources and augmenting data randomly (cropping, flip image randomly, brightness random modification) on fly(code lines 87-105). In this case, not only save the computer memory resources, but also due to the randomness of augmenting data on fly, the total data set is number_of_Epocs(25) * number of original training (around 7200).

To balance the data set (code line 52-68), I'm inspired by the approach used by my mentor AN NGUYEN. I checked the number of images differences between left and right turning images and added additional left/right camera images to balance the data set.

At the end of the process, the vehicle is able to drive autonomously around the track without leaving the road.

####2. Final Model Architecture

The final model architecture (code lines 114-130) consisted of a convolution neural network with the following layers and layer sizes ...

Here is a visualization of the architecture.
![network architecture](examples/network_p3.png)

####3. Creation of the Training Set & Training Process

I decided first to give the Udacity data set a try to train my model and  I randomly shuffled the data set and put 20% of the data into a validation set.

I used this training data for training the model. The validation set helped determine if the model was over or under fitting. The ideal number of epochs was 25 as the training loss is converged after 20 Epochs. I used an adam optimizer so that manually training the learning rate wasn't necessary.
