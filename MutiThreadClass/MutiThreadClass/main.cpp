#include <opencv2/opencv.hpp>
#include <iostream>

#include "MTC.h"

using namespace cv;
using namespace std;

int main()
{
	Mat InputTestImg(1080, 1920, CV_8UC3, Scalar(0, 0, 0));
	Mat OutputTestImg(1080, 1920, CV_8UC3, Scalar(0, 0, 0));

	MTC_OpenCV Processor1(&InputTestImg, 8, 0);

	Processor1.MT_Run();

	OutputTestImg = Processor1.GetResult();

	namedWindow("≤‚ ‘ ‰≥ˆ", WINDOW_AUTOSIZE);
	imshow("≤‚ ‘ ‰≥ˆ", OutputTestImg);

	waitKey(0);
	return 0;
}