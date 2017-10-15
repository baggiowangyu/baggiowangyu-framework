// bgMediaDecoderDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ����Ӧ����һ�������������������������Ƶ�����±���󱣴�
};

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
	char *url = "D:\\V21205-124737.avi";

	TestNotify notifer;
	bgMediaDecoderV3 decoder((bgMediaDecoderV3Notify*)&notifer);
	decoder.OpenMedia(url);

	message_loop.Run();

	return 0;
}

