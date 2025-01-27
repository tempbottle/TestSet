
/**
 CToolBar的一个扩展类，可以使用彩色位图做按钮
**/

#pragma once
#include <afxtempl.h>

enum
{
	BT_BUTTON,		// 普通按钮
	BT_CHECK,		// Check按钮
	BT_DROPDOWN,	// 带菜单的下接按钮
};

enum
{
	DR_OK,			// 正常显示
	DR_HOVER,		// 鼠标在上面
	DR_CLICK,		// 鼠标按下
	DR_ENABLE,		// 可用
	DR_UNABLE,		// 不可用
	DR_CHECK,		// 按下
	DR_UNCHECK,		// 取消按下
};

typedef struct _tagItem
{
	UINT nId;

	WORD wOffsetX;	// 距上一个按钮右边框的x距离

	WORD wX;		// 实现坐标
	WORD wY;
	WORD wWidth;
	WORD wHeight;

	WORD wSrcX;		// 位图中的位置
	WORD wSrcY;
	WORD wType;
	TCHAR tszTip[MAX_PATH];
	CMenu *pMenu;

	BOOL bShow;
	BOOL bEnable;
	BOOL bCheck;

	BOOL bFirst;

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
		bCheck(FALSE),
		bFirst(TRUE)
	{
		memset(tszTip, 0, sizeof(tszTip));
	}

}BARITEM, *PBARITEM;

class CXTDC;

typedef CArray<BARITEM*, BARITEM*>	ITEMLIST;

class CXTBmpToolBar : public CToolBar
{
public:
	CXTBmpToolBar();
	virtual ~CXTBmpToolBar();
	
	DECLARE_DYNAMIC(CXTBmpToolBar)

private:
	ITEMLIST		m_ItemList;
	BARITEM			*m_pLastHoverItem;
	BARITEM			*m_pLastClickItem;
	WORD			m_wSreenWidth;
	WORD			m_wWndHeight;	
	UINT			m_nDropDownButtonX;
	UINT			m_nDropDownButtonY;
	UINT			m_nDropDownButtonCX;
	UINT			m_nDropDownButtonCY;
	UINT			m_nBackground;
	UINT			m_nBackgroundX;
	UINT			m_nBackgroundY;
	UINT			m_nBackgroundCX;
	UINT			m_nBackgroundCY;
	BOOL			m_bNewedDC;

	CXTDC			*m_pxtDC;
	HDC				m_hWndDC;
	HDC				m_hCompDC;
	HDC				m_hBmpDC;

public:
	BOOL			Init(CWnd* pParentWnd, CXTDC *pxtDC, LPCTSTR strIniFile);
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
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnPaint();
};
