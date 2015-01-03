#pragma once


// CEditCell

class CEditCell : public CEdit
{
	DECLARE_DYNAMIC(CEditCell)

public:
	CEditCell();
	virtual ~CEditCell();


	void OnKillFocus(CWnd* pNewWnd);

protected:
	DECLARE_MESSAGE_MAP()
};


