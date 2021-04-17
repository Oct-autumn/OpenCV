#include "ColorCmp-MTver.h"

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct MoseTrans //鼠标点击信息传输变量类型
{
	Mat* OriImg;
	Mat* ImgOut;
}MouseMsg;	//鼠标点击信息传输变量

void MouseEvent(int event, int x, int y, int flags, void* param)
{
	MoseTrans* Msg = reinterpret_cast<MoseTrans*>(param);
	Mat* OriImg = Msg->OriImg;
	Mat* ImgOut = Msg->ImgOut;
	Mat TargetColor(150, 150, CV_8UC3, Scalar(OriImg->at<Vec3b>(Point(x, y))));

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		namedWindow("目标颜色", WINDOW_AUTOSIZE);
		imshow("目标颜色", TargetColor);
		//视觉效果 输出选中像素的RGB数值
		printf("目标颜色为: R %03d  G %03d  B %03d\n", OriImg->at<Vec3b>(Point(x, y))[2], OriImg->at<Vec3b>(Point(x, y))[1], OriImg->at<Vec3b>(Point(x, y))[0]);

		//创建类对象
		ColorDect_MTver ColorDect1(OriImg, OriImg->at<Vec3b>(Point(x, y)), 1000);
		ColorDect1.SetConst(16, 256);//设定 最大线程数 和 分块大小

		ColorDect1.DetcetColor();	//启动查找函数

		*ImgOut = ColorDect1.GetResult();	//获取结果
		
		namedWindow("颜色查找结果", WINDOW_AUTOSIZE);
		imshow("颜色查找结果", *ImgOut);

		break;
	}
}

int main()
{
	//D:\\myPictures\\大头照.jpg
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\ColorCmp_MutiThread\\1.jpg");
	Mat ImgOut(OriImg.size(), CV_8UC1);

	namedWindow("源图像", WINDOW_AUTOSIZE);
	imshow("源图像", OriImg);

	MouseMsg = { &OriImg, &ImgOut };	//传输内容：原始图像与输出图像

	//注册鼠标响应函数
	setMouseCallback("源图像", MouseEvent, reinterpret_cast<void*>(&MouseMsg));

	waitKey(0);
	return 0;
}