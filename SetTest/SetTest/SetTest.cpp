#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;

int main()
{
	Mat img = imread("D:\\MyPictures\\XP-1.png");
	namedWindow("SetTest", WINDOW_AUTOSIZE);
	imshow("SetTest", img);

	namedWindow("TurnTest", WINDOW_AUTOSIZE);
	cvtColor(img, img, COLOR_BGR2GRAY);
	imshow("TurnTest", img);
	waitKey(0);
	return 0;
}