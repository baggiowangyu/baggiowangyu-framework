// AudioSampleFromMic.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
	};
#endif

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
	avcodec_register_all();
	av_register_all();
	avformat_network_init();

	avdevice_register_all();

	// 首先枚举出所有可以用于音频采集的设备
	AVInputFormat *input_audio_format = nullptr;
	
	while (input_audio_format = av_input_audio_device_next(input_audio_format) != nullptr)
	{

	}

	return 0;
}

