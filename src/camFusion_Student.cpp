
#include <iostream>
#include <algorithm>
#include <numeric>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "camFusion.hpp"
#include "dataStructures.h"

using namespace std;


// Create groups of Lidar points whose projection into the camera falls into the same bounding box
void clusterLidarWithROI(std::vector<BoundingBox> &boundingBoxes, std::vector<LidarPoint> &lidarPoints, float shrinkFactor, cv::Mat &P_rect_xx, cv::Mat &R_rect_xx, cv::Mat &RT)
{
    // loop over all Lidar points and associate them to a 2D bounding box
    cv::Mat X(4, 1, cv::DataType<double>::type);
    cv::Mat Y(3, 1, cv::DataType<double>::type);

    for (auto it1 = lidarPoints.begin(); it1 != lidarPoints.end(); ++it1)
    {
        // assemble vector for matrix-vector-multiplication
        X.at<double>(0, 0) = it1->x;
        X.at<double>(1, 0) = it1->y;
        X.at<double>(2, 0) = it1->z;
        X.at<double>(3, 0) = 1;

        // project Lidar point into camera
        Y = P_rect_xx * R_rect_xx * RT * X;
        cv::Point pt;
        // pixel coordinates
        pt.x = Y.at<double>(0, 0) / Y.at<double>(2, 0); 
        pt.y = Y.at<double>(1, 0) / Y.at<double>(2, 0); 

        vector<vector<BoundingBox>::iterator> enclosingBoxes; // pointers to all bounding boxes which enclose the current Lidar point
        for (vector<BoundingBox>::iterator it2 = boundingBoxes.begin(); it2 != boundingBoxes.end(); ++it2)
        {
            // shrink current bounding box slightly to avoid having too many outlier points around the edges
            cv::Rect smallerBox;
            smallerBox.x = (*it2).roi.x + shrinkFactor * (*it2).roi.width / 2.0;
            smallerBox.y = (*it2).roi.y + shrinkFactor * (*it2).roi.height / 2.0;
            smallerBox.width = (*it2).roi.width * (1 - shrinkFactor);
            smallerBox.height = (*it2).roi.height * (1 - shrinkFactor);

            // check wether point is within current bounding box
            if (smallerBox.contains(pt))
            {
                enclosingBoxes.push_back(it2);
            }

        } // eof loop over all bounding boxes

        // check wether point has been enclosed by one or by multiple boxes
        if (enclosingBoxes.size() == 1)
        { 
            // add Lidar point to bounding box
            enclosingBoxes[0]->lidarPoints.push_back(*it1);
        }

    } // eof loop over all Lidar points
}

/* 
* The show3DObjects() function below can handle different output image sizes, but the text output has been manually tuned to fit the 2000x2000 size. 
* However, you can make this function work for other sizes too.
* For instance, to use a 1000x1000 size, adjusting the text positions by dividing them by 2.
*/
void show3DObjects(std::vector<BoundingBox> &boundingBoxes, cv::Size worldSize, cv::Size imageSize, bool bWait)
{
    // create topview image
    cv::Mat topviewImg(imageSize, CV_8UC3, cv::Scalar(255, 255, 255));

    for(auto it1=boundingBoxes.begin(); it1!=boundingBoxes.end(); ++it1)
    {
        // create randomized color for current 3D object
        cv::RNG rng(it1->boxID);
        cv::Scalar currColor = cv::Scalar(rng.uniform(0,150), rng.uniform(0, 150), rng.uniform(0, 150));

        // plot Lidar points into top view image
        int top=1e8, left=1e8, bottom=0.0, right=0.0; 
        float xwmin=1e8, ywmin=1e8, ywmax=-1e8;
        for (auto it2 = it1->lidarPoints.begin(); it2 != it1->lidarPoints.end(); ++it2)
        {
            // world coordinates
            float xw = (*it2).x; // world position in m with x facing forward from sensor
            float yw = (*it2).y; // world position in m with y facing left from sensor
            xwmin = xwmin<xw ? xwmin : xw;
            ywmin = ywmin<yw ? ywmin : yw;
            ywmax = ywmax>yw ? ywmax : yw;

            // top-view coordinates
            int y = (-xw * imageSize.height / worldSize.height) + imageSize.height;
            int x = (-yw * imageSize.width / worldSize.width) + imageSize.width / 2;

            // find enclosing rectangle
            top = top<y ? top : y;
            left = left<x ? left : x;
            bottom = bottom>y ? bottom : y;
            right = right>x ? right : x;

            // draw individual point
            cv::circle(topviewImg, cv::Point(x, y), 4, currColor, -1);
        }

        // draw enclosing rectangle
        cv::rectangle(topviewImg, cv::Point(left, top), cv::Point(right, bottom),cv::Scalar(0,0,0), 2);

        // augment object with some key data
        char str1[200], str2[200];
        sprintf(str1, "id=%d, #pts=%d", it1->boxID, (int)it1->lidarPoints.size());
        putText(topviewImg, str1, cv::Point2f(left-250, bottom+50), cv::FONT_ITALIC, 2, currColor);
        sprintf(str2, "xmin=%2.2f m, yw=%2.2f m", xwmin, ywmax-ywmin);
        putText(topviewImg, str2, cv::Point2f(left-250, bottom+125), cv::FONT_ITALIC, 2, currColor);  
    }

    // plot distance markers
    float lineSpacing = 2.0; // gap between distance markers
    int nMarkers = floor(worldSize.height / lineSpacing);
    for (size_t i = 0; i < nMarkers; ++i)
    {
        int y = (-(i * lineSpacing) * imageSize.height / worldSize.height) + imageSize.height;
        cv::line(topviewImg, cv::Point(0, y), cv::Point(imageSize.width, y), cv::Scalar(255, 0, 0));
    }

    // display image
    string windowName = "3D Objects";
    cv::namedWindow(windowName, 1);
    cv::imshow(windowName, topviewImg);

    if(bWait)
    {
        cv::waitKey(0); // wait for key to be pressed
    }
}


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