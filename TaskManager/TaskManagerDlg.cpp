
// TaskManagerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAboutDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CTaskManagerDlg dialog



CTaskManagerDlg::CTaskManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TASKMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CTaskManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTaskManagerDlg::OnTcnSelchangeTab1)
	//ON_COMMAND(ID_CONTEXTMENU_THREADLIST, &CTaskManagerDlg::OnContextmenuThreadlist)
	ON_BN_CLICKED(IDCANCEL, &CTaskManagerDlg::OnBnClickedCancel)
	ON_COMMAND(ID_CONTEXTMENU2_SUSPENDTHREAD, &CTaskManagerDlg::OnContextmenu2Suspendthread)
END_MESSAGE_MAP()


// CTaskManagerDlg message handlers

BOOL CTaskManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	InitTabCtrl();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTaskManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTaskManagerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTaskManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CTaskManagerDlg::InitTabCtrl()
{

	m_tabCtrl.InsertItem(0, _T("Processes"));
	m_tabCtrl.InsertItem(1, _T("Services"));

	//Create Pages for the tabs
	CRect rc;
	m_tabCtrl.GetClientRect(rc);
	m_processDlg.Create(IDD_PROCESS_DLG, &m_tabCtrl);
	m_processDlg.MoveWindow(30, 30, rc.Width() - 30, rc.Height() - 30);
	m_processDlg.ShowWindow(SW_NORMAL);

	m_serviceDlg.Create(IDD_SERVICE_DLG, &m_tabCtrl); // Create the services dialog
	m_serviceDlg.MoveWindow(30, 30, rc.Width() - 30, rc.Height() - 30);
	m_serviceDlg.ShowWindow(SW_HIDE); // Initially hide the services dialog
	return 0;
}





void CTaskManagerDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nIndex = m_tabCtrl.GetCurSel();
	switch (nIndex) {
	case 0:
		m_processDlg.ShowWindow(SW_NORMAL);
		m_serviceDlg.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_processDlg.ShowWindow(SW_HIDE);
		m_serviceDlg.ShowWindow(SW_NORMAL);
		break;
	}
	*pResult = 0;
}


//void CTaskManagerDlg::OnContextmenuThreadlist()
//{
//	ThreadDlg threadDlg = new ThreadDlg;
//	threadDlg.DoModal();
//}


void CTaskManagerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CTaskManagerDlg::OnContextmenu2Suspendthread()
{
	// TODO: Add your command handler code here
}
