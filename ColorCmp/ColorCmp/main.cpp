#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat OriImg = imread("D:\\UserData\\Documents\\GitHub\\OpenCV\\ColorCmp\\1.jpg");
	
	namedWindow("Ô´Í¼Ïñ", WINDOW_AUTOSIZE);
	imshow("Ô´Í¼Ïñ", OriImg);



	waitKey(0);
	return 0;
}