// RealMediaDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "url/gurl.h"

#include "../bgNetwork/bgNetworkHttp.h"


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
	net::HttpRequestInfo request;
	request.url = GURL("http://localhost/start/1.html");
	request.method = "GET";
	//request.extra_headers.SetHeaderIfMissing("", "");

	net::RequestPriority priority;
	net::HttpCache http_cache;
	bgNetworkHttp http(DEFAULT_PRIORITY, );

	return 0;
}
