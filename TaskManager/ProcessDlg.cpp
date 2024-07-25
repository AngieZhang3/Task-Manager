// ProcessDlg.cpp : implementation file
//

#include "pch.h"
#include "TaskManager.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include "ThreadDlg.h"
#include "ModuleDlg.h"
// ProcessDlg dialog

IMPLEMENT_DYNAMIC(ProcessDlg, CDialogEx)

ProcessDlg::ProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS_DLG, pParent)
{

}

ProcessDlg::~ProcessDlg()
{
}

void ProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lcProcess);
}


BEGIN_MESSAGE_MAP(ProcessDlg, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &ProcessDlg::OnNMRClickList1)
	ON_COMMAND(ID_CONTEXTMENU_THREADLIST, &ProcessDlg::OnContextmenuThreadlist)
	ON_COMMAND(ID_CONTEXTMENU_MODULELIST, &ProcessDlg::OnContextmenuModulelist)
	ON_BN_CLICKED(IDCANCEL, &ProcessDlg::OnBnClickedCancel)
	ON_COMMAND(ID_CONTEXTMENU_ENDPROCESS, &ProcessDlg::OnContextmenuEndprocess)
END_MESSAGE_MAP()


// ProcessDlg message handlers


BOOL ProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitProcessListCtrl();
	GetProcessList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


int ProcessDlg::InitProcessListCtrl()
{
	m_lcProcess.SetExtendedStyle(m_lcProcess.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_lcProcess.InsertColumn(0, _T("Process Name"), LVCFMT_LEFT, 300);
	m_lcProcess.InsertColumn(1, _T("Process ID"), LVCFMT_LEFT, 200);
	m_lcProcess.InsertColumn(2, _T("Thread Count"), LVCFMT_LEFT, 200);
	m_lcProcess.InsertColumn(3, _T("Parent Process ID"), LVCFMT_LEFT, 200);
	m_lcProcess.InsertColumn(4, _T("Priority Base"), LVCFMT_LEFT, 200);
	m_lcProcess.InsertColumn(5, _T("Priority Class"), LVCFMT_LEFT, 200);
	return 0;
}


BOOL ProcessDlg::GetProcessList()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	//Take a snapshot of all processes in the system
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		printError(TEXT("CreateToolhelp32Snapshot(of processes"));
		return(FALSE);
	}

	//fill in the size of the structure before using it
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//Retrieve information about the first process and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printError(TEXT("Process32First")); // show cause of error
		CloseHandle(hProcessSnap);  // clean the snapshot object
		return FALSE;
	}
	//Enable Privilege
	if (!EnablePrivilege()) {
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	//Now walk the snapshot of processes, and
	// display information about each process in turn
	do {
		// add process name to the listCtrl
		int nItem = m_lcProcess.InsertItem(0, pe32.szExeFile);
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
			printError(TEXT("OpenProcess"));
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				printError(TEXT("GetPriorityClass"));
			CloseHandle(hProcess);
		}
		CString strProcessID;
		strProcessID.Format(_T("%u"), pe32.th32ProcessID);
		m_lcProcess.SetItemText(nItem, 1, strProcessID);

		CString strThreadCnt;
		strThreadCnt.Format(_T("%d"), pe32.cntThreads);
		m_lcProcess.SetItemText(nItem, 2, strThreadCnt);

		CString strParentPID;
		strParentPID.Format(_T("%u"), pe32.th32ParentProcessID);
		m_lcProcess.SetItemText(nItem, 3, strParentPID);

		CString strPriBase;
		strPriBase.Format(_T("%d"), pe32.pcPriClassBase);
		m_lcProcess.SetItemText(nItem, 4, strPriBase);

		if (dwPriorityClass) {
			CString strPriClass;
			strPriClass.Format(_T("0x%04X"), dwPriorityClass);
			m_lcProcess.SetItemText(nItem, 5, strPriClass);
		}

	} while (Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return TRUE;
}


void ProcessDlg::printError(TCHAR const* msg)
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


void ProcessDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint clickPoint;
	GetCursorPos(&clickPoint);

	//Convert screen coordinates to client coordinates
	CPoint clientPoint = clickPoint;
	m_lcProcess.ScreenToClient(&clientPoint);

	//Determine the line clicked
	int selectedItem = m_lcProcess.HitTest(clientPoint);
	if (selectedItem != -1)
	{
		m_lcProcess.SetItemState(selectedItem, LVIS_SELECTED, LVIS_SELECTED);

		//Create and load menu
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);

		CMenu* pContextMenu = menu.GetSubMenu(0);

		//display context menu
		if (pContextMenu) {
			pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, clickPoint.x, clickPoint.y, this);
		}
	}
	*pResult = 0;
}


void ProcessDlg::OnContextmenuThreadlist()
{
	int nItem = m_lcProcess.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strProcessID = m_lcProcess.GetItemText(nItem, 1);
	DWORD dwProcessID = static_cast<DWORD>(_ttoi(strProcessID));
	ThreadDlg* pThreadDlg = new ThreadDlg;
	
	//Create a modeless dialog to display the thread information. 
	// do not use modal since it will block. 
	pThreadDlg->Create(IDD_THREAD_DIALOG, this); 
	pThreadDlg->PostMessage(WM_PROCESSID_THREAD, static_cast<WPARAM>(dwProcessID), 0);
	pThreadDlg->ShowWindow(SW_SHOW);

}


void ProcessDlg::OnContextmenuModulelist()
{
	int nItem = m_lcProcess.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strProcessID = m_lcProcess.GetItemText(nItem, 1);
	DWORD dwProcessID = static_cast<DWORD>(_ttoi(strProcessID));
	ModuleDlg* pModuleDlg = new ModuleDlg;

	//Create a modeless dialog to display the thread information. 
	// do not use modal since it will block. 
	pModuleDlg->Create(IDD_MODULE_DIALOG, this);
	pModuleDlg->PostMessage(WM_PROCESSID_MODULE, static_cast<WPARAM>(dwProcessID), 0);
	pModuleDlg->ShowWindow(SW_SHOW);
}


void ProcessDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void ProcessDlg::OnContextmenuEndprocess()
{
	int nItem = m_lcProcess.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strProcessID = m_lcProcess.GetItemText(nItem, 1);
	DWORD dwProcessID = static_cast<DWORD>(_ttoi(strProcessID));
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
	if (hProcess == NULL) {
	//	MessageBox(_T("Failed to open process"), _T("caution"), MB_OK);
		printError(TEXT("Failed to open process"));
		return;
	}
	if (!TerminateProcess(hProcess, 0))
	{
		//MessageBox(_T("Failed to end Process"), _T("caution"), MB_OK);
		printError(TEXT("Failed to end process"));
		CloseHandle(hProcess);
		return;
	}
	m_lcProcess.DeleteItem(nItem);
	CloseHandle(hProcess);
}


BOOL ProcessDlg::EnablePrivilege()
{
	HANDLE hToken;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES,
		&hToken)) {
		printError(TEXT("OpenProcessToken error"));
		return FALSE;
	}

	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		SE_DEBUG_NAME,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printError(TEXT("LookupPrivilegeValue error: %u"));
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		NULL,
		NULL))
	{
		printError(TEXT("AdjustTokenPrivileges error"));
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printError(TEXT("The token does not have the specified privilege."));
		return FALSE;
	}

	return TRUE;
}
