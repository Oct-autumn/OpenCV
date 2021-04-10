#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int ColorCmp(Vec3b CurrentColor, Vec3b TargetColor)
{
	int dest = 0;

	//计算方法 平方欧式距离
	dest += pow(CurrentColor[0] - TargetColor[0], 2);
	dest += pow(CurrentColor[1] - TargetColor[1], 2);
	dest += pow(CurrentColor[2] - TargetColor[2], 2);

	return dest;
}

bool DetcetColor(Mat* ImgIn, Mat* ImgOut, Vec3b TargetColor)	//颜色检测函数
{
	//迭代器
	Mat_<Vec3b>::iterator NowPix_ori = ImgIn->begin<Vec3b>();
	const Mat_<Vec3b>::iterator EndPix_ori = ImgIn->end<Vec3b>();

	Mat_<uchar>::iterator NowPix_out = ImgOut->begin<uchar>();

	//遍历查找颜色
	const int delta = 1000;	//允许误差
	for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)
	{
		if (ColorCmp(*NowPix_ori, TargetColor) <= delta)	//调用ColorCmp函数计算当前像素颜色与目标颜色的“距离”
			*NowPix_out = 255;
		else
			*NowPix_out = 0;
	}
	return true;
}

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

		//调用函数
		DetcetColor(OriImg, ImgOut, OriImg->at<Vec3b>(Point(x, y)));

		namedWindow("颜色查找结果", WINDOW_AUTOSIZE);
		imshow("颜色查找结果", *ImgOut);

		break;
	default:
		break;
	}
}

int main()
{
	//D:\\myPictures\\大头照.jpg
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\ColorCmp\\1.jpg");
	Mat ImgOut(OriImg.size(), CV_8UC1);
	
	namedWindow("源图像", WINDOW_AUTOSIZE);
	imshow("源图像", OriImg);

	MouseMsg = { &OriImg, &ImgOut };	//传输内容：原始图像与输出图像

	//注册鼠标响应函数
	setMouseCallback("源图像", MouseEvent, reinterpret_cast<void*>(&MouseMsg));

	waitKey(0);
	return 0;
}