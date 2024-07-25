#pragma once


// ProcessDlg dialog

class ProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessDlg)

public:
	ProcessDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ProcessDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lcProcess;
	virtual BOOL OnInitDialog();
	int InitProcessListCtrl();
	BOOL GetProcessList();
	void printError(TCHAR const* msg);
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextmenuThreadlist();
	afx_msg void OnContextmenuModulelist();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnContextmenuEndprocess();
	BOOL EnablePrivilege();
};
