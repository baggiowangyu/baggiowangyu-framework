// RealMediaDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "url/gurl.h"

#include "../bgNetwork/bgNetworkHttp.h"


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
	net::HttpRequestInfo request;
	request.url = GURL("http://localhost/start/1.html");
	request.method = "GET";
	//request.extra_headers.SetHeaderIfMissing("", "");

	net::RequestPriority priority;
	bgNetworkHttp http(DEFAULT_PRIORITY, );
	h

	return 0;
}

