#include "MTC.h"

#include <thread>
#include <mutex>

using namespace cv;
using namespace std;

mutex Lock;

void Thread_Function(Mat InputImg, Mat *OutputImg, int BlockX, int BlockY, int Core_ID, int *Thread_Status)	//子线程函数
{//各参数分别为：输入矩阵，输出矩阵的内存地址（线程共享），块起始X，块起始Y，线程核心ID，线程状态记录数组地址
	*Thread_Status = T_BUSY;	//设为正忙

	Mat result(InputImg.size(), InputImg.type(), Scalar(Core_ID*10, Core_ID * 10, Core_ID * 10));	//注：视情况修改矩阵类型，如八位灰阶图

	{
		lock_guard<mutex> G(Lock);	//使用lock_guard锁定共享变量
		Mat ImgROI(*OutputImg, Rect(BlockX, BlockY, InputImg.cols, InputImg.rows));
		result.copyTo(ImgROI);	//注：视情况修改输出方式，如添加掩膜
	}

	printf("Thread Finsh.\tCore ID:%d\n", Core_ID);

	*Thread_Status = T_FREE;	//设为空闲
	return;
}

bool MTC_OpenCV::MT_Run()
{
	//源图像副本
	Mat OriImg(this->ImgIn->size(), this->ImgIn->type());
	this->ImgIn->copyTo(OriImg);

	//输出矩阵
	Mat ImgOut(this->ImgIn->size(), this->ImgIn->type(), Scalar(255, 255, 255));

	int Rows = OriImg.rows;	//源图像行数
	int Cols = OriImg.cols;	//源图像列数

	//计算分块数
	const int BlockCountR = Rows / this->BlockSize + 1;				//行分块数
	int RemainPixR = Rows - (BlockCountR - 1) * this->BlockSize;	//行余量
	const int BlockCountC = Cols / this->BlockSize + 1;				//列分块数
	int RemainPixC = Cols - (BlockCountC - 1) * this->BlockSize;	//列余量

	printf("Block Count: %d Blocks\n", BlockCountC * BlockCountR);	//输出分块数

	int BlockR = this->BlockSize;	//分块行数
	int BlockC = this->BlockSize;	//分块列数
	int BlockX = 0;					//分块起始列数
	int BlockY = 0;					//分块起始行数

	//多线程准备
	thread ColorDectors[MAX_THREAD_COUNT];	//线程对象组

	int thread_status[MAX_THREAD_COUNT];	//线程状态记录数组
	int TCount = 0;	//线程计数器
	const int TCountMax = this->ThreadCount - 1;	//线程最大编号

	//初始化线程状态记录数组
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		thread_status[i] = T_FREE;
	

	//图像分块并启动相应线程
	for (int i = 0; i < BlockCountR; i++)	//行分块
	{
		if (i == (BlockCountR - 1))
			BlockR = RemainPixR;
		else
			BlockR = this->BlockSize;
		BlockY = i * this->BlockSize;
		for (int j = 0; j < BlockCountC; j++)	//列分块
		{
			if (j == (BlockCountC - 1))
				BlockC = RemainPixC;
			else
				BlockC = this->BlockSize;
			BlockX = j * this->BlockSize;

			Mat TransImg(BlockR, BlockC, CV_8UC3);	//创建传输矩阵  注：视情况调整传输矩阵类型
			Mat imgROI(OriImg, Rect(BlockX, BlockY, BlockC, BlockR));	//设定分割块
			imgROI.copyTo(TransImg);	//将分割块拷贝到传输矩阵

			while (thread_status[TCount])	//寻找空闲的线程
			{
				TCount++;
				if (TCount > TCountMax)
					TCount = 0;
			}
			if (ColorDectors[TCount].joinable())
				ColorDectors[TCount].join();

			ColorDectors[TCount] = thread(Thread_Function, TransImg, &ImgOut, BlockX, BlockY, TCount, &thread_status[TCount]);	//传输数据，启动子线程

			TCount++;	//线程计数器自增
			if (TCount > TCountMax)
				TCount = 0;
		}
	}

	while (1)	//确认所有子进程均已结束
	{
		for (int i = 0; i <= TCountMax; i++)
		{
			if (thread_status[i])
			{
				i--;
				continue;
			}
			if (ColorDectors[i].joinable())
				ColorDectors[i].join();
		}
		break;
	}

	printf("ALL Threads Join. Process Finish.\n\n");	//控制台输出确认

	this->ImgOut = ImgOut;

	return true;
}