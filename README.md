# countObjects  
  帧差法流水线计数统计   
  用于工厂流水线物体计数   
  update:  
  2019-1-21 祥恩   尝试用MobileNet&SSD深度学习模型进行目标检测,树莓派CPU占用率达９０％以上，无法运行  
  2019-1-22 祥恩   换yolov3模型,比MobileNet&SSD还要慢．暂时放弃神经网络进行目标检测的想法  
  2019-1-23 祥恩   工厂考察环境  
  2019-1-24 祥恩   重新整理算法，规范代码  
  2019-1-25 祥恩   重写帧差算法，去掉了没必要的图像处理步骤以及缩小图片尺寸,在树莓派上速度有提升，但依旧延时严重  
  2019-1-26 祥恩   改用mog2算法，速度有明显提升，但依旧不行  
  2019-1-26 祥恩   写LK光流算法，进度条20%  
  2019-1-28 祥恩   写LK稀疏光流算法,稠密光流运算量大80%　　
