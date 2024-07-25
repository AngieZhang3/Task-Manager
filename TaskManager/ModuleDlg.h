#pragma once


// ModuleDlg dialog
#define WM_PROCESSID_MODULE (WM_USER + 2)

class ModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ModuleDlg)

public:
	ModuleDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ModuleDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODULE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lcModule;
	int InitListCtrlModule();
	virtual BOOL OnInitDialog();
	BOOL ListProcessModules(DWORD dwPID);
	void printError(TCHAR const* msg);
protected:
	afx_msg LRESULT OnProcessidModule(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCancel();
};
