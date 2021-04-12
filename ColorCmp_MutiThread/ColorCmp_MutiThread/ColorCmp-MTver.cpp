#include "ColorCmp-MTver.h"
#include <opencv2/opencv.hpp>
#include <Thread>
#include <mutex>

using namespace cv;
using namespace std;

mutex Lock;

void ColorDect_MTver::SetConst(int tc, int bs)
{
	ThreadCount = tc;
	BlockSize = bs;
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

bool ColorDector_T(Mat InputImg, Mat* OutputImg, int Sx, int Sy, Vec3b TargetColor, int Delta)
{
	Mat result(InputImg.size(), CV_8UC1, Scalar(0));

	Mat_<Vec3b>::iterator NowPix_ori = InputImg.begin<Vec3b>();
	const Mat_<Vec3b>::iterator EndPix_ori = InputImg.end<Vec3b>();

	Mat_<uchar>::iterator NowPix_out = result.begin<uchar>();

	//遍历查找颜色,写入结果矩阵
	for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)
	{
		if (ColorCmp(*NowPix_ori, TargetColor) <= Delta)	//调用ColorCmp函数计算当前像素颜色与目标颜色的“距离”
			*NowPix_out = 255;
		else
			*NowPix_out = 0;
	}

	{//通过lock_guard锁定与解锁共享变量
		lock_guard<mutex> G(Lock);

		Mat imgROI(*OutputImg, Rect(Sx, Sy, InputImg.rows, InputImg.cols));
		InputImg.copyTo(imgROI, result);
	}

	printf("Thread ID:%d Finsh.\n", this_thread::get_id());

	return true;
}

bool ColorDect_MTver::DetcetColor()
{
	int Rows = this->ImgIn->rows;
	int Cols = ImgIn->cols;

	const int BlockCountR = Rows / this->BlockSize + 1;
	int RemainPixR = Rows - (BlockCountR - 1) * this->BlockSize;
	const int BlockCountC = Cols / this->BlockSize + 1;
	int RemainPixC = Cols - (BlockCountC - 1) * this->BlockSize;

	int BlockR = this->BlockSize, BlockC = this->BlockSize;
	int BlockX = 0, BlockY = 0;

	thread ColorDectors[64];
	bool if_thread_free[64];
	int TCount = 0;

	for (int i = 0; i < 64; i++)
		if_thread_free[i] = true;

	for (int i = 0; i < BlockCountR; i++)
	{
		if (i == (BlockCountR - 1))
			BlockR = RemainPixR;
		else
			BlockR = this->BlockSize;
		BlockX = i * this->BlockSize;
		for (int j = 0; j < BlockCountC; j++)
		{
			if (j == (BlockCountC - 1))
				BlockC = RemainPixC;
			else
				BlockC = this->BlockSize;
			BlockY = j * this->BlockSize;

			Mat TransImg(BlockR, BlockC, CV_8UC3);
			Mat imgROI(*(this->ImgIn), Rect(BlockX, BlockY, BlockR, BlockC));
			imgROI.copyTo(TransImg);
			ColorDectors[TCount] = thread(ColorDector_T, TransImg, this->ImgOut, BlockX, BlockY, TargetColor, Delta);
			if_thread_free[TCount] = false;
			TCount++;
		}
	}

	for (int i = 0; i <= TCount; i++)
		ColorDectors[i].join();

	return true;
}

Mat ColorDect_MTver::GetResult()
{
	return *(this->ImgOut);
}