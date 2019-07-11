#ifndef RGB_IMAGE_PROCESS_H
#define RGB_IMAGE_PROCESS_H

#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;

class RgbImageProcess{
private:
    cv::Mat rgb_img;
public:
    RgbImageProcess();
    RgbImageProcess(cv::Mat img);
    ~RgbImageProcess();
    cv::Mat RgbSegmentation(cv::Mat &rgb_src);
    cv::Point Center(cv::Mat &img);

};

#endif // RGB_IMAGE_PROCESS_H
