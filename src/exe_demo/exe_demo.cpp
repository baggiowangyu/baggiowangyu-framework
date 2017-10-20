// exe_demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/string_util.h"
#include "base/values.h"
#include "base/bind.h"
#include "base/native_library.h"
#include "base/threading/thread.h"
#include "base/threading/worker_pool.h"

#include <iostream>

void ThreadWork_1()
{
	for (int i = 0; i < 10; ++i)
	{
		std::cout<<"This is ThreadWork_1()"<<std::endl;
	}
}

void ThreadWork_2(int index)
{
	for (int i = 0; i < index; ++i)
	{
		std::cout<<"This is ThreadWork_2() >>> "<<i<<std::endl;
	}
}


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

	// 1. 跨平台的多线程
	//base::Thread _thread("thread_name");
	//_thread.Start();
	//_thread.message_loop()->PostTask(FROM_HERE, base::Bind(&ThreadWork_1));
	//_thread.message_loop()->PostTask(FROM_HERE, base::Bind(&ThreadWork_2, 10));

	// 2. 跨平台的工作池
	base::WorkerPool::PostTask(FROM_HERE, base::Bind(&ThreadWork_1), false);
	base::WorkerPool::PostTask(FROM_HERE, base::Bind(&ThreadWork_2, 20), false);

	// 3. 跨平台的库加载,base::NativeLibrary

	system("pause");
	return 0;
}

