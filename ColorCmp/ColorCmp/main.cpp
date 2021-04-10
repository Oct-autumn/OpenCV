#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int ColorCmp(Vec3b CurrentColor, Vec3b TargetColor)
{
	int dest = 0;

	//���㷽�� ƽ��ŷʽ����
	dest += pow(CurrentColor[0] - TargetColor[0], 2);
	dest += pow(CurrentColor[1] - TargetColor[1], 2);
	dest += pow(CurrentColor[2] - TargetColor[2], 2);

	return dest;
}

bool DetcetColor(Mat* ImgIn, Mat* ImgOut, Vec3b TargetColor)	//��ɫ��⺯��
{
	//������
	Mat_<Vec3b>::iterator NowPix_ori = ImgIn->begin<Vec3b>();
	const Mat_<Vec3b>::iterator EndPix_ori = ImgIn->end<Vec3b>();

	Mat_<uchar>::iterator NowPix_out = ImgOut->begin<uchar>();

	//����������ɫ
	const int delta = 1000;	//�������
	for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)
	{
		if (ColorCmp(*NowPix_ori, TargetColor) <= delta)	//����ColorCmp�������㵱ǰ������ɫ��Ŀ����ɫ�ġ����롱
			*NowPix_out = 255;
		else
			*NowPix_out = 0;
	}
	return true;
}

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

		//���ú���
		DetcetColor(OriImg, ImgOut, OriImg->at<Vec3b>(Point(x, y)));

		namedWindow("��ɫ���ҽ��", WINDOW_AUTOSIZE);
		imshow("��ɫ���ҽ��", *ImgOut);

		break;
	default:
		break;
	}
}

int main()
{
	//D:\\myPictures\\��ͷ��.jpg
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\ColorCmp\\1.jpg");
	Mat ImgOut(OriImg.size(), CV_8UC1);
	
	namedWindow("Դͼ��", WINDOW_AUTOSIZE);
	imshow("Դͼ��", OriImg);

	MouseMsg = { &OriImg, &ImgOut };	//�������ݣ�ԭʼͼ�������ͼ��

	//ע�������Ӧ����
	setMouseCallback("Դͼ��", MouseEvent, reinterpret_cast<void*>(&MouseMsg));

	waitKey(0);
	return 0;
}