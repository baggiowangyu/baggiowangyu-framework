// RealMediaDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��
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

	// �������������
	errCode = http.GetResponseRawData(request_handle_, &raw_buffer, &len);
	OutputDebugStringA((LPCSTR)raw_buffer);

	return 0;
}

