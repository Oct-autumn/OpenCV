#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	//以下至 CodePart0 为 读取图像文件与预处理 操作
	Mat OriImg = imread("D:\\MyDocuments\\GitHub\\OpenCV\\PixelOp\\1.jpg");

	namedWindow("Input Img", WINDOW_AUTOSIZE);
	imshow("Input Img", OriImg);

	const int OriChannels = OriImg.channels();	//输入图像的通道数 Grey（灰阶）为单通道、BGR（全彩）为三通道、BGRA（带透明全彩）为四通道

	const int ImgCpix = OriImg.cols;	//宽度为横向像素数
	const int ImgRpix = OriImg.rows;	//高度为图像纵向像素数


	//CodePart0

	//椒盐噪声 （像素值访问）
	{
		int n = 32768;	//噪点数目
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());
		OriImg.copyTo(ImgOut);	//获取源图像副本

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

		namedWindow("椒盐噪点输出", WINDOW_AUTOSIZE);
		imshow("椒盐噪点输出", ImgOut);
	}

	//图像减色 （像素遍历）
	{
		/*
		//以下至 CodePart1 为 图像锐化 操作
		int n = 32;	//减色指数（减至(256/n)^channel 色）
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());
		OriImg.copyTo(ImgOut);	//获取源图像副本

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

		namedWindow("减色输出", WINDOW_AUTOSIZE);
		imshow("减色输出", ImgOut);
		//CodePart1
		*/

		//以上等价于以下至 CodePart2 的代码
		int div = 128;	//减色指数（减至(256/div)^channel 色）
		Mat ImgOut = Mat(OriImg.size(), OriImg.type());	//定义和源图像一样大小的图像矩阵

		int n = static_cast<int>(log(static_cast<double>(div) / log(2.0) + 0.5));

		uchar mask = 0xFF << n; //减色掩码
		uchar div2 = div >> 1;

		//定义迭代器
		MatIterator_<Vec3b> NowPix_ori = OriImg.begin<Vec3b>();
		MatIterator_<Vec3b> EndPix_ori = OriImg.end<Vec3b>();

		MatIterator_<Vec3b> NowPix_out = ImgOut.begin<Vec3b>();

		for (; NowPix_ori != EndPix_ori; NowPix_ori++, NowPix_out++)	//逐像素遍历
		{
			(*NowPix_out)[0] = (*NowPix_ori)[0] / div * div + div2;
			(*NowPix_out)[1] = (*NowPix_ori)[1] / div * div + div2;
			(*NowPix_out)[2] = (*NowPix_ori)[2] / div * div + div2;
		}

		namedWindow("减色输出", WINDOW_AUTOSIZE);
		imshow("减色输出", ImgOut);
		//CodePart2
	}

	//图像锐化 （掩膜操作） 
	{
		/*
		//以下至 CodePart1 为 图像锐化 操作
		int imgR = ImgRpix;
		int imgC = ImgCpix * OriChannels;
		int OffsetR = 1;			//行偏置
		int OffsetC = OriChannels;	//列偏置
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
				//需要使用saturate_cast函数来限定单像素通道值在0~255之间
			}
		}

		namedWindow("Output1 Img", WINDOW_AUTOSIZE);
		imshow("Output1 Img", ImgOut1);
		//CodePart1
		*/

		//以上等价于以下至 CodePart2 的代码
		Mat ImgOut2 = Mat(OriImg.size(), OriImg.type());
		Mat HighComp = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
		filter2D(OriImg, ImgOut2, OriImg.depth(), HighComp);	//掩膜处理函数

		namedWindow("锐化输出", WINDOW_AUTOSIZE);
		imshow("锐化输出", ImgOut2);
		//CodePart2
	}

	waitKey(0);
	return 0;
}