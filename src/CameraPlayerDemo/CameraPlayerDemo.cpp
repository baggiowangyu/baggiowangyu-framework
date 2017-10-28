// CameraPlayerDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include <objbase.h>
#include <Strmif.h>
#include <initguid.h>
#include <uuids.h>
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

	// 先枚举当前系统里面接入的摄像头
	CoInitialize(NULL);

	ICreateDevEnum *pVideoDevEnum = NULL;
	IEnumMoniker *pVideoEnumMoniker = NULL;

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pVideoDevEnum);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = pVideoDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pVideoEnumMoniker, 0 );

	ULONG cFetched = 0;
	IMoniker *pMoniker = NULL;
	while( hr = pVideoEnumMoniker->Next( 1, &pMoniker, &cFetched ), hr == S_OK )
	{
		IPropertyBag *pPropertyBag = NULL;
		//To retrieve the friendly name of the device 
		hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pPropertyBag );
		if( SUCCEEDED(hr) )
		{
			// Get friendly name
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pPropertyBag->Read( L"FriendlyName", &var, NULL );
			if( SUCCEEDED(hr) )
			{
				char friendlyName[MAX_PATH];
				friendlyName[0] = 0;
				WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, friendlyName, 80, NULL, NULL );
				SysFreeString(var.bstrVal);

				std::cout<<friendlyName<<std::endl;

				//IBaseFilter *pFilter = NULL;
				////To create and initialize the DirectShow filter that manages the device
				//hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void **)&pFilter );
				//if( SUCCEEDED(hr) )
				//{
				//	m_VideoDevList.AddString( friendlyName );
				//	m_LogFile.WriteLogFile(friendlyName);
				//	numOfVideoDev ++;
				//}
			}
		}

		pPropertyBag->Release();
		pPropertyBag = NULL;

		pMoniker->Release();
		pMoniker = NULL;
	}

	pVideoEnumMoniker->Release();
	pVideoDevEnum->Release();

	CoUninitialize();

	// 
	av_register_all();
	avformat_network_init();
	avdevice_register_all();

	// 枚举电脑上所有的摄像头
	AVFormatContext *input_format_context = avformat_alloc_context();
	AVInputFormat *input_format = av_find_input_format("vfwcap");

	//int errCode = avformat_open_input(&input_format_context, "0", )

	return 0;
}

