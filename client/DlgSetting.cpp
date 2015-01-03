// DlgSetting.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "DlgSetting.h"
#include "afxdialogex.h"
#include "tinyxml2.h"


// CDlgSetting dialog

IMPLEMENT_DYNAMIC(CDlgSetting, CDialog)

CDlgSetting::CDlgSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetting::IDD, pParent)
	, m_strRoleId(_T(""))
	, m_strAccountId(_T(""))
	, m_strProtoPath(_T(""))
	, m_strIP(_T(""))
	, m_strPort(_T(""))
{

}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROLE_ID, m_strRoleId);
	DDX_Text(pDX, IDC_EDIT_ACCOUT_ID, m_strAccountId);
	DDX_Text(pDX, IDC_EDIT_PROTO_PATH, m_strProtoPath);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strPort);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_PROTO_PATH, &CDlgSetting::OnBnClickedButtonProtoPath)
END_MESSAGE_MAP()


// CDlgSetting message handlers

BOOL CDlgSetting::OnInitDialog() {
	CDialog::OnInitDialog();

	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_NO_ERROR != doc.LoadFile(XML_SETTING_NAME)) {
		return TRUE;
	}


	tinyxml2::XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot) {
		return TRUE;
	}

	tinyxml2::XMLElement* pEle = pRoot->FirstChildElement("proto");
	if (NULL != pEle) {
		const char* lpszProto = pEle->Attribute("path");
		if (NULL != lpszProto) {
			m_strProtoPath = MB2WC(lpszProto);
		}
	}

	pEle = pRoot->FirstChildElement("login");
	if (NULL != pEle) {
		const char* lpszData = pEle->Attribute("role_id");
		if (NULL != lpszData) {
			m_strRoleId = MB2WC(lpszData);
		}
		lpszData = pEle->Attribute("account_id");
		if (NULL != lpszData) {
			m_strAccountId = MB2WC(lpszData);
		}
	}

	pEle = pRoot->FirstChildElement("net");
	if (NULL != pEle) {
		const char* lpszData = pEle->Attribute("ip");
		if (NULL != lpszData) {
			m_strIP = MB2WC(lpszData);
		}
		lpszData = pEle->Attribute("port");
		if (NULL != lpszData) {
			m_strPort = MB2WC(lpszData);
		}
	}

	UpdateData(FALSE);

	return TRUE;

}

void CDlgSetting::OnBnClickedOk()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}


void CDlgSetting::OnBnClickedButtonProtoPath()
{
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		return;
	}

	WCHAR szPath[MAX_PATH];
	BROWSEINFOW info;
	info.hwndOwner = GetSafeHwnd();
	info.pidlRoot = NULL;
	info.pszDisplayName = szPath;
	info.lpszTitle = L"specify...";
	info.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	info.lpfn = NULL;
	info.lParam = 0;
	info.iImage = 0;

	PIDLIST_ABSOLUTE pid = SHBrowseForFolderW(&info);
	if (NULL == pid) {
		return;
	}

	if (!SHGetPathFromIDListW(pid, szPath)) {
		CoTaskMemFree(pid);
		return;
	}

	CoTaskMemFree(pid);
	m_strProtoPath = szPath;
	UpdateData(FALSE);
}

