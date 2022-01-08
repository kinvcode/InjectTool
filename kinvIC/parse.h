#pragma once
#pragma comment(lib,"XEDParse_x86")
#include <vector>
#include "XEDParse.h"
#include <afxstr.h>

BOOL parseAssembly(CString direct, XEDPARSE &parse)
{
	CStringA directA = CStringA(direct);
	// 实例化对象
	//XEDPARSE parse;
#ifdef _WIN64
	parse.x64 = true;
#else
	parse.x64 = false;
#endif
	// 零填充缓冲区
	memset(&parse, 0, sizeof(parse));
	// 指令指针
	parse.cip = 0;
	// 填充指令
	char *directBuffer = directA.GetBuffer();
	// 指令传递到结构体里
	strcpy_s(parse.instr, directBuffer);
	// 解析
	XEDPARSE_STATUS parse_status = XEDParseAssemble(&parse);
	// 解析错误处理
	if (parse_status == XEDPARSE_STATUS::XEDPARSE_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}