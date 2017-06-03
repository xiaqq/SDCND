# Vehicle Detection

In this project, the goal is to write a software pipeline to detect vehicles in a video (project_video.mp4). A writeup documented the detailed explanation of the project frame processing pipeline and the idea behind that.


A great writeup is given, which includes the rubric points as well as a description of how I addressed each point.  They include a detailed description of the code used in each step, and links to other supporting documents or external references.  Images are provided to demonstrate how the code works with examples.  

The Project
---

The goals / steps of this project are the following:

* Perform a Histogram of Oriented Gradients (HOG) feature extraction on a labeled training set of images and train a classifier Linear SVM classifier
* Optionally, you can also apply a color transform and append binned color features, as well as histograms of color, to your HOG feature vector.
* Note: for those first two steps don't forget to normalize your features and randomize a selection for training and testing.
* Implement a sliding-window technique and use your trained classifier to search for vehicles in images.
* Run your pipeline on a video stream (start with the test_video.mp4 and later implement on full project_video.mp4) and create a heat map of recurring detections frame by frame to reject outliers and follow detected vehicles.
* Estimate a bounding box for vehicles detected.

Here are links to the labeled data for [vehicle](https://s3.amazonaws.com/udacity-sdc/Vehicle_Tracking/vehicles.zip) and [non-vehicle](https://s3.amazonaws.com/udacity-sdc/Vehicle_Tracking/non-vehicles.zip) examples to train your classifier.  These example images come from a combination of the [GTI vehicle image database](http://www.gti.ssr.upm.es/data/Vehicle_database.html), the [KITTI vision benchmark suite](http://www.cvlibs.net/datasets/kitti/).

Examples of the output from each stage of the pipeline in the folder called `ouput_images`, and I have included them in my writeup for the project by describing what each image shows.  The video called `project_video_ouput.mp4` is the video my pipeline should work well on.  
