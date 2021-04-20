#include "MTC.h"

#include <thread>
#include <mutex>

using namespace cv;
using namespace std;

mutex Lock;

void Thread_Function(Mat InputImg, Mat *OutputImg, int BlockX, int BlockY, int Core_ID, int *Thread_Status)	//���̺߳���
{//�������ֱ�Ϊ������������������ڴ��ַ���̹߳���������ʼX������ʼY���̺߳���ID���߳�״̬��¼�����ַ
	*Thread_Status = T_BUSY;	//��Ϊ��æ

	Mat result(InputImg.size(), InputImg.type(), Scalar(Core_ID*10, Core_ID * 10, Core_ID * 10));	//ע��������޸ľ������ͣ����λ�ҽ�ͼ

	{
		lock_guard<mutex> G(Lock);	//ʹ��lock_guard�����������
		Mat ImgROI(*OutputImg, Rect(BlockX, BlockY, InputImg.cols, InputImg.rows));
		result.copyTo(ImgROI);	//ע��������޸������ʽ���������Ĥ
	}

	printf("Thread Finsh.\tCore ID:%d\n", Core_ID);

	*Thread_Status = T_FREE;	//��Ϊ����
	return;
}

bool MTC_OpenCV::MT_Run()
{
	//Դͼ�񸱱�
	Mat OriImg(this->ImgIn->size(), this->ImgIn->type());
	this->ImgIn->copyTo(OriImg);

	//�������
	Mat ImgOut(this->ImgIn->size(), this->ImgIn->type(), Scalar(255, 255, 255));

	int Rows = OriImg.rows;	//Դͼ������
	int Cols = OriImg.cols;	//Դͼ������

	//����ֿ���
	const int BlockCountR = Rows / this->BlockSize + 1;				//�зֿ���
	int RemainPixR = Rows - (BlockCountR - 1) * this->BlockSize;	//������
	const int BlockCountC = Cols / this->BlockSize + 1;				//�зֿ���
	int RemainPixC = Cols - (BlockCountC - 1) * this->BlockSize;	//������

	printf("Block Count: %d Blocks\n", BlockCountC * BlockCountR);	//����ֿ���

	int BlockR = this->BlockSize;	//�ֿ�����
	int BlockC = this->BlockSize;	//�ֿ�����
	int BlockX = 0;					//�ֿ���ʼ����
	int BlockY = 0;					//�ֿ���ʼ����

	//���߳�׼��
	thread ColorDectors[MAX_THREAD_COUNT];	//�̶߳�����

	int thread_status[MAX_THREAD_COUNT];	//�߳�״̬��¼����
	int TCount = 0;	//�̼߳�����
	const int TCountMax = this->ThreadCount - 1;	//�߳������

	//��ʼ���߳�״̬��¼����
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		thread_status[i] = T_FREE;
	

	//ͼ��ֿ鲢������Ӧ�߳�
	for (int i = 0; i < BlockCountR; i++)	//�зֿ�
	{
		if (i == (BlockCountR - 1))
			BlockR = RemainPixR;
		else
			BlockR = this->BlockSize;
		BlockY = i * this->BlockSize;
		for (int j = 0; j < BlockCountC; j++)	//�зֿ�
		{
			if (j == (BlockCountC - 1))
				BlockC = RemainPixC;
			else
				BlockC = this->BlockSize;
			BlockX = j * this->BlockSize;

			Mat TransImg(BlockR, BlockC, CV_8UC3);	//�����������  ע����������������������
			Mat imgROI(OriImg, Rect(BlockX, BlockY, BlockC, BlockR));	//�趨�ָ��
			imgROI.copyTo(TransImg);	//���ָ�鿽�����������

			while (thread_status[TCount])	//Ѱ�ҿ��е��߳�
			{
				TCount++;
				if (TCount > TCountMax)
					TCount = 0;
			}
			if (ColorDectors[TCount].joinable())
				ColorDectors[TCount].join();

			ColorDectors[TCount] = thread(Thread_Function, TransImg, &ImgOut, BlockX, BlockY, TCount, &thread_status[TCount]);	//�������ݣ��������߳�

			TCount++;	//�̼߳���������
			if (TCount > TCountMax)
				TCount = 0;
		}
	}

	while (1)	//ȷ�������ӽ��̾��ѽ���
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

	printf("ALL Threads Join. Process Finish.\n\n");	//����̨���ȷ��

	this->ImgOut = ImgOut;

	return true;
}