#ifndef DEPTHPROCESS_H
#define DEPTHPROCESS_H

#include<opencv2/opencv.hpp>
#include<iostream>
#include <Eigen/Core>
#include <Eigen/LU>
#include <thread>

using namespace std;
//using namespace cv;


class DepthProcess
{
public:
    DepthProcess();
    DepthProcess(const cv::String &xml,const cv::String &label);
    ~DepthProcess();
    DepthProcess(const DepthProcess&);
    DepthProcess& operator=(const DepthProcess& );
    cv::Mat DepthRead(const cv::String &xml,const cv::String &label,bool flag=true);
    void DepthWrite(const cv::String &xml);
    void DepthWrite(const cv::String &xml,cv::Mat&src);
    void copyto(cv::Mat&_roi1,cv::Mat&_roi2);
    void DepthShow();
    void SmoothBlur(cv::Mat &);
    void ThresholdSegmentation(cv::Point2i &point);
    cv::Mat GetSmoothImage();
    cv::Mat GetDepthFilter();
    cv::Mat GetRgbFilter();


private:
    cv::Mat _depth;
    cv::Mat _smooth;
    cv::Mat _rgbroi;
    cv::Mat _depthroi;
    cv::Mat _depth_filter;
    cv::Mat _depth_mask;
    cv::Mat _rgb_filter;



};

#endif // DEPTHPROCESS_H
