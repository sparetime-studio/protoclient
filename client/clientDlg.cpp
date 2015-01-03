
// clientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"
#include <stack>
#include "DlgSetting.h"
#include "tinyxml2.h"
#include <fstream>
#include <Winsock2.h>



compiler::DiskSourceTree s_cDirTree;
compiler::Importer s_importer(&s_cDirTree, NULL);
DynamicMessageFactory s_messageFactory;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CclientDlg dialog



CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CclientDlg::IDD, pParent)
	, m_strProtoPath(_T(""))
	, m_strFind(_T("")), m_ConnectSocket(0)
	, m_iCMd(0)
	, m_iRecvCmd(0), m_hRecvThread(NULL)
	, m_strRecvProto(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strProtoPath);
	DDX_Control(pDX, IDC_TREE_PROTO, m_wdnProtoTree);
	DDX_Control(pDX, IDC_LIST_PROTO_CONTENT, m_wndProtoContentList);
	DDX_Text(pDX, IDC_EDIT_FIND_PROTO, m_strFind);
	DDX_Text(pDX, IDC_EDIT_CMD, m_iCMd);
	DDX_Text(pDX, IDC_EDIT_RECV_CMD, m_iRecvCmd);
	DDX_Text(pDX, IDC_EDIT_RECV_PROTO, m_strRecvProto);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CclientDlg::OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CclientDlg::OnBnClickedButtonPreview)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROTO, &CclientDlg::OnTvnSelchangedTreeProto)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CclientDlg::OnBnClickedButtonAdd)
	ON_EN_CHANGE(IDC_EDIT_FIND_PROTO, &CclientDlg::OnEnChangeEditFindProto)
	ON_COMMAND(ID_SETTING, &CclientDlg::OnSetting)
	ON_COMMAND(ID_REFRESH_PROTO, &CclientDlg::OnRefreshProto)
	ON_COMMAND(ID_CONNECT, &CclientDlg::OnConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CclientDlg::OnBnClickedButtonSend)
	ON_COMMAND(ID_DISCONNECT, &CclientDlg::OnDisconnect)
END_MESSAGE_MAP()


// CclientDlg message handlers

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_wndProtoContentList.SetExtendedStyle(m_wndProtoContentList.GetExtendedStyle() | LVS_EX_GRIDLINES);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);

	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DISCONNECT, MF_GRAYED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_CONNECT, MF_ENABLED);

	InitData();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::string CclientDlg::reverseBackSlash(const string& str) {
	std::string s;
	s.reserve(str.length());
	for (unsigned int i = 0; i < str.length(); ++i) {
		if (str[i] != '\\') {
			s += str[i];
		}
		else {
			s += '/';
		}
	}
	return s;
}

void CclientDlg::OnBnClickedButtonGen()
{
	UpdateData(FALSE);

	std::string strPath = reverseBackSlash(WC2MB(m_strProtoPath));
	s_cDirTree.MapPath("", strPath);

	std::stack<CString> stackDir;
	stackDir.push(m_strProtoPath + _T("\\*.proto"));

	m_mapDesc.clear();

	while (!stackDir.empty()) {
		CFileFind finder;
		BOOL bWorking = finder.FindFile(stackDir.top());
		stackDir.pop();
		while (bWorking) {
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory()) {
				if (finder.GetFilePath() == m_strProtoPath)
				{
					continue;
				}
				stackDir.push(finder.GetFilePath() + _T("\\*.proto"));
				continue;
			}
			TRACE(_T("***************>>%s\n"), (LPCTSTR)finder.GetFilePath());
			CString strFileName = finder.GetFileName();
			GenerateDesc(strFileName);
		}
		finder.Close();
	}

	BuildTree(m_mapDesc);
}

void CclientDlg::InsertChildProto(HTREEITEM hMainProto, const Descriptor* pDesc) {
	for (int i = 1; i <= pDesc->field_count(); ++i) {
		const FieldDescriptor* pFieldDesc = pDesc->FindFieldByNumber(i);
		std::string str = pFieldDesc->name();
		str += ": ";
		if (FieldDescriptor::TYPE_MESSAGE == pFieldDesc->type()) {
			str += pFieldDesc->message_type()->name();
		}
		else {
			str += pFieldDesc->type_name();
		}

		HTREEITEM hItem = m_wdnProtoTree.InsertItem(MB2WC(str), hMainProto);
		m_wdnProtoTree.SetItemData(hItem, (DWORD_PTR)pFieldDesc);
	}
}

void CclientDlg::BuildTree(const MapDescriptor& mapDesc) {
	m_wdnProtoTree.DeleteAllItems();

	for (MapDescriptor::const_iterator iter = mapDesc.begin(); iter != mapDesc.end(); ++iter) {
		const Descriptor* pDesc = iter->second;
		HTREEITEM hMainProto = m_wdnProtoTree.InsertItem(MB2WC(pDesc->name()));
		InsertChildProto(hMainProto, pDesc);
		m_wdnProtoTree.SetItemData(hMainProto, (DWORD_PTR)pDesc);
	}
}


void CclientDlg::ErrorMessage(const std::string& strError, const char* lpszCaption) {
	MessageBoxA(this->GetSafeHwnd(), strError.c_str(), lpszCaption, MB_OK);
}

void CclientDlg::GenerateDesc(CString strProtoName) {

	std::string strPath = WC2MB(strProtoName);
	const FileDescriptor* pFileDesc = s_importer.Import(strPath);
	if (NULL == pFileDesc) {
		ErrorMessage(std::string("fail to import: ") + strPath);
		return;
	}

	for (int i = 0; i < pFileDesc->message_type_count(); ++i) {
		const Descriptor* pMessageDesc = pFileDesc->message_type(i);
		if (NULL == pMessageDesc) {
			ErrorMessage("fail to get message descriptor in file: " + strPath);
			continue;
		}
		m_mapDesc.insert(std::make_pair(pMessageDesc->name(), pMessageDesc));
	}
}


void CclientDlg::OnBnClickedButtonBack()
{
	// TODO: Add your control notification handler code here

	m_stackUIDesc.pop();
	ShowContentInList();
	if (m_stackUIDesc.size() == 1) {
		GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
	}
}


void CclientDlg::OnBnClickedButtonPreview()
{
	if (m_stackUIDesc.size() >= 1) {
		ErrorMessage((*m_stackUIDesc._Get_container().begin()).pMsg->DebugString());
	}
}

void CclientDlg::ShowContentInList() {
	if (m_stackUIDesc.size() == 0) {
		return;
	}
	m_wndProtoContentList.DeleteAllItems();
	int nColumnCount = m_wndProtoContentList.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i) {
		m_wndProtoContentList.DeleteColumn(0);
	}

	for (int i = 1; i <= m_stackUIDesc.top().pDesc->field_count(); ++i) {
		const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(i);
		if (NULL == pFieldDesc) {
			continue;
		}
		std::string str;
		if (pFieldDesc->is_required()) {
			str += "*";
		}
		str = pFieldDesc->name();
		str += ": ";
		if (FieldDescriptor::TYPE_MESSAGE == pFieldDesc->type()) {
			str += pFieldDesc->message_type()->name();
		}
		else {
			str += pFieldDesc->type_name();
		}

		if (pFieldDesc->is_repeated()) {
			str += "+";
		}

		m_wndProtoContentList.InsertColumn(i - 1, MB2WC(str), 0, str.length() * 10);
	}

	int iRow = 0, iCol = 0;
	for (int i = 1; i <= m_stackUIDesc.top().pDesc->field_count(); ++i, ++iCol) {
		iRow = 0;
		const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(i);
		if (NULL == pFieldDesc) {
			continue;
		}

		if (pFieldDesc->is_repeated()) {
			if (m_stackUIDesc.top().pMsg->GetReflection()->FieldSize(*m_stackUIDesc.top().pMsg, pFieldDesc) <= 0) {
				continue;
			}
		}
		else if (!m_stackUIDesc.top().pMsg->GetReflection()->HasField(*m_stackUIDesc.top().pMsg, pFieldDesc)) {
			continue;
		}

		CString str;
		WCHAR sz[256];
		if (pFieldDesc->is_repeated()) {
			for (int j = 0; j < m_stackUIDesc.top().pMsg->GetReflection()->FieldSize(*m_stackUIDesc.top().pMsg, pFieldDesc); ++j, ++iRow) {
				if (iRow  >= m_wndProtoContentList.GetItemCount()) {
					m_wndProtoContentList.InsertItem(iRow, L"");
				}
	
				switch (pFieldDesc->type())
				{
				case FieldDescriptor::TYPE_INT32:
					str = _itow(m_stackUIDesc.top().pMsg->GetReflection()->GetRepeatedInt32(*m_stackUIDesc.top().pMsg, pFieldDesc, iRow), sz, 10);
					break;

				case FieldDescriptor::TYPE_INT64:
					str = _i64tow(m_stackUIDesc.top().pMsg->GetReflection()->GetRepeatedInt64(*m_stackUIDesc.top().pMsg, pFieldDesc, iRow), sz, 10);
					break;

				case FieldDescriptor::TYPE_BYTES:
				case FieldDescriptor::TYPE_STRING:
					str = MB2WC(m_stackUIDesc.top().pMsg->GetReflection()->GetRepeatedString(*m_stackUIDesc.top().pMsg, pFieldDesc, iRow));
					break;

				case FieldDescriptor::TYPE_ENUM: {
					const EnumValueDescriptor* pEnumDesc = m_stackUIDesc.top().pMsg->GetReflection()->GetRepeatedEnum(*m_stackUIDesc.top().pMsg, pFieldDesc, iRow);
					if (NULL != pEnumDesc) {
						str = _itow(pEnumDesc->number(), sz, 10);
					}
					break;
				}
				case FieldDescriptor::TYPE_MESSAGE:
					str = L"[message...]";
					break;

				default:
					break;
				}

				m_wndProtoContentList.SetItemText(iRow, iCol, str);
			}
		}
		else {
			if (iRow >= m_wndProtoContentList.GetItemCount()) {
				m_wndProtoContentList.InsertItem(iRow, L"");
			}

			switch (pFieldDesc->type())
			{
			case FieldDescriptor::TYPE_INT32:
				str = _itow(m_stackUIDesc.top().pMsg->GetReflection()->GetInt32(*m_stackUIDesc.top().pMsg, pFieldDesc), sz, 10);
				break;

			case FieldDescriptor::TYPE_INT64:
				str = _i64tow(m_stackUIDesc.top().pMsg->GetReflection()->GetInt64(*m_stackUIDesc.top().pMsg, pFieldDesc), sz, 10);
				break;

			case FieldDescriptor::TYPE_BYTES:
			case FieldDescriptor::TYPE_STRING:
				str = MB2WC(m_stackUIDesc.top().pMsg->GetReflection()->GetString(*m_stackUIDesc.top().pMsg, pFieldDesc));
				break;

			case FieldDescriptor::TYPE_ENUM: {
				const EnumValueDescriptor* pEnumDesc = m_stackUIDesc.top().pMsg->GetReflection()->GetEnum(*m_stackUIDesc.top().pMsg, pFieldDesc);
				if (NULL != pEnumDesc) {
					str = _itow(pEnumDesc->number(), sz, 10);
				}
				break;
			}
			case FieldDescriptor::TYPE_MESSAGE:
				str = L"[message...]";
				break;

			default:
				break;
			}

			m_wndProtoContentList.SetItemText(iRow, iCol, str);
		}
	}

}

BOOL CclientDlg::IsMessageCell(int iCol)const {
	const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(iCol + 1);
	if (NULL == pFieldDesc) {
		return FALSE;
	}
	return pFieldDesc->type() == FieldDescriptor::TYPE_MESSAGE;
}

void CclientDlg::OnOK() {
	CWnd* pWnd = GetFocus();
	if (GetDlgItem(IDOK) == pWnd)
	{
		CDialog::OnOK();
		return;
	}
}

void CclientDlg::OnCancel(){
    CDialog::OnCancel();
}

void CclientDlg::ShowMessageCell(int iRow, int iCol) {
	if (m_stackUIDesc.size() == 0) {
		return;
	}
	const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(iCol + 1);
	if (NULL == pFieldDesc) {
		return;
	}
	if (pFieldDesc->type() != FieldDescriptor::TYPE_MESSAGE) {
		return;
	}
	Message* pMsg = NULL;
	if (pFieldDesc->is_repeated()) {
		int iSize = m_stackUIDesc.top().pMsg->GetReflection()->FieldSize(*m_stackUIDesc.top().pMsg, pFieldDesc);
		if (iSize == 0) {
			pMsg = m_stackUIDesc.top().pMsg->GetReflection()->AddMessage(m_stackUIDesc.top().pMsg, pFieldDesc, &s_messageFactory);
		}
		else if (iSize > iRow) {
			const Message* pMsgOrig = &m_stackUIDesc.top().pMsg->GetReflection()->GetRepeatedMessage(*m_stackUIDesc.top().pMsg, pFieldDesc, iRow);
			pMsg = const_cast<Message*>(pMsgOrig);
		}
		else if (iSize == iRow) {
			pMsg = m_stackUIDesc.top().pMsg->GetReflection()->AddMessage(m_stackUIDesc.top().pMsg, pFieldDesc, &s_messageFactory);
		}
	}
	else {
		const Message* pMsgOrig = &m_stackUIDesc.top().pMsg->GetReflection()->GetMessage(*m_stackUIDesc.top().pMsg, pFieldDesc, &s_messageFactory);
		pMsg->CopyFrom(*pMsgOrig);
		m_stackUIDesc.top().pMsg->GetReflection()->SetAllocatedMessage(m_stackUIDesc.top().pMsg, pMsg, pFieldDesc);
	}

	UIProtoData data;
	data.pDesc = pFieldDesc->message_type();
	data.pMsg = pMsg;

	m_stackUIDesc.push(data);

	const Descriptor* pDesc = pFieldDesc->message_type();
	ShowContentInList();

	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
}

BOOL CclientDlg::SetUIMessage(int iRow, int iCol, CString str) {
	if (m_stackUIDesc.size() == 0) {
		return FALSE;
	}

	const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(iCol + 1);
	if (NULL == pFieldDesc) {
		return FALSE;
	}
	if (pFieldDesc->type() == FieldDescriptor::TYPE_MESSAGE) {
		return FALSE;
	}
	if (iRow >= 1 && !pFieldDesc->is_repeated()) {
		std::string str = pFieldDesc->name();
		str += ": ";
		str += pFieldDesc->type_name();
		str += " is not a repeated field.";
		ErrorMessage(str);
		return FALSE;
	}

	if (pFieldDesc->is_repeated()) {
		const int iSize = m_stackUIDesc.top().pMsg->GetReflection()->FieldSize(*m_stackUIDesc.top().pMsg, pFieldDesc);
		switch (pFieldDesc->type())
		{
		case FieldDescriptor::TYPE_INT32:
			if (!CheckNumber(str)) {
				return FALSE;
			}
			if (iSize == iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->AddInt32(m_stackUIDesc.top().pMsg, pFieldDesc, _wtoi(str));
			}
			else if (iSize > iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->SetRepeatedInt32(m_stackUIDesc.top().pMsg, pFieldDesc, iRow, _wtoi(str));
			}
			else {
				assert(iSize >= iRow);
				return FALSE;
			}
			break;
		case  FieldDescriptor::TYPE_INT64:
			if (!CheckNumber(str)) {
				return FALSE;
			}
			if (iSize == iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->AddInt64(m_stackUIDesc.top().pMsg, pFieldDesc, _wtoi(str));
			}
			else if (iSize > iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->SetRepeatedInt64(m_stackUIDesc.top().pMsg, pFieldDesc, iRow, _wtoi(str));
			}
			else {
				assert(iSize >= iRow);
				return FALSE;
			}
			break;

		case FieldDescriptor::TYPE_BYTES:
		case FieldDescriptor::TYPE_STRING:
			if (iSize == iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->AddString(m_stackUIDesc.top().pMsg, pFieldDesc, WC2MB(str));
			}
			else if (iSize > iRow) {
				m_stackUIDesc.top().pMsg->GetReflection()->SetRepeatedString(m_stackUIDesc.top().pMsg, pFieldDesc, iRow, WC2MB(str));
			}
			else {
				assert(iSize >= iRow);
				return FALSE;
			}
			break;

		case  FieldDescriptor::TYPE_ENUM: {
			if (!CheckNumber(str)) {
				return FALSE;
			}
			const EnumDescriptor* pEnumDesc = pFieldDesc->enum_type();
			m_stackUIDesc.top().pMsg->GetReflection()->SetRepeatedEnum(m_stackUIDesc.top().pMsg, pFieldDesc, iRow, pEnumDesc->FindValueByNumber(_wtoi(str)));
			break;
		}

		default:
			break;
		}
	}
	else {
		switch (pFieldDesc->type())
		{
		case FieldDescriptor::TYPE_INT32:
			if (!CheckNumber(str)) {
				return FALSE;
			}
			m_stackUIDesc.top().pMsg->GetReflection()->SetInt32(m_stackUIDesc.top().pMsg, pFieldDesc, _wtoi(str));

			break;
		case  FieldDescriptor::TYPE_INT64:
			if (!CheckNumber(str)) {
				return FALSE;
			}
			m_stackUIDesc.top().pMsg->GetReflection()->SetInt64(m_stackUIDesc.top().pMsg, pFieldDesc, _wtol(str));
			break;

		case FieldDescriptor::TYPE_BYTES:
		case FieldDescriptor::TYPE_STRING:
			m_stackUIDesc.top().pMsg->GetReflection()->SetString(m_stackUIDesc.top().pMsg, pFieldDesc, WC2MB(str));
			break;

		case  FieldDescriptor::TYPE_ENUM: {
			if (!CheckNumber(str)) {
				return FALSE;
			}
			const EnumDescriptor* pEnumDesc = pFieldDesc->enum_type();
			m_stackUIDesc.top().pMsg->GetReflection()->SetEnum(m_stackUIDesc.top().pMsg, pFieldDesc, pEnumDesc->FindValueByNumber(_wtoi(str)));
			break;
		}

		default:
			break;
		}
	}

	return TRUE;
}

BOOL CclientDlg::CheckNumber(CString str) {
	for (int i = 0; i < str.GetLength(); ++i) {
		if (str[i] < L'0' || str[i] > '9') {
			return FALSE;
		}
	}
	return TRUE;
}

void CclientDlg::OnTvnSelchangedTreeProto(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// is it not a root item??
	if (m_wdnProtoTree.GetParentItem(pNMTreeView->itemNew.hItem) != NULL) {
		return;
	}

	const Descriptor* pMsgDesc = (const Descriptor*)m_wdnProtoTree.GetItemData(pNMTreeView->itemNew.hItem);
	if (NULL == pMsgDesc) {
		return;
	}

	const Message* pMessageFactory = s_messageFactory.GetPrototype(pMsgDesc);
	if (NULL == pMessageFactory) {
		return;
	}

	UIProtoData data;

	data.pMsg = pMessageFactory->New();
	data.pDesc = pMsgDesc;
	while (!m_stackUIDesc.empty())
	{
		delete m_stackUIDesc.top().pMsg;
		m_stackUIDesc.pop();
	}
	
	m_stackUIDesc.push(data);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);

	ShowContentInList();
}


void CclientDlg::OnBnClickedButtonAdd()
{
	if (m_wndProtoContentList.GetHeaderCtrl()->GetItemCount() <= 0) {
		return;
	}
	if (m_wndProtoContentList.GetItemCount() >= 1) {
		BOOL bHasRepeated = FALSE;
		for (int i = 1; i <= m_stackUIDesc.top().pDesc->field_count(); ++i) {
			const FieldDescriptor* pFieldDesc = m_stackUIDesc.top().pDesc->FindFieldByNumber(i);
			if (NULL == pFieldDesc) {
				continue;
			}
			if (pFieldDesc->is_repeated()) {
				bHasRepeated = TRUE;
				break;
			}
		}
		if (!bHasRepeated) {
			return;
		}
	}
	int iItem = m_wndProtoContentList.InsertItem(m_wndProtoContentList.GetItemCount(), L"");
	for (int i = 0; i < m_wndProtoContentList.GetHeaderCtrl()->GetItemCount(); ++i) {
		m_wndProtoContentList.SetItemText(iItem, i, L"");
	}
	m_wndProtoContentList.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
}


void CclientDlg::OnEnChangeEditFindProto()
{
	UpdateData(TRUE);

	if (m_strFind.GetLength() > 0) {
		MapDescriptor mapDesc;
		CString strFind = m_strFind.MakeLower();
		for (MapDescriptor::iterator iter = m_mapDesc.begin(); iter != m_mapDesc.end(); ++iter) {
			CString strProtoName = MB2WC(iter->first).MakeLower();
			if (strProtoName.MakeLower().Find(strFind) != -1) {
				mapDesc.insert(std::make_pair(iter->first, iter->second));
			}
		}

		BuildTree(mapDesc);
	}
	else {
		BuildTree(m_mapDesc);
	}
}


void CclientDlg::OnSetting()
{
	CDlgSetting dlg;
	if (IDOK != dlg.DoModal()) {
		return;
	}

	m_strProtoPath = dlg.m_strProtoPath;
	m_strIP = dlg.m_strIP;
	m_strPort = dlg.m_strPort;

	OnBnClickedButtonGen();

	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_NO_ERROR != doc.LoadFile(XML_SETTING_NAME)) {
		std::fstream file(XML_SETTING_NAME, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		if (!file) {
			ErrorMessage("fail to create the xml file for settings");
			return;
		}

		file << "<setting></setting>";

		file.close();

		if (tinyxml2::XML_NO_ERROR != doc.LoadFile(XML_SETTING_NAME)) {
			ErrorMessage("fail to load the xml file for settings");
			return;
		}
	}


	tinyxml2::XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot) {
		ErrorMessage("fail to get the root in the xml file");
		return;
	}

	tinyxml2::XMLElement* pProto = pRoot->FirstChildElement("proto");
	if (NULL == pProto) {
		pProto = doc.NewElement("proto");
		pRoot->InsertFirstChild(pProto);
	}
	pProto->SetAttribute("path", WC2MB(dlg.m_strProtoPath).c_str());

	pProto = pRoot->FirstChildElement("login");
	if (NULL == pProto) {
		pProto = doc.NewElement("login");
		pRoot->InsertFirstChild(pProto);
	}
	pProto->SetAttribute("role_id", WC2MB(dlg.m_strRoleId).c_str());
	pProto->SetAttribute("account_id", WC2MB(dlg.m_strAccountId).c_str());

	pProto = pRoot->FirstChildElement("net");
	if (NULL == pProto) {
		pProto = doc.NewElement("net");
		pRoot->InsertFirstChild(pProto);
	}
	pProto->SetAttribute("ip", WC2MB(dlg.m_strIP).c_str());
	pProto->SetAttribute("port", WC2MB(dlg.m_strPort).c_str());

	doc.SaveFile(XML_SETTING_NAME);
}

BOOL CclientDlg::InitData() {
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_NO_ERROR != doc.LoadFile(XML_SETTING_NAME)) {
		return FALSE;
	}

	tinyxml2::XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot) {
		return FALSE;
	}

	tinyxml2::XMLElement* pEle = pRoot->FirstChildElement("proto");
	if (NULL != pEle) {
		const char* lpszPath = pEle->Attribute("path");
		if (NULL != lpszPath) {
			m_strProtoPath = MB2WC(lpszPath);
			OnBnClickedButtonGen();
		}
	}

	pEle = pRoot->FirstChildElement("proto");
	if (NULL != pEle) {
		const char* lpszProto = pEle->Attribute("path");
		if (NULL != lpszProto) {
			m_strProtoPath = MB2WC(lpszProto);
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


	// try to login server
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		ErrorMessage("cannot start the net library up!!");
		return FALSE;
	}

	return ConnectServer();
}

void CclientDlg::OnRefreshProto()
{
	OnBnClickedButtonGen();
}

CclientDlg::~CclientDlg() {
	WSACleanup();
}



BOOL CclientDlg::ConnectServer() {
	closesocket(m_ConnectSocket);
	//----------------------
	// Create a SOCKET for connecting to server
	m_ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ConnectSocket == INVALID_SOCKET) {
		ErrorMessage("socket error!");
		return FALSE;
	}

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(WC2MB(m_strIP).c_str());
	clientService.sin_port = htons(atoi(WC2MB(m_strPort).c_str()));

	//----------------------
	// Connect to server.
	if (connect(m_ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		ErrorMessage("connect has error!");
		return FALSE;
	}

	ErrorMessage("connect the server successfully!", "info");

	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DISCONNECT, MF_ENABLED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_CONNECT, MF_GRAYED);

	CloseHandle(m_hRecvThread);
	m_hRecvThread = (HANDLE)_beginthreadex(NULL, 0, (PFN)CclientDlg::RecvMessage, (void*)this, 0, NULL);
	if (NULL == m_hRecvThread) {
		ErrorMessage("fail to begin the new thread for receiving message");
		return FALSE;
	}


	return TRUE;
}

unsigned int CclientDlg::RecvMessage(void* pThis) {
	if (NULL == pThis) {
		return 0;
	}
	CclientDlg* pClient = (CclientDlg*)pThis;

	return pClient->ProcessRecv();
}




void CclientDlg::OnConnect()
{
	ConnectServer();
}

int CclientDlg::ProcessRecv() {
	const int iSize = sizeof(ClientMsgHead);
	std::vector<char> cMessage; 


	while (true) {
		cMessage.resize(iSize);
		int iHasRecved = 0;
		while (iHasRecved < iSize) {
			int iRecv = recv(m_ConnectSocket, (char*)&cMessage[0] + iHasRecved, iSize - iHasRecved, 0);
			if (SOCKET_ERROR == iRecv || 0 == iRecv) {
				ErrorMessage("the receiving thread ends", "info");
				return 0;
			}
			iHasRecved += iRecv;
		}
		ClientMsgHead* pHead = (ClientMsgHead*)&cMessage[0];

		const int iBodySize = reverse(pHead->msgLength);
		const int iCmd = reverse(pHead->cmdID);
		cMessage.resize(iBodySize);
		iHasRecved = 0;
		while (iHasRecved < iBodySize) {
			int iRecv = recv(m_ConnectSocket, (char*)&cMessage[0] + iHasRecved, iBodySize - iHasRecved, 0);
			if (SOCKET_ERROR == iRecv || 0 == iRecv) {
				ErrorMessage("the receiving thread ends", "info");
				return 0;
			}
			iHasRecved += iRecv;
		}

		if (iCmd != m_iRecvCmd) {
			continue;
		}

		const std::string strProtoName = WC2MB(m_strRecvProto);
		MapDescriptor::iterator iter = m_mapDesc.find(strProtoName);
		if (m_mapDesc.end() == iter) {
			ErrorMessage("cannot get the descriptor of " + strProtoName);
			continue;
		}
		const Descriptor* pDesc = iter->second;
		Message* pMsg = s_messageFactory.GetPrototype(pDesc)->New();
		if (!pMsg->ParseFromArray(&cMessage[0], cMessage.size())) {
			ErrorMessage("fail to parse to data to message: " + strProtoName);
			continue;
		}

		char szCmd[256];
		std::string strRecv = "cmd: ";
		strRecv += _itoa(iCmd, szCmd, 10);
		strRecv += '\n';
		strRecv += pMsg->DebugString();
		ErrorMessage(strRecv, "receive message");

		delete pMsg;
	}
}

BOOL CclientDlg::Send() {
	if (m_stackUIDesc.size() <= 0) {
		return FALSE;
	}
	UpdateData(TRUE);
	
	const Message* pMsg = m_stackUIDesc._Get_container().begin()->pMsg;

	const int iSize = sizeof(ClientMsgHead) + pMsg->ByteSize();
	ClientMsgHead* pRawData = (ClientMsgHead*)new char[iSize];
	pRawData->cmdID = reverse(m_iCMd);
	pRawData->TimeStamp = 0;
	pRawData->reserve = 0;
	pRawData->msgLength = reverse(pMsg->ByteSize());
	pMsg->SerializePartialToArray(pRawData->body, pMsg->ByteSize());

	int iHasSent = 0;
	while (iHasSent < iSize) {
		int iSent = send(m_ConnectSocket, (const char*)pRawData + iHasSent, iSize - iHasSent, 0);
		if (SOCKET_ERROR == iSent) {
			ErrorMessage("fail to send!");
			delete pRawData;
			return FALSE;
		}
		iHasSent += iSent;
	}

	delete pRawData;
	return TRUE;
}


void CclientDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	Send();
}


void CclientDlg::OnDisconnect()
{
	closesocket(m_ConnectSocket);

	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DISCONNECT, MF_GRAYED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_CONNECT, MF_ENABLED);

	ErrorMessage("disconnect the server successfully!", "info");
}
