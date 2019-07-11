#include "EdgeDetection.h"


EdgeDetection::EdgeDetection()
{


}

EdgeDetection::EdgeDetection(cv::Mat &it,cv::Mat &depth)
    :_rgbroi(it),_depthroi(depth)
{


}


void EdgeDetection:: edge(cv::Mat &it,cv::Mat &depth)
{
    it.copyTo(_rgbroi);
    depth.copyTo(_depthroi);
    cv::Mat graysrcimage;
    cv::Mat canny_output;
    int thresh=15;
    cvtColor(_rgbroi,graysrcimage,CV_BGR2GRAY);
//    imshow("after_image",_rgbroi);
    Canny(graysrcimage,canny_output,thresh,thresh*2,3);
//    imshow("canny_output",canny_output);
    vector<vector<cv::Point> > contours;   // 轮廓
    vector<vector<cv::Point> > filterContours; // 筛选后的轮廓
    vector<cv::Vec4i> hierarchy;    // 轮廓的结构信息
   contours.clear();
//    hierarchy.clear();
    filterContours.clear();
    findContours(canny_output, contours, hierarchy,
                 CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    int largest_contours_index=0;
    int largest_area=0;
if(contours.size()!=0)
{
    for(size_t i=0;i<contours.size();i++){
        double area=contourArea(contours[i]);

        if(area>largest_area){
            largest_area=area;
            largest_contours_index=i;
        }
    }
    filterContours.push_back(contours[largest_contours_index]);
    cv::Mat dst_mask;
    canny_output.copyTo(dst_mask);
    dst_mask.setTo(0);
    drawContours(dst_mask, filterContours, -1, cv::Scalar(255,255,255), CV_FILLED); //8, hierarchy);
//    imshow("dst_mask",dst_mask);
    _rgbroi.copyTo(_rgb_final,dst_mask);
    _depthroi.copyTo(_depth_final,dst_mask);
//    imshow("rgb_final",_rgb_final);
//    imshow("depth_final",_depth_final);
//    imshow("canny_output",canny_output);
}
else
    cout<<"wrong next";
}

cv::Mat EdgeDetection::GetRgbFinal()
{
    return _rgb_final;
}

cv::Mat EdgeDetection::GetDepthFinal()
{
    return _depth_final;

}


