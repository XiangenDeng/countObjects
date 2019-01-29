//-------------------------------------------------------
//算法描述：目标检测计数算法,LK
//-------------------------------------------------------

#include "count.h"

//主函数
int main()
{
    VideoCapture cap;
    //cap.open(0);
    cap.open("./nice10.avi");
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

    vector<float> error;
    Point2f history;
    namedWindow("show",WINDOW_NORMAL);
    while(true)
    {
        cap>>currentFrame;
        if(currentFrame.empty())
        {
            cout<<"frame is empty"<<endl;
            break;
        }
        cvtColor(currentFrame,curGrayFrame,CV_BGR2GRAY);

        //特征点太少则重新检测添加特征点
        if(addNewPoints())
        {
            //preGrayFrame=curGrayFrame.clone();
            goodFeaturesToTrack(curGrayFrame,featurePoints,cornerCount,qualityLevel,minDistance);
            cornerSubPix(curGrayFrame,featurePoints,cvSize(winSize,winSize),Size(-1,-1),termcrit);//精确检测角点函数
            points[0].insert(points[0].end(),featurePoints.begin(),featurePoints.end());
            trackPoints.insert(trackPoints.end(),featurePoints.begin(),featurePoints.end());
        }

        //第一帧
        if (preGrayFrame.empty())
        {
            curGrayFrame.copyTo(preGrayFrame);
        }

        //LK函数
        calcOpticalFlowPyrLK(preGrayFrame,curGrayFrame,points[0], points[1], status, error,Size(winSize,winSize),maxLevel,termcrit);

        //去掉位置不变的点且超过计数线的点
        int k=0;
        for(int i=0;i<points[1].size();i++)
        {
            if (acceptTrackedPoint(i))
            {
                if(points[1][i].y<=countLine&&points[1][i].x<countLeft&&points[1][i].x>countRight)
                {
                    trackPoints[k] = trackPoints[i];
                    points[1][k] = points[1][i];
                    k++;
                }
            }
        }
        points[1].resize(k);//重新调整容器大小
        trackPoints.resize(k);

        //显示特征点和运动轨迹
        if(isDraw)
        {
            line(currentFrame,Point(countRight,countLine),Point(countLeft,countLine),Scalar(255,0,0),2);//下边界
            line(currentFrame,Point(countRight,0),Point(countRight,countLine),Scalar(255,0,0),2);//右边界
            line(currentFrame,Point(countLeft,0),Point(countLeft,countLine),Scalar(255,0,0),2);//左边界
            for(int i=0;i<points[1].size();i++)
            {
                //line(currentFrame,trackPoints[i],points[1][i],Scalar(0,0,255),2);
                circle(currentFrame,points[1][i],3,Scalar(0,255,0),-1);
            }
        }

        //计数方法,连续通过n个点则计数加１
        for(int i=0;i<points[1].size();i++)
        {
            //cout<<(int)points[1][i].y<<endl;
            if((int)points[1][i].y==countLine-1)
            {
                if(history.y-points[1][i].y<1)
                    passPoint+=1;
                history=points[1][i];
                if(passPoint>=15)
                {
                    number++;
                    passPoint=0;
                }
                break;
            }
        }

        //显示数量
        sprintf(numText,"%d",number);
        putText(currentFrame,numText,Point(60,50),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),4,8);//显示数量

        swap(points[1],points[0]);
        preGrayFrame=curGrayFrame.clone();

        cvResizeWindow("show",640,480);
        imshow("show",currentFrame);
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
	return trackPoints.size() <= cornerCount;
}

//决定那些跟踪点被接受
bool acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 0.5);
}
