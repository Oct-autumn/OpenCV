#include <opencv2/opencv.hpp>
#include <iostream>

#define FUNC 0	//功能片段选择 0.无操作 1.RGB通道分离

using namespace cv;
using namespace std;

void onMouse(int event, int x, int y, int flags, void* param)
{
	Mat* img = reinterpret_cast<Mat*>(param);

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		printf("Click at X:%d Y:%d\n", x, y);
		cout << "\tPixelValue:" << static_cast<int>(img->at<uchar>(Point(x, y))) << endl;
		break;
	default:
		break;
	}
}

int main()
{
	//读入图像
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\MatFunction\\2\\2.jpg");

	if (OriImg.empty())
	{
		printf("Error: Original Image Load Fail...\n");
		waitKey(0);
		return -1;
	}

	namedWindow("Input Image", WINDOW_AUTOSIZE);
	imshow("Input Image", OriImg);
	//setMouseCallback("Input Image", onMouse, reinterpret_cast<void*>(&OriImg));

	const int OriChannel = OriImg.channels();

	const int rows = OriImg.rows;
	const int cols = OriImg.cols * OriChannel;



	//RGB分离 请使用图像 ./MatFunction/1/1.jpg
	if (FUNC == 1)
	{
	//Blue通道分离
	Mat ImgBchannle(OriImg.size(), OriImg.type());

	for (int i = 0; i < rows; i++)
	{
		uchar* currentB_In = OriImg.ptr<uchar>(i);
		uchar* currentB_Out = ImgBchannle.ptr<uchar>(i);
		for (int j = 0; j < cols; j += OriChannel)
		{
			currentB_Out[j] = currentB_In[j];
		}
	}

	namedWindow("Blue Channle", WINDOW_AUTOSIZE);
	imshow("Blue Channle", ImgBchannle);
	imwrite("D:\\MyDocuments\\GitHub\\OpenCV\\MatFunction\\1\\1-Blue.jpg", ImgBchannle);

	//Green通道分离
	Mat ImgGchannle(OriImg.size(), OriImg.type());

	for (int i = 0; i < rows; i++)
	{
		uchar* currentG_In = OriImg.ptr<uchar>(i);
		uchar* currentG_Out = ImgGchannle.ptr<uchar>(i);
		for (int j = 1; j < cols; j += OriChannel)
		{
			currentG_Out[j] = currentG_In[j];
		}
	}

	namedWindow("Green Channle", WINDOW_AUTOSIZE);
	imshow("Green Channle", ImgGchannle);
	imwrite("D:\\MyDocuments\\GitHub\\OpenCV\\MatFunction\\1\\1-Green.jpg", ImgGchannle);

	//Red通道分离
	Mat ImgRchannle(OriImg.size(), OriImg.type());

	for (int i = 0; i < rows; i++)
	{
		uchar* currentR_In = OriImg.ptr<uchar>(i);
		uchar* currentR_Out = ImgRchannle.ptr<uchar>(i);
		for (int j = 2; j < cols; j += OriChannel)
		{
			currentR_Out[j] = currentR_In[j];
		}
	}

	namedWindow("Red Channle", WINDOW_AUTOSIZE);
	imshow("Red Channle", ImgRchannle);
	imwrite("D:\\MyDocuments\\GitHub\\OpenCV\\MatFunction\\1\\1-Red.jpg", ImgRchannle);
	}


	waitKey(0);
	return 0;
}