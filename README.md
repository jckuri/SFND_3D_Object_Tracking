# SFND 3D Object Tracking

Sensor Fusion Nanodegree<br/>
https://www.udacity.com/course/sensor-fusion-engineer-nanodegree--nd313

# Missing File

The file `yolov3.weights` was too large (248,007,048 bytes) to be uploaded to
this github repository. So, I deleted it. You need to copy the file 
`yolov3.weights` again from a fresh Udacity workspace of this project, if you
want to run the project without problems.

```
ls -l dat/yolo/
total 277108
-rw-r--r-- 1 root root       625 Jun  7  2019 coco.names
-rw-r--r-- 1 root root      1914 Jun  7  2019 yolov3-tiny.cfg
-rw-r--r-- 1 root root  35434956 Jun  7  2019 yolov3-tiny.weights
-rw-r--r-- 1 root root      8342 Jun  7  2019 yolov3.cfg
-rw-r--r-- 1 root root 248007048 Jun  7  2019 yolov3.weights
```

# FP.1 Match 3D Objects

The method `matchBoundingBoxes` takes as input the previous and the current data frames and provides as output the ids of the matched regions of interest (the boxID property). Matches are the ones with the highest number of keypoint correspondences. Each bounding box is assigned the match candidate with the highest number of occurrences. 

In my code, I iterate through all the matches. I select the boxes that contain keypoints, both in the previous frame and in the current frame.
I fill a matrix of counts and add 1 to all the combinations of selected boxes in the previous and current frame.
Then I select the best matches based on that matrix of counts.

```
void matchBoundingBoxes(std::vector<cv::DMatch> &matches, std::map<int, int> &best_matches, DataFrame &previous_frame, DataFrame &current_frame) {
    // ...
    cv::KeyPoint previous_keypoint, current_keypoint;
    int previous_size = previous_frame.boundingBoxes.size();
    int current_size = current_frame.boundingBoxes.size();
    int counts[previous_size][current_size] = {};
    vector<int> previous_boxes_ids, current_boxes_ids;
    for(cv::DMatch match : matches) {
        previous_keypoint = previous_frame.keypoints[match.queryIdx];
        current_keypoint = current_frame.keypoints[match.trainIdx];
        previous_boxes_ids.clear();
        current_boxes_ids.clear();
        for(auto bounding_box : previous_frame.boundingBoxes) 
            if(bounding_box.roi.contains(previous_keypoint.pt))
                previous_boxes_ids.push_back(bounding_box.boxID);
        for(auto bounding_box : current_frame.boundingBoxes) 
            //if(bounding_box.roi.contains(previous_keypoint.pt))
            if(bounding_box.roi.contains(current_keypoint.pt))
                current_boxes_ids.push_back(bounding_box.boxID);
        for(int previous_id : previous_boxes_ids)
            for(int current_id : current_boxes_ids)
                counts[previous_id][current_id]++;
    }
    int max_count, max_id;
    for(int previous_id = 0; previous_id < previous_size; previous_id++) {
        max_count = 0;
        for(int current_id = 0; current_id < current_size; current_id++) 
            if(counts[previous_id][current_id] > max_count) {
                max_count = counts[previous_id][current_id];
                max_id = current_id;
            }
        best_matches[previous_id] = max_id;
    }
}
```

# FP.2 Compute Lidar-based TTC

The function `computeTTCLidar` computes the time-to-collision in seconds for all matched 3D objects using only Lidar measurements from the matched bounding boxes between current and previous frame. 
The code is able to deal with outlier Lidar points in a statistically robust way to avoid severe estimation errors. 

I compute a special kind of median of `x` values for selected lidar points in the previous and current frames.
Then I apply the formula in the video lectures to compute the TTC based on the median `x` values.

The special kind of median of `x` values only uses selected lidar points.
Lidar points are selected if their reflectivity values are not so far away from the mean reflectivity.
I also eliminate outliers which are in the maximum 20% and in the minimum 20%.
Then I take the median of the selected `x` values.

```
double median_x(std::vector<LidarPoint> lidar_points) {
    vector<double> xs;
    for(LidarPoint lidar_point : lidar_points)
        xs.push_back(lidar_point.x);
    return median(eliminate_outliers(xs, 0.2));
}

double mean_reflectivity(std::vector<LidarPoint> lidar_points) {
    double mean_r = 0;
    for(LidarPoint lidar_point : lidar_points)
        mean_r += lidar_point.r;
    mean_r /= lidar_points.size();
    return mean_r;
}

std::vector<LidarPoint> select_lidar_points(std::vector<LidarPoint> lidar_points) {
    double mean_r = mean_reflectivity(lidar_points);
    std::vector<LidarPoint> selected_lidar_points;
    for(LidarPoint lidar_point : lidar_points)
        if(lidar_point.r >= 0.2 * mean_r && lidar_point.r < 1.8 * mean_r) 
            selected_lidar_points.push_back(lidar_point);
    return selected_lidar_points;
}

void computeTTCLidar(std::vector<LidarPoint> &lidarPointsPrev,
                     std::vector<LidarPoint> &lidarPointsCurr, double frameRate, double &TTC)
{
    // ...
    if(lidarPointsPrev.size() == 0 || lidarPointsCurr.size() == 0) {
        TTC = NAN;
        return;
    }
    double previous_x = median_x(select_lidar_points(lidarPointsPrev));
    double current_x = median_x(select_lidar_points(lidarPointsCurr));
    double speed = (previous_x - current_x) / (1.0 / frameRate);
    if(speed < 0) {
        TTC = NAN;
        return;
    }
    TTC = current_x / speed;
}
```

# FP.3 Associate Keypoint Correspondences with Bounding Boxes

The function `clusterKptMatchesWithROI` prepares the TTC computation based on camera measurements by associating keypoint correspondences to the bounding boxes which enclose them. All matches which satisfy this condition must be added to a vector in the respective bounding box. The code adds the keypoint correspondences to the "kptMatches" property of the respective bounding boxes. Also, outlier matches have been removed based on the euclidean distance between them in relation to all the matches in the bounding box. 

I iterate through all keypoint matches.
If the bounding box contains the keypoint matches, the keypoints are added to the bounding box matches and the distances from the previous keypoints to the current keypoints are added to the array of distances.
Matches and keypoints are added to the bounding box only if the distances do not exceed a statistical distance threshold: 1.50 * mean distance.

```
// associate a given bounding box with the keypoints it contains
void clusterKptMatchesWithROI(BoundingBox &boundingBox, std::vector<cv::KeyPoint> &kptsPrev, std::vector<cv::KeyPoint> &kptsCurr, std::vector<cv::DMatch> &kptMatches)
{
    // ...
    cv::KeyPoint current_keypoints, previous_keypoints;
    vector<cv::DMatch> bb_matches;
    vector<double> distances;
    for(cv::DMatch keypoint_match : kptMatches) {
        current_keypoints = kptsCurr[keypoint_match.trainIdx];
        previous_keypoints = kptsCurr[keypoint_match.queryIdx];
        if(boundingBox.roi.contains(current_keypoints.pt)) {
            bb_matches.push_back(keypoint_match);
            distances.push_back(cv::norm(current_keypoints.pt - previous_keypoints.pt));
        }
    }
    int n_distances = distances.size();
    double distance_mean = std::accumulate(distances.begin(), distances.end(), 0.0) / n_distances;
    double distance_threshold = distance_mean * 1.50;
    for(int i = 0; i < n_distances; i++) {
        double distance = distances[i];
        cv::DMatch match = bb_matches[i];
        if(distance < distance_threshold) {
            boundingBox.kptMatches.push_back(match);
            boundingBox.keypoints.push_back(kptsCurr[match.trainIdx]);
        }
    }
}
```

# FP.4 Compute Camera-based TTC

The function `computeTTCCamera` computes the time-to-collision in seconds for all matched 3D objects using only keypoint correspondences from the matched bounding boxes between current and previous frame.
The code is able to deal with outlier correspondences in a statistically robust way to avoid severe estimation errors. 

Basically, I iterate through all the combinations of keypoint matches.
I compute the current distances between all the combinations of current keypoint matches.
And I compute the previous distances between all the combinations of previous keypoint matches.
I compute the distance ratios between the current distances and the previous distances.
I eliminate the distance ratios in the maximum 20% and in the minimum 20%.
I compute the median distance ratio by using only the selected distance ratios, to make this metric more robust.
I compute the TTC by using the formula in the video lectures.

```
double median(vector<double> xs) {
    std::sort(xs.begin(), xs.end());
    long n = xs.size();
    long middle_index = floor(n / 2.0);
    if(n % 2 == 1) return xs[middle_index];
    return (xs[middle_index - 1] + xs[middle_index]) / 2.0;
}

vector<double> eliminate_outliers(vector<double> xs, double percentage) {
    std::sort(xs.begin(), xs.end());
    long index = floor(xs.size() * percentage + 0.5);
    return std::vector<double>(xs.begin() + index, xs.end() - index);
}

// Compute time-to-collision (TTC) based on keypoint correspondences in successive images
void computeTTCCamera(std::vector<cv::KeyPoint> &kptsPrev, std::vector<cv::KeyPoint> &kptsCurr, 
                      std::vector<cv::DMatch> kptMatches, double frameRate, double &TTC, cv::Mat *visImg)
{
    // ...
    double min_distance = 90.0;
    double epsilon = std::numeric_limits<double>::epsilon();
    vector<double> distance_ratios;
    for(auto match1 = kptMatches.begin(); match1 != kptMatches.end() - 1; ++match1) {
        cv::KeyPoint current_keypoint1 = kptsCurr.at(match1->trainIdx);
        cv::KeyPoint previous_keypoint1 = kptsPrev.at(match1->queryIdx);
        for(auto match2 = kptMatches.begin() + 1; match2 != kptMatches.end(); ++match2) {
            cv::KeyPoint current_keypoint2 = kptsCurr.at(match2->trainIdx);
            cv::KeyPoint previous_keypoint2 = kptsPrev.at(match2->queryIdx);
            double current_distance = cv::norm(current_keypoint1.pt - current_keypoint2.pt);
            double previous_distance = cv::norm(previous_keypoint1.pt - previous_keypoint2.pt);
            if(previous_distance > epsilon && current_distance >= min_distance) 
                distance_ratios.push_back(current_distance / previous_distance);
        }
    }
    if(distance_ratios.size() == 0) {
        TTC = NAN;
        return;
    }
    double median_distance = median(eliminate_outliers(distance_ratios, 0.2));
    TTC = (-1 / frameRate) / (1 - median_distance);
}
```

# FP.5 Performance Evaluation 1

Here are good results with the best combination of detector and descriptor:
AKAZE as detector and BRIEF as descriptor. As you can see both the TTC LIDAR
(9.67 seconds) and the TTC CAMERA (9.72 seconds) are similar.

![images/Final_Results_TTC.png](images/Final_Results_TTC.png)

Moreover, the object detection algorithm works well:

![images/Object_classification.png](images/Object_classification.png)

**TASK:** Find examples where the TTC estimate of the Lidar sensor does not
seem plausible. Describe your observations and provide a sound argumentation 
why you think this happened. Several examples (2-3) have been identified and 
described in detail.

**TTC of LIDAR**

| Iteration | TTC_LIDAR |
|-----------|-----------|
| 1         | 12.51     |
| 2         | 11.51     |
| 3         | 15.47     |
| 4         | 16.33     |
| 5         | 16.23     |
| 6         | 13.11     |
| 7         | 12.47     |
| 8         | 12.79     |
| 9         | 12.8      |
| 10        | 11.09     |
| 11        | 12.7      |
| 12        | 9.42      |
| 13        | 9.97      |
| 14        | 8.67      |
| 15        | 8.46      |
| 16        | 10.35     |
| 17        | 9.68      |
| 18        | 8.4       |

From the table and graph, we can see that the TTC LIDAR is not a monotonically
decreasing function. It rather has bumps with ups and downs.

For example, from iteration 2 to iteration 3, the TTC goes from 11.51 seconds
to 15.47 seconds, which seems to be implausible.

From iteration 3 to iteration 4, the TTC goes up again instead of going down, 
from 15.47 seconds to 16.33 seconds, which seems to be wrong.

From iteration 5 to iteration 6, the TTC goes down abruptly with a difference
of more than 3 seconds in just 1 iteration, from 16.23 seconds to 13.11 seconds,
which is not smooth as the movements of cars in the video.

From iteration 15 to iteration 16, the TTC goes up again instead of going down, 
from 8.46 seconds to 10.35 seconds, which seems to be wrong.

![images/ttc_lidar.png](images/ttc_lidar.png)

These ups and downs in the TTC instead of a monotonically decreasing function 
as the cars in the video suggest could be caused by many reasons.

First, the mathematical model to predict the TTC is based on a linear model of
constant velocity without acceleration. In reality, the car in front is
decelerating and so is the ego car.

Second, both the car in front and the ego car are decelerating at different
paces and have different velocities. That could explain the ups and downs in 
the TTC estimates.

Third, errors in the estimation algorithms.

# FP.6 Performance Evaluation 2

**Task:**

Run several detector / descriptor combinations and look at the differences in TTC estimation. Find out which methods perform best and also include several examples where camera-based TTC estimation is way off. As with Lidar, describe your observations again and also look into potential reasons.

All detector / descriptor combinations implemented in previous chapters have been compared with regard to the TTC estimate on a frame-by-frame basis. To facilitate comparison, a spreadsheet and graph should be used to represent the different TTCs. 

**Method:**

I parametrize the application so it receives the detector algorithm, the 
descriptor algorithm, and a flag to show graphics or not. The application
writes a file `<DETECTOR>_<DESCRIPTOR>.csv` with the TTC estimates.

I computed all the possible combinations of detector algorithms and 
descriptor algorithms with the Unix script `test_all_combinations.sh`:

```
for DETECTOR in "SHITOMASI" "HARRIS" "FAST" "BRISK" "ORB" "AKAZE" "SIFT"
do
	for DESCRIPTOR in "BRISK" "BRIEF" "ORB" "FREAK" "AKAZE" "SIFT"
    do
        ./3D_object_tracking $DETECTOR $DESCRIPTOR NO_GRAPHICS
    done
done
```

Then, I wrote a Python script `mix_csv.py` that reads the generated CSV files
with the TTC estimates. This Python scripts discards the invalid combinations
that generated runtime errors or that generated NAN and infinity numbers.

Here is the list of all **invalid** combinations:
- 'SHITOMASI_AKAZE.csv'
- 'SHITOMASI_SIFT.csv'
- 'HARRIS_BRISK.csv'
- 'HARRIS_BRIEF.csv'
- 'HARRIS_ORB.csv'
- 'HARRIS_FREAK.csv'
- 'HARRIS_AKAZE.csv'
- 'HARRIS_SIFT.csv'
- 'FAST_AKAZE.csv'
- 'FAST_SIFT.csv'
- 'BRISK_AKAZE.csv'
- 'BRISK_SIFT.csv'
- 'ORB_BRISK.csv'
- 'ORB_ORB.csv'
- 'ORB_FREAK.csv'
- 'ORB_AKAZE.csv'
- 'ORB_SIFT.csv'
- 'AKAZE_BRISK.csv'
- 'AKAZE_SIFT.csv'
- 'SIFT_ORB.csv'
- 'SIFT_AKAZE.csv'
- 'SIFT_SIFT.csv'

The Python script `mix_csv.py` generated the following table with all the valid
results:

**TTCs of all combinations of detectors and descriptors**

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

I imported this CSV table to LibreOffice Calc and I produced this 3D graph:

![images/all_combinations.png](images/all_combinations.png)

The Python script `mix_csv.py` also computes the RMSE between Lidar TTC and the
TTC estimates for all the valid combinations of detector algorithms and 
descriptor algorithms.

| Detector & Descriptor      | RMSE    |
|----------------------------|---------|
| TTC_CAMERA_AKAZE_BRIEF     | 1.7250  |
| TTC_CAMERA_SIFT_BRISK      | 1.7434  |
| TTC_CAMERA_AKAZE_AKAZE     | 1.7612  |
| TTC_CAMERA_AKAZE_ORB       | 1.9963  |
| TTC_CAMERA_SHITOMASI_BRISK | 1.9969  |
| TTC_CAMERA_SIFT_FREAK      | 2.0050  |
| TTC_CAMERA_AKAZE_FREAK     | 2.0850  |
| TTC_CAMERA_SHITOMASI_FREAK | 2.1760  |
| TTC_CAMERA_SHITOMASI_ORB   | 2.3067  |
| TTC_CAMERA_SIFT_BRIEF      | 2.5985  |
| TTC_CAMERA_SHITOMASI_BRIEF | 2.7481  |
| TTC_CAMERA_BRISK_BRIEF     | 3.1940  |
| TTC_CAMERA_FAST_ORB        | 3.6636  |
| TTC_CAMERA_FAST_BRISK      | 4.8945  |
| TTC_CAMERA_BRISK_BRISK     | 6.2411  |
| TTC_CAMERA_BRISK_FREAK     | 6.3032  |
| TTC_CAMERA_BRISK_ORB       | 6.4085  |
| TTC_CAMERA_FAST_BRIEF      | 9.7233  |
| TTC_CAMERA_FAST_FREAK      | 11.6108 |
| TTC_CAMERA_ORB_BRIEF       | 16.7477 |

**Top 5 combinations of detectors and descriptors**

The Top 5 combinations of algorithms are:

| Detector & Descriptor      | RMSE    |
|----------------------------|---------|
| TTC_CAMERA_AKAZE_BRIEF     | 1.7250  |
| TTC_CAMERA_SIFT_BRISK      | 1.7434  |
| TTC_CAMERA_AKAZE_AKAZE     | 1.7612  |
| TTC_CAMERA_AKAZE_ORB       | 1.9963  |
| TTC_CAMERA_SHITOMASI_BRISK | 1.9969  |

I selected those valid combinations of algorithms.

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

And I draw their results in LibreOffice calc.
I included 2 additional columns: `TTC_LIDAR` and `AVERAGE_TTC`.

![images/top_5_combinations.png](images/top_5_combinations.png)

`AVERAGE_TTC` is not a monotonically decreasing function as the cars moving in
the video suggest. But `AVERAGE_TTC` is smoother than the other TTC curves.

These ups and downs in the TTC instead of a monotonically decreasing function 
as the cars in the video suggest could be caused by many reasons.

First, the mathematical model to predict the TTC is based on a linear model of
constant velocity without acceleration. In reality, the car in front is
decelerating and so is the ego car.

Second, both the car in front and the ego car are decelerating at different
paces and have different velocities. That could explain the ups and downs in 
the TTC estimates.

Third, errors in the estimation algorithms.

Fourth, cameras don't have depth information as lidar does. Depth in cameras
is estimated by algorithms in an indirect way.

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
