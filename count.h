/*
 *帧差法流水线计数统计
 *用于工厂流水线物体计数
 *@update:
 *2019-1-21 祥恩　尝试用MobileNet&SSD深度学习模型进行目标检测,树莓派CPU占用率达９０％以上，无法运行
　*2019-1-22 祥恩  换yolov3模型,比MobileNet&SSD还要慢．暂时放弃神经网络进行目标检测的想法
 *2019-1-23 祥恩　工厂考察环境
 *2019-1-24 祥恩　重新帧间差分算法,规范代码
　*2019-1-25 祥恩　重写帧差算法，去掉了没必要的图像处理步骤以及缩小图片尺寸
 *               在树莓派上速度有提升，但依旧延时严重
 *2019-1-26 祥恩　重写算法，改用mog2.提速效果明显，但达不到实时要求
 *2019-1-26 祥恩  写LK光流算法，进度条20%　
 *2019-1-28 祥恩  写LK稀疏光流算法,稠密光流运算量大80%
　*2019-1-29 祥恩  写lk算法，角点过多，且更新乱，计数难度大
 */
#ifndef COUNT_H_INCLUDED
#define COUNT_H_INCLUDED

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
Mat currentFrame,preFrame;
Mat curGrayFrame,preGrayFrame;
vector<Point2f> points[2];
vector<Point2f> trackPoints;
vector<Point2f> featurePoints;
vector<uchar> status;//输出状态vector，特征点被成功跟踪的标志
//LK函数参数
int maxLevel=1;//基于最大金字塔层次数
int winSize =15;//每级金字塔的搜索窗口大小
//getFeatures参数
int cornerCount=100;//返回的最大角点数目
double qualityLevel=0.01;//角点可接受的最小特征值
double minDistance=10;//角点之间最小距离
int iteration=40;   //迭代次数
double epislon=0.01; //最小精度
TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, iteration, epislon);//准则,迭代次数及精度
bool isDraw=true;
int countLine=280;
int countRight=60;
int countLeft=500;
int passPoint=0;
int number=0;
char numText[10];

#endif // COUNT_H_INCLUDED
