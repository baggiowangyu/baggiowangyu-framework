// bgFaceDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/file_util.h"
#include "base/stringprintf.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/face/facerec.hpp"
#include "opencv2/face.hpp"

#include <iostream>


int face_detection_demo(CommandLine cmdline)
{
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

int get_face_pic_from_camera()
{
	// 首先
	cv::CascadeClassifier cascade;
	cascade.load("E:\\OpenSource\\OpenCV\\opencv\\data\\lbpcascades\\lbpcascade_frontalface.xml");

	// 这里是采集程序
	cv::VideoCapture cap;
	cap.open(0);
	cv::Mat frame;
	long long pic_num = 1;

	while (1)
	{
		cap>>frame;

		std::vector<cv::Rect> faces;
		cv::Mat frame_gray;
		cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

		cascade.detectMultiScale(frame_gray, faces, 1.1, 4, 0, cv::Size(100, 100), cv::Size(500, 500));

		for (size_t index = 0; index < faces.size(); ++index)
		{
			cv::rectangle(frame, faces[index], cv::Scalar(255, 0, 0), 2, 8, 0);
		}

		if (faces.size() == 1)
		{
			cv::Mat faceROI = frame_gray(faces[0]);
			cv::Mat myFace;
			cv::resize(faceROI, myFace, cv::Size(92, 112));
			cv::putText(frame, std::to_string(pic_num), faces[0].tl(), 3, 1.2, (0, 0, 255), 2, cv::LINE_AA);

			std::string filename = cv::format("E:\\FaceRecogitionDB\\att_faces\\s42\\pic%d.jpg", pic_num);
			imwrite(filename, myFace);
			imshow(filename, myFace);
			cv::waitKey(500);
			cv::destroyWindow(filename);
			pic_num++;
			if (pic_num == 11)
			{				
				return 0;
			}
		}

		imshow("frame", frame);
		cv::waitKey(100);
	}

	return 0;
}

void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';')
{
	std::ifstream file(filename.c_str(), std::ifstream::in);
	if (!file)
	{
		std::string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}

	std::string line, path, classlabel;
	while (std::getline(file, line))
	{
		std::stringstream liness(line);
		std::getline(liness, path, separator);
		std::getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty())
		{
			images.push_back(cv::imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

int face_recognition_trainning_demo(CommandLine cmdline)
{
	// 读取标签文件，后面可以考虑接入到mongoDB，或者HBase + solr，首先，支持大数据，第二，存储速度快
	base::FilePath faces_db_path = base::FilePath::FromUTF8Unsafe("E:\\FaceRecogitionDB\\att_faces\\labels.txt");

	// 首先构建特征列表，遍历之
	base::FilePath faces_root_dir = base::FilePath::FromUTF8Unsafe("E:\\FaceRecogitionDB\\att_faces");

	int face_index = 0;
	file_util::FileEnumerator file_enumerator(faces_root_dir, false, file_util::FileEnumerator::DIRECTORIES);
	do 
	{
		base::FilePath sub_path = file_enumerator.Next();

		if (sub_path.empty())
			break;

		base::FilePath face_root_dir = sub_path;
		file_util::FileEnumerator face_enumerator(face_root_dir, true, file_util::FileEnumerator::FILES);

		do 
		{
			base::FilePath face_file = face_enumerator.Next();

			if (face_file.empty())
				break;

			std::string data = base::StringPrintf("%s;%d\r\n", face_file.AsUTF8Unsafe().c_str(), face_index);

			if (file_util::PathExists(faces_db_path))
				file_util::AppendToFile(faces_db_path, data.c_str(), data.length());
			else
				file_util::WriteFile(faces_db_path, data.c_str(), data.length());

		} while (true);

		++face_index;

	} while (true);

	// 用两个容器来存放图像和对应的标签
	std::vector<cv::Mat> images;
	std::vector<int> labels;

	// 读取数据，如果文件不合法
	try 
	{
		read_csv(faces_db_path.AsUTF8Unsafe(), images, labels);
	}
	catch (cv::Exception &e)
	{
		std::cerr<<"Error opening file \""<<faces_db_path.AsUTF8Unsafe().c_str()<<"\". Reason: "<<e.msg<<std::endl;
		return -1;
	}

	// 如果结果中没有图片，则退出
	if (images.size() <= 1)
	{
		std::string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(CV_StsError, error_message);
	}

	// 
	cv::Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];

	images.pop_back();
	labels.pop_back();

	cv::Ptr<cv::face::BasicFaceRecognizer> model = cv::createEigenFaceRecognizer();
	model->train(images, labels);
	model->save("MyFacePCAModel.xml");

	cv::Ptr<cv::face::BasicFaceRecognizer> model1 = createFisherFaceRecongizer();

	return 0;
}

int face_recognition_demo(CommandLine cmdline)
{
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
	std::cout<<"功能列表："<<std::endl;
	std::cout<<"1. 使用摄像头采集样本："<<std::endl;
	std::cout<<"2. 识别学习"<<std::endl;
	std::cout<<"3. 识别人脸"<<std::endl;

	char result[4096] = {0};
	std::cin.clear();
	std::cin>>result;

	if (_stricmp(result, "1") == 0)
	{
		// 这里进入摄像头采集人脸范例
		get_face_pic_from_camera();
	}
	else if (_stricmp(result, "2") == 0)
	{
		// 这里进入识别学习范例
		face_recognition_trainning_demo(cmdline);
	}
	else if (_stricmp(result, "3") == 0)
	{
		// 这里进入人脸识别范例
	}
	else if (_stricmp(result, "4") == 0)
	{
	}
	else
	{
	}

	int errCode = 0;
	//errCode = face_detection_demo(cmdline);
	//errCode = face_recognition_demo(cmdline);

	return 0;
}

