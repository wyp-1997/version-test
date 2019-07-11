#include<RgbSubscribe.h>
#include<DepthSubscribe.h>
#include<CameraParam.h>
#include<DepthProcess.h>
#include<RgbImageProcess.h>
#include<RemoveHand.h>
#include<EdgeDetection.h>
#include<iostream>
#include<thread>
#include<mutex>
#include<iostream>
#include<cmath>
#include<unistd.h>
#include<pcl/io/pcd_io.h>
#include<pcl/point_types.h>
#include<pcl/visualization/pcl_visualizer.h>
#include<ctime>


const double camera_cx =307.7303;
const double camera_cy =  275.8931;
const double camera_fx =595.8248;
const double camera_fy =609.1815;


//using namespace cv;
using namespace std;
int i=0;
bool IsWorking =true;

typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;

void PointCloudCompose(cv::Mat&rgbImage,cv::Mat &depthImage);


void func(RgbConverter&rgb_converter ,DepthConverter&depth_converter)
{
    DepthProcess depth_process;
    CameraParam camera_param;
    EdgeDetection edge_detection;
    RgbImageProcess rgb_image_process;
    RemoveHand remove_hand;
    clock_t start1,end1;
    cv::Mat rgb_src,depth_src;
    cv::Mat depth_smooth,rgb_roi,depth_roi,output_img,depth_filter,rgb_filter,remove_image,depth_final,rgb_final;
    while(IsWorking)
    {
        start1=clock();
        rgb_converter.PrepareGrab();
        depth_converter.PrepareGrab();

        rgb_src=rgb_converter.GrabSingleImage();
        depth_src=depth_converter.GrabSingleImage();

        depth_process.SmoothBlur(depth_src);
        depth_smooth=depth_process.GetSmoothImage();
//      camera_param.Registration(rgb_src,depth_smooth);
        camera_param.Registration(rgb_src,depth_smooth,true);

        rgb_roi=camera_param._rgbroi;
        depth_roi=camera_param._depthroi;
        depth_process.copyto(rgb_roi,depth_roi);
        output_img=rgb_image_process.RgbSegmentation(rgb_roi);
        cv::Point2i K=rgb_image_process.Center(output_img);
        depth_process.ThresholdSegmentation(K);
        depth_filter=depth_process.GetDepthFilter();
        rgb_filter=depth_process.GetRgbFilter();
        remove_image=remove_hand.HandSegmentation(rgb_filter);
        edge_detection.edge(remove_image,depth_roi);
        depth_final=edge_detection.GetDepthFinal();
        rgb_final=edge_detection.GetRgbFinal();
        PointCloudCompose(rgb_final,depth_final);
        end1=clock();
        cout<<double(end1-start1)/CLOCKS_PER_SEC<<endl;

    }
    
    depth_process.DepthShow();
}

void PointCloudCompose(cv::Mat&rgbImage,cv::Mat &depthImage)
{
    cv::Mat dst(rgbImage.size(),rgbImage.type(),cv::Scalar::all(255));
    for (int i= 0;  i<rgbImage.rows; i++)
    {
        for(int j=0;j<rgbImage.cols;j++)
        {
            if(depthImage.at<float>(i,j)!=0)
                for(int u=0;u<3;u++)
                    dst.at<cv::Vec3b>(i,j)[u]=static_cast<int>(rgbImage.at<cv::Vec3b>(i,j)[u]);
        }
    }


    PointCloud::Ptr cloud ( new PointCloud );
    // 遍历深度图
    for(int m=0; m<depthImage.rows;m++)
    {
        for(int n=0;n<depthImage.cols;n++)
        {
            // 获取深度图中(m,n)处的值
            float d = depthImage.ptr<float>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointT p;

            // 计算这个点的空间坐标
            p.z = d/1000;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z/ camera_fy;

            // 从rgb图像中获取它的颜色
            // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
            p.b = rgbImage.ptr<uchar>(m)[n*3];
            p.g = rgbImage.ptr<uchar>(m)[n*3+1];
            p.r = rgbImage.ptr<uchar>(m)[n*3+2];

            // 把p加入到点云中
            cloud->points.push_back( p );
        }
    }
    // 设置并保存点云
    cloud->height = 1;
    cloud->width = cloud->points.size();
    cout<<"point cloud size = "<<cloud->points.size()<<endl;
    cloud->is_dense = false;
    stringstream ss;

    ss<<"test"<<i<<".pcd";
    pcl::io::savePCDFile( ss.str(), *cloud );
    i++;
    //    pcl::visualization::PCLVisualizer viewer ("Simple Cloud Viewer");
    //    viewer.addPointCloud (cloud);


    //    cout<<"Point cloud saved."<<endl;
    //    while(!viewer.wasStopped())
    //    {
    //        viewer.spinOnce(100);
    //        this_thread::sleep_for(chrono::seconds(1));
    //    }

    // 清除数据并退出
}

int main (int argc, char** argv)
{
    ros::init(argc, argv, "image_converter");
    RgbConverter rgb_converter;
    DepthConverter depth_converter;

    thread t(func,ref(rgb_converter),ref(depth_converter));
    //ROS_INFO("Thread has been started!");

    ros::Rate r(100);
    while(ros::ok())
    {
        ros::spinOnce();
        r.sleep();
    }
    IsWorking = false;
    sleep(1);
    //ROS_INFO("Program quited!");
    return 0;
}

int main1 (int argc, char** argv)
{
    ros::init(argc, argv, "image_converter");
    DepthConverter depth_converter;
    RgbConverter rgb_converter;
    sleep(1);
    ros::spinOnce();
    thread t(func,ref(rgb_converter),ref(depth_converter));


    while(1)
    {
        char key=cv::waitKey(1);
        if(key=='q')
        {
            break;
        }
        if(key=='a')
        {
            continue;
        }
        rgb_converter.PrepareGrab();
        depth_converter.PrepareGrab();
        sleep(1);
       ros::spinOnce();
    }
    t.join();
    return 0;
}
