
/************************************************************************/
/*                     ����ؼ�ֻ�������ĵ�����                         */
/************************************************************************/


#pragma once
#include <afxtempl.h>

enum
{
	BT_BUTTON,		// ��ͨ��ť
	BT_CHECK,		// Check��ť
	BT_DROPDOWN,	// ���˵����½Ӱ�ť
};

enum
{
	DR_OK,			// ������ʾ
	DR_HOVER,		// ���������
	DR_CLICK,		// ��갴��
	DR_ENABLE,		// ����
	DR_UNABLE,		// ������
	DR_CHECK,		// ����
	DR_UNCHECK,		// ȡ������
};

typedef struct _tagItem
{
	UINT nId;

	WORD wOffsetX;	// ����һ����ť�ұ߿��x����

	WORD wX;		// ʵ������
	WORD wY;
	WORD wWidth;
	WORD wHeight;

	WORD wSrcX;		// λͼ�е�λ��
	WORD wSrcY;
	WORD wType;
	TCHAR tszTip[MAX_PATH];
	CMenu *pMenu;

	BOOL bShow;
	BOOL bEnable;
	BOOL bCheck;

	_tagItem::_tagItem()
		:nId(0),
		wOffsetX(0),
		wX(0),
		wY(0),
		wWidth(0),
		wHeight(0),
		wType(0),
		pMenu(NULL),
		bShow(TRUE),
		bEnable(TRUE),
		bCheck(FALSE)
	{
		memset(tszTip, 0, sizeof(tszTip));
	}

}BARITEM, *PBARITEM;

class CXTDC;

typedef CArray<BARITEM*, BARITEM*>	ITEMLIST;

class CXTBarCtrl : public CControlBar
{
public:
	CXTBarCtrl();
	virtual ~CXTBarCtrl();
	
	DECLARE_DYNAMIC(CXTBarCtrl)

private:
	ITEMLIST		m_ItemList;
	BARITEM			*m_pLastHoverItem;
	BARITEM			*m_pLastClickItem;
	WORD			m_wSreenWidth;
	WORD			m_wWndHeight;
	BOOL			m_bNewedDC;

	CXTDC			*m_pxtDC;
	HDC				m_hWndDC;
	HDC				m_hCompDC;
	HDC				m_hBmpDC;

public:
	BOOL			Init(CWnd* pParentWnd, DWORD dwStyle, UINT nID, WORD wWndHeight, CXTDC *pxtDC, UINT nBmpId);
	BOOL			Init(CWnd* pParentWnd, DWORD dwStyle, UINT nID, WORD wWndHeight, CXTDC *pxtDC, LPCTSTR strBmpFile);
	BOOL			AddButton(UINT nId, WORD wType, WORD wOffsetX, WORD wY, WORD wWidth, WORD wHeight,
								WORD wSrcX, WORD wSrcY,	LPCTSTR strTip, CMenu *pMenu = NULL);
	void			RedrawButton(void);

	void			SetShow(WORD wButtonId, BOOL bShow);
	void			SetEnable(WORD wButtonId, BOOL bEnable);
	void			SetCheck(WORD wButtonId, BOOL bCheck);

	BARITEM* 		HitTest(const CPoint& point) const;

	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);		
	virtual int		OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CSize	CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual void	DoPaint(CDC* pDC);

private:
	void			DrawButton(HDC hDC, BARITEM *pItem, WORD wType);

protected:
	DECLARE_MESSAGE_MAP()
	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LONG OnOpenFlash(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
