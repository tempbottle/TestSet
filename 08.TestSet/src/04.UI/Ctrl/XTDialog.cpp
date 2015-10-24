#include "StdAfx.h"
#include "XTDialog.h"

CXTDialog::CXTDialog()
	:heigh_(20)
{
}

CXTDialog::CXTDialog(UINT nIDTemplate, CWnd* pParentWnd/* = NULL*/)
	:CDialog(nIDTemplate, pParentWnd),
	heigh_(20)
{
}


CXTDialog::~CXTDialog()
{
}

BEGIN_MESSAGE_MAP(CXTDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CXTDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (point.y < heigh_)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	CDialog::OnMouseMove(nFlags, point);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CXTDialog::OnPaint()
{
	if (IsIconic())
	{
		/*
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
		*/
	}
	else
	{		
		//CDialog::OnPaint();
		CPaintDC dc(this);
		PaintWindowlessControls(&dc);

		if (NULL == dc_ || NULL == compDC_)	return;

		DrawDlg();
	}
}

BOOL CXTDialog::OnEraseBkgnd(CDC* pDC)
{
	//return CDialog::OnEraseBkgnd(pDC);
	return TRUE;
}

// ���ô��壬Ҳ���Լ�����SetDlgRect,Ȼ��������XTDC

// cx		[IN] �����
// cy		[IN] �����
// nId		[IN] ����ͼƬID
// pstrBmp	]IN] ����ͼƬ·��
// hDll		[IN] ��DLL��������Դ
void CXTDialog::InitBkgBmp(int cx, int cy, int cr, int id)
{
	SetDlgRect(cx, cy, cr);
	SetBkgBmp(id, NULL);
}

void CXTDialog::SetDlgRect(int cx, int cy, int cr)
{
	heigh_ = cy;
	
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	MoveWindow(rcWnd.left, rcWnd.top, cx, cy);
	HRGN hRgn = CreateRoundRectRgn(0, 0, cx, cy, cr, cr);
	SetWindowRgn(hRgn, TRUE);
	CenterWindow();

	GetClientRect(&rcWnd);
	rectList_.push_back(rcWnd);
}

void CXTDialog::SetBkgBmp(int id, HMODULE module)
{
	dc_ = xtDC_.Init(module, m_hWnd);
	compDC_ = xtDC_.AddCompDC(0);
	imageDC_ = xtDC_.AddPngDC(0, id);
	
	int cx = 0;
	int cy = 0;
	xtDC_.GetBmpRect(0, cx, cy);

	BitBlt(compDC_, 0, 0, cx, cy, imageDC_, 0, 0, SRCCOPY);
}

CRect CXTDialog::OutputText(int x, int y, const char *text)
{
	if (NULL != text)
	{
		CRect rcText(x,y,0,0);
		DrawText(compDC_, text, strlen(text), &rcText, DT_CALCRECT);	// �������ֵķ�Χ
		DrawText(compDC_, text, strlen(text), &rcText, DT_LEFT);
		BitBlt(dc_, x, y, rcText.Width(), rcText.Height(), compDC_, x, y, SRCCOPY);

		return rcText;
	}

	return CRect(0,0,0,0);
}

void CXTDialog::ClearText(CRect rcText, HDC bkgDC)
{
	BitBlt(compDC_, rcText.left, rcText.top, rcText.Width(), rcText.Height(), bkgDC, rcText.left, rcText.top, SRCCOPY);
}

void CXTDialog::DrawDlg()
{
	RECT_LIST::iterator iter = rectList_.begin();

	for (; iter != rectList_.end(); ++iter)
	{
		RECT rc = *iter;

		BitBlt(dc_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			compDC_, rc.left, rc.top, SRCCOPY);		
	}
}

void CXTDialog::DelRect(int x, int y, int cx, int cy)
{
	CRect rcDel(x, y, x + cx, y + cy);

	RECT_LIST tmpeRectList;

	RECT_LIST::iterator iter = rectList_.begin();

	for (; iter != rectList_.end(); ++iter)
	{
		UpdateRect(*iter, rcDel, &tmpeRectList);
	}

	rectList_.clear();

	rectList_ = tmpeRectList;
}

// 1,  rcDel��rcSrc���
// 2,  rcDel��rcSrc�����
// 3,  rcDel��rcSrc���
// 4,  rcDel��rcSrc��
// 5,  rcDel��rcSrc�ұ���
// 6,  rcDel��rcSrc�ұ�

// 7,  rcDel��rcSrc�ϱ�
// 8,  rcDel��rcSrc�ϱ���
// 9,  rcDel��rcSrc���
// 10, rcDel��rcSrc��(ͬ4)
// 11, rcDel��rcSrc�±���
// 12, rcDel��rcSrc�±�

// 13, rcDel��rcSrc���Ͻ�
// 14, rcDel��rcSrc���Ͻ�
// 15, rcDel��rcSrc���½�
// 16, rcDel��rcSrc���½�

// 17, rcSrc��rcDel��
// 18, rcDel��rcSrc���ཻ

void CXTDialog::UpdateRect(CRect rcSrc, CRect rcDel, RECT_LIST *rectList)
{
	if (NULL == rectList) return;

	POINT ptSrcLeftTop		= {rcSrc.left,  rcSrc.top};
	POINT ptSrcRightTop		= {rcSrc.right, rcSrc.top};
	POINT ptSrcLeftBottom	= {rcSrc.left,  rcSrc.bottom};
	POINT ptSrcRightBottom	= {rcSrc.right, rcSrc.bottom};

	POINT ptDelLeftTop		= {rcDel.left,  rcDel.top};
	POINT ptDelRightTop		= {rcDel.right, rcDel.top};
	POINT ptDelLeftBottom	= {rcDel.left,  rcDel.bottom};
	POINT ptDelRightBottom	= {rcDel.right, rcDel.bottom};

	BOOL bDelOnSrcLeftTop		= rcDel.PtInRect(ptSrcLeftTop);
	BOOL bDelOnSrcRightTop		= rcDel.PtInRect(ptSrcRightTop);
	BOOL bDelOnSrcLeftBottom	= rcDel.PtInRect(ptSrcLeftBottom);
	BOOL bDelOnSrcRightBottom	= rcDel.PtInRect(ptSrcRightBottom);

	BOOL bSrcOnDelLeftTop		= rcSrc.PtInRect(ptDelLeftTop);
	BOOL bSrcOnDelRightTop		= rcSrc.PtInRect(ptDelRightTop);
	BOOL bSrcOnDelLeftBottom	= rcSrc.PtInRect(ptDelLeftBottom);
	BOOL bSrcOnDelRightBottom	= rcSrc.PtInRect(ptDelRightBottom);

	// 1,  rcDel��rcSrc���
	if (bDelOnSrcLeftTop && bDelOnSrcLeftBottom && !bDelOnSrcRightTop && !bDelOnSrcRightBottom)
	{
		RECT rc = {rcDel.right, rcSrc.top, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc);
	}
	// 2,  rcDel��rcSrc�����
	else if (bSrcOnDelLeftTop && bSrcOnDelLeftBottom && !bSrcOnDelRightTop && !bSrcOnDelRightBottom)
	{
		RECT rc1 = {rcSrc.left,  rcSrc.top,    rcSrc.right, rcDel.top};
		RECT rc2 = {rcDel.right, rcDel.top,    rcSrc.right, rcDel.bottom};
		RECT rc3 = {rcSrc.left,  rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
		rectList->push_back(rc3);
	}
	// 3,  rcDel��rcSrc���
	else if (!bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && !bDelOnSrcRightTop && !bDelOnSrcRightBottom &&
		!bSrcOnDelLeftTop && !bSrcOnDelLeftBottom && !bSrcOnDelRightTop && !bSrcOnDelRightBottom &&
		rcDel.top < rcSrc.top && rcDel.bottom > rcSrc.bottom && rcDel.left > rcSrc.left && rcDel.right < rcDel.right)
	{
		RECT rc1 = {rcSrc.left,  rcSrc.top,    rcDel.left,  rcSrc.top};
		RECT rc2 = {rcDel.right, rcSrc.top,    rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 4,  rcDel��rcSrc��
	else if (bSrcOnDelLeftTop && bSrcOnDelLeftBottom && bSrcOnDelRightTop && bSrcOnDelRightBottom)
	{
		RECT rc1 = {rcSrc.left, rcSrc.top,    rcSrc.right, rcDel.top};
		RECT rc2 = {rcSrc.left, rcDel.top,    rcDel.left,  rcDel.bottom};
		RECT rc3 = {rcDel.right, rcDel.top,   rcSrc.right, rcDel.bottom};
		RECT rc4 = {rcSrc.left, rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
		rectList->push_back(rc3);
		rectList->push_back(rc4);
	}
	// 5,  rcDel��rcSrc�ұ���
	else if (!bSrcOnDelLeftTop && !bSrcOnDelLeftBottom && bSrcOnDelRightTop && bSrcOnDelRightBottom)
	{
		RECT rc1 = {rcSrc.left, rcSrc.top,    rcSrc.right, rcDel.top};
		RECT rc2 = {rcSrc.left, rcDel.top,    rcDel.left,  rcDel.bottom};
		RECT rc3 = {rcSrc.left, rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
		rectList->push_back(rc3);
	}
	// 6,  rcDel��rcSrc�ұ�
	else if (!bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && bDelOnSrcRightTop && bDelOnSrcRightBottom)
	{
		RECT rc = {rcSrc.left, rcSrc.top, rcDel.left, rcSrc.bottom};
		rectList->push_back(rc);
	}
	// 7,  rcDel��rcSrc�ϱ�
	else if (bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && bDelOnSrcRightTop && !bDelOnSrcRightBottom)
	{
		RECT rc = {rcSrc.left, rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc);
	}
	// 8,  rcDel��rcSrc�ϱ���
	else if (bSrcOnDelLeftTop && !bSrcOnDelLeftBottom && bSrcOnDelRightTop && !bSrcOnDelRightBottom)
	{
		RECT rc1 = {rcSrc.left,  rcSrc.top,    rcDel.left,  rcDel.bottom};
		RECT rc2 = {rcDel.right, rcSrc.top,    rcSrc.right, rcDel.bottom};
		RECT rc3 = {rcSrc.left,  rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
		rectList->push_back(rc3);
	}
	// 9,  rcDel��rcSrc���
	else if (!bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && !bDelOnSrcRightTop && !bDelOnSrcRightBottom &&
		!bSrcOnDelLeftTop && !bSrcOnDelLeftBottom && !bSrcOnDelRightTop && !bSrcOnDelRightBottom &&
		rcDel.top > rcSrc.top && rcDel.bottom < rcSrc.bottom && rcDel.left < rcSrc.left && rcDel.right > rcDel.right)
	{
		RECT rc1 = {rcSrc.left, rcSrc.top,    rcSrc.right, rcDel.top};
		RECT rc2 = {rcSrc.left, rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 11, rcDel��rcSrc�±���
	else if (!bSrcOnDelLeftTop && bSrcOnDelLeftBottom && !bSrcOnDelRightTop && bSrcOnDelRightBottom)
	{
		RECT rc1 = {rcSrc.left,  rcSrc.top,    rcSrc.right, rcDel.top};
		RECT rc2 = {rcSrc.left,  rcDel.bottom, rcDel.left,  rcSrc.bottom};
		RECT rc3 = {rcDel.right, rcDel.top,    rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
		rectList->push_back(rc3);
	}
	// 12, rcDel��rcSrc�±�
	else if (!bDelOnSrcLeftTop && bDelOnSrcLeftBottom && !bDelOnSrcRightTop && bDelOnSrcRightBottom)
	{
		RECT rc = {rcSrc.left, rcSrc.top, rcSrc.right, rcDel.top};
		rectList->push_back(rc);
	}
	// 13, rcDel��rcSrc���Ͻ�
	else if (bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && !bDelOnSrcRightTop && !bDelOnSrcRightBottom)
	{
		RECT rc1 = {rcDel.right, rcSrc.top,    rcSrc.right, rcDel.bottom};
		RECT rc2 = {rcSrc.left,  rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 14, rcDel��rcSrc���Ͻ�
	else if (!bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && bDelOnSrcRightTop && !bDelOnSrcRightBottom)
	{
		RECT rc1 = {rcSrc.left, rcSrc.top,    rcDel.left,  rcDel.bottom};
		RECT rc2 = {rcSrc.left, rcDel.bottom, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 15, rcDel��rcSrc���½�
	else if (!bDelOnSrcLeftTop && !bDelOnSrcLeftBottom && !bDelOnSrcRightTop && bDelOnSrcRightBottom)
	{
		RECT rc1 = {rcSrc.left, rcSrc.top, rcSrc.right, rcDel.top};
		RECT rc2 = {rcSrc.left, rcDel.top, rcDel.left,  rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 16, rcDel��rcSrc���½�
	else if (!bDelOnSrcLeftTop && bDelOnSrcLeftBottom && !bDelOnSrcRightTop && !bDelOnSrcRightBottom)
	{
		RECT rc1 = {rcSrc.left,  rcSrc.top, rcSrc.right, rcDel.top};
		RECT rc2 = {rcDel.right, rcDel.top, rcSrc.right, rcSrc.bottom};
		rectList->push_back(rc1);
		rectList->push_back(rc2);
	}
	// 17, rcSrc��rcDel��
	else if (bDelOnSrcLeftTop && bDelOnSrcLeftBottom && bDelOnSrcRightTop && bDelOnSrcRightBottom)
	{
	}
	// 18, rcDel��rcSrc���ཻ
	else
	{
		rectList->push_back(rcSrc);
	}
}

