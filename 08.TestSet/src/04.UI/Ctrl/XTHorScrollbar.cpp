
#include "stdafx.h"
#include "XTHorScrollbar.h"
#include "XTDC.h"
#include <math.h>


/* xΪ���������м䲿�ֵĳ���

nLimit = nMax - rect.Width()


x                 rect.Width()
-------------------------- = --------------
rect.Width() - 16 * 2 - x       nLimit

*/

CXTHorScrollbar::CXTHorScrollbar()
	:list_(NULL),
	thumbPos_(15),
	thumbLen_(0),
	curPart_(NOT),
	lastPart_(NOT)
{
	memset(&thumb_, 0, sizeof(thumb_));
	memset(&leftArraw_, 0, sizeof(leftArraw_));
	memset(&rightArraw_, 0, sizeof(rightArraw_));
	memset(&lastDown_, 0, sizeof(lastDown_));
}

CXTHorScrollbar::~CXTHorScrollbar()
{
}


BEGIN_MESSAGE_MAP(CXTHorScrollbar, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


void CXTHorScrollbar::PreSubclassWindow() 
{
	ModifyStyle(0, BS_OWNERDRAW);
	CWnd::PreSubclassWindow();
}

LRESULT CXTHorScrollbar::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	DrawImage(DLGDC, OK, curPart_);
	lastPart_ = NOT;
	return 0; 
}

void CXTHorScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	//SetCapture();

	// ��������뿪�¼�
	TRACKMOUSEEVENT   stTRACKMOUSEEVENT = {0};
	stTRACKMOUSEEVENT.cbSize =  sizeof(stTRACKMOUSEEVENT);
	stTRACKMOUSEEVENT.dwFlags = TME_LEAVE;
	stTRACKMOUSEEVENT.hwndTrack = m_hWnd;
	
	_TrackMouseEvent(&stTRACKMOUSEEVENT);

	UINT state = (nFlags == MK_LBUTTON) ? MOUSE_DOWN : MOUSE_ON;

	point.x += x_;
	point.y = y_;

	//m_eCurrentPart = NUL;

	if (leftArraw_.PtInRect(point))
	{
		curPart_ = LEFTARRAW;
	}
	else if (rightArraw_.PtInRect(point))
	{
		curPart_ = RIGHTARRAW;
	}
	else if (thumb_.PtInRect(point))
	{
		curPart_ = THUMB;

		if (nFlags == MK_LBUTTON)
		{
			int nMoved = point.x - lastDown_.x;

			if (0 != nMoved)
			{
				lastDown_ = point;

				int	limit = list_->GetScrollLimit(SB_HORZ);
				int moved = (int)ceil((double)limit * nMoved / (cx_ - 15 * 2 - thumbLen_));

				CSize size;
				size.cx = moved;
				size.cy = 0;
				list_->Scroll(size);

				UpdateThumbPosition();
			}
			goto end;
		}
	}
	
	if (curPart_ == NOT && lastPart_ != NOT)
	{
		DrawImage(DLGDC, OK, lastPart_);			// �ָ�������
		lastPart_ = NOT;
	}
	else if (curPart_ != NOT && lastPart_ == NOT)
	{
		DrawImage(DLGDC, state, curPart_);	// �������ڵ��������
		lastPart_ = curPart_;
	}
	else if (curPart_ != NOT && lastPart_ != NOT && curPart_ != lastPart_)
	{
		DrawImage(DLGDC, state, curPart_);
		DrawImage(DLGDC, OK, lastPart_);
		lastPart_ = curPart_;
	}

end:
	CWnd::OnMouseMove(nFlags, point);
}

void CXTHorScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	point.x += x_;
	point.y += y_;

	if (leftArraw_.PtInRect(point))
	{
		curPart_ = LEFTARRAW;
		ScrollLeft();
		SetTimer(1, 50, NULL);
	}
	else if (rightArraw_.PtInRect(point))
	{
		curPart_ = RIGHTARRAW;
		ScrollRight();
		SetTimer(1, 50, NULL);
	}
	else if (thumb_.PtInRect(point))
	{
		curPart_ = THUMB;
		lastDown_ = point;
	}
	else if (point.x < (x_ + thumbPos_))
	{
		curPart_ = LEFTARRAW;
		PageLeft();
		SetTimer(2, 50, NULL);
	}
	else if (point.x > (x_ + thumbPos_ + thumbLen_))
	{
		curPart_ = RIGHTARRAW;
		PageRight();
		SetTimer(2, 50, NULL);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CXTHorScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();

	DrawImage(DLGDC, OK, lastPart_);

	curPart_ = NOT;
	lastPart_ = NOT;	

	KillTimer(1);
	KillTimer(2);

	CWnd::OnLButtonUp(nFlags, point);
}

void CXTHorScrollbar::OnTimer(UINT nIDEvent) 
{
	if (1 == nIDEvent)
	{
		if (RIGHTARRAW == curPart_)
			ScrollRight();
		else if (LEFTARRAW == curPart_)
			ScrollLeft();
	}
	else if (2 == nIDEvent)
	{
		if (RIGHTARRAW == curPart_)
			PageRight();
		else if (LEFTARRAW == curPart_)
			PageLeft();
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CXTHorScrollbar::OnEraseBkgnd(CDC* pDC)
{
	//���汳��
	//return CButton::OnEraseBkgnd(pDC);
	return TRUE;
}

void CXTHorScrollbar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)//���Ӵ˺����������
{
	DrawImage(DLGDC, 0, 0);
	DrawImage(COMPDC, 0, 0);
}

//��ͼ
//uObject:[IN]Ϊ0��m_hDlgDC�ϻ棬Ϊ1��m_hDlgDC��m_hCompDC��(ֻ�ڳ�ʼ��ʱ��)
//state:[IN]Ϊ0��ƽ��״̬��1����������棬2����갴��
//uPart:[IN]Ϊ0�����ͷ��Ϊ1�Ҽ�ͷ��Ϊ2�м䲿�֣�Ϊ3�м�ͱ���
void CXTHorScrollbar::DrawImage(int obj, int state, int part)
{
	HDC dc = (DLGDC == obj)?dc_:compDC_;

	switch(part)
	{
	case LEFTARRAW://���ͷ
		{				
			BitBlt(dc, x_, y_, 15, 15, imageDC_, sx_, sy_ + state * 15, SRCCOPY);
			break;
		}
	case RIGHTARRAW://�Ҽ�ͷ
		{
			BitBlt(dc,  x_ + cx_ - 15, y_, 15, 15, imageDC_, sx_ + 36, sy_ + state * 15, SRCCOPY);
			break;
		}
	case THUMB:
		{
			if (thumbLen_ >= 20)
			{
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_, y_, 5, 15, imageDC_, sx_ + 15, sy_ + state * 15, SRCCOPY);
				//�м䲿��.����
				::StretchBlt(dc, x_ + thumbPos_ + 5, y_, thumbLen_ - 10, 15, imageDC_, sx_ + 19, sy_ + state * 15, 1, 15, SRCCOPY);
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_ + thumbLen_/2 - 5, y_, 10, 15, imageDC_, sx_ + 20, sy_ + state * 15, SRCCOPY);
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_ + thumbLen_ - 5, y_, 5, 15, imageDC_, sx_ + 30, sy_ + state * 15, SRCCOPY);		
			}
			else if (thumbLen_ >= 15)
			{
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_, y_, 5, 15, imageDC_, sx_ + 15, sy_ + state * 15, SRCCOPY);
				//�м䲿��.����
				::StretchBlt(dc, x_ + thumbPos_ + 5, y_, thumbLen_ - 10, 15, imageDC_, sx_ + 19, sy_ + state * 15, 1, 15, SRCCOPY);
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_ + thumbLen_ - 5, y_, 5, 15, imageDC_, sx_ + 30, sy_ + state * 15, SRCCOPY);		
			}
			else if (thumbLen_ >= 5)
			{
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_, y_, 5, 15, imageDC_, sx_ + 15, sy_ + state * 15, SRCCOPY);
				//�м䲿��.��
				::BitBlt(dc, x_ + thumbPos_ + 3, y_, 3, 15, imageDC_, sx_ + 33, sy_ + state * 15, SRCCOPY);		
			}

			break;
		}
	case THUMB_BKG:
		{
			//�м䲿��.��
			::BitBlt(dc, x_ + thumbPos_, y_, 5, 15, imageDC_, sx_ + 15, sy_ + state * 15, SRCCOPY);
			//�м䲿��.����
			::StretchBlt(dc, x_ + thumbPos_ + 5, y_, thumbLen_ - 10, 15, imageDC_, sx_ + 19, sy_ + state * 15, 1, 15, SRCCOPY);
			//�м䲿��.��
			::BitBlt(dc, x_ + thumbPos_ + thumbLen_/2 - 5, y_, 10, 15, imageDC_, sx_ + 20, sy_ + state * 15, SRCCOPY);
			//�м䲿��.��
			::BitBlt(dc, x_ + thumbPos_ + thumbLen_ - 5, y_, 5, 15, imageDC_, sx_ + 30, sy_ + state * 15, SRCCOPY);
			//����.��
			::StretchBlt(dc, x_ + 15, y_, thumbPos_ - 15, 15, imageDC_, sx_ + 35, sy_ + state * 15, 1, 15, SRCCOPY);
			//����.��			
			::StretchBlt(dc, x_ + thumbPos_ + thumbLen_, y_,
				x_ + cx_ - 15 - x_ - thumbPos_ - thumbLen_, 15, imageDC_, sx_ + 35, sy_ + state * 15, 1, 15, SRCCOPY);
			break;
		}
	default:
		break;
	}
}

void CXTHorScrollbar::ScrollLeft()
{
	list_->SendMessage(WM_HSCROLL, MAKELONG(SB_LINELEFT, 0), NULL);
	UpdateThumbPosition();
}

void CXTHorScrollbar::ScrollRight()
{
	list_->SendMessage(WM_HSCROLL, MAKELONG(SB_LINERIGHT, 0), NULL);
	UpdateThumbPosition();
}

void CXTHorScrollbar::PageRight()
{
	list_->SendMessage(WM_HSCROLL, MAKELONG(SB_PAGERIGHT, 0), NULL);
	UpdateThumbPosition();
}

void CXTHorScrollbar::PageLeft()
{
	list_->SendMessage(WM_HSCROLL, MAKELONG(SB_PAGELEFT, 0), NULL);
	UpdateThumbPosition();
}

void CXTHorScrollbar::UpdateThumbPosition()
{
	int pos	= list_->GetScrollPos(SB_HORZ);	// ��ǰλ��
	int limit	= list_->GetScrollLimit(SB_HORZ);	// ʵ����Ҫ�����ĳ��� = ʵ�ʿ��(Max) - ��������

	if (0 != limit)
	{
		thumbLen_ = (int)ceil((double)cx_ * (cx_ - 15*2) / double(cx_ + limit));
		if (thumbLen_ < 5) thumbLen_ = 5;
		thumbPos_ = 15 + (int)ceil((double)pos * (cx_ - 15*2 - thumbLen_) / limit);
	}
	else
	{
		thumbLen_ = 0;
	}

	//�м䲿��
	thumb_.left	  = x_ + thumbPos_;
	thumb_.top	  = y_;
	thumb_.right  = x_ + thumbPos_ + thumbLen_;
	thumb_.bottom = y_ + cy_;

	DrawImage(DLGDC, 0, LEFTARRAW);
	DrawImage(DLGDC, 0, RIGHTARRAW);
	DrawImage(DLGDC, 0, THUMB_BKG);
	DrawImage(COMPDC, 0, LEFTARRAW);
	DrawImage(COMPDC, 0, RIGHTARRAW);
	DrawImage(COMPDC, 0, THUMB_BKG);
}

bool CXTHorScrollbar::Initial(int x, int y, int cx, int cy, int sx, int sy, CXTDC *xtDC)
{
	if (NULL == xtDC) return false;

	xtDC_ = xtDC;

	dc_      = xtDC_->GetHDC(WNDDC, 0);
	compDC_  = xtDC_->GetHDC(COMPDC, 0);
	imageDC_ = xtDC_->GetHDC(IMAGEDC, 0);

	// �Լ�
	x_ = x;
	y_ = y;
	cx_	= cx;
	cy_	= cy;
	
	// ͼƬλ��
	sx_	= sx;
	sy_	= sy;
	
	// �����ͷ
	leftArraw_.left	   = x_;
	leftArraw_.top	   = y_;
	leftArraw_.right   = x_ + 15;
	leftArraw_.bottom  = y_ + cy_;
	// ���Ҽ�ͷ
	rightArraw_.left   = x_ + cx_ - 15;
	rightArraw_.top	   = y_;
	rightArraw_.right  = x_ + cx_;
	rightArraw_.bottom = y_ + cy_;
	// �м䲿��
	thumb_.left		   = x_ + thumbLen_;
	thumb_.top		   = y_;
	thumb_.right	   = x_ + thumbPos_ + thumbLen_;
	thumb_.bottom	   = y_ + cy_;

	return true;
}

void CXTHorScrollbar::ResetImage(int sx, int sy)
{
	if (-1 != sx) sx_ = sx;
	if (-1 != sy) sy_ = sy;

	imageDC_ = xtDC_->curImageDC_;

	UpdateThumbPosition();
}

void CXTHorScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if ((thumbPos_ + thumbLen_) > (clientRect.right - 15))
		thumbPos_ = clientRect.right - 15 - thumbLen_;

	if (thumbPos_ < (clientRect.left + 15))
		thumbPos_ = clientRect.left + 5;
}

