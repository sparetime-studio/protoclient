// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "EditListCtrl.h"
#include "clientDlg.h"


// CEditListCtrl

IMPLEMENT_DYNAMIC(CEditListCtrl, CListCtrl)

CEditListCtrl::CEditListCtrl() :m_iSelCol(0), m_iSelRow(0)
{

}

CEditListCtrl::~CEditListCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CEditListCtrl message handlers

void CEditListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();
	ModifyStyle(LVS_EDITLABELS, 0);
	ModifyStyle(0, LVS_REPORT);
	ModifyStyle(0, LVS_SHOWSELALWAYS);
	SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cell.Create(WS_CHILD | WS_CLIPSIBLINGS | WS_EX_TOOLWINDOW | WS_BORDER, CRect(0, 40, 10, 50), this, 1001);
}



void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) {
	CListCtrl::OnLButtonDblClk(nFlags, point);
	LVHITTESTINFO info;
	info.pt = point;
	info.flags = LVHT_ONITEMLABEL;
	if (SubItemHitTest(&info) >= 0) {
		m_iSelRow = info.iItem;                         
		m_iSelCol = info.iSubItem;                    

		CclientDlg* pMainDlg = (CclientDlg*)GetParent();
		if (pMainDlg->IsMessageCell(m_iSelCol)) {
			pMainDlg->ShowMessageCell(m_iSelRow, m_iSelCol);
			return;
		}

		CRect rect;
		GetSubItemRect(m_iSelRow, m_iSelCol, LVIR_LABEL, rect);   
		CString str;
		str = GetItemText(m_iSelRow, m_iSelCol);                           
		m_cell.MoveWindow(rect);                           
		m_cell.SetWindowText(str);                           
		m_cell.ShowWindow(SW_SHOW);            
		m_cell.SetSel(0, -1);
		m_cell.SetFocus();
		UpdateWindow();
	}
}


void CEditListCtrl::SetCellText() {
	CString strEdit;
	m_cell.GetWindowText(strEdit);
	m_cell.ShowWindow(SW_HIDE);
	CclientDlg* pMainDlg = (CclientDlg*)GetParent();
	if (pMainDlg->SetUIMessage(m_iSelRow, m_iSelCol, strEdit)) {
		SetItemText(m_iSelRow, m_iSelCol, strEdit);
	}
}