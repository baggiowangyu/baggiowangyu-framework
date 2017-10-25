// SDL2ConsolePlayWithMultiScreen.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "SDL2PlayerEx.h"
#include <iostream>

#define SUB_SCREEN_TYPE		SubScreen_Four


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

	if (!cmdline.HasSwitch("url"))
	{
		std::cout<<"not find param \"--url=...\""<<std::endl;
		system("pause");
		return 0;
	}
	std::string url = cmdline.GetSwitchValueASCII("url");

	// �����Ȳ��Ե������ţ������������û����Ļ���ʼ����4������

	SDL2PlayerEx player;
	int errCode = player.Init(848, 480, SUB_SCREEN_TYPE);

	player.PlayOnSubScreen(url.c_str(), 1);

	// ʵ�ⷢ�ֶ�������ʱ�������ر����������Ҳ���ʱ��Ҳû�б���
	// 0����������λ�ö��������Ⱦ���治ȫ�����

	//for (int index = 0; index < (int)SUB_SCREEN_TYPE; ++index)
	//{
	//	char path[4096] = {0};
	//	sprintf_s(path, 4096, "D:\\test\\%d.mp4", index);
	//	errCode = player.PlayOnSubScreen(path);
	//}

	message_loop.Run();

	return 0;
}

