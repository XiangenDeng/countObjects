//-------------------------------------------------------
//算法描述：目标检测计数算法
//@update
//2019-1-26 祥恩　重写算法，改用mog2.提速效果明显，但达不到实时要求
//2019-1-26 祥恩  写LK光流算法，进度条20%　
//2019-1-28 祥恩  写LK稀疏光流算法,进度60%,稠密光流运算量大
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
bool addNewPoints();
bool acceptTrackedPoint(int i);

//---------------------------------
// 描述：全局变量的定义
//---------------------------------
int thresholdValue=40;//二值化参数
int maxThresholdValue=200;
Mat currentFrame,preFrame;
Mat curGrayFrame,preGrayFrame;
vector<Point2f> points[2];
vector<Point2f> trackPoints;
vector<uchar> status;//输出状态vector

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
    cap>>preFrame;
    if(preFrame.empty())
    {
        cout<<"video is empty"<<endl;
        return -1;
    }

    //LK函数参数
    int maxLevel=3;//基于最大金字塔层次数
    int winSize =15;//每级金字塔的搜索窗口大小
    vector<float> error;
    //getFeatures参数
    int cornerCount=150;//返回的最大角点数目
    double qualityLevel=0.01;//角点可接受的最小特征值
    double minDistance=10;//角点之间最小距离
    int iteration=20;   //迭代次数
    double epislon=0.1; //最小精度
    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, iteration, epislon);//准则,迭代次数及精度

    bool isDraw=true;

    namedWindow("show",WINDOW_AUTOSIZE);
    //createTrackbar("二值化","show",&thresholdValue,maxThresholdValue,onThreshold);
    while(true)
    {
        cap>>currentFrame;
        if(currentFrame.empty())
        {
            cout<<"frame is empty"<<endl;
            break;
        }
        cvtColor(currentFrame,curGrayFrame,CV_BGR2GRAY);
        //onThreshold(thresholdValue,0);
        if(addNewPoints())
        {
            //preGrayFrame=curGrayFrame.clone();
            goodFeaturesToTrack(curGrayFrame,points[0],cornerCount,qualityLevel,minDistance);
            cornerSubPix(curGrayFrame,points[0],cvSize(winSize,winSize),Size(-1,-1),termcrit);//精确检测角点函数
            trackPoints.insert(trackPoints.end(),points[0].begin(),points[0].end());
        }

        if (preGrayFrame.empty())
        {
            curGrayFrame.copyTo(preGrayFrame);
        }
        //LK函数
        calcOpticalFlowPyrLK(preGrayFrame,curGrayFrame,points[0], points[1], status, error,Size(winSize,winSize),maxLevel,termcrit);

        //去掉位置不变的点
        int k=0;
        for(int i=0;i<points[1].size();i++)
        {
            if (acceptTrackedPoint(i))
            {
                trackPoints[k] = trackPoints[i];
                points[1][k] = points[1][i];
                k++;
            }
        }
        points[1].resize(k);//重新调整容器大小
        trackPoints.resize(k);

        //显示特征点和运动轨迹
        if(isDraw)
        {
            for(int i=0;i<points[1].size();i++)
            {
                line(currentFrame,trackPoints[i],points[1][i],Scalar(0,0,255),2);
                circle(currentFrame,points[1][i],3,Scalar(0,255,0),-1);
            }
        }

        imshow("show",currentFrame);
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


//检测是否添加新点
bool addNewPoints()
{
	return points[0].size() <= 10;
}

//决定那些跟踪点被接受
bool acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}
