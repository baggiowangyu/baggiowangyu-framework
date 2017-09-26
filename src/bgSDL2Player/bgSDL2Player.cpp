// bgSDL2Player.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"

#include "bgMediaDecoder/bgMediaDecoder.h"
#include "bgMediaPlayerEx/SDL2Player.h"


int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager at_exit_manager;
	base::MessageLoop message_loop;

	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ����������
	SDL2Player player;
	player.Initialize("123", 1920, 1080);
	player.Play("D:\\3.mp4");

	message_loop.Run();

	return 0;
}

