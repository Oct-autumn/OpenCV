#include "ColorCmp-MTver.h"
#include <opencv2/opencv.hpp>
#include <Thread>
#include <mutex>

#define MAX_THREAD_COUNT 64

#define T_FREE true
#define T_BUSY false

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

void ColorDector_T(Mat InputImg, Mat* OutputImg, int Sx, int Sy, Vec3b TargetColor, int Delta, int Core_ID, bool* Status)	//子线程函数
{
	*Status = T_BUSY;	//将相应线程状态置为 正忙

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

	{//通过Lock锁定与解锁共享变量
		Lock.lock();
		Mat imgROI(*OutputImg, Rect(Sy, Sx, InputImg.cols, InputImg.rows));
		InputImg.copyTo(imgROI, result);
		Lock.unlock();
	}

	printf("Thread Finsh.\tCore ID:%d\n", Core_ID);
	*Status = T_FREE;	//将相应线程状态置为 空闲

	return;
}

bool ColorDect_MTver::DetcetColor()
{
	//获取源图像副本
	Mat OriImg(this->ImgIn->size(), this->ImgIn->type());
	this->ImgIn->copyTo(OriImg);

	//创建输出矩阵
	Mat ImgOut(this->ImgIn->size(), this->ImgIn->type(), Scalar(255, 255, 255));

	//获取源图像行列数
	int Rows = OriImg.rows;
	int Cols = OriImg.cols;

	//计算分块数
	const int BlockCountR = Rows / this->BlockSize + 1;
	int RemainPixR = Rows - (BlockCountR - 1) * this->BlockSize;
	const int BlockCountC = Cols / this->BlockSize + 1;
	int RemainPixC = Cols - (BlockCountC - 1) * this->BlockSize;

	printf("Block Count: %d Blocks\n", BlockCountC * BlockCountR);

	//创建变量：分块行数、分块列数、分块起始列数、分块起始行数
	int BlockR = this->BlockSize, BlockC = this->BlockSize;
	int BlockX = 0, BlockY = 0;

	//多线程准备
	thread ColorDectors[MAX_THREAD_COUNT];	//创建线程对象组

	bool if_thread_free[MAX_THREAD_COUNT];	//创建线程状态记录数组
	int TCount = 0;	//线程计数器
	const int TCountMax = this->ThreadCount - 1;

	//初始化线程状态记录数组
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		if_thread_free[i] = T_FREE;

	//图像分块并启动相应线程
	for (int i = 0; i < BlockCountR; i++)	//行分块
	{
		if (i == (BlockCountR - 1))
			BlockR = RemainPixR;
		else
			BlockR = this->BlockSize;
		BlockX = i * this->BlockSize;
		for (int j = 0; j < BlockCountC; j++)	//列分块
		{
			if (j == (BlockCountC - 1))
				BlockC = RemainPixC;
			else
				BlockC = this->BlockSize;
			BlockY = j * this->BlockSize;

			Mat TransImg(BlockR, BlockC, CV_8UC3);	//创建传输矩阵
			Mat imgROI(OriImg, Rect(BlockY, BlockX, BlockC, BlockR));	//设定分割块
			imgROI.copyTo(TransImg);	//将分割块拷贝到传输矩阵

			while (!if_thread_free[TCount])	//寻找空闲的线程
			{
				TCount++;
				if (TCount > TCountMax)
					TCount = 0;					
			}
			if (ColorDectors[TCount].joinable())
				ColorDectors[TCount].join();

			ColorDectors[TCount] = thread(ColorDector_T, TransImg, &ImgOut, BlockX, BlockY, TargetColor, Delta, TCount, &if_thread_free[TCount]);	//传输数据，启动子线程
			//ColorDectors[TCount].detach();	//子线程解除锁定，运行结束后会自动回收资源

			TCount++;	//线程计数器自增
			if (TCount > TCountMax)
				TCount = 0;
		}
	}

	while (1)
	{
		for (int i = 0; i <= TCountMax; i++)
		{
			if (!if_thread_free[i])
			{
				i--;
				continue;
			}	
				ColorDectors[i].join();
		}
		break;
	}

	//this_thread::sleep_for(30ms);
	printf("ALL Threads Join. Process Finish.\n\n");	//控制台输出确认
	
	this->ImgOut = ImgOut;

	return true;
}

Mat ColorDect_MTver::GetResult()
{
	return this->ImgOut;
}