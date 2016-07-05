
// rtuToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#define MAX_ADD_NUM	15
#define MAX_NEW_PRM_NUM	8
#define PRM_TREE_ID_BASE	1
#define NET_TREE_ID_BASE	1000

#define FAILURE -1
#define SUCCESS 0

#define NO_CHANGE_UID_IP	0
#define ADD_UID_IP	1
#define MINUS_UID_IP	2

class CSinglePrm
{
public:
	CString lineData;
	HTREEITEM hTree;
};

// CrtuToolDlg 对话框
class CrtuToolDlg : public CDialogEx
{
// 构造
public:
	CrtuToolDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CrtuToolDlg();
	//int AsciihexDataAddOne(char* pSrc, char bytenum);
	//int AsciihexDataMinusOne(char* pSrc, char bytenum);
	void changeUidIp();
	CString GetErrorMsg();
	void ftpStorFileFirst();
	char ftpWaitReport(char send[]);
	char telnetWaitReport();
	void deleteDbdatFile();
	void OnReceive();
// 对话框数据
	enum { IDD = IDD_RTUTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	void saveFiles();
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int sFlag,dsFlag,tFlag;
	//CSocket cmdSocket;
	CMysocket cmdSocket;
	CSocket dataSocket;
	CSocket telnetSocket;

	// 用来显示term.ini里的参数名
	CTreeCtrl terminiKeys;
	HTREEITEM hRoot;
	HTREEITEM hNet;
	CStdioFile fileTi;
	CStdioFile fileNet;
	CSinglePrm *pPrmHead;
	CSinglePrm *pNetHead;
	int prmnum;
	int maxAddNum;
	int lastDat;
	int sendReboot;
	char addMinusOrNo,sendSccu,sendAllFlag;
	char sendSuccess;
		
	CString connectIp;
	CString addKey;
	CString terminiValue;
	afx_msg void OnClickTerminiTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSure();
	afx_msg void OnBnClickedConnectB();
	afx_msg void OnBnClickedCloseB();
	afx_msg void OnBnClickedSendToRtu();
	afx_msg void OnBnClickedSendTerm();
	afx_msg void OnBnClickedSendIp();
	afx_msg void OnBnClickedAdd1();
	afx_msg void OnBnClickedMinus1();
	afx_msg void OnBnClickedNoChange();
	afx_msg void OnBnClickedOnlySendSccu();
	afx_msg void OnBnClickedSendSccu();
	afx_msg void OnBnClickedNoSendSccu();
	afx_msg void OnBnClickedSendAutorun();
	afx_msg void OnBnClickedSetReboot();
	afx_msg void OnBnClickedReboot();
	afx_msg void OnBnClickedAbrReboot();
};
