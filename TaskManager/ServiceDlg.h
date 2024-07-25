#pragma once


// ServiceDlg dialog

class ServiceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ServiceDlg)

public:
	ServiceDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ServiceDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lcService;
	virtual BOOL OnInitDialog();
	int InitServiceListCtrl();
	void printError(TCHAR* const msg);
	void EnumerateServices();
	afx_msg void OnLvnItemchangedListservice(NMHDR* pNMHDR, LRESULT* pResult);
	CString ConvertStatusToString(DWORD dwServiceStatus);
	afx_msg void OnContextmenu3Start();
	afx_msg void OnContextmenu3Stop();
	afx_msg void OnContextmenu3Restart();
	afx_msg void OnNMRClickListservice(NMHDR* pNMHDR, LRESULT* pResult);
};
