#pragma once
#include <opencv2\opencv.hpp>

using namespace cv;

class ColorDect_MTver
{
public:
	int ThreadCount = 1;
	int BlockSize = 127;

	ColorDect_MTver(Mat* input, Vec3b Color, int delta)
	{
		ImgIn = input;
		TargetColor = Color;
		Delta = delta;
	}

	void SetConst(int tc = 1, int bs = 127);	//常量设定函数

	bool DetcetColor();

	Mat GetResult();
private:
	Mat *ImgIn, ImgOut;
	Vec3b TargetColor;
	int Delta = 1000;
};