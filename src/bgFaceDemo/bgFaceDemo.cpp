// bgFaceDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	// 初始化程序环境
	base::AtExitManager at_exit_manager;	// 程序退出管理器，必须在入口函数声明
	base::MessageLoop message_loop;			// 程序消息循环，必须在入口函数声明

	// 托管命令行参数
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// 接下来可以开展核心业务
	if (!cmdline.HasSwitch("img"))
	{
		std::cout<<"没有发现图片路径参数！"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("face_data"))
	{
		std::cout<<"没有发现人脸特征参数！"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("eyes_data"))
	{
		std::cout<<"没有发现人眼特征参数！"<<std::endl;
		system("pause");
		return 0;
	}

	std::string img_path = cmdline.GetSwitchValueASCII("img");
	std::string face_factor = cmdline.GetSwitchValueASCII("face_data");
	std::string eyes_factor = cmdline.GetSwitchValueASCII("eyes_data");


	// 首先加载图片
	// 这里我们可以加载文件
	// 加载从视频中提取出来的视频帧
	// 还可以是摄像头中提取的视频帧
	cv::Mat image_frame = cv::imread(img_path.c_str());

	// 加载特征值
	cv::CascadeClassifier face_cascade;	// 人脸分类器
	bool result = face_cascade.load(face_factor.c_str());
	if (!result)
	{
		std::cout<<"加载人脸特征参数失败！"<<std::endl;
		system("pause");
		return 0;
	}

	cv::CascadeClassifier eyes_cascade;	// 人眼分类器
	result = eyes_cascade.load(eyes_factor.c_str());
	if (!result)
	{
		std::cout<<"加载人眼特征参数失败！"<<std::endl;
		system("pause");
		return 0;
	}

	// 接下来开始检测
	cv::Mat image_frame_gray;
	cv::cvtColor(image_frame, image_frame_gray, cv::COLOR_BGR2GRAY);

	// 特征分为HAAR特征和LBP特征
	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(image_frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10), cv::Size(800, 800));

	for (size_t index = 0; index < faces.size(); ++index)
	{
		// 在区域上画框
		cv::rectangle(image_frame, faces[index], cv::Scalar(255, 0, 0), 2, 8, 0);

		// 以当前得到的人脸区域为范围，寻找人眼，并画框
		cv::Mat faceROI = image_frame_gray(faces[index]);
		std::vector<cv::Rect> eyes;

		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 1, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10));

		for (size_t j = 0; j < eyes.size(); ++j)
		{
			cv::Rect rect(faces[index].x + eyes[j].x, faces[index].y + eyes[j].y, eyes[j].width, eyes[j].height);

			cv::rectangle(image_frame, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
		}
	}

	// 显示图像，这里显示出问题了
	cv::namedWindow("First face detection ...");
	cv::imshow("First face detection ...", image_frame);

	cv::waitKey();

	return 0;
}

