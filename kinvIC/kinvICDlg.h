
// kinvICDlg.h: 头文件
//

#pragma once
#include <vector>

typedef std::vector<unsigned char> AseemblyByte;
typedef struct {
	DWORD pid;
	CString pname;
}ProcessItem;

// CkinvICDlg 对话框
class CkinvICDlg : public CDialogEx
{
	// 构造
public:
	CkinvICDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KINVIC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void InitProcesses();
public:
	// 进程列表控件
	CListCtrl m_processes;
	// 进程列表点击事件
	afx_msg void OnNMClickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	// 进程列表容器
	std::vector<ProcessItem> m_process_container;
	CStatic m_cur_pid;
	DWORD m_cur_pid_number;
	CStatic m_cur_pname;
	CButton m_button_inject_code;
	CButton m_button_inject_dll;
	CButton m_button_suspend_thread;
	CButton m_button_resume_thread;
	CButton m_button_terminate_thread;
	CEdit m_assembly_code;
	afx_msg void OnBnClickedButtonInjectCode();
	BOOL Assembly2ByteBuffer(AseemblyByte& container);
	void injectDll(HANDLE handle);
	void injectCode(HANDLE handle, AseemblyByte& container);
};
