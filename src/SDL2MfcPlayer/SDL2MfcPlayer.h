
// SDL2MfcPlayer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSDL2MfcPlayerApp:
// �йش����ʵ�֣������ SDL2MfcPlayer.cpp
//

class CSDL2MfcPlayerApp : public CWinApp
{
public:
	CSDL2MfcPlayerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSDL2MfcPlayerApp theApp;