
// clientDlg.h : header file
//

#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include "EditListCtrl.h"
#include <map>
#include "afxcmn.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

extern compiler::DiskSourceTree s_cDirTree;
extern compiler::Importer s_importer;
extern DynamicMessageFactory s_messageFactory;

typedef std::map<std::string, const Descriptor*> MapDescriptor;

typedef unsigned int(__stdcall * PFN)(void *);

// CclientDlg dialog
class CclientDlg : public CDialogEx
{
// Construction
public:
	CclientDlg(CWnd* pParent = NULL);	// standard constructor
	~CclientDlg();

	BOOL IsMessageCell(int iCol)const;
	void ShowMessageCell(int iRow, int iCol);
	BOOL SetUIMessage(int iRow, int iCol, CString str);

	static  unsigned int RecvMessage(void* pThis);
	int ProcessRecv();

// Dialog Data
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditPath();
	afx_msg void OnBnClickedButtonGen();
	afx_msg void OnChooseDir();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedButtonPreview();
private:
	void GenerateDesc(CString strProtoName);
	void ErrorMessage(const std::string& strError, const char* lpszCaption = "error");
	std::string reverseBackSlash(const string& str);
	void BuildTree(const MapDescriptor& mapDesc);
	void InsertChildProto(HTREEITEM hMainProto, const Descriptor* pDesc);
	void ShowContentInList();
	BOOL IsRepeatedField(int iCol);
	BOOL CheckNumber(CString str);
	BOOL InitData();
	BOOL ConnectServer();
	BOOL Send();

	void OnOK();
	void OnCancel();


private:
	MapDescriptor m_mapDesc;
	CTreeCtrl m_wdnProtoTree;
	CEditListCtrl m_wndProtoContentList;

	struct UIProtoData {
		const Descriptor* pDesc;
		Message* pMsg;
	};

	typedef std::stack<UIProtoData> UIDescStack;
	UIDescStack m_stackUIDesc;

	CString m_strFind;
	CString m_strProtoPath;
	CString m_strIP;
	CString m_strPort;
	SOCKET m_ConnectSocket;
	int m_iCMd;
	int m_iRecvCmd;
	HANDLE m_hRecvThread;
	CString m_strRecvProto;

public:
	afx_msg void OnTvnSelchangedTreeProto(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnEnChangeEditFindProto();
	afx_msg void OnSetting();
	afx_msg void OnRefreshProto();
	afx_msg void OnConnect();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnDisconnect();
};
