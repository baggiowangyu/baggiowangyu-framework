
// QQFindCanceledImages.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQQFindCanceledImagesApp:
// �йش����ʵ�֣������ QQFindCanceledImages.cpp
//

class CQQFindCanceledImagesApp : public CWinApp
{
public:
	CQQFindCanceledImagesApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CQQFindCanceledImagesApp theApp;