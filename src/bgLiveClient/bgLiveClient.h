
// bgLiveClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CbgLiveClientApp:
// �йش����ʵ�֣������ bgLiveClient.cpp
//

class CbgLiveClientApp : public CWinApp
{
public:
	CbgLiveClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CbgLiveClientApp theApp;