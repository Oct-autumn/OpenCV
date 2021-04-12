#include "ColorCmp-MTver.h"
#include <opencv2/opencv.hpp>

using namespace cv;

void ColorDect_MTver::SetConst(int tc, int bs)
{
	ThreadCount = tc;
	BlockSize = bs;
}

bool ColorDect_MTver::DetcetColor(Vec3b TargetColor)
{
	return true;
}

int ColorCmp(Vec3b CurrentColor, Vec3b TargetColor)
{
	int dest = 0;

	//计算方法 平方欧式距离
	dest += pow(CurrentColor[0] - TargetColor[0], 2);
	dest += pow(CurrentColor[1] - TargetColor[1], 2);
	dest += pow(CurrentColor[2] - TargetColor[2], 2);

	return dest;
}


Mat ColorDect_MTver::ColorDector_T()
{

}