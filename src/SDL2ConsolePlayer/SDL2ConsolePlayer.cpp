// SDL2ConsolePlayer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "SDL2Player.h"


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
	SDL2Player player;
	int errCode = player.Init("D:\\V21205-124737.avi");

	message_loop.Run();

	return 0;
}
