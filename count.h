/*
 *帧差法流水线计数统计
 *用于工厂流水线物体计数
 *@update:
 *2019-1-24 祥恩　重新整理算法，规范代码
 */
#ifndef COUNT_H_INCLUDED
#define COUNT_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
//运动物体检测函数声明
Mat moveDetect(Mat c_frame);//运动目标检测
Mat diff,diff2;             //帧差图像
Mat diff_thresh;
static void on_value(int,void*);
int number = 0;
int flag = 1;
int erode_value=1;
int dilate_value = 5;
int thre_value=30;
int gauss_value=3;
float locate[100];

#endif // COUNT_H_INCLUDED
