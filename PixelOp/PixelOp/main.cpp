#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	//������ CodePart0 Ϊ ��ȡͼ���ļ���Ԥ���� ����
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\PixelOp\\1.jpg");

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
		/*
		//������ CodePart1 Ϊ ͼ���� ����
		int n = 32;	//��ɫָ��������(256/n)^channel ɫ��
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());
		OriImg.copyTo(ImgOut);	//��ȡԴͼ�񸱱�

		int imgR = ImgRpix;
		int imgC = ImgCpix * OriChannels;

		for (int R = 0; R < imgR; R++)
		{
			uchar* CurrentR = ImgOut.ptr<uchar>(R);

			for (int Cp = 0; Cp < imgC; Cp++)
			{
				CurrentR[Cp] = CurrentR[Cp] / n * n + n / 2;
			}
		}

		namedWindow("��ɫ���", WINDOW_AUTOSIZE);
		imshow("��ɫ���", ImgOut);
		//CodePart1
		*/

		//���ϵȼ��������� CodePart2 �Ĵ���
		int div = 128;	//��ɫָ��������(256/div)^channel ɫ��
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());	//�����Դͼ��һ����С��ͼ�����

		int n = static_cast<int>(log(static_cast<double>(div) / log(2.0) + 0.5));

		uchar mask = 0xFF << n; //��ɫ����
		uchar div2 = div >> 1;

		//���������
		MatIterator_<Vec3b> NowPix_ori = OriImg.begin<Vec3b>();
		MatIterator_<Vec3b> EndPix_ori = OriImg.end<Vec3b>();

		MatIterator_<Vec3b> NowPix_out = ImgOut.begin<Vec3b>();

		for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)	//�����ر���
		{
			(*NowPix_out)[0] = (*NowPix_ori)[0] / div * div + div2;
			(*NowPix_out)[1] = (*NowPix_ori)[1] / div * div + div2;
			(*NowPix_out)[2] = (*NowPix_ori)[2] / div * div + div2;
		}

		namedWindow("��ɫ���", WINDOW_AUTOSIZE);
		imshow("��ɫ���", ImgOut);
		//CodePart2
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