// ModuleDlg.cpp : implementation file
//

#include "pch.h"
#include "TaskManager.h"
#include "ModuleDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

// ModuleDlg dialog

IMPLEMENT_DYNAMIC(ModuleDlg, CDialogEx)

ModuleDlg::ModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODULE_DIALOG, pParent)
{

}

ModuleDlg::~ModuleDlg()
{
}

void ModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lcModule);
}


BEGIN_MESSAGE_MAP(ModuleDlg, CDialogEx)
	ON_MESSAGE(WM_PROCESSID_MODULE, &ModuleDlg::OnProcessidModule)
	ON_BN_CLICKED(IDCANCEL, &ModuleDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// ModuleDlg message handlers


int ModuleDlg::InitListCtrlModule()
{
	m_lcModule.SetExtendedStyle(m_lcModule.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_lcModule.InsertColumn(0, _T("Module Name"), LVCFMT_LEFT, 300);
	m_lcModule.InsertColumn(1, _T("Executable"), LVCFMT_LEFT, 400);
	m_lcModule.InsertColumn(2, _T("Ref Count (g)"), LVCFMT_LEFT, 150);
	m_lcModule.InsertColumn(3, _T("Ref Count (p)"), LVCFMT_LEFT, 150);
	m_lcModule.InsertColumn(4, _T("Base Address"), LVCFMT_LEFT, 150);
	m_lcModule.InsertColumn(5, _T("Base Size"), LVCFMT_LEFT, 150);
	return 0;
}


BOOL ModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitListCtrlModule();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



BOOL ModuleDlg::ListProcessModules(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	//take a snap of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		printError(TEXT("CreateToolhelp32Snapshot (of modules"));
		return FALSE;
	}

	// set the size of the structure before using it
	me32.dwSize = sizeof(MODULEENTRY32);

	// retrieve information about the first module, 
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32)) {
		printError(TEXT("Module32First"));
		CloseHandle(hModuleSnap);
		return FALSE;
	}

	do {
		int nItem = m_lcModule.InsertItem(0, me32.szModule);

		m_lcModule.SetItemText(nItem, 1, me32.szExePath);

		CString strRefCntg;
		strRefCntg.Format(_T("0x%04X"), me32.GlblcntUsage);
		m_lcModule.SetItemText(nItem, 2, strRefCntg);

		CString strRefCntp;
		strRefCntp.Format(_T("0x%04X"), me32.ProccntUsage);
		m_lcModule.SetItemText(nItem, 3, strRefCntp);

		CString strBaseAddr;
		strBaseAddr.Format(_T("0x%08X"), me32.modBaseAddr);
		m_lcModule.SetItemText(nItem, 4, strBaseAddr);

		CString strBaseSize;
		strBaseSize.Format(_T("%d"), me32.modBaseSize);
		m_lcModule.SetItemText(nItem, 5, strBaseSize);
	} while (Module32Next(hModuleSnap, &me32));
	CloseHandle(hModuleSnap);
	return TRUE;
}


void ModuleDlg::printError(TCHAR const* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR formattedMsg[512]; // Buffer to hold the final formatted message
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);
	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Format the message
	_stprintf_s(formattedMsg, 512, TEXT("[tm] WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);

	// Output the message using OutputDebugString
	OutputDebugString(formattedMsg);
}


afx_msg LRESULT ModuleDlg::OnProcessidModule(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPID = (DWORD)wParam;
	ListProcessModules(dwPID);
	return 0;
}


void ModuleDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
