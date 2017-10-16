// AudioSampleFromMic.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��
	avcodec_register_all();
	av_register_all();
	avformat_network_init();

	avdevice_register_all();

	// ����ö�ٳ����п���������Ƶ�ɼ����豸
	AVInputFormat *input_audio_format = nullptr;
	
	while (input_audio_format = av_input_audio_device_next(input_audio_format) != nullptr)
	{

	}

	return 0;
}

