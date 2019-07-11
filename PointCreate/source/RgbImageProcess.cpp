#include"RgbImageProcess.h"


RgbImageProcess::RgbImageProcess(){

}

RgbImageProcess::RgbImageProcess(cv::Mat img):rgb_img(img){
    cv::namedWindow("image_rgb");
    imshow("image_rgb",img);


}

RgbImageProcess::~RgbImageProcess(){

}

cv::Mat RgbImageProcess::RgbSegmentation(cv::Mat &rgb_src){

    cv::Mat output_mask;
    cv::Mat img=rgb_src.clone();
    cv::Mat skinCrCbHist=cv::Mat::zeros(cv::Size(256,256),CV_8UC1);

    //椭圆皮肤检测
    ellipse(skinCrCbHist,cv::Point(113,115.6),cv::Size(23.4,15.2),
            43.0,0.0,360.0,cv::Scalar(255,255,255),-1);
    output_mask=cv::Mat(img.size(),CV_8UC1);
    output_mask=cv::Scalar::all(0);
    cv::Mat ycrcb_image;
    cvtColor(img,ycrcb_image,cv::COLOR_BGR2YCrCb);

    for(int j=0;j<ycrcb_image.rows;j++){
        for(int i=0;i<ycrcb_image.cols;i++){
            cv::Vec3b ycrcb=ycrcb_image.at<cv::Vec3b>(j,i);

            if(skinCrCbHist.at<uchar>(ycrcb[1],ycrcb[2])>0){
                output_mask.at<uchar>(j,i)=0;
            }
            else
            {
                output_mask.at<uchar>(j,i)=255;
            }
        }
    }
    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    morphologyEx(output_mask,output_mask,cv::MORPH_OPEN,element);  //对输出的掩膜进行开运算

    vector<vector<cv::Point> > contours;   // 轮廓
    vector<vector<cv::Point> > filterContours; // 筛选后的轮廓
    contours.clear();
    filterContours.clear();

    //进行轮廓查找,并返回最大轮廓
    findContours(output_mask, contours,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    int largest_contours_index=0;
    int largest_area=0;

    if(contours.size()!=0)
    {

        for(size_t i=0;i<contours.size();i++)
        {
            double area=contourArea(contours[i]);

            if(area>largest_area)
            {
                largest_area=area;
                largest_contours_index=i;
            }

        }
        filterContours.push_back(contours[largest_contours_index]);
        output_mask.setTo(0);
        drawContours(output_mask, filterContours, -1, cv::Scalar(255,0,0), CV_FILLED); //8, hierarchy);
    }
    else
    {
        cout<<"Error: The Contors is Empty! "<<endl;
    }
    //在掩膜上画出最大轮廓二值图像

    return output_mask;

}

    //计算二值图像重心
cv::Point RgbImageProcess::Center(cv::Mat &img){
    cv::Moments m=moments(img,true);
    cv::Point2i center(m.m01/m.m00,m.m10/m.m00);
    return center;

}
