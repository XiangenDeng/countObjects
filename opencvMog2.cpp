//-----------------------------
//@update
//2019-1-26 祥恩　重写算法，改用mog2
//-----------------------------
//-----------------------------
//    描述：包含头文件和命名空间
//-----------------------------
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include  <iostream>
using namespace std;
using namespace cv;

//宏定义
#define FSCALE 0.5

//-------------------------------
//     描述：全局函数声明
//-------------------------------
static void frameProcess();
static void onOpen(int,void*);//开运算
static void onClose(int,void*);//闭运算
static void onGaussianBlur(int,void*);//高斯滤波

//定义轮廓结构题
struct  contourBlock{
    double area;        //轮廓面积
    Point2d mc;         //轮廓质心
};

//---------------------------------
// 描述：全局变量的定义
//---------------------------------
bool isDraw=true;
double minArea=500;
int morphOpenValue=1;//开运算参数
int maxMorphValue=10;//最大值
int morphCloseValue=5;//闭运算参数
int gaussianBulrValue=1;//高斯滤波参数
int maxGaussianBlurValue=40;//最大值
int number=0;           //计数值
int countLine=150;      //计数线
int roiLeft=40;         //Roi区域左
int roiRight=250;       //Roi区域右
Mat tmpFrame,bgMask;

//主函数
int main()
{
    char numberText[10];
    vector<vector<Point> > contours;//轮廓容器
    Rect boundRect;//定义Rect类型的vector容器roRect存放最小外接矩形，
    Moments mu;
    double contoursArea;
    int historyLocate[10]={0};
    struct contourBlock trackBlocks[10];

    VideoCapture cap;
    cap.open("nice10.avi");
    //cap.open(0);
    if( !cap.isOpened() )
    {
        printf("\nCan not open camera or video file\n");
        return -1;
    }
    cap >> tmpFrame;
    if(tmpFrame.empty())
    {
        printf("can not read data from the video source\n");
        return -1;
    }

    Ptr<BackgroundSubtractorMOG2> bgSubTractor=createBackgroundSubtractorMOG2(100,16,false);//初始化背景减法

    for(int i=0;i<10;i++)
    {
        trackBlocks[i].area=0;
        trackBlocks[i].mc=Point2d(0,0);
    }
    namedWindow("mask",WINDOW_NORMAL);
   // createTrackbar("开运算参数","mask",&morphOpenValue,maxMorphValue*2+1,onOpen);
    createTrackbar("闭运算参数","mask",&morphCloseValue,maxMorphValue*2+1,onClose);
    createTrackbar("高斯滤波参数","mask",&gaussianBulrValue,maxGaussianBlurValue*2+1,onGaussianBlur);
    double start;
    double t1;
    double t2;
    int flag=1;
    while(true)
    {
        cap >> tmpFrame;
        start = getTickCount();//开始时间
        resize(tmpFrame,tmpFrame, Size(tmpFrame.cols*FSCALE,tmpFrame.rows*FSCALE), (0, 0), (0, 0), INTER_LINEAR);//调整图片分辨率
        if( tmpFrame.empty() )
            break;
        bgSubTractor->apply(tmpFrame, bgMask, 0.01);//learningrate越大，背景更新越快
        t1=getTickCount();

        frameProcess();
        findContours(bgMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        //获得满足条件的轮廓,质心,面积
        int j=0;
        for(int i=0;i < contours.size();i++)
        {
            contoursArea=contourArea(contours[i]);
            if(contoursArea>minArea)
            {
                //cout<<contoursArea<<endl;
                trackBlocks[j].area=contoursArea;
                mu = moments(contours[i], false );
                trackBlocks[j].mc=Point2d( mu.m10/mu.m00 , mu.m01/mu.m00 );
                if(isDraw)//判断是否画矩形or质心
                {
                    circle(tmpFrame, trackBlocks[j].mc, 3,Scalar(0,0,255),1); //画质心
                    //boundRect = boundingRect(Mat(contours[i]));//获取矩形轮廓
                    //rectangle(bgMask, boundRect, Scalar(255, 0, 0), 2, 8);//画外接矩形
                }
                j++;
                if(j>9)
                    break;
            }
         }

        //防止重复计数
        for(int i=0;i<j;i++)
        {
            if(trackBlocks[i].mc.x>roiLeft&&trackBlocks[i].mc.x<roiRight)
                if(historyLocate[i]<countLine&&trackBlocks[i].mc.y>countLine)
                    number++;
            historyLocate[i]=trackBlocks[i].mc.y;
        }

        if(isDraw)
        {
            //显示
            line(tmpFrame,Point(roiLeft,countLine),Point(roiRight,countLine),Scalar(255,0,0),1);//下边界
            line(tmpFrame,Point(roiLeft,0),Point(roiLeft,countLine),Scalar(255,0,0),1);//左边界
            line(tmpFrame,Point(roiRight,0),Point(roiRight,countLine),Scalar(255,0,0),1);//右边界
        }
        sprintf(numberText,"%d",number);
        putText(tmpFrame,numberText,Point(100*FSCALE,150*FSCALE),FONT_HERSHEY_SIMPLEX,2,Scalar(255,23,0),4,8);//显示数量
        imshow("mask",tmpFrame);
        cvResizeWindow("mask",640,480);
        imshow("process",bgMask);
        t2=getTickCount();
        t2=(t2-t1)*1000/getTickFrequency();
        t1=(t1-start)*1000/getTickFrequency();
        flag++;
        if(flag==100)
        {
            cout<<"t1:"<<t1<<"ms"<<endl;
            cout<<"t2:"<<t2<<"ms"<<endl;
            cout<<tmpFrame.size<<endl;
        }
        if( waitKey(30)== 27 )
            break;
    }
    cap.release();
    destroyWindow("mask");
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

//计数算法，未用
static int countBlocks()
{
    return 0;
}
