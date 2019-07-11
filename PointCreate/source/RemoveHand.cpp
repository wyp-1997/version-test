#include"RemoveHand.h"
//#include<opencv2/opencv.hpp>

RemoveHand::RemoveHand(){

}

RemoveHand::RemoveHand(cv::Mat img):rgb_img(img){
//    cv::namedWindow("image_rgb");
//    imshow("image_rgb",img);

}

RemoveHand::~RemoveHand(){

}

cv::Mat RemoveHand::HandSegmentation(cv::Mat &img){

    cv::Mat img1,img2;
//    Mat img3;
//    cvtColor(img,img3,CV_BGR2GRAY);
//    imshow("img3",img3);
//    medianBlur(img,img,7);


    cv::Mat dst,result;
    vector<cv::Mat> channels;
    cv::Mat Y,Cr,Cb;

    cv::medianBlur(img,img1,7);
    cv::cvtColor(img,img2,CV_BGR2YCrCb);
    cv::split(img2,channels);
    Y=channels.at(0);
    Cr=channels.at(1);
    Cb=channels.at(2);

    result.create(img.rows, img.cols, CV_8UC1);

    for (int j = 1; j < Y.rows ; j++)
    {      //ptr< uchar>(j)[i]是第j行的第i个元素，也就是j行i列的像素点

        uchar* current = result.ptr<uchar>(j);
        for (int i = 1; i < Y.cols ; i++)
        {

        int currentCr=Cr.at<uchar>(j,i);
        int currentCb = Cb.at<uchar>(j,i);
//      cout<<currentCr<<endl;


//      if ((currentCr > 137) && (currentCr < 175) && (currentCb> 100) && (currentCb < 118))
    if ((currentCr > 133) && (currentCr < 173) && (currentCb> 77) && (currentCb < 127))
        current[i] = 255;
    else{
        current[i] = 0 ;//否则为黑
        }//满足条件为白

        }
    }

    cv::Mat element=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::morphologyEx(result,result,cv::MORPH_OPEN,element);
    cv::morphologyEx(result,result,cv::MORPH_DILATE,element);
//    imshow("result_before_open",result);

    cv::Mat result1=255-result;
    cv::Mat items;
    img.copyTo(items,result1);
    return items;
}
