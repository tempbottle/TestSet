
/**
 ������ô����������Ҫ��Ӧ��Ϣ:
 WM_NCPAINT, WM_NCACTIVATE, WM_NCMOUSEMOVE, WM_NCLBUTTONUP
 WM_NCLBUTTONDOWN, WM_NCCALCSIZE
 ��ִ�� WM_NCACTIVATE ��Ϣ���ദ����,ֱ�ӷ��� TRUE
 ��ִ�� WM_NCCALCSIZE ��Ϣ���ദ����,ֱ�Ӹ�ֵ
*/

#pragma once
#include <Icm.h>
#include "XTDC.h"

class CXTTitleBar
{
public:
	CXTTitleBar();
	virtual ~CXTTitleBar();

private:
	WORD		m_wTitleHeight;
	WORD		m_wFillType;
	WORD		m_wColorType;		// ����ǽ���ɫ,��Ϊ��������(0����, 1����)
	WORD		m_wColorCount;
	RGBCOLOR	m_clBackColor[20];

	WORD		m_wLastButton;
	bool		m_bWndChang;

	HICON		m_hIcon;
	HBITMAP		m_hBackground;
	HWND		m_hWnd;	
	CXTDC		m_xtDC;
	HDC			m_hDC;
	HDC			m_hCompDC;
	HDC			m_hBmpDC;
	HFONT		m_hFont;
	RGBCOLOR	m_clFontColor;

	SIZE		m_szIcon;
	POINT		m_ptSrcMinButton;	// ��С����ťͼƬ���ļ��е�λ��
	POINT		m_ptSrcMaxButton;
	POINT		m_ptSrcResumeButton;// �ָ���ť
	POINT		m_ptSrcCloseButton;
	SIZE		m_szMinButton;
	SIZE		m_szMaxButton;
	SIZE		m_szResumeButton;
	SIZE		m_szCloseButton;
	CRect		m_rcMinButton;		// ��ť��ʵ��λ��	
	CRect		m_rcMaxButton;
	CRect		m_rcResumeButton;
	CRect		m_rcCloseButton;
	CRect		m_rcTitleBar;

	enum{ DLGDC, COMPDC };
	enum{ OK, MOUSE_ON, MOUSE_DOWN };
	enum{ MINBUTTON = 10000, MAXBUTTON, RESUMEBUTTON, CLOSEBUTTON};

	void GetColor(LPCTSTR pstrRGB, RGBCOLOR *pRGB);
	WORD GetColorGroup(LPCTSTR pstrGroupRGB);
	void InitBackground(RECT *pRect = NULL);
	void DrawButton(int nType, CRect &Rect, POINT &ps, int nStat);

public:
	BOOL Init(HWND hWnd, HICON hIcon, LPCTSTR pstrIniFile);

	BOOL OnNcActivate(void);
	void OnNcCalcSize(NCCALCSIZE_PARAMS* lpncsp);
	void OnNcPaint(void);
	UINT OnNcHitTest(CPoint point);
	void OnNcMouseMove(UINT nHitTest);
	void OnNcLButtonDown(UINT nHitTest);
	void OnNcLButtonUp(UINT nHitTest);
};
