// SDL2ConsolePlayer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "SDL2Player.h"
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

	if (!cmdline.HasSwitch("url"))
	{
		std::cout<<"not find param \"--url=...\""<<std::endl;
		system("pause");
		return 0;
	}
	std::string url = cmdline.GetSwitchValueASCII("url");

	// ���Ƚ�����Ƶ
	SDL2Player player;
	player.Play(url.c_str());

	//message_loop.Run();

	return 0;
}

