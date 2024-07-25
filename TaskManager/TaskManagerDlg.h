
// TaskManagerDlg.h : header file
//

#pragma once
#include "ProcessDlg.h"
#include "ServiceDlg.h"
// CTaskManagerDlg dialog
class CTaskManagerDlg : public CDialogEx
{
// Construction
public:
	CTaskManagerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabCtrl;
	int InitTabCtrl();
private:
	ProcessDlg m_processDlg;
	ServiceDlg m_serviceDlg;
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnContextmenuThreadlist();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnContextmenu2Suspendthread();
};
