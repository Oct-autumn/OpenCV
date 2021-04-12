#pragma once
#include <opencv2\opencv.hpp>

using namespace cv;

class ColorDect_MTver
{
public:
	int ThreadCount = 1;
	int BlockSize = 127;

	void SetConst(int tc = 1, int bs = 127);	//常量设定函数

	static bool DetcetColor(Vec3b TargetColor);

	ColorDect_MTver(Mat* input, Mat* output, Vec3b Color)
	{
		ImgIn = input;
		ImgOut = output;
		TargetColor = Color;
	}
private:
	Mat *ImgIn, *ImgOut;
	Vec3b TargetColor;
	Mat ColorDector_T();
};