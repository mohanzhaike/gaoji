// Mysocket.cpp : ʵ���ļ�
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


// CMysocket ��Ա����
void CMysocket::SetParent(CDialog *pWnd)
{
   m_pWnd = pWnd;
}

void CMysocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	((CrtuToolDlg*)m_pWnd)->OnReceive();
	CAsyncSocket::OnReceive(nErrorCode);
}
