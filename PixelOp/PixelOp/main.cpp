#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	//������ CodePart0 Ϊ ��ȡͼ���ļ���Ԥ���� ����
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\PixelOp\\1.jpg", IMREAD_GRAYSCALE);

	namedWindow("Input Img", WINDOW_AUTOSIZE);
	imshow("Input Img", OriImg);

	const int OriChannels = OriImg.channels();	//����ͼ���ͨ���� Grey���ҽף�Ϊ��ͨ����BGR��ȫ�ʣ�Ϊ��ͨ����BGRA����͸��ȫ�ʣ�Ϊ��ͨ��

	const int ImgCpix = OriImg.cols;	//���Ϊ����������
	const int ImgRpix = OriImg.rows;	//�߶�Ϊͼ������������	
	//CodePart0

	//�������� ������ֵ���ʣ�
	{
		int n = 32768;	//�����Ŀ
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());
		OriImg.copyTo(ImgOut);	//��ȡԴͼ�񸱱�

		srand(time(NULL));
		for (int i = 0; i < n; i++)
		{
			int R = (rand() % (ImgRpix));
			int C = (rand() % (ImgCpix));

			if (ImgOut.type() == CV_8UC1)
			{
				ImgOut.at<uchar>(R, C) = 255;
			}
			else if (OriImg.type() == CV_8UC3)
			{
				ImgOut.at<Vec3b>(R, C) = { 255, 255, 255 };
			}
			else
			{
				printf("Unknown Image Type...");
				return -1;
			}
		}

		namedWindow("����������", WINDOW_AUTOSIZE);
		imshow("����������", ImgOut);
	}

	//ͼ���ɫ �����ر�����
	{
		int n = 32;	//��ɫָ����(256/n)^channel ɫ��
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());
		OriImg.copyTo(ImgOut);	//��ȡԴͼ�񸱱�

		int imgR = ImgRpix;
		int imgC = ImgCpix * OriChannels;

		for (int R = 0; R < imgR; R++)
		{
			uchar* CurrentR = ImgOut.ptr<uchar>(R);

			for (int Cp = 0; Cp < imgC; Cp++)
			{
				CurrentR[Cp] = CurrentR[Cp] - CurrentR[Cp] % n + n / 2;
				
			}
		}

		namedWindow("��ɫ���", WINDOW_AUTOSIZE);
		imshow("��ɫ���", ImgOut);
	}

	//ͼ���� ����Ĥ������
	{
		/*
		//������ CodePart1 Ϊ ͼ���� ����
		int imgR = ImgRpix;
		int imgC = ImgCpix * OriChannels;
		int OffsetR = 1;			//��ƫ��
		int OffsetC = OriChannels;	//��ƫ��
		Mat ImgOut1 = Mat(OriImg.size(), OriImg.type());

		for (int i = OffsetR; i < imgR - OffsetR; i++)
		{
			const uchar* In_PreviousR = OriImg.ptr<uchar>(i - 1);
			const uchar* In_CurrentR = OriImg.ptr<uchar>(i);
			const uchar* In_NextR = OriImg.ptr<uchar>(i + 1);

			uchar* Out_CurrentR = ImgOut1.ptr<uchar>(i);

			for (int j = OffsetC; j < imgC - OffsetC; j++)
			{
				Out_CurrentR[j] = saturate_cast<uchar>(-3 * In_CurrentR[j] + (In_PreviousR[j] + In_NextR[j] + In_CurrentR[j - OriChannels] + In_CurrentR[j + OriChannels]));
				//��Ҫʹ��saturate_cast�������޶�������ͨ��ֵ��0~255֮��
			}
		}

		namedWindow("Output1 Img", WINDOW_AUTOSIZE);
		imshow("Output1 Img", ImgOut1);
		//CodePart1
		*/

		//���ϵȼ��������� CodePart2 �Ĵ���
		Mat ImgOut2 = Mat(OriImg.size(), OriImg.type());
		Mat HighComp = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
		filter2D(OriImg, ImgOut2, OriImg.depth(), HighComp);	//��Ĥ������

		namedWindow("�����", WINDOW_AUTOSIZE);
		imshow("�����", ImgOut2);
		//CodePart2
	}

	waitKey(0);
	return 0;
}