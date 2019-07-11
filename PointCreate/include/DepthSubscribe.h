#ifndef DEPTH_H
#define DEPTH_H


//ros area
#include <ros/ros.h>
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>

//opencv area
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

//c++ STL
#include<stdio.h>
#include<math.h>
#include<vector>

using namespace std;



static cv_bridge::CvImagePtr depth_ptr;
static cv::Point po;



void on_mouse(int event, int x, int y, void *param);

class DepthConverter
{
private:
    ros::NodeHandle nh_;
    //用于将msg信息转换为openCV中的Mat数据
    image_transport::ImageTransport it_;
    //订阅摄像头发布的信息
    image_transport::Subscriber image_sub_;
    bool flag_depth=false;


public:
    cv::Mat _src_depth;
    DepthConverter()
        : it_(nh_)
    {
        //设置订阅摄像机 /camera/depth/image_raw
        image_sub_ = it_.subscribe("/camera/depth/image_raw", 1, &DepthConverter::depth_Callback,this);
    }

    ~DepthConverter()
    {

    }

    //收到摄像机后的回调函数
    void depth_Callback(const sensor_msgs::ImageConstPtr& depth_msg)//调用深度图像
    {
        try
        {
            if(flag_depth==false)
            {
                depth_ptr = cv_bridge::toCvCopy(depth_msg, sensor_msgs::image_encodings::TYPE_32FC1);
                _src_depth=depth_ptr->image;
                flag_depth=true;
            }
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("Could not convert from ‘%s‘ to ‘32fc1‘.", depth_msg->encoding.c_str());
            return;
        }
        //        cv::imshow("aa",_src_depth);
        //        cv::waitKey(1);
    }

    void PrepareGrab()
    {
        flag_depth=false;
    }

    cv::Mat GrabSingleImage()
    {
        //超时计数
        int count=0;
        while(flag_depth==false)
        {
            usleep(1000);
            count++;
            if(count>1000)
                break;
        }
        return _src_depth.clone();
    }


    friend  void on_mouse(int event, int x, int y, void *param);
};



void on_mouse(int event, int x, int y, void *param)
{
    cv::Mat *img = reinterpret_cast<cv::Mat*>(param);

    if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标
    {
        po.x = x;
        po.y = y;
        cout <<"position: "<< po <<endl;
        float depth_point=static_cast<float>(img->at<float>(cv::Point(x, y)));
        if(depth_point == 0)
        {
            cout<<"NAN点"<<endl;
        }
        else
        {
            float depth_point_cm=(depth_point);
            cout <<"distance: " <<depth_point_cm<< endl;
            cout<<"请输入下一点:"<<endl;
        }
    }
}
















#endif // DEPTH_H
