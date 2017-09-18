// MediaDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#define __STDC_CONSTANT_MACROS

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/strings/sys_string_conversions.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include <iostream>


void EnumInputOutputFormat()
{
	// Ѱ�ҵ�һ��֧�ֵ������ʽ
	std::cout<<"ö������֧�ֵ������ʽ"<<std::endl;
	LOG(INFO)<<"ö������֧�ֵ������ʽ";
	AVInputFormat *input_format = av_iformat_next(nullptr);

	while(input_format != nullptr)
	{
		// ��������Ϣ
		std::cout<<"AVInputFormat::name : "<<input_format->name<<std::endl;
		LOG(INFO)<<"AVInputFormat::name : "<<input_format->name;
		std::cout<<"AVInputFormat::long_name : "<<input_format->long_name<<std::endl;
		LOG(INFO)<<"AVInputFormat::long_name : "<<input_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// Ѱ����һ��֧�ֵ������ʽ
		input_format = av_iformat_next(input_format);
	}

	std::cout<<"ö������֧�ֵ������ʽ"<<std::endl;
	LOG(INFO)<<"ö������֧�ֵ������ʽ";
	AVOutputFormat *output_format = av_oformat_next(nullptr);

	while(output_format != nullptr)
	{
		// ��������Ϣ
		std::cout<<"AVOutputFormat::name : "<<output_format->name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::name : "<<output_format->name;
		std::cout<<"AVOutputFormat::long_name : "<<output_format->long_name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::long_name : "<<output_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// Ѱ����һ��֧�ֵ������ʽ
		output_format = av_oformat_next(output_format);
	}
}

void GetMediaInfo(std::string media_path_)
{
	base::FilePath media_path(base::SysMultiByteToWide(media_path_, CP_ACP));

	AVFormatContext *input_context = nullptr;
	int errCode = avformat_open_input(&input_context, media_path.c_str(), nullptr, nullptr);
	if (0 != errCode)
	{
		std::cout<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ý���ļ� "<<media_path_.c_str()<<" ʧ�ܣ������룺"<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode<<std::endl;
		LOG(ERROR)<<"��ѯý���ļ� "<<media_path_.c_str()<<" ý����Ϣʧ�ܣ������룺"<<errCode;
		return ;
	}

	std::cout<<"======= ý����Ϣ ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *stream = input_context->streams[index];
		AVCodecContext *avcodec_context = stream->codec;

		std::cout<<"ý���� - "<<index<<" ��Ϣ��"<<std::endl;
		std::cout<<"\tCodec ���ƣ�"<<avcodec_context->codec_name<<"\n�����ʣ�"<<avcodec_context->bit_rate<<
			"\tͼ��ߴ磨��*�ߣ���"<<avcodec_context->width<<"*"<<avcodec_context->height<<"\t���ߴ磨��*�ߣ�"<<
			avcodec_context->coded_width<<"*"<<avcodec_context->coded_height;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager _at_exit_manager;
	base::MessageLoop _message_loop;

#ifdef WIN32
	LPCTSTR commandline = GetCommandLine();
#endif
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, NULL);

	base::FilePath current_dir;
	bool result = file_util::GetCurrentDirectory(&current_dir);
	base::FilePath logging_file = current_dir.Append(_T("log"));

	// ���������Ŀ¼���򴴽�֮
	if (!file_util::PathExists(logging_file))
		result = file_util::CreateDirectoryW(logging_file);

	// �õ����������
	base::Time today = base::Time::Now();

	logging_file = logging_file.Append(_T("log.txt"));

	// ��ʼ����־
	result = logging::InitLogging(logging_file.LossyDisplayName().c_str(), 
		logging::LOG_ONLY_TO_FILE, 
		logging::DONT_LOCK_LOG_FILE, 
		logging::APPEND_TO_OLD_LOG_FILE, 
		logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

	logging::SetMinLogLevel(logging::LOG_INFO);

	av_register_all();
	avformat_network_init();

	while (true)
	{
		std::cout<<"========FFmepgѧϰ�̳�========"<<std::endl;
		std::cout<<"1. ö�ٵ�ǰ֧�ֵ����������ʽ"<<std::endl;
		std::cout<<"2. ��ѯ��Ƶ�ļ�������Ϣ"<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<"������ѡ�";

		char input_data[512] = {0};
		std::cin.clear();
		std::cin>>input_data;

		if (_stricmp(input_data, "1") == 0){
			EnumInputOutputFormat();
		} else if (_stricmp(input_data, "2") == 0){
			std::cout<<"��������Ƶ�ļ�·��";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;
		}
	}
	

	return 0;
}

