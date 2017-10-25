// SDL2ConsolePlayWithMultiScreen.cpp : 定义控制台应用程序的入口点。
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

	// 这里先测试单屏播放，如果单屏播放没问题的话开始测试4屏播放

	SDL2PlayerEx player;
	int errCode = player.Init(848, 480, SUB_SCREEN_TYPE);

	player.PlayOnSubScreen(url.c_str(), 1);

	// 实测发现多屏播放时，闪屏特别厉害，并且播放时间也没有保障
	// 0号索引其他位置都会出现渲染画面不全的情况

	//for (int index = 0; index < (int)SUB_SCREEN_TYPE; ++index)
	//{
	//	char path[4096] = {0};
	//	sprintf_s(path, 4096, "D:\\test\\%d.mp4", index);
	//	errCode = player.PlayOnSubScreen(path);
	//}

	message_loop.Run();

	return 0;
}

