#pragma once


// ThreadDlg dialog

#define WM_PROCESSID_THREAD (WM_USER + 1)
class ThreadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ThreadDlg)

public:
	ThreadDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ThreadDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREAD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int InitThreadListCtrl();
	CListCtrl m_lcThread;
protected:
	afx_msg LRESULT OnProcessidThread(WPARAM wParam, LPARAM lParam);
public:
	BOOL ListProcessThreads(DWORD dwOwnerPID);
	void printError(TCHAR const* msg);
	afx_msg void OnNMRClickListthread(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextmenu2Terminatethread();
	afx_msg void OnContextmenu2Resumethread();
	afx_msg void OnContextmenu2Suspendthread();
private:

};
