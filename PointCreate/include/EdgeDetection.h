#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>

using namespace std;

class EdgeDetection
{
public:
    EdgeDetection();
    EdgeDetection(cv::Mat &it,cv::Mat &depth);
    void edge(cv::Mat &it,cv::Mat &depth);
    cv::Mat GetDepthFinal();
    cv::Mat GetRgbFinal();
private:
    cv::Mat _rgbroi;
    cv::Mat _depthroi;
    cv::Mat _rgb_final,_depth_final;
};



#endif // EDGEDETECTION_H
