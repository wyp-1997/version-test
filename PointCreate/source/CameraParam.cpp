#include "CameraParam.h"

CameraParam::CameraParam()
{

}


bool loadParm(AtraParm* AtraParm)
{
    // 加载参数
    ifstream parm("..//registration2.txt");
    string stringLine;
    if (parm.is_open())
    {
        // rgb相机参数：fx,fy,cx,cy
        getline(parm, stringLine);
        stringstream lin(stringLine);
        string s1, s2, s3, s4, s5, s6, s7, s8, s9;
        lin >> s1 >> s2 >> s3 >> s4;
        AtraParm->fx_rgb = atof(s1.c_str());
        AtraParm->fy_rgb = atof(s2.c_str());
        AtraParm->cx_rgb = atof(s3.c_str());
        AtraParm->cy_rgb = atof(s4.c_str());
        stringLine.clear();
        // ir相机参数：fx,fy,cx,cy
        getline(parm, stringLine);
        stringstream lin2(stringLine);
        lin2 << stringLine;
        lin2 >> s1 >> s2 >> s3 >> s4;
        AtraParm->fx_ir = atof(s1.c_str());
        AtraParm->fy_ir = atof(s2.c_str());
        AtraParm->cx_ir = atof(s3.c_str());
        AtraParm->cy_ir = atof(s4.c_str());
        stringLine.clear();
    }
    else
    {
        cout << "parm.txt not right!!!";
        return false;
    }
    cout << "******************************************" << endl;
    cout << "fx_rgb:    " << AtraParm->fx_rgb << endl;
    cout << "fy_rgb:    " << AtraParm->fy_rgb << endl;
    cout << "cx_rgb:    " << AtraParm->cx_rgb << endl;
    cout << "cy_rgb:    " << AtraParm->cy_rgb << endl;
    cout << "******************************************" << endl;
    cout << "fx_ir:     " << AtraParm->fx_ir << endl;
    cout << "fy_ir:     " << AtraParm->fy_ir << endl;
    cout << "cx_ir:     " << AtraParm->cx_ir << endl;
    cout << "cy_ir:     " << AtraParm->cy_ir << endl;
    cout << "******************************************" << endl;
    return true;
}

void CameraParam::Registration(cv::Mat &rgb,cv::Mat&smooth)
{
    AtraParm *parm = new AtraParm();
    if(!loadParm(parm))
        cout<<"a";
    cv::Mat depth2;
    smooth.copyTo(depth2);

    for(int i=0;i<depth2.rows;i++)
    {
        for(int j=0;j<depth2.cols;j++)
        {
            if(isnan( depth2.at<float>(i,j))||(depth2.at<float>(i,j)==0))
            {
                depth2.at<float>(i,j)=0;}
            depth2.at<float>(i,j)*=1;
            //            cout<<depth2.at<float>(i,j)<<" ";
        }
    }
    cout << "read depth" << endl;

    // 4. 变换

    // 4.1 计算各个矩阵
#pragma region  非齐次
    Eigen::Matrix4f K_ir;           // ir内参矩阵
    K_ir <<
            parm->fx_ir, 0, parm->cx_ir,0,
            0, parm->fy_ir, parm->cy_ir,0,
            0, 0, 1,0,
            0,0,0,1;
    Eigen::Matrix4f K_rgb;          // rgb内参矩阵
    K_rgb <<
             parm->fx_rgb, 0, parm->cx_rgb,0,
            0, parm->fy_rgb, parm->cy_rgb,0,
            0, 0, 1,0,
            0,0,0,1;

    Eigen::Matrix4f M;
    M<<0.999896524724011	,0.000120852952807020	,-0.0143848962251749,-10.9577,
            5.18731407349032e-06	,0.999961616198379	,0.00876162673355735,40.1410,
            0.0143854029466340,	-0.00876079474078761	,0.999858144267262,2.8770,
            0,0,0,1;
    Eigen::Matrix4f W;
    W=K_rgb*M*K_ir.inverse();

    cout << "K_rgb:\n" << K_rgb << endl;
    cout << "K_ir:\n" << K_ir << endl;
    cout << "M:\n" <<M<< endl;
    cout << "W:\n" << W << endl;
    // 4.2 计算投影
    cv::Mat result(480, 640, CV_8UC3);
    int i = 0;
    for (int row = 0; row < 480; row++)
    {
        for (int col = 0; col < 640; col++)
        {
            float* p = (float*)depth2.data;
            float depthValue = p[row * 640 + col];
            if (depthValue != -std::numeric_limits<float>::infinity() && depthValue != -std::numeric_limits<float>::infinity() && depthValue != 0 && depthValue != -NAN)
            {
                // 投影到彩色图上的坐标
                Eigen::Vector4f uv_depth(col, row,1.0f,1.0/depthValue);                            // !!!p_ir
                Eigen::Vector4f uv_color = W*uv_depth/1;   // !!!Z_rgb*p_rgb=R*Z_ir*p_ir+T; (除以1000，是为了从毫米变米)
//                   cout << "uv_color[0]:       " << uv_color[0] << "     uv_color[1]:      " <<uv_color[1]<< endl;
                int X = static_cast<int>(uv_color[0]);                // !!!Z_rgb*p_rgb -> p_rgb
                int Y = static_cast<int>(uv_color[1]);                // !!!Z_rgb*p_rgb -> p_rgb

                //                            cout << "X:       " << X << "     Y:      " << Y << endl;
                if ((X >= 0 && X < 640) && (Y >= 0 && Y < 480))
                {
                    //                  cout << "X:       " << X << "     Y:      " << Y << endl;
                    result.data[i * 3] = rgb.data[3 * (Y * 640 + X)];
                    result.data[i * 3 + 1] = rgb.data[3 * (Y * 640 + X) + 1];
                    result.data[i * 3 + 2] = rgb.data[3 * (Y * 640 + X) + 2];

                }
                else
                {
                    result.data[i * 3] = 255;
                    result.data[i * 3 + 1] = 255;
                    result.data[i * 3 + 2] = 255;
                }
            }
            else
            {
                result.data[i * 3] = 0;
                result.data[i * 3 + 1] = 0;
                result.data[i * 3 + 2] = 0;
            }
            i++;
        }
    }
    result.copyTo(_rgbRegistration);
    _depthroi=smooth(cv::Rect(40,40,550,400)).clone();
    _rgbroi=_rgbRegistration(cv::Rect(40,40,550,400)).clone();
}


void CameraParam::Registration(cv::Mat &rgb,cv::Mat&smooth,bool flag)
{

    cv::Mat depth2;
    smooth.copyTo(depth2);

    for(int i=0;i<depth2.rows;i++)
    {
        for(int j=0;j<depth2.cols;j++)
        {
            if(isnan( depth2.at<float>(i,j))||(depth2.at<float>(i,j)==0))
            {
                depth2.at<float>(i,j)=0;}
            depth2.at<float>(i,j)*=1;
            //            cout<<depth2.at<float>(i,j)<<" ";
        }
    }
    cout << "read depth" << endl;
    Eigen::Matrix4f W;
    W << 1.03967  ,-0.00433145   ,  -17.9573   ,  -5848.79,
            0.00562138    ,  1.02301    ,  -45.921  ,    25768.1,
            2.41437e-05 ,-1.43811e-05   ,  0.996396   ,     2.877,
            0         ,   0         ,   0     ,       1;
    // 4.2 计算投影
    cv::Mat result(480, 640, CV_8UC3);
    int i = 0;
    for (int row = 0; row < 480; row++)
    {
        for (int col = 0; col < 640; col++)
        {
            float* p = (float*)depth2.data;
            float depthValue = p[row * 640 + col];
            if (depthValue != -std::numeric_limits<float>::infinity() && depthValue != -std::numeric_limits<float>::infinity() && depthValue != 0 && depthValue != -NAN)
            {
                // 投影到彩色图上的坐标
                Eigen::Vector4f uv_depth(col, row,1.0f,1.0/depthValue);                            // !!!p_ir
                Eigen::Vector4f uv_color = W*uv_depth/1;   // !!!Z_rgb*p_rgb=R*Z_ir*p_ir+T; (除以1000，是为了从毫米变米)
//                   cout << "uv_color[0]:       " << uv_color[0] << "     uv_color[1]:      " <<uv_color[1]<< endl;
                int X = static_cast<int>(uv_color[0]);                // !!!Z_rgb*p_rgb -> p_rgb
                int Y = static_cast<int>(uv_color[1]);                // !!!Z_rgb*p_rgb -> p_rgb

                //                            cout << "X:       " << X << "     Y:      " << Y << endl;
                if ((X >= 0 && X < 640) && (Y >= 0 && Y < 480))
                {
                    //                  cout << "X:       " << X << "     Y:      " << Y << endl;
                    result.data[i * 3] = rgb.data[3 * (Y * 640 + X)];
                    result.data[i * 3 + 1] = rgb.data[3 * (Y * 640 + X) + 1];
                    result.data[i * 3 + 2] = rgb.data[3 * (Y * 640 + X) + 2];

                }
                else
                {
                    result.data[i * 3] = 255;
                    result.data[i * 3 + 1] = 255;
                    result.data[i * 3 + 2] = 255;
                }
            }
            else
            {
                result.data[i * 3] = 0;
                result.data[i * 3 + 1] = 0;
                result.data[i * 3 + 2] = 0;
            }
            i++;
        }
    }

    result.copyTo(_rgbRegistration);
    _depthroi=smooth(cv::Rect(40,40,550,400)).clone();
    _rgbroi=_rgbRegistration(cv::Rect(40,40,550,400)).clone();

}
