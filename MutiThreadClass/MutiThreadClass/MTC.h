#pragma once

#include <opencv2\opencv.hpp>
#include <thread>

using namespace cv;

//线程状态记录
#define T_FREE 0;
#define T_BUSY 1;
#define T_LOCK 2;

//程序支持的最大线程数
#define MAX_THREAD_COUNT 64

class MTC_OpenCV
{
public:
	int ThreadCount = 0;	//最大线程数
	int BlockSize = 127;	//分块大小

	MTC_OpenCV(Mat* Input, int bs = 127, int tc = 0)
	{
		ImgIn = Input;
		BlockSize = bs;	//设定分块大小
		if (tc == 0)	//设定最大线程数
			ThreadCount = std::thread::hardware_concurrency();
		else
			ThreadCount = tc;
	}

	Mat GetResult()	//结果返回函数
	{
		return ImgOut;
	}

	bool MT_Run();	//主运行函数
private:
	Mat *ImgIn, ImgOut;
};