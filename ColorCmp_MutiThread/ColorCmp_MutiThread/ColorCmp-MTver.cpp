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

	//���㷽�� ƽ��ŷʽ����
	dest += pow(CurrentColor[0] - TargetColor[0], 2);
	dest += pow(CurrentColor[1] - TargetColor[1], 2);
	dest += pow(CurrentColor[2] - TargetColor[2], 2);

	return dest;
}

void ColorDector_T(Mat InputImg, Mat* OutputImg, int Sx, int Sy, Vec3b TargetColor, int Delta, int Core_ID, bool* Status)	//���̺߳���
{
	*Status = T_BUSY;	//����Ӧ�߳�״̬��Ϊ ��æ

	Mat result(InputImg.size(), CV_8UC1, Scalar(0));

	Mat_<Vec3b>::iterator NowPix_ori = InputImg.begin<Vec3b>();
	const Mat_<Vec3b>::iterator EndPix_ori = InputImg.end<Vec3b>();

	Mat_<uchar>::iterator NowPix_out = result.begin<uchar>();

	//����������ɫ,д��������
	for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)
	{
		if (ColorCmp(*NowPix_ori, TargetColor) <= Delta)	//����ColorCmp�������㵱ǰ������ɫ��Ŀ����ɫ�ġ����롱
			*NowPix_out = 255;
		else
			*NowPix_out = 0;
	}

	{//ͨ��Lock����������������
		Lock.lock();
		Mat imgROI(*OutputImg, Rect(Sy, Sx, InputImg.cols, InputImg.rows));
		InputImg.copyTo(imgROI, result);
		Lock.unlock();
	}

	printf("Thread Finsh.\tCore ID:%d\n", Core_ID);
	*Status = T_FREE;	//����Ӧ�߳�״̬��Ϊ ����

	return;
}

bool ColorDect_MTver::DetcetColor()
{
	//��ȡԴͼ�񸱱�
	Mat OriImg(this->ImgIn->size(), this->ImgIn->type());
	this->ImgIn->copyTo(OriImg);

	//�����������
	Mat ImgOut(this->ImgIn->size(), this->ImgIn->type(), Scalar(255, 255, 255));

	//��ȡԴͼ��������
	int Rows = OriImg.rows;
	int Cols = OriImg.cols;

	//����ֿ���
	const int BlockCountR = Rows / this->BlockSize + 1;
	int RemainPixR = Rows - (BlockCountR - 1) * this->BlockSize;
	const int BlockCountC = Cols / this->BlockSize + 1;
	int RemainPixC = Cols - (BlockCountC - 1) * this->BlockSize;

	printf("Block Count: %d Blocks\n", BlockCountC * BlockCountR);

	//�����������ֿ��������ֿ��������ֿ���ʼ�������ֿ���ʼ����
	int BlockR = this->BlockSize, BlockC = this->BlockSize;
	int BlockX = 0, BlockY = 0;

	//���߳�׼��
	thread ColorDectors[MAX_THREAD_COUNT];	//�����̶߳�����

	bool if_thread_free[MAX_THREAD_COUNT];	//�����߳�״̬��¼����
	int TCount = 0;	//�̼߳�����
	const int TCountMax = this->ThreadCount - 1;

	//��ʼ���߳�״̬��¼����
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		if_thread_free[i] = T_FREE;

	//ͼ��ֿ鲢������Ӧ�߳�
	for (int i = 0; i < BlockCountR; i++)	//�зֿ�
	{
		if (i == (BlockCountR - 1))
			BlockR = RemainPixR;
		else
			BlockR = this->BlockSize;
		BlockX = i * this->BlockSize;
		for (int j = 0; j < BlockCountC; j++)	//�зֿ�
		{
			if (j == (BlockCountC - 1))
				BlockC = RemainPixC;
			else
				BlockC = this->BlockSize;
			BlockY = j * this->BlockSize;

			Mat TransImg(BlockR, BlockC, CV_8UC3);	//�����������
			Mat imgROI(OriImg, Rect(BlockY, BlockX, BlockC, BlockR));	//�趨�ָ��
			imgROI.copyTo(TransImg);	//���ָ�鿽�����������

			while (!if_thread_free[TCount])	//Ѱ�ҿ��е��߳�
			{
				TCount++;
				if (TCount > TCountMax)
					TCount = 0;					
			}
			if (ColorDectors[TCount].joinable())
				ColorDectors[TCount].join();

			ColorDectors[TCount] = thread(ColorDector_T, TransImg, &ImgOut, BlockX, BlockY, TargetColor, Delta, TCount, &if_thread_free[TCount]);	//�������ݣ��������߳�
			//ColorDectors[TCount].detach();	//���߳̽�����������н�������Զ�������Դ

			TCount++;	//�̼߳���������
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
	printf("ALL Threads Join. Process Finish.\n\n");	//����̨���ȷ��
	
	this->ImgOut = ImgOut;

	return true;
}

Mat ColorDect_MTver::GetResult()
{
	return this->ImgOut;
}