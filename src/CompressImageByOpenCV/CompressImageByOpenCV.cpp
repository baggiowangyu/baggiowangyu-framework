// CompressImageByOpenCV.cpp : 定义控制台应用程序的入口点。
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


int test_1(CommandLine cmdline)
{
	std::string src_img = cmdline.GetSwitchValueASCII("imgsrc");
	std::string tar_img = cmdline.GetSwitchValueASCII("imgtar");

	// 加载图片
	IplImage *img = cvLoadImage(src_img.c_str());
	if (!img)
	{
		std::cout<<""<<std::endl;
		return -1;
	}

	//IplImage *new_img = cvCreateImage(cvSize(192, 108), IPL_DEPTH_8U, 3);
	IplImage *new_img = cvCreateImage(cvSize(128, 72), IPL_DEPTH_8U, 3);

	cvResize(img, new_img, CV_INTER_LINEAR);

	int errCode = cvSaveImage(tar_img.c_str(), new_img);

	return errCode;
}

int test_2(CommandLine cmdline)
{
	std::string src_img = cmdline.GetSwitchValueASCII("imgsrc");
	std::string tar_img = cmdline.GetSwitchValueASCII("imgtar");

	cv::Mat img = cv::imread(src_img.c_str());

	cv::Mat new_img;
	resize(img, new_img, cv::Size(192, 108));

	std::vector<int> p;
	p.push_back(1);
	cv::imwrite(tar_img.c_str(), new_img, p);

	return 0;
}

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
	if (!cmdline.HasSwitch("imgsrc"))
	{
		std::cout<<"没有发现图片路径参数！"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("imgtar"))
	{
		std::cout<<"没有发现图片路径参数！"<<std::endl;
		system("pause");
		return 0;
	}

	test_1(cmdline);

	return 0;
}

