// bgFaceDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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

int get_face_pic_from_camera()
{
	// ����
	cv::CascadeClassifier cascade;
	cascade.load("E:\\OpenSource\\OpenCV\\opencv\\data\\lbpcascades\\lbpcascade_frontalface.xml");

	// �����ǲɼ�����
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
	// ��ȡ��ǩ�ļ���������Կ��ǽ��뵽mongoDB������HBase + solr�����ȣ�֧�ִ����ݣ��ڶ����洢�ٶȿ�
	base::FilePath faces_db_path = base::FilePath::FromUTF8Unsafe("E:\\FaceRecogitionDB\\att_faces\\labels.txt");

	// ���ȹ��������б�����֮
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

	// ���������������ͼ��Ͷ�Ӧ�ı�ǩ
	std::vector<cv::Mat> images;
	std::vector<int> labels;

	// ��ȡ���ݣ�����ļ����Ϸ�
	try 
	{
		read_csv(faces_db_path.AsUTF8Unsafe(), images, labels);
	}
	catch (cv::Exception &e)
	{
		std::cerr<<"Error opening file \""<<faces_db_path.AsUTF8Unsafe().c_str()<<"\". Reason: "<<e.msg<<std::endl;
		return -1;
	}

	// ��������û��ͼƬ�����˳�
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��
	std::cout<<"�����б�"<<std::endl;
	std::cout<<"1. ʹ������ͷ�ɼ�������"<<std::endl;
	std::cout<<"2. ʶ��ѧϰ"<<std::endl;
	std::cout<<"3. ʶ������"<<std::endl;

	char result[4096] = {0};
	std::cin.clear();
	std::cin>>result;

	if (_stricmp(result, "1") == 0)
	{
		// �����������ͷ�ɼ���������
		get_face_pic_from_camera();
	}
	else if (_stricmp(result, "2") == 0)
	{
		// �������ʶ��ѧϰ����
		face_recognition_trainning_demo(cmdline);
	}
	else if (_stricmp(result, "3") == 0)
	{
		// �����������ʶ����
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

