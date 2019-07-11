#ifndef RGB_H
#define RGB_H
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
#include<unistd.h>


using namespace std;

static cv_bridge::CvImagePtr rgb_ptr;
static cv::Point p;


void onMouse(int event, int x, int y, void *param);

class RgbConverter
{
private:
    ros::NodeHandle nh_;
    //用于将msg信息转换为openCV中的Mat数据
    image_transport::ImageTransport it_;
    //订阅摄像头发布的信息
    image_transport::Subscriber image_sub_;
    bool flag_rgb=false;


public:
    cv::Mat _src_rgb;
    RgbConverter()
        : it_(nh_)
    {

        //设置订阅摄像机 /camera/rgb/image_raw
        image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1, &RgbConverter::rgb_Callback,this);

    }

    ~RgbConverter()
    {

    }


    //收到摄像机后的回调函数
    void rgb_Callback(const sensor_msgs::ImageConstPtr& msg){
        try{

            if(flag_rgb==false)
            {
                rgb_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_8UC3);
                _src_rgb=rgb_ptr->image;
                flag_rgb=true;
            }
        }

        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }
        //        cv::imshow("_ss",_src_rgb);
        //        cv::waitKey(1);
    }
    void PrepareGrab()
    {
        flag_rgb=false;
    }

    cv::Mat GrabSingleImage()
    {
        int count=0;
        while(flag_rgb==false)
        {
            usleep(1000);
            count++;
            if(count>1000)
                break;
        }
        return _src_rgb.clone();
    }

    friend  void onMouse(int event, int x, int y, void *param);
};



void onMouse(int event, int x, int y,  void *param)
{
    cv::Mat *img = reinterpret_cast<cv::Mat*>(param);
    if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标
    {
        p.x = x;
        p.y = y;
        cout <<"position: "<< p <<endl;
        int rgb_point_B=static_cast<int>(img->at<cv::Vec3b>(x,y)[0]);
        int rgb_point_G=static_cast<int>(img->at<cv::Vec3b>(x,y)[1]);
        int rgb_point_R=static_cast<int>(img->at<cv::Vec3b>(x,y)[2]);
        cout <<"rgb_point_B:"<<rgb_point_B<<endl;
        cout <<"rgb_point_G:"<<rgb_point_G<<endl;
        cout <<"rgb_point_R:"<<rgb_point_R<<endl;
        cout<<"请输入下一点:"<<endl;
    }
}



#endif // RGB_H

