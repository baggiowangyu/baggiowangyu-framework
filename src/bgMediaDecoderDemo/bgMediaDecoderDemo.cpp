// bgMediaDecoderDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/string_util.h"
#include "base/values.h"
#include "base/bind.h"
#include "base/native_library.h"
#include "base/threading/thread.h"
#include "base/threading/worker_pool.h"

#include "bgMediaDecoder/bgMediaDecoder_V3.h"

#include <iostream>

class TestNotify : bgMediaDecoderV3Notify
{
public:
	TestNotify(){}
	~TestNotify(){}

public:
	virtual void StateNotify(std::string statestr, int stateindex)
	{

	}

	virtual void ErrorNotify(std::string errstr, int errcode)
	{

	}

	virtual void DecodeNotify(AVFrame *frame_data, int frame_type)
	{
		std::cout<<"recv a decode frame ..."<<std::endl;
	}

	virtual void VideoInfoNotify(MediaVideoInfo video_info)
	{
		std::cout<<"recv MediaVideoInfo ..."<<std::endl;
	}

	virtual void AudioInfoNotify()
	{

	}

public:
	// 这里应该有一个编码器，将解码出来的视音频流重新编码后保存
};

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
	char *url = "D:\\V21205-124737.avi";

	TestNotify notifer;
	bgMediaDecoderV3 decoder((bgMediaDecoderV3Notify*)&notifer);
	decoder.OpenMedia(url);

	message_loop.Run();

	return 0;
}

