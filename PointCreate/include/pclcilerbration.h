#ifndef PCLCILERBRATION_H
#define PCLCILERBRATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include<iostream>
#include<cmath>
#include<pcl/io/pcd_io.h>
#include<pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>

#define picture1 "depth_final.xml"
#define picture2 "rgb_final.xml"

#define label1 "depth_final"
#define label2 "rgb_final"


//using namespace cv;
using namespace std;


typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;

class PclCilerbration
{
public:
    PclCilerbration();
    void Cilerbr();



private:
    cv::Mat rgbImage;
    cv::Mat depthImage;
    const double camera_cx =307.7303;
    const double camera_cy =  275.8931;
    const double camera_fx =595.8248;
    const double camera_fy =609.1815;
};

#endif // PCLCILERBRATION_H
