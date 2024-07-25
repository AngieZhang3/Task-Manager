// ThreadDlg.cpp : implementation file
//

#include "pch.h"
#include "TaskManager.h"
#include "ThreadDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

// ThreadDlg dialog


IMPLEMENT_DYNAMIC(ThreadDlg, CDialogEx)

ThreadDlg::ThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREAD_DIALOG, pParent)
{

}

ThreadDlg::~ThreadDlg()
{
}

void ThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTTHREAD, m_lcThread);
}


BEGIN_MESSAGE_MAP(ThreadDlg, CDialogEx)
	ON_MESSAGE(WM_PROCESSID_THREAD, &ThreadDlg::OnProcessidThread)
	ON_NOTIFY(NM_RCLICK, IDC_LISTTHREAD, &ThreadDlg::OnNMRClickListthread)
	ON_COMMAND(ID_CONTEXTMENU2_TERMINATETHREAD, &ThreadDlg::OnContextmenu2Terminatethread)
	ON_COMMAND(ID_CONTEXTMENU2_RESUMETHREAD, &ThreadDlg::OnContextmenu2Resumethread)
	ON_COMMAND(ID_CONTEXTMENU2_SUSPENDTHREAD, &ThreadDlg::OnContextmenu2Suspendthread)
END_MESSAGE_MAP()


// ThreadDlg message handlers


BOOL ThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitThreadListCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


int ThreadDlg::InitThreadListCtrl()
{
	// TODO: Add your implementation code here.
	m_lcThread.SetExtendedStyle(m_lcThread.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_lcThread.InsertColumn(0, _T("Thread ID"), LVCFMT_LEFT, 200);
	m_lcThread.InsertColumn(1, _T("Base Priority"), LVCFMT_LEFT, 200);
	m_lcThread.InsertColumn(2, _T("Delta Priority"), LVCFMT_LEFT, 200);
	
	return 0;
}




afx_msg LRESULT ThreadDlg::OnProcessidThread(WPARAM wParam, LPARAM lParam)
{
	DWORD dwProcessID = DWORD(wParam);
	ListProcessThreads(dwProcessID);
	return 0;
}


BOOL ThreadDlg::ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	//take a snapshot of all running threads
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	//fill in the size of the structure before using it
	te32.dwSize = sizeof(THREADENTRY32);
	//retrive the information about the first thread
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32)) {
		printError(TEXT("Thread32First"));
		CloseHandle(hThreadSnap);   //must clean up the snapshot object
		return FALSE;
	}

	//Now walk the thread list of the syste
	// and display information about each thread
	// associated with the specified process.
	do {
		if (te32.th32OwnerProcessID == dwOwnerPID) {
			CString strThreadID;
			strThreadID.Format(_T("%u"), te32.th32ThreadID);
			int nItem = m_lcThread.InsertItem(0, strThreadID);

			CString strBasePri;
			strBasePri.Format(_T("%d"), te32.tpBasePri);
			m_lcThread.SetItemText(nItem, 1, strBasePri);

			CString strDeltaPri;
			strDeltaPri.Format(_T("%d"), te32.tpDeltaPri);
			m_lcThread.SetItemText(nItem, 2, strDeltaPri);

		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);

	return TRUE;
}


void ThreadDlg::printError(TCHAR const* msg)
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


void ThreadDlg::OnNMRClickListthread(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint clickPoint;
	GetCursorPos(&clickPoint);

	//Convert screen coordinates to client coordinates
	CPoint clientPoint = clickPoint;
	m_lcThread.ScreenToClient(&clientPoint);

	//Determine the line clicked
	int selectedItem = m_lcThread.HitTest(clientPoint);
	if (selectedItem != -1)
	{
		m_lcThread.SetItemState(selectedItem, LVIS_SELECTED, LVIS_SELECTED);

		//Create and load menu
		CMenu contextMenu;
		contextMenu.LoadMenu(IDR_MENU2);
		CMenu* pPopup = contextMenu.GetSubMenu(0);

		//display context menu
		if (pPopup) {
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, clickPoint.x, clickPoint.y, this);
		}
	}
	*pResult = 0;
}


void ThreadDlg::OnContextmenu2Terminatethread()
{
	int nItem = m_lcThread.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strThreadId = m_lcThread.GetItemText(nItem, 0);
	DWORD dwThreadId = static_cast<DWORD>(_ttoi(strThreadId));
	HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, dwThreadId);
	if (hThread == NULL) {
		printError(TEXT("Failed to open thread"));
		return;
	}
	if (!TerminateThread(hThread, 0))
	{
		printError(TEXT("[tm] Failed to terminate thread"));
		CloseHandle(hThread);
		return;
	}
	m_lcThread.DeleteItem(nItem);
	CloseHandle(hThread);
	return;
}


void ThreadDlg::OnContextmenu2Resumethread()
{
	int nItem = m_lcThread.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strThreadId = m_lcThread.GetItemText(nItem, 0);
	DWORD dwThreadId = static_cast<DWORD>(_ttoi(strThreadId));
	HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwThreadId);
	if (hThread == NULL) {
		printError(TEXT("[tm] Failed to open thread"));
		return;
	}
	if (ResumeThread(hThread) == (DWORD)-1) {
		printError(TEXT("[tm] ResumeThread"));
		CloseHandle(hThread);
		return;
	}
	OutputDebugString(TEXT("[tm] ResumeThread success"));
	CloseHandle(hThread);
	return;
}


void ThreadDlg::OnContextmenu2Suspendthread()
{
	int nItem = m_lcThread.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}
	CString strThreadId = m_lcThread.GetItemText(nItem, 0);
	DWORD dwThreadId = static_cast<DWORD>(_ttoi(strThreadId));
	HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwThreadId);
	if (hThread == NULL) {
		printError(TEXT("[tm] Failed to open thread"));
		return;
	}
	if (SuspendThread(hThread) == (DWORD)-1) {
		printError(TEXT("[tm] SuspendThread"));
		CloseHandle(hThread);
		return;
	}
	OutputDebugString(TEXT("[tm] SuspendThread success"));
	CloseHandle(hThread);
	return;
}
