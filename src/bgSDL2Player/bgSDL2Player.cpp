// bgSDL2Player.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"

#include "bgMediaDecoder/bgMediaDecoder.h"
#include "bgMediaPlayerEx/SDL2Player.h"

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	base::AtExitManager at_exit_manager;
	base::MessageLoop message_loop;

	LPTSTR cmdline = GetCommandLine();
	CommandLine commandline = CommandLine::FromString(cmdline);
	commandline.Init(argc, nullptr);

	std::string width_str;
	std::string height_str;
	std::string media_url;

	if (commandline.HasSwitch("url"))
		media_url = commandline.GetSwitchValueASCII("url");
	else
	{
		// 没有目标播放地址
		std::cout<<"缺少开关参数\"--url=...\""<<std::endl;
		system("pause");
		return 0;
	}

	if (commandline.HasSwitch("width"))
		width_str = commandline.GetSwitchValueASCII("width");
	else
		width_str = "800";

	if (commandline.HasSwitch("height"))
		height_str = commandline.GetSwitchValueASCII("height");
	else
		height_str = "600";

	int w = atoi(width_str.c_str());
	int h = atoi(height_str.c_str());

	// 声明播放器
	SDL2Player player;
	player.Initialize("bgSDL2Player", w, h);
	player.Play(media_url.c_str());

	message_loop.Run();

	return 0;
}

