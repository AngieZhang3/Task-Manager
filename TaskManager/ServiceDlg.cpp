// ServiceDlg.cpp : implementation file
//

#include "pch.h"
#include "TaskManager.h"
#include "ServiceDlg.h"
#include "afxdialogex.h"
#include <winsvc.h>

// ServiceDlg dialog

IMPLEMENT_DYNAMIC(ServiceDlg, CDialogEx)

ServiceDlg::ServiceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVICE_DLG, pParent)
{

}

ServiceDlg::~ServiceDlg()
{
}

void ServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSERVICE, m_lcService);
}


BEGIN_MESSAGE_MAP(ServiceDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTSERVICE, &ServiceDlg::OnLvnItemchangedListservice)
	ON_COMMAND(ID_CONTEXTMENU3_START, &ServiceDlg::OnContextmenu3Start)
	ON_COMMAND(ID_CONTEXTMENU3_STOP, &ServiceDlg::OnContextmenu3Stop)
	ON_COMMAND(ID_CONTEXTMENU3_RESTART, &ServiceDlg::OnContextmenu3Restart)
	ON_NOTIFY(NM_RCLICK, IDC_LISTSERVICE, &ServiceDlg::OnNMRClickListservice)
END_MESSAGE_MAP()


// ServiceDlg message handlers


BOOL ServiceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitServiceListCtrl();
	EnumerateServices();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


int ServiceDlg::InitServiceListCtrl()
{
	m_lcService.SetExtendedStyle(m_lcService.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_lcService.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 400);
	m_lcService.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 200);
	m_lcService.InsertColumn(2, _T("Description"), LVCFMT_LEFT, 400);
	m_lcService.InsertColumn(3, _T("Status"), LVCFMT_LEFT, 200);
	return 0;
}



void ServiceDlg::printError(TCHAR* const msg)
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


void ServiceDlg::EnumerateServices()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (NULL == hSCManager) {
		printError(TEXT("[tm] OpenSCManager failed"));
		return;
	}
	DWORD dwBytesNeeded = 0;
	DWORD dwServicesReturned = 0;
	DWORD dwResumeHandle = 0;
	DWORD dwServiceType = SERVICE_WIN32;
	DWORD dwServiceState = SERVICE_STATE_ALL;

	//First call to EnumServicesStatusEx to get the buffer size needed
	EnumServicesStatusEx(
		hSCManager,
		SC_ENUM_PROCESS_INFO,   //infoLevel, Use SC_ENUM_PROCESS_INFO to retrieve the name and service status information for each service in the database.
		dwServiceType,
		dwServiceState,
		NULL,
		0,
		&dwBytesNeeded,
		&dwServicesReturned,
		&dwResumeHandle,
		NULL);

	//buffer size needed
	DWORD dwSize = dwBytesNeeded;
	//allocates a block of memory from the heap 
	// to hold an array of "ENUM_SERVICE_STATUS_PROCESS” structures
	LPENUM_SERVICE_STATUS_PROCESS lpServiceStatus = (LPENUM_SERVICE_STATUS_PROCESS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
	if (NULL == lpServiceStatus) {
		printError(TEXT("[tm] HeapAlloc failed"));
		CloseHandle(hSCManager);
		return;
	}

	//second call to EnumServicesStatusEx to get the service information
	BOOL bSuccess = EnumServicesStatusEx(
		hSCManager,
		SC_ENUM_PROCESS_INFO,
		dwServiceType,
		dwServiceState,
		(LPBYTE)lpServiceStatus,
		dwSize,
		&dwBytesNeeded,
		&dwServicesReturned,
		&dwResumeHandle,
		NULL);
	if (!bSuccess)
	{
		printError(TEXT("[tm] EnumServicesStatusEx failed"));
	}
	for (DWORD i = 0; i < dwServicesReturned; i++) {
		int nItem = m_lcService.InsertItem(0, lpServiceStatus[i].lpServiceName);

		CString strProcessId;
		if (lpServiceStatus[i].ServiceStatusProcess.dwProcessId != 0)
		{
			strProcessId.Format(_T("%u"), lpServiceStatus[i].ServiceStatusProcess.dwProcessId);
		}
		else {
			strProcessId = "No associated process";
		}
		m_lcService.SetItemText(nItem, 1, strProcessId);

		m_lcService.SetItemText(nItem, 2, lpServiceStatus[i].lpDisplayName);

		CString strStatus = ConvertStatusToString(lpServiceStatus[i].ServiceStatusProcess.dwCurrentState);
		//strStatus.Format(_T("%d"), lpServiceStatus[i].ServiceStatusProcess.dwCurrentState);
		m_lcService.SetItemText(nItem, 3, strStatus);

	}
	HeapFree(GetProcessHeap(), 0, lpServiceStatus);
	CloseServiceHandle(hSCManager);
	return;
}


void ServiceDlg::OnLvnItemchangedListservice(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}



CString ServiceDlg::ConvertStatusToString(DWORD dwServiceStatus)
{
	CString strServiceStatus;

	switch (dwServiceStatus) {
	case 1:
		strServiceStatus = "SERVICE_STOPPED";
		break;
	case 2:
		strServiceStatus = "SERVICE_START_PENDING";
		break;
	case 3:
		strServiceStatus = "SERVICE_STOP_PENDING";
		break;
	case 4:
		strServiceStatus = "SERVICE_RUNNING";
		break;
	case 5:
		strServiceStatus = "SERVICE_CONTINUE_PENDING";
		break;
	case 6:
		strServiceStatus = "SERVICE_PAUSE_PENDING";
		break;
	case 7:
		strServiceStatus = "SERVICE_PAUSED";
		break;
	}
	return strServiceStatus;
}


void ServiceDlg::OnContextmenu3Start()
{
	//get selected service name
	int nItem = m_lcService.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}

	CString strServiceName = m_lcService.GetItemText(nItem, 0);
	//open SCManager
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (NULL == hSCManager)
	{
		printError(TEXT("[tm] OpenSCManager failed"));
		return;
	}
	//Open selected service
	SC_HANDLE hService = OpenService(hSCManager, strServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
	if (NULL == hService)
	{
		printError(TEXT("[tm] OpenService failed"));
		CloseServiceHandle(hSCManager);
		return;
	}
	//start service
	if (!StartService(hService, 0, NULL))
	{
		printError(TEXT("[tm] StartService failed"));
		DWORD dwError = GetLastError();
		if (dwError == ERROR_SERVICE_ALREADY_RUNNING) {
			MessageBox(_T("Service is already running"), _T("WARNING"), MB_OK);
		}

	}
	else {
		OutputDebugString(TEXT("[tm] Service start successfully"));
		SERVICE_STATUS_PROCESS ssp;
		DWORD dwBytesNeeded;
		const int maxWaitTime = 30 * 1000; // if exceeds 30 seconds, timeout 
		int elapsedTime = 0;
		CString strState;
		while (QueryServiceStatusEx(
			hService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			//update state in list control
			strState = ConvertStatusToString(ssp.dwCurrentState);
			m_lcService.SetItemText(nItem, 3, strState);
			m_lcService.Invalidate();
			m_lcService.UpdateWindow();

			if (ssp.dwCurrentState == SERVICE_RUNNING || ssp.dwCurrentState == SERVICE_STOPPED)
			{
				break;
			}

			Sleep(1000);
			elapsedTime += 1000;
			if (elapsedTime >= maxWaitTime)
			{
				MessageBox(_T("Timeout waiting for service to reach final state"), _T("WARNING"), MB_OK);
				break;
			}
		}
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}


void ServiceDlg::OnContextmenu3Stop()
{
	int nItem = m_lcService.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}

	CString strServiceName = m_lcService.GetItemText(nItem, 0);

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (NULL == hSCManager)
	{
		printError(TEXT("[tm] OpenSCManager failed"));
		return;
	}

	SC_HANDLE hService = OpenService(hSCManager, strServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (NULL == hService)
	{
		printError(TEXT("[tm] OpenService failed"));
		CloseServiceHandle(hSCManager);
		return;
	}
	SERVICE_STATUS serviceStatus;

	if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		printError(TEXT("[tm] ControlService failed"));
		DWORD dwError = GetLastError();
		if (dwError == ERROR_SERVICE_NOT_ACTIVE) {
			MessageBox(_T("Service has not been started"), _T("WARNING"), MB_OK);
		}

	}
	else {
		OutputDebugString(TEXT("[tm] Service stopped successfully"));
		SERVICE_STATUS_PROCESS ssp;
		DWORD dwBytesNeeded;
		const int maxWaitTime = 30 * 1000; // if exceeds 30 seconds, timeout 
		int elapsedTime = 0;
		CString strState;
		while (QueryServiceStatusEx(
			hService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			strState = ConvertStatusToString(ssp.dwCurrentState);
			m_lcService.SetItemText(nItem, 3, strState);
			m_lcService.Invalidate();
			m_lcService.UpdateWindow();
			if (ssp.dwCurrentState == SERVICE_STOPPED)
			{
				break;
			}
			Sleep(1000);
			elapsedTime += 1000;
			if (elapsedTime >= maxWaitTime)
			{
				MessageBox(_T("Timeout waiting for service to reach final state"), _T("WARNING"), MB_OK);
				break;
			}
		}
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}


void ServiceDlg::OnContextmenu3Restart()
{
	//get selected service name
	int nItem = m_lcService.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1) {
		return;
	}

	CString strServiceName = m_lcService.GetItemText(nItem, 0);
	// open SCManager
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	//open service
	SC_HANDLE hService = OpenService(hSCManager, strServiceName, SERVICE_STOP | SERVICE_START | SERVICE_QUERY_STATUS);
	if (NULL == hService)
	{
		printError(TEXT("[tm] OpenService failed"));
		CloseServiceHandle(hSCManager);
		return;
	}
	//check status of the selected service
	//only enable "restart" if service is currently running
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	if (!QueryServiceStatusEx(
		hService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded)) {
		printError(TEXT("QueryServiceStatusEx failed"));
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return;
	}
	if (ssp.dwCurrentState != SERVICE_RUNNING) {
		MessageBox(_T("Service is not running"), _T("WARNING"), MB_OK);
		return;
	}

	//stop service
	SERVICE_STATUS serviceStatus;

	if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		printError(TEXT("[tm] ControlService failed"));
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return;
	}

	//start service
	if (!StartService(hService, 0, NULL))
	{
		printError(TEXT("[tm] StartService failed"));
		//DWORD dwError = GetLastError();
		//if (dwError == ERROR_SERVICE_ALREADY_RUNNING) {
		//	MessageBox(_T("Service is already running"), _T("WARNING"), MB_OK);
		//}
	}
	else {
		OutputDebugString(TEXT("[tm] Service start successfully"));
		SERVICE_STATUS_PROCESS ssp;
		DWORD dwBytesNeeded;
		const int maxWaitTime = 30 * 1000; // if exceeds 30 seconds, timeout 
		int elapsedTime = 0;
		CString strState;
		while (QueryServiceStatusEx(
			hService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			//update state in list control
			strState = ConvertStatusToString(ssp.dwCurrentState);
			m_lcService.SetItemText(nItem, 3, strState);
			m_lcService.Invalidate();
			m_lcService.UpdateWindow();
			if (ssp.dwCurrentState == SERVICE_RUNNING || ssp.dwCurrentState == SERVICE_STOPPED)
			{
				break;
			}
			Sleep(1000);
			elapsedTime += 1000;
			if (elapsedTime >= maxWaitTime)
			{
				MessageBox(_T("Timeout waiting for service to reach final state"), _T("WARNING"), MB_OK);
				break;
			}
		}
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

}



void ServiceDlg::OnNMRClickListservice(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint clickPoint;
	GetCursorPos(&clickPoint);

	//Convert screen coordinates to client coordinates
	CPoint clientPoint = clickPoint;
	m_lcService.ScreenToClient(&clientPoint);

	//Determine the line clicked
	int selectedItem = m_lcService.HitTest(clientPoint);
	if (selectedItem != -1)
	{
		m_lcService.SetItemState(selectedItem, LVIS_SELECTED, LVIS_SELECTED);

		//Create and load menu
		CMenu contextMenu;
		contextMenu.LoadMenu(IDR_MENU3);
		CMenu* pPopup = contextMenu.GetSubMenu(0);

		//display context menu
		if (pPopup) {
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, clickPoint.x, clickPoint.y, this);
		}
	}
	*pResult = 0;
}
