#pragma once
#pragma comment(lib,"XEDParse_x86")
#include <vector>
#include "XEDParse.h"
#include <afxstr.h>

BOOL parseAssembly(CString direct, XEDPARSE &parse)
{
	CStringA directA = CStringA(direct);
	// ʵ��������
	//XEDPARSE parse;
#ifdef _WIN64
	parse.x64 = true;
#else
	parse.x64 = false;
#endif
	// ����仺����
	memset(&parse, 0, sizeof(parse));
	// ָ��ָ��
	parse.cip = 0;
	// ���ָ��
	char *directBuffer = directA.GetBuffer();
	// ָ��ݵ��ṹ����
	strcpy_s(parse.instr, directBuffer);
	// ����
	XEDPARSE_STATUS parse_status = XEDParseAssemble(&parse);
	// ����������
	if (parse_status == XEDPARSE_STATUS::XEDPARSE_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}