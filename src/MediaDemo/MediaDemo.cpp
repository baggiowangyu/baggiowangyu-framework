// MediaDemo.cpp : 定义控制台应用程序的入口点。
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
	// 寻找第一个支持的输入格式
	std::cout<<"枚举所有支持的输入格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输入格式";
	AVInputFormat *input_format = av_iformat_next(nullptr);

	while(input_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVInputFormat::name : "<<input_format->name<<std::endl;
		LOG(INFO)<<"AVInputFormat::name : "<<input_format->name;
		std::cout<<"AVInputFormat::long_name : "<<input_format->long_name<<std::endl;
		LOG(INFO)<<"AVInputFormat::long_name : "<<input_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
		input_format = av_iformat_next(input_format);
	}

	std::cout<<"枚举所有支持的输出格式"<<std::endl;
	LOG(INFO)<<"枚举所有支持的输出格式";
	AVOutputFormat *output_format = av_oformat_next(nullptr);

	while(output_format != nullptr)
	{
		// 输出相关信息
		std::cout<<"AVOutputFormat::name : "<<output_format->name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::name : "<<output_format->name;
		std::cout<<"AVOutputFormat::long_name : "<<output_format->long_name<<std::endl;
		LOG(INFO)<<"AVOutputFormat::long_name : "<<output_format->long_name;
		std::cout<<"-----------------------------------"<<std::endl;
		LOG(INFO)<<"-----------------------------------";

		// 寻找下一个支持的输入格式
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
		std::cout<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"打开媒体文件 "<<media_path_.c_str()<<" 失败！错误码："<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(input_context, NULL);
	if (errCode < 0)
	{
		std::cout<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode<<std::endl;
		LOG(ERROR)<<"查询媒体文件 "<<media_path_.c_str()<<" 媒体信息失败！错误码："<<errCode;
		return ;
	}

	std::cout<<"======= 媒体信息 ======="<<std::endl;
	int number_of_streams = input_context->nb_streams;
	for (int index = 0; index < number_of_streams; ++index)
	{
		AVStream *stream = input_context->streams[index];
		AVCodecContext *avcodec_context = stream->codec;

		std::cout<<"媒体流 - "<<index<<" 信息："<<std::endl;
		std::cout<<"\tCodec 名称："<<avcodec_context->codec_name<<"\n比特率："<<avcodec_context->bit_rate<<
			"\t图像尺寸（宽*高）："<<avcodec_context->width<<"*"<<avcodec_context->height<<"\t流尺寸（宽*高）"<<
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

	// 不存在这个目录，则创建之
	if (!file_util::PathExists(logging_file))
		result = file_util::CreateDirectoryW(logging_file);

	// 得到今天的日期
	base::Time today = base::Time::Now();

	logging_file = logging_file.Append(_T("log.txt"));

	// 初始化日志
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
		std::cout<<"========FFmepg学习教程========"<<std::endl;
		std::cout<<"1. 枚举当前支持的输入输出格式"<<std::endl;
		std::cout<<"2. 查询视频文件基本信息"<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<""<<std::endl;
		std::cout<<"请输入选项：";

		char input_data[512] = {0};
		std::cin.clear();
		std::cin>>input_data;

		if (_stricmp(input_data, "1") == 0){
			EnumInputOutputFormat();
		} else if (_stricmp(input_data, "2") == 0){
			std::cout<<"请输入视频文件路径";
			char media_path[512] = {0};
			std::cin.clear();
			std::cin>>media_path;
		}
	}
	

	return 0;
}

