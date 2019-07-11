#ifndef CAMERAPARAM_H
#define CAMERAPARAM_H

#include<opencv2/opencv.hpp>
#include<iostream>
#include <Eigen/Core>
#include <Eigen/LU>


//using namespace cv;
using namespace std;


struct AtraParm
{
    float fx_rgb;
    float fy_rgb;
    float cx_rgb;
    float cy_rgb;
    float fx_ir;
    float fy_ir;
    float cx_ir;
    float cy_ir;

    Eigen::Matrix3f R_ir2rgb;
    Eigen::Vector3f T_ir2rgb;
};



class CameraParam
{
public:
    CameraParam();
    void Registration(cv::Mat&rgb,cv::Mat&smooth);
    void Registration(cv::Mat &rgb,cv::Mat&smooth,bool flag);
    cv::Mat _rgbRegistration,_depthroi,_rgbroi;
};

#endif // CAMERAPARAM_H







