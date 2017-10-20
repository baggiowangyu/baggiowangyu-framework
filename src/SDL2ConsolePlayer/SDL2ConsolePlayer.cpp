// SDL2ConsolePlayer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "SDL2Player.h"
#include <iostream>

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

	if (!cmdline.HasSwitch("url"))
	{
		std::cout<<"not find param \"--url=...\""<<std::endl;
		system("pause");
		return 0;
	}
	std::string url = cmdline.GetSwitchValueASCII("url");

	// 首先解码视频
	SDL2Player player;
	player.Play(url.c_str());

	//message_loop.Run();

	return 0;
}

