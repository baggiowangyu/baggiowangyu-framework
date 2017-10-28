// bgFaceDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��
	if (!cmdline.HasSwitch("img"))
	{
		std::cout<<"û�з���ͼƬ·��������"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("face_data"))
	{
		std::cout<<"û�з�����������������"<<std::endl;
		system("pause");
		return 0;
	}

	if (!cmdline.HasSwitch("eyes_data"))
	{
		std::cout<<"û�з�����������������"<<std::endl;
		system("pause");
		return 0;
	}

	std::string img_path = cmdline.GetSwitchValueASCII("img");
	std::string face_factor = cmdline.GetSwitchValueASCII("face_data");
	std::string eyes_factor = cmdline.GetSwitchValueASCII("eyes_data");


	// ���ȼ���ͼƬ
	// �������ǿ��Լ����ļ�
	// ���ش���Ƶ����ȡ��������Ƶ֡
	// ������������ͷ����ȡ����Ƶ֡
	cv::Mat image_frame = cv::imread(img_path.c_str());

	// ��������ֵ
	cv::CascadeClassifier face_cascade;	// ����������
	bool result = face_cascade.load(face_factor.c_str());
	if (!result)
	{
		std::cout<<"����������������ʧ�ܣ�"<<std::endl;
		system("pause");
		return 0;
	}

	cv::CascadeClassifier eyes_cascade;	// ���۷�����
	result = eyes_cascade.load(eyes_factor.c_str());
	if (!result)
	{
		std::cout<<"����������������ʧ�ܣ�"<<std::endl;
		system("pause");
		return 0;
	}

	// ��������ʼ���
	cv::Mat image_frame_gray;
	cv::cvtColor(image_frame, image_frame_gray, cv::COLOR_BGR2GRAY);

	// ������ΪHAAR������LBP����
	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(image_frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10), cv::Size(800, 800));

	for (size_t index = 0; index < faces.size(); ++index)
	{
		// �������ϻ���
		cv::rectangle(image_frame, faces[index], cv::Scalar(255, 0, 0), 2, 8, 0);

		// �Ե�ǰ�õ�����������Ϊ��Χ��Ѱ�����ۣ�������
		cv::Mat faceROI = image_frame_gray(faces[index]);
		std::vector<cv::Rect> eyes;

		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 1, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10));

		for (size_t j = 0; j < eyes.size(); ++j)
		{
			cv::Rect rect(faces[index].x + eyes[j].x, faces[index].y + eyes[j].y, eyes[j].width, eyes[j].height);

			cv::rectangle(image_frame, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
		}
	}

	// ��ʾͼ��������ʾ��������
	cv::namedWindow("First face detection ...");
	cv::imshow("First face detection ...", image_frame);

	cv::waitKey();

	return 0;
}

