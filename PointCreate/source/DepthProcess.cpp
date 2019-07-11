#include "DepthProcess.h"


DepthProcess::DepthProcess()
{


}


DepthProcess::DepthProcess(const cv::String &xml,const cv::String &label)
{
    cv::FileStorage fs(xml, cv::FileStorage::READ);
    fs[label] >>_depth;
    _depth.copyTo(_smooth);
}

DepthProcess::~DepthProcess()
{

}



DepthProcess::DepthProcess(const DepthProcess& another)
{

}

DepthProcess& DepthProcess::operator=(const DepthProcess& another)
{

}

cv::Mat DepthProcess::DepthRead(const cv::String &xml,const cv::String &label,bool flag)
{
    if(flag)
    {
        cv::Mat rgb(480,640, CV_8UC3);
        cv::FileStorage fs(xml, cv::FileStorage::READ);
        fs[label] >>rgb;
        return rgb;
    }
    else
    {
        cv::Mat depth(480,640, CV_32FC1);
        cv::FileStorage fs(xml, cv::FileStorage::READ);
        fs[label] >>depth;
        return depth;
    }
}

void DepthProcess::DepthWrite(const cv::String &xml)
{
    cv::FileStorage fa("SmoothBlur.xml",cv::FileStorage::WRITE);
    if(xml=="SmoothBlur.xml")
        fa<<"SmoothBlur"<<_smooth;

    cv::FileStorage fb("DepthProcess.xml",cv::FileStorage::WRITE);
    if(xml=="DepthProcess.xml")
    {
        fb <<"depth_settle"<<_depth_filter;
        fb <<"depth_mask"<<_depth_mask;
    }

    cv::FileStorage fc("RgbSettle.xml",cv::FileStorage::WRITE);
    if(xml=="RgbSettle.xml")
        fc <<"rgb_settle"<<_rgb_filter;

}



void DepthProcess::copyto(cv::Mat&_roi1,cv::Mat&_roi2)
{
    _roi1.copyTo(_rgbroi);
    _roi2.copyTo(_depthroi);
}

void DepthProcess::DepthWrite(const cv::String &xml,cv::Mat&src)
{

    if(xml=="RgbRoi.xml")
    {
        cv::FileStorage fa("RgbRoi.xml",cv::FileStorage::WRITE);
        fa<<"RgbRoi"<<src;
        src.copyTo(_rgbroi);
    }

    if(xml=="DepthRoi.xml")
    {
        cv::FileStorage fb("DepthRoi.xml",cv::FileStorage::WRITE);
        fb<<"DepthRoi"<<src;
        src.copyTo(_depthroi);
    }




}
void DepthProcess::DepthShow()
{
//    namedWindow("depth");
//    namedWindow("smooth");
//    namedWindow("Rgbroi");
//    namedWindow("Depthroi");
//    namedWindow("rgb_settle");
//    namedWindow("outimage");
//    imshow("depth",_depth);
//    imshow("smooth",_smooth);
//    imshow("Rgbroi",_rgbroi);
//    imshow("Depthroi",_depthroi);
//    imshow("rgb_settle",_rgb_filter);
//    imshow("outimage",_depth_filter);
   cv:: waitKey(0);
}

void DepthProcess::SmoothBlur(cv::Mat &src)

{
    src.copyTo(_depth);
    cv::Mat i_result(480,640,CV_32FC1);
    // 滤波后深度图的缓存
    float* smoothDepthArray = (float *)i_result.data;
    // 我们用这两个值来确定索引在正确的范围内
    int widthBound = 640 - 1;
    int heightBound = 480 - 1;

    // 内（8个像素）外（16个像素）层阈值
    int innerBandThreshold = 3;
    int outerBandThreshold = 7;

    // 处理每行像素
    for (int depthArrayRowIndex = 0; depthArrayRowIndex<480;depthArrayRowIndex++)
    {
        // 处理一行像素中的每个像素
        for (int depthArrayColumnIndex = 0; depthArrayColumnIndex < 640; depthArrayColumnIndex++)
        {
            int depthIndex = depthArrayColumnIndex + (depthArrayRowIndex * 640);
            float* p=(float*)_depth.data;
            float depthValue=p[depthIndex];
            //cout<<depthValue<<endl;
            // 我们认为深度值为0的像素即为候选像素
            if (isnan(depthValue)||depthValue==0)
            {
                // 通过像素索引，我们可以计算得到像素的横纵坐标
                int x = depthIndex % 640;
                int y = (depthIndex - x) / 640;
                //                cout<<x<<y<<endl;

                // filter collection 用来计算滤波器内每个深度值对应的频度，在后面
                // 我们将通过这个数值来确定给候选像素一个什么深度值。
                float filterCollection[24][2] = {0};

                // 内外层框内非零像素数量计数器，在后面用来确定候选像素是否滤波
                int innerBandCount = 0;
                int outerBandCount = 0;

                // 下面的循环将会对以候选像素为中心的5 X 5的像素阵列进行遍历。这里定义了两个边界。如果在
                // 这个阵列内的像素为非零，那么我们将记录这个深度值，并将其所在边界的计数器加一，如果计数器
                // 高过设定的阈值，那么我们将取滤波器内统计的深度值的众数（频度最高的那个深度值）应用于候选
                // 像素上
                for (int yi = -2; yi < 3; yi++)
                {
                    for (int xi = -2; xi < 3; xi++)
                    {
                        // yi和xi为操作像素相对于候选像素的平移量

                        // 我们不要xi = 0&&yi = 0的情况，因为此时操作的就是候选像素
                        if (xi != 0 || yi != 0)
                        {
                            // 确定操作像素在深度图中的位置
                            int xSearch = x + xi;
                            int ySearch = y + yi;

                            // 检查操作像素的位置是否超过了图像的边界（候选像素在图像的边缘）
                            if (xSearch >= 0 && xSearch <= widthBound &&
                                    ySearch >= 0 && ySearch <= heightBound)
                            {
                                int index = xSearch + (ySearch * 640);
                                // 我们只要非零量
                                if (!isnan(p[index])&&p[index]!=0)
                                {
                                    // 计算每个深度值的频度
                                    for (int i = 0; i < 24; i++)
                                    {
                                        if (filterCollection[i][0] == p[index])
                                        {
                                            // 如果在 filter collection中已经记录过了这个深度
                                            // 将这个深度对应的频度加一
                                            filterCollection[i][1]++;
                                            break;
                                        }
                                        else if (filterCollection[i][0] == 0)
                                        {
                                            // 如果filter collection中没有记录这个深度
                                            // 那么记录
                                            filterCollection[i][0] = p[index];
                                            //                                            cout<<filterCollection[i][0];
                                            filterCollection[i][1]++;
                                            break;
                                        }
                                    }

                                    // 确定是内外哪个边界内的像素不为零，对相应计数器加一
                                    if (yi != 2 && yi != -2 && xi != 2 && xi != -2)
                                        innerBandCount++;
                                    else
                                        outerBandCount++;
                                }
                            }
                        }
                    }
                }

                // 判断计数器是否超过阈值，如果任意层内非零像素的数目超过了阈值，
                // 就要将所有非零像素深度值对应的统计众数
                if (innerBandCount >= innerBandThreshold || outerBandCount >= outerBandThreshold)
                {

                    short frequency = 0;
                    float depth2 = 0;
                    // 这个循环将统计所有非零像素深度值对应的众数
                    for (int i = 0; i < 24; i++)
                    {
                        // 当没有记录深度值时（无非零深度值的像素）
                        if (filterCollection[i][0]==0)
                            break;
                        if (filterCollection[i][1] > frequency)
                        {
                            //                            cout<<filterCollection[i][0];
                            depth2 = filterCollection[i][0];

                            frequency = filterCollection[i][1];
                        }
                    }

                    smoothDepthArray[depthIndex] = depth2;
                }
                else
                {
                    smoothDepthArray[depthIndex] = 0;
                }
            }
            else
            {
                // 如果像素的深度值不为零，保持原深度值
                smoothDepthArray[depthIndex] = p[depthIndex];
            }

            //                     cout<<smoothDepthArray[depthIndex];
        }
    }

    i_result.copyTo(_smooth);

}



void DepthProcess::ThresholdSegmentation(cv::Point2i &POINT)
{
    cv::Mat hist;
    int histSize[1];
    float hranges[2];
    const float* ranges[1];
    int channels[1];
    histSize[0] = 200;
    hranges[0] = 0.4*1000;
    hranges[1] = 8.0*1000;
    ranges[0] = hranges;
    channels[0] = 0;
    calcHist(&_depthroi,1, channels, cv::Mat(), hist, 1, histSize, ranges);
    int scale=1;
    cv::Mat dstimage(*histSize,*histSize*scale,CV_8UC1,cv::Scalar(0));
    double min=0.0;
    double max=0.0;
    float z=_depthroi.at<float>(POINT.x,POINT.y);
    while((z<400)&&(POINT.x<640)&&(POINT.y<480))
    {
        cout<<" this is a NAN point";
        POINT.y+=1;
        POINT.x+=1;
        z=_depthroi.at<float>(POINT.x,POINT.y);
        cout<<POINT;
        cout<<"distance: "<<z<<endl;
    }
    
    cout<<"distance: "<<z<<endl;
    cv::Point2i point(0,int((200*(z-(0.4*1000))/(7.6*1000))));
    minMaxLoc(hist,&min,&max,0,0);
    cout<<"point: "<<point<<endl;
    int point_min_y1,point_min_y2;
    for(int i=point.y;i<200;i++)
    {
        float max_1=hist.at<float>(i);
        float max_2=hist.at<float>(i+1);
        if(max_1>=max_2)
            continue;
        else
        {
            point_min_y2=i;
            break;
        }
    }
    cout<<"point_min_y2: "<<point_min_y2<<endl;
    for(int i=point.y;i>0;i--)
    {
        float max_1=hist.at<float>(i);
        float max_2=hist.at<float>(i-1);
        if((max_1>=max_2)&&(i!=1))
            continue;
        else if((max_1<max_2)||(i==1))
        {
            point_min_y1=i;
            break;
        }
    }
    cout<<"point_min_y1: "<<point_min_y1<<endl;


    int hpt=cv::saturate_cast<int>(0.9*(*histSize));
    for(int i=0;i<200;i++)
    {
        float binvalue=hist.at<float>(i);
        int realvalue=cv::saturate_cast<int>(binvalue*hpt/max);
        cv::rectangle(dstimage,cv::Point(i*scale,(*histSize)-1),cv::Point((i+1)*scale-1,(*histSize)-realvalue),cv::Scalar(255));
    }

//    imshow("dstimage",dstimage);

    float max2=(point.y/200.0)*(8000-400)+400;
    float max3=(point_min_y1/200.0)*(8000-400)+400;
    float max4=(point_min_y2/200.0)*(8000-400)+400;
    //    cout<<max2<<endl;
    cout<<"threhold_min: "<<max3<<endl;
    cout<<"threhold_max: "<<max4<<endl;

    static float result[3];
    result[0]=max2;
    result[1]=max3;
    result[2]=max4;

    float threhold1=*result;
    float threhold2=*(result+1);
    float threhold3=*(result+2);
    cv::Mat depth_settle(_depthroi.rows,_depthroi.cols,CV_32FC1);
    cv::Mat depth_mask(_depthroi.rows,_depthroi.cols,CV_8UC1);
    //        cout<<threhold1<<" "  <<endl;
    for(int i=0;i<_depthroi.rows;i++)
    {
        for(int j=0;j<_depthroi.cols;j++)
        {
            if((_depthroi.at<float>(i,j)<threhold3)&&(_depthroi.at<float>(i,j)>threhold2))
            {
                depth_settle.at<float>(i,j)=_depthroi.at<float>(i,j);
                depth_mask.at<uchar>(i,j)=1;
            }
            else {
                depth_settle.at<float>(i,j)=0;
                depth_mask.at<uchar>(i,j)=0;
            }
        }
    }
    depth_settle.copyTo(_depth_filter);
    depth_mask.copyTo(_depth_mask);
    _rgbroi.copyTo(_rgb_filter,_depth_mask);
    cout << "******************************************" << endl;


}

cv::Mat DepthProcess::GetSmoothImage()
{
    return _smooth;
}

cv::Mat DepthProcess::GetDepthFilter()
{

    return _depth_filter;

}

cv::Mat DepthProcess::GetRgbFilter()
{

    return _rgb_filter;

}




