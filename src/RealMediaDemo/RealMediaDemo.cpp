// RealMediaDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#include "ext/libcurl/include/curl/curl.h"
#include "bgNetwork/bgNetworkHttp_V3.h"

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
	bgNetworkHttpV3 http;
	int errCode = http.ConnectToServer("www.163.com");

	http.AddExternHeader("Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
	http.AddExternHeader("Accept-Language:zh-CN,zh;q=0.8");
	http.AddExternHeader("Connection:keep-alive");
	http.AddExternHeader("Host:www.163.com");
	http.AddExternHeader("Accept-Encoding: gzip, deflate, sdch");

	HINTERNET request_handle_ = nullptr;
	errCode = http.SendGetRequest("/", &request_handle_);

	TCHAR *header = nullptr;
	errCode = http.GetResponseRawHeader(request_handle_, &header);
	OutputDebugString(header);

	DWORD index = 0;
	TCHAR *cookie = nullptr;
	errCode = http.GetResponseCookie(request_handle_, &cookie, &index);
	OutputDebugString(cookie);

	DWORD len = 0;
	errCode = http.GetResponseContentLength(request_handle_, &len, &index);
	

	unsigned char *raw_buffer = new unsigned char[len];
	memset(raw_buffer, 0, len);

	// 这个函数有问题
	errCode = http.GetResponseRawData(request_handle_, &raw_buffer, &len);
	OutputDebugStringA((LPCSTR)raw_buffer);

	return 0;
}

