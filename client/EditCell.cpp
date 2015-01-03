// EditCell.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "EditCell.h"
#include "EditListCtrl.h"


// CEditCell

IMPLEMENT_DYNAMIC(CEditCell, CEdit)

CEditCell::CEditCell()
{

}

CEditCell::~CEditCell()
{
}


BEGIN_MESSAGE_MAP(CEditCell, CEdit)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CEditCell message handlers
void CEditCell::OnKillFocus(CWnd* pNewWnd) {
	CEdit::OnKillFocus(pNewWnd);
	CEditListCtrl * pEditCtrl = (CEditListCtrl*)GetParent();
	pEditCtrl->SetCellText();
}


