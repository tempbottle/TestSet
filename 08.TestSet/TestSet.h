// TestSet.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������

// CTestSetApp:
// �йش����ʵ�֣������ TestSet.cpp
//

class CTestSetApp : public CWinApp
{
public:
	CTestSetApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��
	DECLARE_MESSAGE_MAP()
};

extern CTestSetApp theApp;
