
// kinvICDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "kinvIC.h"
#include "kinvICDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include "parse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CkinvICDlg 对话框



CkinvICDlg::CkinvICDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KINVIC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CkinvICDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_processes);
	DDX_Control(pDX, IDC_STATIC_PID, m_cur_pid);
	DDX_Control(pDX, IDC_STATIC_PNAME, m_cur_pname);
	DDX_Control(pDX, IDC_BUTTON_INJECT_CODE, m_button_inject_code);
	DDX_Control(pDX, IDC_BUTTON_INJECT_DLL, m_button_inject_dll);
	DDX_Control(pDX, IDC_BUTTON_SUSPEND_THREAD, m_button_suspend_thread);
	DDX_Control(pDX, IDC_BUTTON_RESUME_THREAD, m_button_resume_thread);
	DDX_Control(pDX, IDC_BUTTON_TERMINATE_THREAD, m_button_terminate_thread);
	DDX_Control(pDX, IDC_EDIT_ASSEMBLY_CODE, m_assembly_code);
}

BEGIN_MESSAGE_MAP(CkinvICDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, &CkinvICDlg::OnNMClickListProcess)
	ON_BN_CLICKED(IDC_BUTTON_INJECT_CODE, &CkinvICDlg::OnBnClickedButtonInjectCode)
	ON_BN_CLICKED(IDC_BUTTON_INJECT_DLL, &CkinvICDlg::OnBnClickedButtonInjectDll)
END_MESSAGE_MAP()


// CkinvICDlg 消息处理程序

BOOL CkinvICDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_button_inject_code.EnableWindow(false);
	m_button_inject_dll.EnableWindow(false);
	m_button_suspend_thread.EnableWindow(false);
	m_button_resume_thread.EnableWindow(false);
	m_button_terminate_thread.EnableWindow(false);

	m_cur_pid_number = -1;
	m_processes.InsertColumn(0, L"PID", LVCFMT_LEFT, 100);
	m_processes.InsertColumn(1, L"进程名", LVCFMT_LEFT, 600);
	InitProcesses();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CkinvICDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CkinvICDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 刷新进程列表
void CkinvICDlg::InitProcesses()
{
	m_process_container.clear();
	HANDLE process_handle;
	PROCESSENTRY32 process_info;
	process_info.dwSize = sizeof(PROCESSENTRY32);
	BOOL open_status;

	process_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	open_status = Process32First(process_handle, &process_info);
	int i = 0;
	while (open_status) {
		ProcessItem item;
		item.pid = process_info.th32ProcessID;
		item.pname = CString(process_info.szExeFile);
		m_process_container.push_back(item);
		DWORD cur_pid = process_info.th32ProcessID;
		CString format_pid;
		format_pid.Format(L"%d", cur_pid);
		m_processes.InsertItem(i, format_pid);
		m_processes.SetItemText(i, 1, process_info.szExeFile);
		open_status = Process32Next(process_handle, &process_info);
		i++;
	}

	m_processes.SetExtendedStyle(m_processes.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

// 选择进程
void CkinvICDlg::OnNMClickListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_button_inject_code.EnableWindow(true);
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	ProcessItem cur_process = m_process_container.at(pNMItemActivate->iItem);
	m_cur_pname.SetWindowTextW(cur_process.pname);
	CString pid_str;
	pid_str.Format(L"%d", cur_process.pid);
	m_cur_pid.SetWindowTextW(pid_str);
	m_cur_pid_number = cur_process.pid;
	m_button_inject_dll.EnableWindow(true);
	CkinvICDlg::UpdateWindow();

}

void CkinvICDlg::injectDll(HANDLE handle)
{
	// DLL路径
	char dllPath[MAX_PATH] = "C:/Dll01.dll";
	// 说明：跨进程申请堆空间
	void* dllAddr = VirtualAllocEx(handle, NULL, sizeof(dllPath), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	// 说明：跨进程写入数据
	WriteProcessMemory(handle, dllAddr, dllPath, sizeof(dllPath), NULL);
	// 获取模块句柄
	HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
	// 导出模块中的函数的地址
	LPTHREAD_START_ROUTINE addr = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");
	CreateRemoteThread(handle, NULL, 0, addr, dllAddr, 0, NULL);
}

void CkinvICDlg::injectCode(HANDLE handle, AseemblyByte& funData)
{

	//HANDLE process_handle;
	//process_handle = CreateToolhelp32Snapshot(TH32CS_SNAPALL, m_cur_pid_number);
	//MODULEENTRY32 lpme;
	//lpme.dwSize = sizeof(MODULEENTRY32);
	//Module32First(process_handle, &lpme);
	//DWORD addr = (DWORD)lpme.modBaseAddr;

	size_t funSize = (size_t)funData.size();

	byte* ThreadFunc = new byte[funSize];
	for (size_t i = 0; i < funSize; i++)
	{
		ThreadFunc[i] = funData[i];
	}

	void* funAddr = VirtualAllocEx(handle, NULL, funSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (funAddr != NULL)
	{
		if (WriteProcessMemory(handle, funAddr, ThreadFunc, funSize, NULL) == 0) {
			MessageBox(L"写入内存失败！");
			return;
		}

		HANDLE thread_handle = CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)funAddr, NULL, 0, NULL);
		Sleep(500);
		if (thread_handle == NULL)
		{
			MessageBox(L"创建线程失败！");
			return;
		}

		VirtualFreeEx(handle, funAddr, 0, MEM_RELEASE);
	}
	delete[] ThreadFunc;
	funData.clear();
}

// 汇编转字节数组
BOOL CkinvICDlg::Assembly2ByteBuffer(AseemblyByte& directContainer)
{
	// 每行限制字符250
	int i, nLineCount = m_assembly_code.GetLineCount();

	unsigned int j;
	CString strText, message, parse_error;
	BOOL parse_status;
	for (i = 0; i < nLineCount; i++)
	{
		int len = m_assembly_code.LineLength(m_assembly_code.LineIndex(i));
		if (len > 250 || len < 1) {
			MessageBox(L"每行指令字符数量：1~250");
			return FALSE;
		}
		m_assembly_code.GetLine(i, strText.GetBuffer(len), len);
		strText.ReleaseBuffer(len);
		XEDPARSE parse;
		parse_status = parseAssembly(strText, parse);
		if (!parse_status)
		{
			parse_error = CString(parse.error);
			message.Format(L"行号：%d\n解析错误：%s", i + 1, parse_error);
			MessageBox(message);
			return FALSE;
		}
		for (j = 0; j < parse.dest_size; j++)
		{
			directContainer.push_back(parse.dest[j]);
		}
	}
	return TRUE;
}

// 注入代码
void CkinvICDlg::OnBnClickedButtonInjectCode()
{
	// 转机器指令
	AseemblyByte directContainer;
	BOOL format_res = Assembly2ByteBuffer(directContainer);
	if (!format_res)
	{
		return;
	}
	// 获取进程句柄
	DWORD processId = m_cur_pid_number;
	HANDLE process_handle;
	process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (process_handle != NULL)
	{
		injectCode(process_handle, directContainer);
	}
}


void CkinvICDlg::OnBnClickedButtonInjectDll()
{
	TCHAR szFilters[] = _T("DLL File (*.dll)|*.dll|");

	CFileDialog fileDlg(TRUE, _T("dll"), _T("*.dll"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
	}
}
