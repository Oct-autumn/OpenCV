#include "ColorCmp-MTver.h"

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct MoseTrans //�������Ϣ�����������
{
	Mat* OriImg;
	Mat* ImgOut;
}MouseMsg;	//�������Ϣ�������

void MouseEvent(int event, int x, int y, int flags, void* param)
{
	MoseTrans* Msg = reinterpret_cast<MoseTrans*>(param);
	Mat* OriImg = Msg->OriImg;
	Mat* ImgOut = Msg->ImgOut;
	Mat TargetColor(150, 150, CV_8UC3, Scalar(OriImg->at<Vec3b>(Point(x, y))));

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		namedWindow("Ŀ����ɫ", WINDOW_AUTOSIZE);
		imshow("Ŀ����ɫ", TargetColor);
		//�Ӿ�Ч�� ���ѡ�����ص�RGB��ֵ
		printf("Ŀ����ɫΪ: R %03d  G %03d  B %03d\n", OriImg->at<Vec3b>(Point(x, y))[2], OriImg->at<Vec3b>(Point(x, y))[1], OriImg->at<Vec3b>(Point(x, y))[0]);

		//���������
		ColorDect_MTver ColorDect1(OriImg, OriImg->at<Vec3b>(Point(x, y)), 1000);
		ColorDect1.SetConst(16, 256);//�趨 ����߳��� �� �ֿ��С

		ColorDect1.DetcetColor();	//�������Һ���

		*ImgOut = ColorDect1.GetResult();	//��ȡ���
		
		namedWindow("��ɫ���ҽ��", WINDOW_AUTOSIZE);
		imshow("��ɫ���ҽ��", *ImgOut);

		break;
	}
}

int main()
{
	//D:\\myPictures\\��ͷ��.jpg
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\ColorCmp_MutiThread\\1.jpg");
	Mat ImgOut(OriImg.size(), CV_8UC1);

	namedWindow("Դͼ��", WINDOW_AUTOSIZE);
	imshow("Դͼ��", OriImg);

	MouseMsg = { &OriImg, &ImgOut };	//�������ݣ�ԭʼͼ�������ͼ��

	//ע�������Ӧ����
	setMouseCallback("Դͼ��", MouseEvent, reinterpret_cast<void*>(&MouseMsg));

	waitKey(0);
	return 0;
}