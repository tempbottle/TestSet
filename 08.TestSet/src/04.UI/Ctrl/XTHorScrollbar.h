#pragma once
#include "XTUI.h"


class CXTHorScrollbar : public CWnd, public CXTUI
{
public:
	CXTHorScrollbar();
	virtual ~CXTHorScrollbar();

private:
	int			thumbPos_;	// �м�鵱ǰλ��
	int			thumbLen_;	// �м�鳤��
	CRect		thumb_;		// �м�����
	CRect		leftArraw_;	// ���ͷλ��
	CRect		rightArraw_;// �Ҽ�ͷλ��
	POINT		lastDown_;	// ��һ�����ĵ�

	enum{ DLGDC, COMPDC, DLG_COMP_DC };
	enum{ OK, MOUSE_ON, MOUSE_DOWN };
	enum{ LEFTARRAW, RIGHTARRAW, THUMB, THUMB_BKG, NOT } 
	curPart_, lastPart_;	// ���»����·����һ����

	void		LimitThumbPosition();	
	void		ScrollLeft();
	void		ScrollRight();
	void		PageLeft();
	void		PageRight();
	void		DrawImage(int obj, int state, int part);

public:
	CListCtrl	*list_;
	bool		Initial(int x, int y, int cx, int cy, int sx, int sy, CXTDC *xtDC);
	void		UpdateThumbPosition();
	void		ResetImage(int sx = -1, int sy = -1);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

protected:
	LRESULT	OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};