
#pragma once

class CXTDC;

class CXTCheckBox : public CButton
{
	DECLARE_DYNAMIC(CXTCheckBox)

public:
	CXTCheckBox();
	virtual ~CXTCheckBox();

private:
	HDC		m_hDlgDC;	// ������ʾDC������DC
	HDC		m_hCompDC;	// �м���λͼ��DC
	HDC		m_hBmpDC;	// ����λͼDC

	SIZE	m_Sz;
	POINT	m_Pt;
	POINT	m_SrcPt;

    BOOL    m_bCheck;

	CToolTipCtrl m_ToolTip;

	enum{ DLGDC, COMPDC, COMPTODLG, CLEAR };
	enum{ UNCHECK, UNCHECK_ON, CHECK, CHECK_ON, CHECK_DISABLE, UNCHECK_DISABLE };

public:
	bool Initial(int x, int y, int cx, int cy, int sx, int sy, CXTDC *pDBuffDC, LPCTSTR lpszToolTipText);
	void ResetImage(int sx = -1, int sy = -1);
	void DrawImage(UINT nObject, UINT nState = NULL);
	
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);// ���ӻ�����������������Ի����ԣ��ͱ����д˺�����Ŀǰ�˺���Ϊ��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetCheck(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


