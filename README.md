# SFND 3D Object Tracking

Sensor Fusion Nanodegree
https://www.udacity.com/course/sensor-fusion-engineer-nanodegree--nd313

# FP.1 Match 3D Objects


# FP.2 Compute Lidar-based TTC


# FP.3 Associate Keypoint Correspondences with Bounding Boxes


# FP.4 Compute Camera-based TTC


# FP.5 Performance Evaluation 1


# FP.6 Performance Evaluation 2

**Data of all combinations of detectors and descriptors**

| TTC_LIDAR | TTC_CAMERA_SHITOMASI_BRISK | TTC_CAMERA_SHITOMASI_BRIEF | TTC_CAMERA_SHITOMASI_ORB | TTC_CAMERA_SHITOMASI_FREAK | TTC_CAMERA_FAST_BRISK | TTC_CAMERA_FAST_BRIEF | TTC_CAMERA_FAST_ORB | TTC_CAMERA_FAST_FREAK | TTC_CAMERA_BRISK_BRISK | TTC_CAMERA_BRISK_BRIEF | TTC_CAMERA_BRISK_ORB | TTC_CAMERA_BRISK_FREAK | TTC_CAMERA_ORB_BRIEF | TTC_CAMERA_AKAZE_BRIEF | TTC_CAMERA_AKAZE_ORB | TTC_CAMERA_AKAZE_FREAK | TTC_CAMERA_AKAZE_AKAZE | TTC_CAMERA_SIFT_BRISK | TTC_CAMERA_SIFT_BRIEF | TTC_CAMERA_SIFT_FREAK |
|-----------|----------------------------|----------------------------|--------------------------|----------------------------|-----------------------|-----------------------|---------------------|-----------------------|------------------------|------------------------|----------------------|------------------------|----------------------|------------------------|----------------------|------------------------|------------------------|-----------------------|-----------------------|-----------------------|
| 12.51     | 12.5                       | 13.99                      | 13.7                     | 12.52                      | 11.66                 | 12.18                 | 11.75               | 8.46                  | 11.06                  | 15.15                  | 18.45                | 14.41                  | 15.04                | 13.58                  | 11.92                | 11.8                   | 12.65                  | 13.15                 | 11.41                 | 16.92                 |
| 11.51     | 12.92                      | 14.97                      | 12.66                    | 14.34                      | 11.76                 | 12.65                 | 22.33               | 12.18                 | 23.51                  | 12.9                   | 20.28                | 16.33                  | 11.76                | 15.67                  | 15.66                | 16.14                  | 15.29                  | 13.18                 | 15.46                 | 13.95                 |
| 15.47     | 17.74                      | 11.02                      | 11.79                    | 15.35                      | 15.07                 | 16.26                 | 13.76               | 13.02                 | 15.67                  | 13.11                  | 13.25                | 14.39                  | 35.37                | 13.44                  | 14.33                | 14.35                  | 12.45                  | 12.06                 | 15.62                 | 15.26                 |
| 16.33     | 13.25                      | 12.66                      | 12.43                    | 11.99                      | 11.47                 | 11.26                 | 12.37               | 12.45                 | 15.14                  | 14.58                  | 19.27                | 15.25                  | 26.26                | 14.64                  | 14.17                | 14.78                  | 14.56                  | 17.33                 | 21.91                 | 18.13                 |
| 16.23     | 11.57                      | 13.0                       | 14.06                    | 12.37                      | 35.1                  | 56.34                 | 22.2                | 64.54                 | 36.81                  | 13.11                  | 32.17                | 39.43                  | 42.56                | 15.45                  | 12.88                | 14.84                  | 14.73                  | 16.9                  | 17.84                 | 14.16                 |
| 13.11     | 13.91                      | 12.65                      | 15.18                    | 13.4                       | 15.48                 | 15.49                 | 13.58               | 12.04                 | 12.78                  | 15.67                  | 29.08                | 14.22                  | 9.63                 | 11.64                  | 14.2                 | 16.03                  | 14.17                  | 11.38                 | 12.35                 | 11.19                 |
| 12.47     | 12.24                      | 13.44                      | 11.88                    | 12.41                      | 13.43                 | 12.99                 | 15.34               | 12.82                 | 17.73                  | 17.51                  | 14.14                | 16.78                  | 37.27                | 15.83                  | 15.76                | 16.38                  | 15.24                  | 17.56                 | 18.34                 | 14.71                 |
| 12.79     | 13.93                      | 13.89                      | 12.95                    | 11.87                      | 11.91                 | 11.25                 | 11.4                | 11.76                 | 14.75                  | 18.98                  | 15.08                | 19.73                  | 31.15                | 14.04                  | 14.32                | 13.29                  | 14.32                  | 14.89                 | 15.61                 | 14.97                 |
| 12.8      | 11.96                      | 11.5                       | 10.72                    | 13.09                      | 13.02                 | 13.05                 | 15.51               | 13.72                 | 11.73                  | 14.56                  | 16.29                | 12.89                  | 66.24                | 14.49                  | 14.5                 | 14.11                  | 13.97                  | 13.92                 | 12.11                 | 12.83                 |
| 11.09     | 11.68                      | 12.21                      | 12.5                     | 12.56                      | 14.4                  | 11.99                 | 14.04               | 13.73                 | 11.94                  | 15.19                  | 14.6                 | 15.04                  | 9.65                 | 11.88                  | 12.26                | 11.15                  | 11.59                  | 10.82                 | 14.57                 | 11.52                 |
| 12.7      | 10.96                      | 11.29                      | 10.92                    | 11.04                      | 11.58                 | 12.14                 | 12.21               | 12.13                 | 12.72                  | 16.9                   | 13.01                | 11.47                  | 13.31                | 13.2                   | 11.67                | 12.09                  | 12.69                  | 12.47                 | 12.81                 | 15.14                 |
| 9.42      | 11.1                       | 11.5                       | 12.93                    | 12.59                      | 11.8                  | 9.55                  | 12.56               | 12.48                 | 10.49                  | 13.11                  | 12.29                | 12.59                  | 12.92                | 10.95                  | 11.56                | 13.46                  | 11.75                  | 10.69                 | 12.33                 | 11.54                 |
| 9.97      | 12.87                      | 12.74                      | 12.27                    | 11.73                      | 11.76                 | 12.78                 | 11.31               | 13.85                 | 13.23                  | 13.05                  | 10.11                | 12.62                  | 11.27                | 10.83                  | 9.88                 | 11.46                  | 11.28                  | 9.9                   | 9.29                  | 9.14                  |
| 8.67      | 11.8                       | 13.94                      | 12.06                    | 11.79                      | 11.04                 | 10.85                 | 11.7                | 10.65                 | 10.99                  | 9.55                   | 10.78                | 11.57                  | 9.56                 | 10.35                  | 10.57                | 9.42                   | 10.0                   | 9.47                  | 9.95                  | 10.25                 |
| 8.46      | 10.5                       | 13.35                      | 10.43                    | 9.42                       | 9.1                   | 11.91                 | 11.7                | 9.05                  | 16.63                  | 12.17                  | 14.29                | 14.25                  | 10.86                | 9.7                    | 11.2                 | 9.93                   | 10.4                   | 8.76                  | 9.82                  | 10.4                  |
| 10.35     | 10.45                      | 12.93                      | 13.81                    | 7.87                       | 11.99                 | 13.62                 | 12.73               | 11.39                 | 11.55                  | 11.77                  | 13.65                | 9.48                   | 12.49                | 9.91                   | 11.06                | 9.6                    | 9.61                   | 9.24                  | 8.83                  | 8.65                  |
| 9.68      | 10.4                       | 10.34                      | 9.21                     | 12.3                       | 9.81                  | 8.19                  | 10.42               | 11.4                  | 9.36                   | 8.95                   | 8.91                 | 10.77                  | 12.05                | 9.73                   | 9.18                 | 9.55                   | 9.06                   | 9.46                  | 9.19                  | 8.79                  |
| 8.4       | 9.68                       | 8.41                       | 7.89                     | 9.25                       | 12.06                 | 12.67                 | 10.74               | 11.81                 | 11.73                  | 11.29                  | 12.08                | 9.7                    | 9.49                 | 9.47                   | 9.32                 | 10.05                  | 9.21                   | 8.74                  | 9.05                  | 10.56                 |

**Top 5 combinations of detectors and descriptors**

| TTC_LIDAR | TTC_CAMERA_SHITOMASI_BRISK | TTC_CAMERA_AKAZE_BRIEF | TTC_CAMERA_AKAZE_ORB | TTC_CAMERA_AKAZE_AKAZE | TTC_CAMERA_SIFT_BRISK | AVERAGE_TTC |
|-----------|----------------------------|------------------------|----------------------|------------------------|-----------------------|-------------|
| 12.51     | 12.5                       | 13.58                  | 11.92                | 12.65                  | 13.15                 | 12.72       |
| 11.51     | 12.92                      | 15.67                  | 15.66                | 15.29                  | 13.18                 | 14.04       |
| 15.47     | 17.74                      | 13.44                  | 14.33                | 12.45                  | 12.06                 | 14.25       |
| 16.33     | 13.25                      | 14.64                  | 14.17                | 14.56                  | 17.33                 | 15.05       |
| 16.23     | 11.57                      | 15.45                  | 12.88                | 14.73                  | 16.9                  | 14.63       |
| 13.11     | 13.91                      | 11.64                  | 14.2                 | 14.17                  | 11.38                 | 13.07       |
| 12.47     | 12.24                      | 15.83                  | 15.76                | 15.24                  | 17.56                 | 14.85       |
| 12.79     | 13.93                      | 14.04                  | 14.32                | 14.32                  | 14.89                 | 14.05       |
| 12.8      | 11.96                      | 14.49                  | 14.5                 | 13.97                  | 13.92                 | 13.61       |
| 11.09     | 11.68                      | 11.88                  | 12.26                | 11.59                  | 10.82                 | 11.55       |
| 12.7      | 10.96                      | 13.2                   | 11.67                | 12.69                  | 12.47                 | 12.28       |
| 9.42      | 11.1                       | 10.95                  | 11.56                | 11.75                  | 10.69                 | 10.91       |
| 9.97      | 12.87                      | 10.83                  | 9.88                 | 11.28                  | 9.9                   | 10.79       |
| 8.67      | 11.8                       | 10.35                  | 10.57                | 10                     | 9.47                  | 10.14       |
| 8.46      | 10.5                       | 9.7                    | 11.2                 | 10.4                   | 8.76                  | 9.84        |
| 10.35     | 10.45                      | 9.91                   | 11.06                | 9.61                   | 9.24                  | 10.10       |
| 9.68      | 10.4                       | 9.73                   | 9.18                 | 9.06                   | 9.46                  | 9.59        |
| 8.4       | 9.68                       | 9.47                   | 9.32                 | 9.21                   | 8.74                  | 9.14        |

-----------------------------------------------------------------------------------

# SFND 3D Object Tracking

Welcome to the final project of the camera course. By completing all the lessons, you now have a solid understanding of keypoint detectors, descriptors, and methods to match them between successive images. Also, you know how to detect objects in an image using the YOLO deep-learning framework. And finally, you know how to associate regions in a camera image with Lidar points in 3D space. Let's take a look at our program schematic to see what we already have accomplished and what's still missing.

<img src="images/course_code_structure.png" width="779" height="414" />

In this final project, you will implement the missing parts in the schematic. To do this, you will complete four major tasks: 
1. First, you will develop a way to match 3D objects over time by using keypoint correspondences. 
2. Second, you will compute the TTC based on Lidar measurements. 
3. You will then proceed to do the same using the camera, which requires to first associate keypoint matches to regions of interest and then to compute the TTC based on those matches. 
4. And lastly, you will conduct various tests with the framework. Your goal is to identify the most suitable detector/descriptor combination for TTC estimation and also to search for problems that can lead to faulty measurements by the camera or Lidar sensor. In the last course of this Nanodegree, you will learn about the Kalman filter, which is a great way to combine the two independent TTC measurements into an improved version which is much more reliable than a single sensor alone can be. But before we think about such things, let us focus on your final project in the camera course. 

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level project directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./3D_object_tracking`.