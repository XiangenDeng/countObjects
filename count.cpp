#include "count.h"
#define N 3         //帧数
Mat preFrames[N];   //存储每三帧图片队列
float fscale=1;     //图片分辨率缩放倍数
int roiWidth=520*fscale;//roi区域宽度
int roiHeight=280*fscale;//roi区域高度
//主函数
int main()
{
    Mat result;//存储处理后的结果图像
    Mat currentFrame;//当前帧
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
    rect.width=roi_width;
    rect.height=roi_height;

    //初始化两帧图片并调整分辨率,提取感兴趣区域，灰度化
    for(int i=0;i<N-1;i++)
    {
        capture>>preFrames[i];
        resize(preFrames[i],preFrames[i], Size(preFrames[i].cols*fscale,preFrames[i].rows*fscale), (0, 0), (0, 0), INTER_LINEAR);
        preFrames[i]=preFrames[i](rect);
        cvtColor(preFrames[i], preFrames[i], CV_BGR2GRAY);
    }

    cout<<preFrames[0].size<<endl;//输出图片大小

    //展示结果输出窗口，并创建参数调整滑动条
    namedWindow("result",0);
    cvResizeWindow("result",640,480);
    createTrackbar("threshold","result",&thre_value,255,on_value);
    createTrackbar("erode","result",&erode_value,21,on_value);
    createTrackbar("gaussian","result",&gauss_value,50,on_value);
    createTrackbar("dilate","result",&dilate_value,21,on_value);

    //主循环
    while(true)
      {
        capture >> currentFrame;//读帧进frames
        if (currentFrame.empty())//对当前帧进行异常检测
        {
          cout << "frame is empty!" << endl;
          break;
        }
        resize(cimg2,cimg, Size(cimg2.cols*fscale,cimg2.rows*fscale), (0, 0), (0, 0), INTER_LINEAR);//重新调整图像大小
        rectangle(cimg,rect,Scalar(0, 0, 255),2);//画感兴趣区域ROI
        cimg1=cimg(rect);
        cvtColor(cimg1,gray3,CV_BGR2GRAY);
        absdiff(gray1, gray2, diff);
        absdiff(gray2,gray3,diff2);
        add(diff,diff2,diff);

        result = moveDetect(cimg);
        char tam[10];
        sprintf(tam,"%d",number);
        putText(result,tam,Point(100,150),FONT_HERSHEY_SIMPLEX,5,Scalar(255,23,0),4,8);//在图片上写文字
        cvResizeWindow("result",640,480);
        imshow("result", result);
        if (waitKey(5) == 27)//按原FPS显示
        {
          cout << "ESC退出!" << endl;
          break;
        }
        gray1 = gray2.clone();
        gray2 = gray3.clone();
      }
   return 0;

}

Mat moveDetect(Mat c_frame)
{
    Mat result=c_frame.clone();
    //Canny(diff,diff_thresh,100.200,3);
    on_value(thre_value,0);
    on_value(erode_value,0);
    on_value(gauss_value,0);
    on_value(dilate_value,0);
    //6.查找轮廓并绘制轮廓
    vector<vector<Point> > contours;
    findContours(diff_thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    drawContours(result, contours, -1, Scalar(0, 255, 0), 1,8);//在result上绘制轮廓
    //计算轮廓矩
    vector<Moments> mu(contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{
		mu[i] = moments( contours[i], false );
	}
	//计算轮廓的质心
	vector<Point2f> mc( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{
		mc[i] = Point2d( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
	}
    //7.查找正外接矩形
    vector<Rect> boundRect(contours.size());
    //2.2 定义Rect类型的vector容器roRect存放最小外接矩形，初始化大小为contours.size()即轮廓个数
	vector<RotatedRect> roRect(contours.size());
	if (flag==1)
	{
	    for (int i = 0; i < contours.size(); i++)
      {
		boundRect[i] = boundingRect(Mat(contours[i]));//获取目标序列号放置位置
		int width = boundRect[i].width;//获得正外接矩形的左上角坐标及宽高
		int height = boundRect[i].height;
		int x = boundRect[i].x;
		int y = boundRect[i].y;
        if (width>60 && height > 60)
        {
		  rectangle(result, Rect(x, y, width, height), Scalar(255, 0, 0), 2, 8);
		  if(x<roi_width&&y<roi_height)
		    if(mc[i].y>roi_height&&locate[i]<=roi_height&&flag == 1)
            {
              number++;
              flag =0;
            }
        }
        locate[i] = mc[i].y;
       }
    }
    else if(flag==0)
     {
      int flag2 =0;
      for (int i = 0; i < contours.size(); i++)
      {
		boundRect[i] = boundingRect(Mat(contours[i]));//获取目标序列号放置位置
		int width = boundRect[i].width;//获得正外接矩形的左上角坐标及宽高
		int height = boundRect[i].height;
		int x = boundRect[i].x;
		int y = boundRect[i].y;
        if (width>80 && height > 80)
        {
		  rectangle(result, Rect(x, y, width, height), Scalar(255, 0, 0), 2, 8);
		  if(x<roi_width&&y<roi_height)
		    if(mc[i].y>roi_height)
              flag2 +=1;
        }
        locate[i] = mc[i].y;
       }
       if (flag2==0)
        flag = 1;
     }
    return result;//返回result
}

static void on_value(int,void*)
{
   threshold(diff, diff_thresh, thre_value, 255, CV_THRESH_BINARY);
   Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(erode_value,erode_value));
   Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(dilate_value, dilate_value));
   erode(diff_thresh, diff_thresh, kernel_erode);
   GaussianBlur(diff_thresh,diff_thresh,Size(gauss_value*2+1,gauss_value*2+1),0,0);
   dilate(diff_thresh, diff_thresh, kernel_dilate);
}
