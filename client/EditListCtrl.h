#pragma once

#include "EditCell.h"
// CEditListCtrl

class CEditListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditListCtrl)

public:
	CEditListCtrl();
	virtual ~CEditListCtrl();


	virtual void PreSubclassWindow();

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetCellText();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CEditCell m_cell;
	int m_iSelRow;
	int m_iSelCol;
};


