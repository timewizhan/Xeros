
// CXerosApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "XerosDlg.h"

// CXerosApp:
// See XerosApp.cpp for the implementation of this class
//

class CXerosApp : public CWinApp
{
	CXerosDlg *m_pXerosDlg;
public:
	CXerosApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CXerosApp theApp;