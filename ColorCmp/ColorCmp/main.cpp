#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat OriImg = imread("D:\\UserData\\Documents\\GitHub\\OpenCV\\ColorCmp\\1.jpg");
	
	namedWindow("Դͼ��", WINDOW_AUTOSIZE);
	imshow("Դͼ��", OriImg);



	waitKey(0);
	return 0;
}