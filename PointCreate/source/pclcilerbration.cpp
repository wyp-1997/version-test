#include "pclcilerbration.h"

PclCilerbration::PclCilerbration()
{
    cv::FileStorage fs(picture1, cv::FileStorage::READ);
    fs[label1] >>depthImage;
//    imshow("1",depthImage);
    cv::FileStorage fp(picture2, cv::FileStorage::READ);
    fp[label2] >>rgbImage;

//    imshow("2",rgbImage);

}

void PclCilerbration::Cilerbr()
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

            // 计算这个点的空间坐标,根据相机焦距,X,Y方向上的畸变
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
    pcl::io::savePCDFile( "pointcloud.pcd", *cloud );
    // 清除数据并退出

    pcl::visualization::CloudViewer viewer ("Simple Cloud Viewer");
    while(1)
    {
    viewer.showCloud (cloud);
}//    cloud->points.clear();
//    cout<<"Point cloud saved."<<endl;




}





