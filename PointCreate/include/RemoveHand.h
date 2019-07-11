#ifndef REMOVE_HAND_H
#define REMOVE_HAND_H

#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;

class RemoveHand{
private:
    cv::Mat rgb_img;
public:
    RemoveHand();
    RemoveHand(cv::Mat img);
    ~RemoveHand();
    cv::Mat HandSegmentation(cv::Mat &img);

};

#endif // REMOVE_HAND_H
