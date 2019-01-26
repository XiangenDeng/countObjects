/*
 *帧差法流水线计数统计
 *用于工厂流水线物体计数
 *@update:
 *2019-1-24 祥恩　重新整理算法，规范代码
　*2019-1-25 祥恩　重写帧差算法，去掉了没必要的图像处理步骤以及缩小图片尺寸
 *               在树莓派上速度有提升，但依旧延时严重
 *2019-1-25 祥恩　改用mog2算法，速度有明显提升，但依旧不行
 *2019-1-25 祥恩　写光流算法
 */
#ifndef COUNT_H_INCLUDED
#define COUNT_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat moveDetect();//运动目标检测计数
static void onValue(int,void*);
//定义轮廓结构题
struct  contourBlock{
    double area;        //轮廓面积
    Point2d mc;         //轮廓质心
};

#endif // COUNT_H_INCLUDED
