#pragma once

#include <opencv2\opencv.hpp>
#include <thread>

using namespace cv;

//�߳�״̬��¼
#define T_FREE 0;
#define T_BUSY 1;
#define T_LOCK 2;

//����֧�ֵ�����߳���
#define MAX_THREAD_COUNT 64

class MTC_OpenCV
{
public:
	int ThreadCount = 0;	//����߳���
	int BlockSize = 127;	//�ֿ��С

	MTC_OpenCV(Mat* Input, int bs = 127, int tc = 0)
	{
		ImgIn = Input;
		BlockSize = bs;	//�趨�ֿ��С
		if (tc == 0)	//�趨����߳���
			ThreadCount = std::thread::hardware_concurrency();
		else
			ThreadCount = tc;
	}

	Mat GetResult()	//������غ���
	{
		return ImgOut;
	}

	bool MT_Run();	//�����к���
private:
	Mat *ImgIn, ImgOut;
};