//-------------------------------------------------------
//@update
//2019-1-26 祥恩　重写算法，改用mog2.提速效果明显，但达不到要求
//2019-1-26 祥恩  写LK光流算法　
//-------------------------------------------------------


//-----------------------------
//    描述：包含头文件和命名空间
//-----------------------------
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include  <opencv.hpp>
#include  <iostream>
using namespace std;
using namespace cv;

//-------------------------------
//     描述：全局函数声明
//-------------------------------
static void frameProcess();
static void onOpen(int,void*);//开运算
static void onClose(int,void*);//闭运算
static void onGaussianBlur(int,void*);//高斯滤波
static void onThreshold(int,void*)

//---------------------------------
// 描述：全局变量的定义
//---------------------------------
int morphOpenValue=1;//开运算参数
int maxMorphValue=10;//最大值
int morphCloseValue=5;//闭运算参数
int gaussianBulrValue=1;//高斯滤波参数
int maxGaussianBlurValue=40;//最大值
int thresholdValue=40;//二值化参数
int maxThresholdValue=200;
Mat currentFrame,gray;
Mat bgMask;
//主函数
int main()
{
    VideoCapture cap;
    cap.open(0);
    if(!cap.isOpened())
    {
        cout<<"unable to open video"<<endl;
        return -1;
    }
    Mat currentFrame,gray;
    vector<Point2f> points[2];
    vector<uchar> status;//输出状态vector
    vector<float> error;
    int winSizeValue =15;
    namedWindow("show",WINDOW_AUTOSIZE);
    createTrackbar("二值化","show",&thresholdValue,maxThresholdValue,onThreshold);
    while(true)
    {
        cap>>currentFrame;
        if(currentFrame.empty())
        {
            cout<<"frame is empty"<<endl;
            break;
        }
        cvtColor(currentFrame,currentFrame,CV_BGR2GRAY);
        onThreshold(thresholdValue,0);
        calcOpticalFlowPyrLK(currentFrame, gray, points[0], points[1], status, error,Size(winSizeValue,winSizeValue),3,);
        imshow("show",gray);
        cvResizeWindow("show",640,480);
        if(waitKey(30)==27)
        {
            cout<<"quit"<<endl;
            break;
        }
    }
    cap.release();
    return 0;
}

//对获得的帧差图片进一步处理
static void frameProcess()
{
   onClose(morphCloseValue,0);
   //onOpen(morphOpenValue,0);//效果差，没必要
   onGaussianBlur(gaussianBulrValue,0);//除噪点
}

//开运算
static void onOpen(int,void*)
{
   Mat KrenelElement = getStructuringElement(MORPH_RECT,Size(morphOpenValue,morphOpenValue));
   morphologyEx(bgMask,bgMask,MORPH_OPEN,KrenelElement);//开运算
}

//闭运算
static void onClose(int,void*)
{
   Mat KrenelElement = getStructuringElement(MORPH_RECT,Size(morphCloseValue,morphCloseValue));
   morphologyEx(bgMask,bgMask,MORPH_CLOSE,KrenelElement);//开运算
}

//高斯滤波
static void onGaussianBlur(int,void*)
{
    GaussianBlur(bgMask,bgMask,Size(gaussianBulrValue*2+1,gaussianBulrValue*2+1),0,0);
}

//二值化
static void onThreshold(int,void*)
{
    threshold(currentFrame, currentFrame,thresholdValue,200,THRESH_BINARY);
}
