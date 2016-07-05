// Mysocket.cpp : 实现文件
//

#include "stdafx.h"
#include "rtuTool.h"
#include "mySocket.h"
#include "rtuToolDlg.h"

// CMysocket

CMysocket::CMysocket()
{
}

CMysocket::~CMysocket()
{
}


// CMysocket 成员函数
void CMysocket::SetParent(CDialog *pWnd)
{
   m_pWnd = pWnd;
}

void CMysocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	((CrtuToolDlg*)m_pWnd)->OnReceive();
	CAsyncSocket::OnReceive(nErrorCode);
}
