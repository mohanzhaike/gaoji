
// rtuToolDlg.cpp : ʵ���ļ�
//

#include "string.h"
#include "stdafx.h"
#include "rtuTool.h"
#include "mySocket.h"
#include "rtuToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CrtuToolDlg �Ի���




CrtuToolDlg::CrtuToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CrtuToolDlg::IDD, pParent)
{
	CString str;
	dsFlag = sFlag = FALSE;

	maxAddNum = 0;
	connectIp = _T("192.168.1.2");
	addKey = _T("");
	terminiValue = _T("");
	
	fileTi.Open(_T(".\\term.ini"),CFile::typeText|CFile::modeReadWrite );
	fileNet.Open(_T(".\\net_params.conf"),CFile::typeText|CFile::modeReadWrite);
	
	prmnum = 0;
	pPrmHead=NULL;
	pNetHead = new CSinglePrm[MAX_NEW_PRM_NUM];
	lastDat = -1;
	while(TRUE == fileTi.ReadString(str)){
		prmnum++;
	}
	//AfxMessageBox(str);
//	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(FALSE);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CrtuToolDlg::~CrtuToolDlg()
{
	if(pPrmHead != NULL)
		delete []pPrmHead;
	if(pNetHead != NULL)
		delete []pNetHead;
	if(sFlag != FALSE){
		char buf[10];
		sprintf( buf, "QUIT\r\n");
		cmdSocket.Send(buf,strlen(buf));
		cmdSocket.Close();
	}
	if(dsFlag != FALSE){
		dataSocket.Close();
	}
	if(tFlag != FALSE)
		telnetSocket.Close();
}

void CrtuToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TERMINI_TREE, terminiKeys);
	DDX_Text(pDX, IDC_CONNECT_IP, connectIp);
	DDX_Text(pDX, IDC_KEY_ADD, addKey);
	DDX_Text(pDX, IDC_VALUE, terminiValue);
}

BEGIN_MESSAGE_MAP(CrtuToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_TERMINI_TREE, OnClickTerminiTree)
	ON_BN_CLICKED(IDC_SAVE, &CrtuToolDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_SURE, &CrtuToolDlg::OnBnClickedSure)
	ON_BN_CLICKED(IDC_CONNECT_B, &CrtuToolDlg::OnBnClickedConnectB)
	ON_BN_CLICKED(IDC_CLOSE_B, &CrtuToolDlg::OnBnClickedCloseB)
	ON_BN_CLICKED(IDC_SEND_TO_RTU, &CrtuToolDlg::OnBnClickedSendToRtu)
	ON_BN_CLICKED(IDC_SEND_TERM, &CrtuToolDlg::OnBnClickedSendTerm)
	ON_BN_CLICKED(IDC_SEND_IP, &CrtuToolDlg::OnBnClickedSendIp)
	ON_BN_CLICKED(IDC_ADD_1, &CrtuToolDlg::OnBnClickedAdd1)
	ON_BN_CLICKED(IDC_MINUS_1, &CrtuToolDlg::OnBnClickedMinus1)
	ON_BN_CLICKED(IDC_NO_CHANGE, &CrtuToolDlg::OnBnClickedNoChange)
	ON_BN_CLICKED(IDC_ONLY_SEND_SCCU, &CrtuToolDlg::OnBnClickedOnlySendSccu)
	ON_BN_CLICKED(IDC_SEND_SCCU, &CrtuToolDlg::OnBnClickedSendSccu)
	ON_BN_CLICKED(IDC_NO_SEND_SCCU, &CrtuToolDlg::OnBnClickedNoSendSccu)
	ON_BN_CLICKED(IDC_SEND_AUTORUN, &CrtuToolDlg::OnBnClickedSendAutorun)
	ON_BN_CLICKED(IDC_SET_REBOOT, &CrtuToolDlg::OnBnClickedSetReboot)
	ON_BN_CLICKED(IDC_REBOOT, &CrtuToolDlg::OnBnClickedReboot)
	ON_BN_CLICKED(IDC_ABR_REBOOT, &CrtuToolDlg::OnBnClickedAbrReboot)
END_MESSAGE_MAP()


// CrtuToolDlg ��Ϣ�������

BOOL CrtuToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	int i=0;
	if(prmnum > 0){
		pPrmHead = new CSinglePrm[prmnum+MAX_ADD_NUM];
	}else{
		pPrmHead = NULL;
	}
	if(pPrmHead != NULL){
		fileTi.SeekToBegin();
		for(;i<prmnum;i++)
			fileTi.ReadString((pPrmHead+i)->lineData);
	}
	fileNet.SeekToBegin();
	for(i=0;i<MAX_NEW_PRM_NUM;i++)
		fileNet.ReadString((pNetHead+i)->lineData);

	hRoot = terminiKeys.InsertItem(_T("+term.ini"), 0, 0);
	//HTREEITEM prm1;
	
	for(i=0;i< prmnum;i++){
		(pPrmHead+i)->hTree = 
			terminiKeys.InsertItem((pPrmHead+i)->lineData, 1, 1, hRoot, TVI_LAST);
		terminiKeys.SetItemData((pPrmHead+i)->hTree,i+PRM_TREE_ID_BASE);
	}
	//terminiKeys.InsertItem(_T("��˾1"), 1, 1, hRoot, TVI_LAST);
	hNet = terminiKeys.InsertItem(_T("+netParam"), 0, 0);
	for(i = 0;i< MAX_NEW_PRM_NUM;i++){
		(pNetHead+i)->hTree = 
			terminiKeys.InsertItem((pNetHead+i)->lineData, 1, 1, hNet, TVI_LAST);
		terminiKeys.SetItemData((pNetHead+i)->hTree,i+NET_TREE_ID_BASE);
	}
	fileTi.Close();
	fileNet.Close();

	cmdSocket.SetParent(this);
	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_CLOSE_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_NO_CHANGE)->EnableWindow(FALSE);
	addMinusOrNo = NO_CHANGE_UID_IP;

	GetDlgItem(IDC_NO_SEND_SCCU)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND_SCCU)->EnableWindow(FALSE);
	sendSccu = TRUE;

	sendReboot = FALSE;
	GetDlgItem(IDC_SET_REBOOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ABR_REBOOT)->EnableWindow(FALSE);

	sendAllFlag = FALSE;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CrtuToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CrtuToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CrtuToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void sleep_little_time(int times)
{
	for(int i=0;i<times;i++)
		for(int j=0;j<1000;j++)
			;
}

int AsciihexDataAddOne(char* pSrc, char bytenum)
{
	char* pSrcTmp = pSrc + bytenum -1;
	int tmp;
	char i;
	for(i = 0;i < bytenum;i++){
		tmp = (*pSrcTmp) + 1;
		//printf("tmp=%d,'a'=%d,'A'=%d,bytenum=%d,*pSrcTmp=%d\n",tmp,'a','A',bytenum,*pSrcTmp);
		if(tmp > '0' && tmp <= '9'){
			*pSrcTmp = tmp;
			break;
		}else if(tmp == ('9' + 1)){
			*pSrcTmp = 'a';
			break;
		}else if((tmp > 'a' && tmp <= 'f') || (tmp > 'A' && tmp <= 'F')){
			*pSrcTmp = tmp;
			break;
		}else if(tmp == ('f'+1) || tmp == ('F'+1)){
			//���Ͻ�һ
			*pSrcTmp = '0';
			pSrcTmp--;
		}else{
			return FAILURE;
		}
	}
	return SUCCESS;
}

// �ַ�����ʮ�������Զ���1������
int AsciihexDataMinusOne(char* pSrc, char bytenum)
{
	char* pSrcTmp = pSrc + bytenum -1;
	int tmp;
	char i;
	for(i = 0;i < bytenum;i++){
		tmp = (*pSrcTmp) - 1;
		//printf("tmp=%d,'a'=%d,'A'=%d,bytenum=%d,*pSrcTmp=%d\n",tmp,'a','A',bytenum,*pSrcTmp);
		if(tmp >= '0' && tmp <= '9'){
			*pSrcTmp = tmp;
			break;
		}else if(tmp == ('a' - 1) || tmp == ('A' - 1)){
			*pSrcTmp = '9';
			break;
		}else if((tmp >= 'a' && tmp < 'f') || (tmp >= 'A' && tmp < 'F')){
			*pSrcTmp = tmp;
			break;
		}else if(tmp == ('0'-1)){
			//������һ
			*pSrcTmp = 'f';
			pSrcTmp--;
		}else{
			return FAILURE;
		}
	}
	return SUCCESS;
}

void CrtuToolDlg::OnClickTerminiTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint pt = GetCurrentMessage()->pt;
	terminiKeys.ScreenToClient(&pt);
	UINT uFlags = 0;
	int nIndex = -1;
	
	HTREEITEM hItem = terminiKeys.HitTest(pt, &uFlags);
	
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		terminiKeys.SelectItem(hItem);
		UpdateData(TRUE);
		int nDat = terminiKeys.GetItemData(hItem); 
		if((nDat >= PRM_TREE_ID_BASE && nDat < PRM_TREE_ID_BASE+prmnum) ||
				(nDat >= NET_TREE_ID_BASE && nDat < NET_TREE_ID_BASE+MAX_NEW_PRM_NUM)){

			if(lastDat >= PRM_TREE_ID_BASE && lastDat < PRM_TREE_ID_BASE+prmnum){
				CString tmp=_T("");
				nIndex = (pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData.Find('=');
				if(nIndex > 0){
					tmp = (pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData.Left(nIndex) 
						+ '=' + terminiValue;
					(pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData = _T("");
					(pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData = tmp;
					terminiKeys.SetItemText((pPrmHead+lastDat-PRM_TREE_ID_BASE)->hTree, tmp);
				}
			}
			else if(lastDat >= NET_TREE_ID_BASE && lastDat < NET_TREE_ID_BASE+MAX_NEW_PRM_NUM){
				CString tmp=_T("");
				nIndex = (pNetHead+lastDat-NET_TREE_ID_BASE)->lineData.Find('=');
				if(nIndex > 0){
					tmp = (pNetHead+lastDat-NET_TREE_ID_BASE)->lineData.Left(nIndex) 
						+ '=' + terminiValue;
					(pNetHead+lastDat-NET_TREE_ID_BASE)->lineData = _T("");
					(pNetHead+lastDat-NET_TREE_ID_BASE)->lineData = tmp;
					terminiKeys.SetItemText((pNetHead+lastDat-NET_TREE_ID_BASE)->hTree, tmp);
				}
			}
			if(nDat >= PRM_TREE_ID_BASE && nDat < PRM_TREE_ID_BASE+prmnum){
				nIndex = (pPrmHead+nDat-PRM_TREE_ID_BASE)->lineData.Find('=');
				terminiValue = (pPrmHead+nDat-PRM_TREE_ID_BASE)->lineData.Mid(nIndex+1);
			}else{
				nIndex = (pNetHead+nDat-NET_TREE_ID_BASE)->lineData.Find('=');
				terminiValue = (pNetHead+nDat-NET_TREE_ID_BASE)->lineData.Mid(nIndex+1);
			}
			lastDat = nDat;
		}
	}
	UpdateData(FALSE);
	*pResult = 0;
}

void CrtuToolDlg::saveFiles()
{
	char dtmp[100];
	int j;
	int len;
	FILE * pT,*pN;
	//fileTi.SeekToBegin();
	//fileTi.Close();
	pT = fopen("term.ini", "wb+");
	for(int i=0;i< prmnum;i++){
		//fileTi.WriteString((pPrmHead+i)->lineData+'\n');
		len = (pPrmHead+i)->lineData.GetLength();
		memset(dtmp, 0, sizeof(dtmp));
		for(j=0;j< len;j++)
			memcpy(dtmp+j,(pPrmHead+i)->lineData.GetBuffer()+j,1);
		//fwrite(dtmp,sizeof(char),len+1,fileTi.m_pStream);
		fprintf(pT, "%s%c",dtmp,0xa);
		
//		fwrite(&an,sizeof(char),1,fileTi.m_pStream);
	}
	fclose(pT);
	//fileNet.SeekToBegin();
	//fileNet.Close();
	pN = fopen("net_params.conf", "wb+");
	for(int i=0;i< MAX_NEW_PRM_NUM;i++){
		memset(dtmp, 0, sizeof(dtmp));
		for(int j=0;j< (pNetHead+i)->lineData.GetLength();j++)
			memcpy(dtmp+j,(pNetHead+i)->lineData.GetBuffer()+j,1);
		//fwrite(dtmp,sizeof(char),(pNetHead+i)->lineData.GetLength(),fileNet.m_pStream);
		fprintf(pN, "%s%c",dtmp,0xa);
//		fwrite(&an,sizeof(char),1,fileNet.m_pStream);
	}
	fclose(pN);

	AfxMessageBox(_T("save success"));
}

void CrtuToolDlg::OnBnClickedSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CString tmp=_T("");
	if(lastDat >= PRM_TREE_ID_BASE && lastDat < PRM_TREE_ID_BASE+prmnum){
		int nIndex = (pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData.Find('=');
		if(nIndex > 0)
			tmp = (pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData.Left(nIndex) 
				+ '=' + terminiValue;
		else
			tmp = terminiValue;
		(pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData = _T("");
		(pPrmHead+lastDat-PRM_TREE_ID_BASE)->lineData = tmp;
		terminiKeys.SetItemText((pPrmHead+lastDat-PRM_TREE_ID_BASE)->hTree, tmp);
		lastDat = -1;
	}
	else if(lastDat >= NET_TREE_ID_BASE && lastDat < NET_TREE_ID_BASE+MAX_NEW_PRM_NUM){
		int nIndex = (pNetHead+lastDat-NET_TREE_ID_BASE)->lineData.Find('=');
		if(nIndex > 0)
			tmp = (pNetHead+lastDat-NET_TREE_ID_BASE)->lineData.Left(nIndex) 
				+ '=' + terminiValue;
		else
			tmp = terminiValue;
		(pNetHead+lastDat-NET_TREE_ID_BASE)->lineData = _T("");
		(pNetHead+lastDat-NET_TREE_ID_BASE)->lineData = tmp;
		terminiKeys.SetItemText((pNetHead+lastDat-NET_TREE_ID_BASE)->hTree, tmp);
		lastDat = -1;
	}
	saveFiles();
}

void CrtuToolDlg::OnBnClickedSure()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str=_T("");
	UpdateData(TRUE);
	if(maxAddNum < MAX_ADD_NUM && addKey.CompareNoCase(str) != 0){
		maxAddNum++;
		(pPrmHead+prmnum-1)->lineData = addKey+'=';
		terminiKeys.SetItemText((pPrmHead+prmnum-1)->hTree, (pPrmHead+prmnum-1)->lineData);
		(pPrmHead+prmnum)->hTree = 
			terminiKeys.InsertItem((pPrmHead+prmnum)->lineData, 1, 1, hRoot, TVI_LAST);
		terminiKeys.SetItemData((pPrmHead+prmnum)->hTree,prmnum+1);
		prmnum++;
		addKey="";
		UpdateData(FALSE);
	}
}
unsigned int UTSwapInt(unsigned int num)
{
	return  ((num >> 24) & 0x000000ff) |
		((num >>  8) & 0x0000ff00) |
		((num <<  8) & 0x00ff0000) |
		((num << 24) & 0xff000000);
}
void CrtuToolDlg::changeUidIp()
{
	CString str,tmp;
	in_addr saddr;
	char dtmp[100]={0};
	int i,nIndex=-1;
	char *pTmp;
	char asciiHex[20]={0},offset=0;
	unsigned int ipaddr=0,lowadd;
	if(addMinusOrNo == NO_CHANGE_UID_IP)
		return ;
	for(i=0;i<prmnum;i++){
		if((pPrmHead+i)->lineData.Find(_T("cc_uid")) == 0 &&
				(pPrmHead+i)->lineData.Find(_T("#cc_uid")) != 0){
			nIndex = (pPrmHead+i)->lineData.Find('=');
			str = (pPrmHead+i)->lineData.Mid(nIndex+1);
			for(int j=0;j< str.GetLength();j++)
				memcpy(dtmp+j,str.GetBuffer()+j,1);
			if(addMinusOrNo == ADD_UID_IP)
				AsciihexDataAddOne(dtmp, strlen(dtmp));
			else
				AsciihexDataMinusOne(dtmp, strlen(dtmp));
			str = dtmp;
			tmp = (pPrmHead+i)->lineData.Left(nIndex) 
						+ '=' + str;
			(pPrmHead+i)->lineData = tmp;
			terminiKeys.SetItemText((pPrmHead+i)->hTree, tmp);
		}
	}
	for(i=0;i<7;i++){
		if((pNetHead+i)->lineData.Find(_T("IPADDR")) == 0 &&
				(pNetHead+i)->lineData.Find(_T("#IPADDR")) != 0){
			memset(dtmp,0 ,sizeof(dtmp));
			nIndex = (pNetHead+i)->lineData.Find('=');
			str = (pNetHead+i)->lineData.Mid(nIndex+1);
			for(int j=0;j< str.GetLength();j++)
				memcpy(dtmp+j,str.GetBuffer()+j,1);
			ipaddr = inet_addr(dtmp);
			ipaddr = UTSwapInt(ipaddr);
			lowadd = ipaddr&0x0ff;
			//printf("ip:lowadd=%d\n",lowadd);
			if(addMinusOrNo == ADD_UID_IP){
				if(lowadd >= 253){
					ipaddr -= lowadd;
					ipaddr += 2;
				}else{
					ipaddr += 1;
				}
			}
			else{
				if(lowadd <= 2){
					ipaddr -= lowadd;
					ipaddr += 253;
				}else{
					ipaddr -= 1;
				}
			}
			
			ipaddr = UTSwapInt(ipaddr);
			saddr.s_addr = ipaddr;
			memset(dtmp, 0, sizeof(dtmp));
			//תIP�ַ���
			strcpy(dtmp,inet_ntoa(saddr));
			str = dtmp;
			tmp = (pNetHead+i)->lineData.Left(nIndex) 
						+ '=' + str;
			(pNetHead+i)->lineData = tmp;
			terminiKeys.SetItemText((pNetHead+i)->hTree, tmp);
		}
		if((pNetHead+i)->lineData.Find(_T("MAC_ADDR")) == 0 &&
				(pNetHead+i)->lineData.Find(_T("#MAC_ADDR")) != 0){
			memset(dtmp,0 ,sizeof(dtmp));
			nIndex = (pNetHead+i)->lineData.Find('=');
			str = (pNetHead+i)->lineData.Mid(nIndex+1);
			for(int j=0;j< str.GetLength();j++)
				memcpy(dtmp+j,str.GetBuffer()+j,1);
			pTmp = dtmp;
			do{
				if(*pTmp == ':')
					pTmp++;
				while(*pTmp !='\0' && *pTmp != ':'){
					asciiHex[offset++] = *pTmp;
					pTmp++;
				}
				pTmp++;
			}while(*pTmp !='\0');
			
			if(addMinusOrNo == ADD_UID_IP)
				AsciihexDataAddOne(asciiHex, strlen(asciiHex));
			else
				AsciihexDataMinusOne(asciiHex, strlen(asciiHex));

			memset(dtmp, 0, sizeof(dtmp));
			//len = strlen(asciiHex);
			offset = 0;
			for(int j=0;j<12;j+=2){
				memcpy(dtmp+offset, asciiHex + j, 2);
				offset +=2;
				if(offset < 15)
					dtmp[offset] =':';
				offset++;
			}
			str = dtmp;
			tmp = (pNetHead+i)->lineData.Left(nIndex) 
						+ '=' + str;
			(pNetHead+i)->lineData = tmp;
			terminiKeys.SetItemText((pNetHead+i)->hTree, tmp);
		}
	}
	saveFiles();
}

#define FTP_CTRL_PORT	21
void CrtuToolDlg::OnBnClickedConnectB()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(sFlag == FALSE){
		sFlag = cmdSocket.Create();
	}
	UpdateData(TRUE);
	if(SOCKET_ERROR == cmdSocket.Connect((LPCTSTR)connectIp, FTP_CTRL_PORT))
	{
		//AfxMessageBox("connect failure");
		MessageBox(_T("����ʧ��"), _T("Warning!!"), MB_OK | MB_ICONWARNING);
		return;
	}
	
	GetDlgItem(IDC_CONNECT_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_CLOSE_B)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(TRUE);
}


void CrtuToolDlg::OnBnClickedCloseB()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(sFlag != FALSE){
		char buf[10];
		sprintf( buf, "QUIT\r\n");
		cmdSocket.Send(buf,strlen(buf));
		cmdSocket.Close();
		sFlag = FALSE;
	}
	if(dsFlag != FALSE){
		dataSocket.Close();
		dsFlag = FALSE;
	}
	GetDlgItem(IDC_CONNECT_B)->EnableWindow(TRUE);
	GetDlgItem(IDC_CLOSE_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(FALSE);
}

char CrtuToolDlg::ftpWaitReport(char send[])
{
	char buf[1025];
	int times=0;
	int result=0;
	char ret=FALSE;
	CString strRecvd;
	while(cmdSocket.Receive(buf, sizeof(buf)) <= 0){
		if(times++ > 1000000){
			strRecvd = send;
			MessageBox(strRecvd);
			break;
		}
	}
	sscanf(buf, "%d", &result );
	switch(result)
	{
	case 110:case 120:case 125:case 150:
	case 200:case 220:case 225:case 227:case 230:case 250:case 226:
	case 331:
		{
		//�ɹ�
			if(result == 220)
			{
				sprintf( buf, "USER %s\r\n", "root" );
				cmdSocket.Send(buf,strlen(buf));
			}
			/*if(result == 226){
				char errorData[200]={0};
				sprintf(errorData,"226,%d:��%s",result,buf);
				strRecvd = errorData;
				MessageBox(strRecvd);
			}*/
			if(result == 331)
			{
				sprintf( buf, "PASS %s\r\n", "linux" );
				cmdSocket.Send(buf,strlen(buf));
			}
			if(result == 227){
				int addr[6];
				int port;
				char ip[50]={0};
				//д�뱻��ģʽ��ip�Ͷ˿�
				sscanf(buf, "%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
				sprintf(ip, "%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
				port = addr[4]*256+addr[5];
				if(dsFlag == FALSE)
					dsFlag = dataSocket.Create();
				if(SOCKET_ERROR == dataSocket.Connect((LPCTSTR)connectIp, port))
				{
					//AfxMessageBox("connect failure");
					MessageBox(_T("���ݿ�����ʧ��"), _T("Warning!!"), MB_OK | MB_ICONWARNING);
				}
			}
			ret = TRUE;
		}
		break;
	case 221:case 421:
		sprintf( buf, "QUIT\r\n");
		cmdSocket.Send(buf,strlen(buf));
	case 202:
	default:
		{
			char errorData[200]={0};
			sprintf(errorData,"����%d:��%s\n��:%s",result,send,buf);
			strRecvd = errorData;
			MessageBox(strRecvd);
			/*if(dsFlag != FALSE){
				dataSocket.Close();
				dsFlag = FALSE;
			}*/
		}
		break;
	}
	return ret;
}

void CrtuToolDlg::ftpStorFileFirst()
{
	char buf[100];
	sprintf(buf, "TYPE %c\r\n", 'I' );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	sprintf( buf, "PASV\r\n");
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
}

void CrtuToolDlg::OnBnClickedSendToRtu()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	sendAllFlag = TRUE;
	sendSuccess = FALSE;
	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(FALSE);
	OnBnClickedSendTerm();
	if(sendSuccess == TRUE){
		sendSuccess = FALSE;
		OnBnClickedSendIp();
	}
	if(sendSuccess == TRUE && sendSccu == TRUE){
		sendSuccess = FALSE;
		OnBnClickedOnlySendSccu();
	}
	
	if(sendSuccess == TRUE)
		MessageBox(_T("����term.ini,net_params.conf,sccu�ɹ�"));
	
	sendAllFlag = FALSE;
	if(sendSuccess == TRUE && sendReboot == TRUE){
		OnBnClickedReboot();
	}
	if(sendSuccess == TRUE && addMinusOrNo != NO_CHANGE_UID_IP)
		changeUidIp();
	GetDlgItem(IDC_SEND_TO_RTU)->EnableWindow(TRUE);
}

void CrtuToolDlg::OnBnClickedSendTerm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = FALSE;
	char buf[50]={0};
	char filedata[50]={0};
	int datalen=0;
	FILE * pT;
	if(sFlag == FALSE){
		OnBnClickedConnectB();
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
	}
	if(dsFlag != FALSE){
		dataSocket.Close();
		dsFlag = FALSE;
	}
	ftpStorFileFirst();
	if(dsFlag == FALSE){
		MessageBox(_T("����term.iniʧ��1"));
		return ;
	}
	sprintf(buf, "CWD %s\r\n", "/nand" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	//wait250 = 0;
	sprintf(buf, "STOR %s\r\n", "term.ini.good");
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	
	pT = fopen("term.ini", "rb+");
	datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
	while(datalen > 0){
		dataSocket.Send(filedata,datalen);
		datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
		ret = TRUE;
	}
	fclose(pT);
	dataSocket.Close();
	dsFlag = FALSE;
	ftpWaitReport(buf);

	if(ret == TRUE){
		if(sendAllFlag == FALSE)
			MessageBox(_T("����term.ini�ɹ�"));
		sendSuccess = TRUE;
	}else{
		MessageBox(_T("����term.iniʧ��"));
	}
	/*sprintf(buf, "CDUP\r\n");
	//sprintf(buf, "CWD %s\r\n", "../" );
	cmdSocket.Send(buf, sizeof(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;*/
	/*sprintf(buf, "REIN\r\n");
	cmdSocket.Send(buf, sizeof(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;*/
//	cmdSocket.Close();
//	sFlag = FALSE;
}


void CrtuToolDlg::OnBnClickedSendIp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	int ret = FALSE;
	char buf[50]={0};
	char filedata[50]={0};
	int datalen=0;
	FILE * pT;
	if(sFlag == FALSE){
		OnBnClickedConnectB();
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
	}
	if(dsFlag != FALSE){
		dataSocket.Close();
		dsFlag = FALSE;
	}
	ftpStorFileFirst();
	if(dsFlag == FALSE){
		MessageBox(_T("����net_params.confʧ��1"));
		return ;
	}
	sprintf(buf, "CWD %s\r\n", "/nor" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	
	sprintf(buf, "STOR %s\r\n", "net_params.conf");
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	//wait250 = 0;
	pT = fopen("net_params.conf", "rb+");
	datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
	while(datalen > 0){
		dataSocket.Send(filedata,datalen);
		datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
		ret = TRUE;
	}
	//dataSocket.Send(filedata,datalen);
	dataSocket.Close();
	dsFlag = FALSE;
	ftpWaitReport(buf);
	fclose(pT);
	if(ret == TRUE){
		if(sendAllFlag == FALSE)
			MessageBox(_T("����net_params.conf�ɹ�"));
		sendSuccess = TRUE;
	}else{
		MessageBox(_T("����net_params.confʧ��"));
	}

/*	sprintf(buf, "CDUP\r\n");
	//sprintf(buf, "CWD %s\r\n", "../" );
	cmdSocket.Send(buf, sizeof(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;*/
//	cmdSocket.Close();
//	sFlag = FALSE;
}

CString CrtuToolDlg::GetErrorMsg()
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0, GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Ĭ������
		(LPTSTR)&lpMsgBuf, 0, NULL );
	//��ʾ
	//MessageBox(0,(LPCTSTR)lpMsgBuf,_T("GetLastError"),MB_OK|MB_ICONINFORMATION );
	CString m_strError;
	m_strError.Format(_T("errorNum��%d\nerrorData��%s"),GetLastError(), (LPCTSTR)lpMsgBuf);
	
	//�ͷ��ڴ�
	::LocalFree( lpMsgBuf );
    return m_strError;
}

void CrtuToolDlg::OnReceive()
{
	char buf[1025];
	int iBufSize = 1024;
	int iRcvd,result;
	CString strRecvd;
	iRcvd = cmdSocket.Receive(buf,iBufSize);
	if(iRcvd == SOCKET_ERROR)
	{
		CString m_ErrorMsg;
		m_ErrorMsg = GetErrorMsg();
		MessageBox(m_ErrorMsg);
		return;
	}
	else
	{
		buf[iRcvd] = 0;
		strRecvd = buf;
		//strRecvd = "�Է�˵ : "+strRecvd; 
		//MessageBox(strRecvd);
		sscanf(buf, "%d", &result );
		UpdateData(FALSE);
		switch(result)
		{
		case 150:
			{
				// �ļ�OK
			}
			break;
		case 200:
			{
				// ����ģʽ�ɹ�
			/*	sprintf( buf, "PASV\r\n");
				cmdSocket.Send(buf, strlen(buf));*/
			}
			break;
		case 220:
			{
				sprintf( buf, "USER %s\r\n", "root" );
				cmdSocket.Send(buf,strlen(buf));
			}
			break;
		case 227:
			{
				/*int addr[6];
				int port;
				char ip[50]={0};
				//д�뱻��ģʽ��ip�Ͷ˿�
				sscanf(buf, "%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
				sprintf(ip, "%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
				port = addr[4]*256+addr[5];
				if(dsFlag == FALSE)
					dsFlag = dataSocket.Create();
				if(SOCKET_ERROR == dataSocket.Connect((LPCTSTR)connectIp, port))
				{
					//AfxMessageBox("connect failure");
					MessageBox(_T("���ݿ�����ʧ��"), _T("Warning!!"), MB_OK | MB_ICONWARNING);
				}*/
			}
			break;
		case 230:
			{
				//��¼�ɹ�
				/*sprintf(buf, "TYPE %c\r\n", 'I' );
				cmdSocket.Send(buf, strlen(buf));*/
			}
			break;
		case 250:
			{
				//wait250 = 250;
			}
			break;
		case 331:
			{
				sprintf( buf, "PASS %s\r\n", "linux" );
				cmdSocket.Send(buf,strlen(buf));
			}
			break;
		default:
			break;
		}
	}
}

void CrtuToolDlg::OnBnClickedAdd1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_ADD_1)->EnableWindow(FALSE);
	GetDlgItem(IDC_MINUS_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_NO_CHANGE)->EnableWindow(TRUE);
	addMinusOrNo = ADD_UID_IP;
}

void CrtuToolDlg::OnBnClickedMinus1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_ADD_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_MINUS_1)->EnableWindow(FALSE);
	GetDlgItem(IDC_NO_CHANGE)->EnableWindow(TRUE);
	addMinusOrNo = MINUS_UID_IP;
}

void CrtuToolDlg::OnBnClickedNoChange()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_ADD_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_MINUS_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_NO_CHANGE)->EnableWindow(FALSE);
	addMinusOrNo = NO_CHANGE_UID_IP;
}
void CrtuToolDlg::deleteDbdatFile()
{
	char buf[50]={0};
	if(sFlag == FALSE){
		OnBnClickedConnectB();
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
	}
	ftpStorFileFirst();
	if(dsFlag == FALSE){
		MessageBox(_T("ɾ��db.datʧ��1"));
		return ;
	}
	sprintf(buf, "CWD %s\r\n", "/nand/sqlite" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	sprintf(buf, "DELE %s\r\n", "db.dat" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		;
//	cmdSocket.Close();
//	sFlag = FALSE;
}
void CrtuToolDlg::OnBnClickedOnlySendSccu()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = FALSE;
	char buf[50]={0};
	char filedata[500]={0};
	int datalen=0;
	FILE * pT;

	/*****/
	deleteDbdatFile();
	if(sFlag != FALSE){
		cmdSocket.Close();
		sFlag = FALSE;
	}
	/*****/
	//sleep_little_time(1);
	if(sFlag == FALSE){
		OnBnClickedConnectB();
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
	}

	sprintf(buf, "CWD %s\r\n", "/nand" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	sprintf(buf, "DELE %s\r\n", "sccu" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		;//return;
	if(dsFlag != FALSE){
		dataSocket.Close();
		dsFlag = FALSE;
	}
	ftpStorFileFirst();
	if(dsFlag == FALSE){
		MessageBox(_T("����sccuʧ��1"));
		return ;
	}
	sprintf(buf, "STOR %s\r\n", "sccu");
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	//wait250 = 0;
	pT = fopen("sccu", "rb+");
	datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
	while(datalen > 0){
		dataSocket.Send(filedata,datalen);
		datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
		ret = TRUE;
	}
	fclose(pT);
	dataSocket.Close();
	dsFlag = FALSE;
	ftpWaitReport(buf);

	if(ret == TRUE){
		if(sendAllFlag == FALSE)
			MessageBox(_T("����sccu�ɹ�"));
		sendSuccess = TRUE;
	}else{
		MessageBox(_T("����sccuʧ��"));
	}
	/*sprintf(buf, "CDUP\r\n");
	//sprintf(buf, "CWD %s\r\n", "../" );
	cmdSocket.Send(buf, sizeof(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;*/
//	cmdSocket.Close();
//	sFlag = FALSE;
}


void CrtuToolDlg::OnBnClickedSendSccu()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_NO_SEND_SCCU)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND_SCCU)->EnableWindow(FALSE);
	sendSccu = TRUE;
}


void CrtuToolDlg::OnBnClickedNoSendSccu()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_NO_SEND_SCCU)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND_SCCU)->EnableWindow(TRUE);
	sendSccu = FALSE;
}


void CrtuToolDlg::OnBnClickedSendAutorun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = FALSE;
	char buf[50]={0};
	char filedata[500]={0};
	int datalen=0;
	FILE * pT;

	if(sFlag != FALSE){
		cmdSocket.Close();
		sFlag = FALSE;
	}
	
//	sleep_little_time(1);
	if(sFlag == FALSE){
		OnBnClickedConnectB();
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
		if(ftpWaitReport(buf) == FALSE)
			return;
	}

	sprintf(buf, "CWD %s\r\n", "/nor" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	/*sprintf(buf, "DELE %s\r\n", "autorun" );
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;*/
	if(dsFlag != FALSE){
		dataSocket.Close();
		dsFlag = FALSE;
	}
	ftpStorFileFirst();
	if(dsFlag == FALSE){
		MessageBox(_T("����sccuʧ��1"));
		return ;
	}
	sprintf(buf, "STOR %s\r\n", "autorun.sh");
	cmdSocket.Send(buf, strlen(buf));
	if(ftpWaitReport(buf) == FALSE)
		return;
	//wait250 = 0;
	pT = fopen("autorun.sh", "rb+");
	datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
	while(datalen > 0){
		dataSocket.Send(filedata,datalen);
		datalen = fread(filedata,sizeof(char),sizeof(filedata),pT);
		ret = TRUE;
	}
	fclose(pT);
	dataSocket.Close();
	dsFlag = FALSE;
	ftpWaitReport(buf);

	if(ret == TRUE){
		if(sendAllFlag == FALSE)
			MessageBox(_T("����autorun.sh�ɹ�"));
	}else{
		MessageBox(_T("����autorun.shʧ��"));
	}

//	cmdSocket.Close();
//	sFlag = FALSE;
}


void CrtuToolDlg::OnBnClickedSetReboot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	sendReboot = TRUE;
	GetDlgItem(IDC_SET_REBOOT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ABR_REBOOT)->EnableWindow(TRUE);
}

void CrtuToolDlg::OnBnClickedAbrReboot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	sendReboot = FALSE;
	GetDlgItem(IDC_SET_REBOOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ABR_REBOOT)->EnableWindow(FALSE);
}

#if 0
char *mystrstr(const char *s1,const char *s2)
{
	int n;
	if(*s2)
	{
		while(*s1)
		{
			for(n=0;*(s1+n)==*(s2+n);n++)
			{
				if(!*(s2+n+1))
					return (char*)s1;
			}
			s1++;
		}
		return NULL;
	}
	else
		return (char*)s1;
}
#endif

char CrtuToolDlg::telnetWaitReport()
{
	char buf[200]={0};
	int times=0;
	char ret=FALSE;
	CString strRecvd;

	while(telnetSocket.Receive(buf, sizeof(buf)) <= 0){
		if(times++ > 1000000){
			MessageBox(_T("����ʧ��2"), _T("Warning!!"), MB_OK | MB_ICONWARNING);
			break;
		}
	}
	strRecvd = buf;
	MessageBox(strRecvd);
	if(strstr(buf,"login:") || strstr(buf,"Password:")
			|| strstr(buf,"~ #") || strstr(buf,"root"))
		return TRUE;
	//sprintf(errorData,"226,%d:��%s",result,buf);
	//telnetSocket.Close();
	return ret;
}

#define TELNET_PORT 23
void CrtuToolDlg::OnBnClickedReboot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char buf[100];
	if(tFlag != FALSE)
		telnetSocket.Close();
	tFlag = telnetSocket.Create();
	
	UpdateData(TRUE);
	if(SOCKET_ERROR == telnetSocket.Connect((LPCTSTR)connectIp, TELNET_PORT))
	{
		//AfxMessageBox("connect failure");
		MessageBox(_T("����ʧ��1"), _T("Warning!!"), MB_OK | MB_ICONWARNING);
		return;
	}
	sleep_little_time(10000);
	telnetWaitReport();
	sleep_little_time(100);
	sprintf(buf, "root\n");
	telnetSocket.Send(buf, strlen(buf));
	if(telnetWaitReport() == FALSE){
		MessageBox(_T("����ʧ��2"));
		return;
	}
	sleep_little_time(100);
	sprintf(buf, "linux\n");
	telnetSocket.Send(buf, strlen(buf));
	if(telnetWaitReport() == FALSE){
		MessageBox(_T("����ʧ��3"));
		return;
	}
	sleep_little_time(100);
	sprintf(buf, "reboot\n");
	telnetSocket.Send(buf, strlen(buf));
	telnetSocket.Send(buf, strlen(buf));
	sleep_little_time(10);
	MessageBox(_T("�����ɹ�"));
	if(sendSuccess == TRUE){
		if(sFlag != FALSE){
			cmdSocket.Close();
			sFlag = FALSE;
		}
	}
	telnetSocket.Close();
	tFlag = FALSE;
}

