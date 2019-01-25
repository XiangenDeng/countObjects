#include "count.h"
#define FSCALE 0.7        //图片分辨率缩放倍数

int roiWidth=500*FSCALE;//roi区域宽度
int roiHeight=280*FSCALE;//roi区域高度
double minArea=3000;
int number = 0; //数量
int flag = 0;   //重复计数标志
int erodeValue=1;
int dilateValue = 2;
int threValue=100;
int gaussValue=3;
struct contourBlock trackBlocks[20];
Mat diffFrame,diffFrame1,diffFrame2;             //帧差图像
//主函数
int main()
{
    Mat preFrames[2];   //存储每三帧图片队列
    char numberText[10];//显示文本
    Mat result;//存储处理后的结果图像
    Mat currentFrame;//当前帧
    Mat previousFrame;//前一帧
    VideoCapture capture("./test2.avi");

    if(!capture.isOpened())
    {
      cout << "video open error!" << endl;
      return -1;
    }
    //定义ROI区域
    Rect rect;
    rect.x=0;
    rect.y=0;
    rect.width=roiWidth;
    rect.height=roiHeight;

    for(int i=0;i<20;i++)
    {
        trackBlocks[i].area=0;
        trackBlocks[i].mc=Point2d(0,0);
    }
    //初始化两帧图片并调整分辨率,提取感兴趣区域，灰度化
    for(int i=0;i<2;i++)
    {
        capture>>preFrames[i];
        resize(preFrames[i],preFrames[i], Size(preFrames[i].cols*FSCALE,preFrames[i].rows*FSCALE), (0, 0), (0, 0), INTER_LINEAR);
        preFrames[i]=preFrames[i](rect);
        cvtColor(preFrames[i], preFrames[i], CV_BGR2GRAY);
        threshold(preFrames[i], preFrames[i], threValue, 255, CV_THRESH_BINARY);
    }
    cout<<preFrames[0].size<<endl;//输出图片大小
    previousFrame=preFrames[1];
    absdiff(preFrames[1],preFrames[0],diffFrame1);//初始化第一次帧差

    //展示结果输出窗口，并创建参数调整滑动条
    namedWindow("result",WINDOW_NORMAL);
    createTrackbar("threshold","result",&threValue,255,onValue);
    createTrackbar("erode","result",&erodeValue,21,onValue);
    createTrackbar("gaussian","result",&gaussValue,50,onValue);
    createTrackbar("dilate","result",&dilateValue,21,onValue);

    Mat OriginFrame;
    //主循环
    while(true)
      {
        capture >> currentFrame;//读帧进frames
        OriginFrame=currentFrame.clone();
        if (currentFrame.empty())//对当前帧进行异常检测
        {
          cout << "frame is empty!" << endl;
          break;
        }
        resize(currentFrame,currentFrame, Size(currentFrame.cols*FSCALE,currentFrame.rows*FSCALE), (0, 0), (0, 0), INTER_LINEAR);//重新调整图像大小
        currentFrame=currentFrame(rect);                 //提取感兴趣区域
        cvtColor(currentFrame,currentFrame,CV_BGR2GRAY); //灰度化
        threshold(currentFrame, currentFrame, threValue, 255, CV_THRESH_BINARY);    //二值化

        absdiff(currentFrame, previousFrame, diffFrame2);        //图像帧差
        add(diffFrame1,diffFrame2,diffFrame);

        result = moveDetect();      //目标检测处理

        sprintf(numberText,"%d",number);
        putText(OriginFrame,numberText,Point(100,150),FONT_HERSHEY_SIMPLEX,5,Scalar(255,23,0),4,8);//显示数量
        rectangle(OriginFrame,rect,Scalar(0, 0, 255),2);//画感兴趣区域ROI
        imshow("OriginVideo",OriginFrame);
        cvResizeWindow("OriginVideo",640,480);

        cvResizeWindow("result",640,480);
        imshow("result", result);

        if (waitKey(30) == 27)//按原FPS显示
        {
          cout << "ESC退出!" << endl;
          break;
        }
        previousFrame=currentFrame.clone();
        diffFrame1=diffFrame2.clone();
      }
   return 0;

}

//目标检测
Mat moveDetect()
{
    int j=0;
    double contoursArea;              //轮廓面积
    onValue(threValue,0);             //调整二值化
    onValue(erodeValue,0);            //腐蚀
    onValue(gaussValue,0);            //高斯滤波
    onValue(dilateValue,0);           //膨胀
    //Canny(diffFrame,diffFrame,100,200,3);//边缘检测
    Mat result=diffFrame.clone();
    //查找轮廓
    vector<vector<Point> > contours;//轮廓容器
    findContours(diffFrame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //drawContours(result, contours, -1, Scalar(0, 255, 0), 1,8);//在result上绘制轮廓

    //查找正外接矩形
    Rect boundRect;
    //定义Rect类型的vector容器roRect存放最小外接矩形，初始化大小为contours.size()即轮廓个数

    //获得满足条件的轮廓,质心,面积
    Moments mu;
    for(int i=0;i < contours.size();i++)
    {
        contoursArea=contourArea(contours[i]);
        if(contoursArea>minArea)
        {
            //cout<<contoursArea<<endl;
            trackBlocks[j].area=contoursArea;
            mu = moments(contours[i], false );
            trackBlocks[j].mc=Point2d( mu.m10/mu.m00 , mu.m01/mu.m00 );
            boundRect = boundingRect(Mat(contours[i]));//获取目标序列号放置位置
            j++;
            rectangle(result, boundRect, Scalar(255, 0, 0), 2, 8);
            if(j>=19)
                break;
        }
    }
    //计数，防止重复计数
    for (int i = 0; i < j+1; i++)
      {
        if(trackBlocks[i].mc.y>=roiHeight)
            {
              number++;
            }
       }
    return result;//返回result
}

static void onValue(int,void*)
{
   //threshold(diff, diff_thresh, threValue, 255, CV_THRESH_BINARY);
   Mat kernelErode = getStructuringElement(MORPH_RECT, Size(erodeValue,erodeValue));
   Mat kernelDilate = getStructuringElement(MORPH_RECT, Size(dilateValue, dilateValue));
   erode(diffFrame, diffFrame, kernelErode);
   GaussianBlur(diffFrame,diffFrame,Size(gaussValue*2+1,gaussValue*2+1),0,0);
   dilate(diffFrame, diffFrame, kernelDilate);
}
