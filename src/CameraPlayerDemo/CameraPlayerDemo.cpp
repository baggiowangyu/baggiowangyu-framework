// CameraPlayerDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ�����򻷾�
	base::AtExitManager at_exit_manager;	// �����˳�����������������ں�������
	base::MessageLoop message_loop;			// ������Ϣѭ������������ں�������

	// �й������в���
	LPTSTR commandline = GetCommandLine();
	CommandLine cmdline = CommandLine::FromString(commandline);
	cmdline.Init(argc, nullptr);

	// ���������Կ�չ����ҵ��

	// ��ö�ٵ�ǰϵͳ������������ͷ
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

	// ö�ٵ��������е�����ͷ
	AVFormatContext *input_format_context = avformat_alloc_context();
	AVInputFormat *input_format = av_find_input_format("vfwcap");

	//int errCode = avformat_open_input(&input_format_context, "0", )

	return 0;
}

