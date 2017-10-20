// exe_demo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��

	// 1. ��ƽ̨�Ķ��߳�
	//base::Thread _thread("thread_name");
	//_thread.Start();
	//_thread.message_loop()->PostTask(FROM_HERE, base::Bind(&ThreadWork_1));
	//_thread.message_loop()->PostTask(FROM_HERE, base::Bind(&ThreadWork_2, 10));

	// 2. ��ƽ̨�Ĺ�����
	base::WorkerPool::PostTask(FROM_HERE, base::Bind(&ThreadWork_1), false);
	base::WorkerPool::PostTask(FROM_HERE, base::Bind(&ThreadWork_2, 20), false);

	// 3. ��ƽ̨�Ŀ����,base::NativeLibrary

	system("pause");
	return 0;
}

