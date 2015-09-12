
// XerosAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XerosApp.h"
#include "XerosDlg.h"
#include "afxdialogex.h"

#include "MainOperation\DataSingletonh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_TRAYICON		0x01
#define MSG_TRAYICON	WM_USER + 1

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CXerosAppDlg dialog



CXerosDlg::CXerosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXerosDlg::IDD, pParent),
	bTrayActive(FALSE),
	bDialogShow(FALSE),
	m_bServiceStart(FALSE),
	m_bGetURLs(FALSE),
	m_bStartState(TRUE),
	m_pOperation(NULL),
	m_pBrowser(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_XEROS_TRAY);
}

void CXerosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CXerosDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(MSG_TRAYICON, OnMessageFromTrayIcon)
	ON_BN_CLICKED(ID_START, &CXerosDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_HISTORY, &CXerosDlg::OnBnClickedHistory)
	ON_BN_CLICKED(ID_URLS, &CXerosDlg::OnBnClickedUrls)
	ON_BN_CLICKED(ID_CONFIGURATION, &CXerosDlg::OnBnClickedConfiguration)
	ON_BN_CLICKED(ID_ABOUT, &CXerosDlg::OnBnClickedAbout)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CXerosDlg::OnEnChangeDlgText)
	ON_COMMAND(ID_XEROS_XEROSOPEN, &CXerosDlg::OnXerosXerosopen)
	ON_COMMAND(ID_XEROS_URLS, &CXerosDlg::OnXerosUrls)
	ON_COMMAND(ID_XEROS_START, &CXerosDlg::OnXerosStart)
	ON_COMMAND(ID_XEROS_ABOUT, &CXerosDlg::OnXerosAbout)
	ON_COMMAND(ID_XEROS_CONFIGURATION, &CXerosDlg::OnXerosConfiguration)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_XEROS_CLOSE, &CXerosDlg::OnXerosClose)
	ON_UPDATE_COMMAND_UI(ID_XEROS_START, &CXerosDlg::OnUpdateXerosStart)
END_MESSAGE_MAP()


// CXerosAppDlg message handlers

BOOL CXerosDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//// Add "About..." menu item to system menu.

	//// IDM_ABOUTBOX must be in the system command range.
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	//// Set the icon for this dialog.  The framework does this automatically
	////  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_hIcon = NULL;
	NOTIFYICONDATA stTaskBaricon = { 0, };
	stTaskBaricon.cbSize = sizeof(NOTIFYICONDATA);
	stTaskBaricon.hWnd = this->GetSafeHwnd();
	stTaskBaricon.uID = ID_TRAYICON;
	stTaskBaricon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	stTaskBaricon.hIcon = AfxGetApp()->LoadIcon(IDI_TEST_ICON);
	stTaskBaricon.uCallbackMessage = MSG_TRAYICON;

	_tcscpy_s(stTaskBaricon.szTip, 6, TEXT("szTip"));
	_tcscpy_s(stTaskBaricon.szInfoTitle, 6, TEXT("Title"));
	_tcscpy_s(stTaskBaricon.szInfo, 5, TEXT("Info"));

	stTaskBaricon.uVersion = NOTIFYICON_VERSION;
	stTaskBaricon.dwInfoFlags = NIIF_USER;

	bTrayActive = TRUE;
	::Shell_NotifyIcon(NIM_ADD, &stTaskBaricon);

	/*
		initialize log 
	*/
	DWORD dwRet;
	dwRet = InitLog(E_LOG_FILE);
	if (dwRet != E_RET_SUCCESS) {
		MessageBox(L"Fail to init log");
		// Must exit xeros app
		// because this program cannot record log to monitor operation of xeros program
		OnOK();
	}

	/*
		initialize Browser 
	*/
	m_pBrowser = new CBrowser();
	if (!m_pBrowser) {
		MessageBox(L"Fail to init browser");
		OnOK();
	}
	
	// default internet type is explorer
	dwRet = m_pBrowser->InitBrowser(E_BROWSER_IE);
	if (dwRet != E_RET_SUCCESS) {
		dwRet = m_pBrowser->InitBrowser(E_BROWSER_CHROME);
		if (dwRet != E_RET_SUCCESS) {
			/*
				Cannot find internet browser
			*/
			MessageBox(L"Fail to init browser");
			OnOK();
		}
	}

	/*
		allocate memory of Main Operation class
	*/
	LPTSTR lpBuffer = NULL;
	BOOL bExist;
	GetCurrentDirectory(sizeof(lpBuffer), lpBuffer);
#ifdef _UNICODE
	std::wstring wstrFilePath = lpBuffer;
	wstrFilePath += L"/";
	wstrFilePath += DRIVER_FILE_NAME;

	bExist = PathFileExists(wstrFilePath.c_str());
	std::string strFilePath;
	strFilePath.assign(wstrFilePath.begin(), wstrFilePath.end());
#else
	std::string strFilePath;
	strFilePath = lpBuffer;
	strFilePath += L"/";
	strFilePath += DRIVER_FILE_NAME;
	bExist = PathFileExists(strFilePath.c_str());
#endif

	if (bExist) {
		m_pOperation = new COperation(strFilePath.c_str());
		if (!m_pOperation) {
			MessageBox(L"Fail to operate xeros");
			OnOK();
		}
	}
	else {
		/*
			You cannot execute this program because driver is not loaded
		*/
		MessageBox(L"File not exist");
		OnOK();
	}

	PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CXerosDlg::OnMessageFromTrayIcon(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_TRAYICON)
		if (lParam == WM_LBUTTONDOWN || lParam == WM_RBUTTONDOWN)
		{
			CPoint pos;
			CMenu m_menu;
			m_menu.LoadMenu(IDR_XEROS_TRAY);
			::GetCursorPos(&pos);
			::SetForegroundWindow(this->GetSafeHwnd());

			CMenu *pSubMenu = NULL;
			pSubMenu = m_menu.GetSubMenu(0);
		
			/*
				Start / End State
				If m_bStartState is TRUE, it's Start state.
				If m_bStartState is FALSE, it's End state.
			*/
			if (m_bStartState) {
				m_menu.ModifyMenuW(ID_XEROS_START, MF_BYCOMMAND | MF_STRING, ID_XEROS_START, TEXT("Start"));
			}
			else {
				m_menu.ModifyMenuW(ID_XEROS_START, MF_BYCOMMAND | MF_STRING, ID_XEROS_START, TEXT("End"));
			}

			/*
				Insert URL
				if URLContents have more then 1 item, add items
				otherwise, add message (nothing)
			*/
			CMenu *pURLMenu = NULL;
			pURLMenu = m_menu.GetSubMenu(0)->GetSubMenu(1);

			CDataSingleton *pDataSingleton = NULL;
			pDataSingleton = CDataSingleton::GetInstance();
			BOOL bData = TRUE;
			if (pDataSingleton == NULL) {
				bData = FALSE;
			}

			ST_RESULT_URLS stResultURLs;
			DWORD dwRet;
			dwRet = pDataSingleton->GetURLs(stResultURLs);
			if (dwRet != E_RET_SUCCESS) {
				bData = FALSE;
			}
			
			/*
				if data is not received from DataSingleton, 
				Dialog cannot input data to trayicon bar
			*/
			if (bData && stResultURLs.vecstrURLs.size() > 0) {
				pURLMenu->DeleteMenu(0, MF_BYPOSITION);

				std::vector<std::string>::size_type i;
				for (i = 0; i <stResultURLs.vecstrURLs.size(); i++) {
					std::wstring wURLContents;
					wURLContents.clear();
					wURLContents.assign(stResultURLs.vecstrURLs[i].begin(), stResultURLs.vecstrURLs[i].end());
					pURLMenu->AppendMenuW(MF_STRING, 2001 + 2 * i, wURLContents.c_str());
				}
			}
			else {
				pURLMenu->AppendMenuW(MF_STRING, 2001 + 2 * 0, TEXT("Nothing"));
			}

			BOOL bRet;
			bRet = pSubMenu->TrackPopupMenu(/*TPM_LEFTALIGN | */TPM_RIGHTBUTTON, pos.x, pos.y, this);
		}
		else if (lParam == WM_LBUTTONDBLCLK)
		{
			ShowWindow(SW_SHOW);
			//SetFocus();
		}


	return 0;
}

void CXerosDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXerosDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXerosDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXerosDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
}


void CXerosDlg::OnBnClickedHistory()
{
	// TODO: Add your control notification handler code here
}


void CXerosDlg::OnBnClickedUrls()
{
	// TODO: Add your control notification handler code here
}


void CXerosDlg::OnBnClickedConfiguration()
{
	// TODO: Add your control notification handler code here
}


void CXerosDlg::OnBnClickedAbout()
{
	// TODO: Add your control notification handler code here
}


void CXerosDlg::OnEnChangeDlgText()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CXerosDlg::OnXerosXerosopen()
{
	// TODO: Add your command handler code here
	PostMessage(WM_SHOWWINDOW, TRUE, SW_OTHERUNZOOM);
}


void CXerosDlg::OnXerosUrls()
{
	// TODO: Add your command handler code here
}


void CXerosDlg::OnXerosStart()
{
	// TODO: Add your command handler code here
}


void CXerosDlg::OnXerosAbout()
{
	// TODO: Add your command handler code here
}


void CXerosDlg::OnXerosConfiguration()
{
	// TODO: Add your command handler code here
}

/*
	OnWindowPosChanging method is used to hide dialog,
	when it is firstly made.
*/

void CXerosDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
	// TODO: Add your message handler code here
}


void CXerosDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: Add your message handler code here
	CMenu Menu, *pContextMenu = NULL;
	Menu.LoadMenuW(IDR_XEROS_TRAY);
	pContextMenu = Menu.GetSubMenu(0);

	CMenu *pSubMenu = pContextMenu->GetSubMenu(0);
	if (pContextMenu == NULL)
		return;

	if (!m_bGetURLs) {
		pSubMenu->EnableMenuItem(ID_XEROS_URLS, MF_BYPOSITION | MF_GRAYED);
	}
}


void CXerosDlg::OnXerosClose()
{
	// TODO: Add your command handler code here
	/*
		Xeros Dialog is exited
	*/
	if (m_pOperation != NULL) {
		delete m_pOperation;
	}
	OnOK();
}


void CXerosDlg::OnUpdateXerosStart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_bStartState) {
		m_pOperation->StartOperation();
		m_bStartState = FALSE;
	}
	else {
		m_pOperation->StopOperation();
		m_bStartState = TRUE;
	}
}
