// XTMessageBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XTMessageBox.h"


#define BUTTON_CX		77
#define BUTTON_CY		21
#define BUTTON_INTER_CX	6
#define BUTTON_INTER_CY	10
#define ICON_CX			32
#define ICON_CY			32
#define ICON_INTER_CX	10
#define LT_CORNER_CX1	10
#define LT_CORNER_CY1	30
#define LT_CORNER_CX2	10
#define LT_CORNER_CY2	10
#define RB_CORNER_CX1	10
#define RB_CORNER_CY1	10
#define RB_CORNER_CX2	10
#define RB_CORNER_CY2	10


int XTMessageBox(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	CMessageDlg dlg;
	dlg.type_ = uType;
	dlg.text_ = lpText;
	dlg.caption_ = lpCaption;
	dlg.DoModal();
	return dlg.return_;
}

/*

#define IDC_MESSAGE_BTN_CLOSE			3000
#define IDC_MESSAGE_BTN_1				3001
#define IDC_MESSAGE_BTN_2				3002
#define IDC_MESSAGE_BTN_3				3003
#define IDD_MESSAGEDLG					3004
#define IDB_BMP_MESSAGEBOX              3005

IDD_MESSAGEDLG DIALOGEX 0, 0, 98, 44
STYLE DS_SETFONT | WS_POPUP | WS_SYSMENU
FONT 9, "����, MS Sans Serif", 0, 0, 0x0
BEGIN
PUSHBUTTON      "CLOSE",IDC_MESSAGE_BTN_CLOSE,66,6,24,12
PUSHBUTTON      "BTN1",IDC_MESSAGE_BTN_1,6,24,24,12
PUSHBUTTON      "BTN2",IDC_MESSAGE_BTN_2,36,24,24,12
PUSHBUTTON      "BTN3",IDC_MESSAGE_BTN_3,66,24,24,12
END
*/


IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CXTDialog(CMessageDlg::IDD, pParent),
	type_(0),
	return_(0),
	icon_(FALSE),
	fileDc_(NULL),
	color1_(NULL),
	color2_(NULL),
	fillBmp1_(NULL),
	fillBmp2_(NULL),
	fill1_(NULL),
	fill2_(NULL)
{
}

CMessageDlg::~CMessageDlg()
{
	font_.DeleteObject();
	::DeleteDC(fileDc_);
	::DeleteObject(color1_);
	::DeleteObject(color2_);
	::DeleteObject(fillBmp1_);
	::DeleteObject(fillBmp2_);
	::DeleteObject(fill1_);
	::DeleteObject(fill2_);
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_MESSAGE_BTN_1,		btn1_);
	DDX_Control(pDX,IDC_MESSAGE_BTN_2,		btn2_);
	DDX_Control(pDX,IDC_MESSAGE_BTN_3,		btn3_);
	DDX_Control(pDX,IDC_MESSAGE_BTN_CLOSE,	btnClose_);
}

BEGIN_MESSAGE_MAP(CMessageDlg, CXTDialog)
	ON_BN_CLICKED(IDC_MESSAGE_BTN_1,		OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_MESSAGE_BTN_2,		OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_MESSAGE_BTN_3,		OnBnClickedBtn3)
	ON_BN_CLICKED(IDC_MESSAGE_BTN_CLOSE,	OnBnClickedBtnClose)
END_MESSAGE_MAP()

/*
MB_DEFBUTTON1		 ���� ȱʡ��ťΪ��һ����ť
MB_DEFBUTTON2		 ���� ȱʡ��ťΪ�ڶ�����ť
MB_DEFBUTTON3		 ���� ȱʡ��ťΪ��������ť

MB_ICONEXCLAMATION	 ���� ��ʾͼ��Ϊ��̾��
MB_ICONQUESTION		 ���� ��ʾͼ��Ϊ�ʺ�
MB_ICONSTOP			 ���� ��ʾͼ��Ϊ����

MB_OK				 ���� ֻ��ʾȷ����ť
MB_OKCANCEL			 ���� ��ʾȷ����ȡ����ť
MB_ABORTRETRYIGNORE	 ���� ��ʾ��ֹ�����ԡ����԰�ť
MB_YESNOCANCEL		 ���� ��ʾ�ǡ���ȡ����ť
MB_YESNO			 ���� ��ʾ�ǡ���ť
MB_RETRYCANCEL		 ���� ��ʾ���ԡ�ȡ����ť
MB_CANCELTRYCONTINUE ���� ��ʾȡ�������ԡ�������ť

ͬʱӦ�ö������ʱ����|���ӡ�

����ֵ��
IDOK				 ���� ������ȷ����ť
IDCANCEL			 ���� ������ȡ����ť
IDABORT				 ���� ��������ֹ��ť
IDRETRY				 ���� ���������԰�ť
IDIGNORE			 ���� �����˺��԰�ť
IDYES				 ���� �������ǰ�ť
IDNO				 ���� �����˷�ť
IDCLOSE				 ���� �����˹رհ�ť
IDTRYAGAIN			 ���� ���������԰�ť
IDCONTINUE			 ���� �����˼�����ť
*/


BOOL CMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetBkgBmp(IDB_PNG_MESSAGE_BOX, NULL);

	LOGFONT lf = {0};
	lf.lfHeight = 15;
	font_.CreateFontIndirect(&lf);
	oldFont_ = SelectObject(compDC_, font_.GetSafeHandle());

	color1_ = CreateSolidBrush(RGB(255,255,255));
	color2_ = CreateSolidBrush(RGB(241,255,255));

	fileDc_ = CreateCompatibleDC(dc_);
	fillBmp1_ = CreateCompatibleBitmap(compDC_, 6, 1);
	fillBmp2_ = CreateCompatibleBitmap(compDC_, 1, 6);

	SelectObject(fileDc_, fillBmp1_);
	BitBlt(fileDc_, 0, 0, 6, 1, imageDC_, 11, 22, SRCCOPY);
	fill1_ = CreatePatternBrush(fillBmp1_);

	SelectObject(fileDc_, fillBmp2_);
	BitBlt(fileDc_, 0, 0, 1, 6, imageDC_, 11, 23, SRCCOPY);
	fill2_ = CreatePatternBrush(fillBmp2_);

	switch (type_ & 0x07)
	{
	case MB_OKCANCEL:
		SetDlg(2, 0, 21, 0, "ȷ��", "ȡ��", "");
		break;
	case MB_ABORTRETRYIGNORE:
		SetDlg(3, 42, 63, 84, "��ֹ", "����", "����");
		break;
	case MB_YESNOCANCEL:
		SetDlg(3, 105, 126, 21, "��", "��", "ȡ��");
		break;
	case MB_YESNO:
		SetDlg(2, 105, 126, 0, "��", "��", "");
		break;
	case MB_RETRYCANCEL:
		SetDlg(2, 63, 21, 0, "����", "ȡ��", "");
		break;
	case MB_CANCELTRYCONTINUE:
		SetDlg(3, 21, 63, 147, "ȡ��", "����", "����");
		break;
	default:
		SetDlg(1, 0, 0, 0, "ȷ��", "", "");
		break;
	}

	return TRUE;
}

void CMessageDlg::SetDlg(int btnNum, int ny1, int ny2, int ny3, 
	const char *btn1Name, const char *btn2Name, const char *btn3Name)
{
	// �������������С
	CRect rcText(0,0,0,0);
	DrawText(compDC_, text_.c_str(), text_.length(), &rcText, DT_CALCRECT);

	// ��ͼ��
	if ((type_ & MB_ICONEXCLAMATION) || (type_ & MB_ICONQUESTION) || (type_ & MB_ICONSTOP))
	{
		icon_ = TRUE;
	}

	int ncx = (icon_ ?  ICON_CX + ICON_INTER_CX : 0) + rcText.Width();
	int ncy = (icon_ ?  ICON_CY : 0) + rcText.Height();
	int cx = LT_CORNER_CX1 + LT_CORNER_CX2 + RB_CORNER_CX1 + RB_CORNER_CX2 + (ncx < BUTTON_CX*btnNum ?  BUTTON_CX*btnNum : ncx);
	int cy = LT_CORNER_CY1+LT_CORNER_CY2+RB_CORNER_CY1+RB_CORNER_CY2+BUTTON_CY+BUTTON_INTER_CY + (rcText.Height() > ICON_CY ?  rcText.Height() : ICON_CY);
	int nB1 = (cx - BUTTON_CX*btnNum - BUTTON_INTER_CX*(btnNum-1)) / 2;
	int nB2 = nB1 + BUTTON_CX + BUTTON_INTER_CX;
	int nB3 = nB2 + BUTTON_CX + BUTTON_INTER_CX;

	this->SetDlgRect(cx+1, cy+1, 32);

	::BitBlt(compDC_,       0,      0, 20, 20, imageDC_,  0,  0, SRCCOPY);	// ���Ͻ�
	::BitBlt(compDC_, cx - 20,      0, 20, 20, imageDC_, 23,  0, SRCCOPY);	// ���Ͻ�
	::BitBlt(compDC_, 0,      cy - 20, 20, 20, imageDC_,  0, 39, SRCCOPY);	// ���½�
	::BitBlt(compDC_, cx - 20,cy - 20, 20, 20, imageDC_, 23, 39, SRCCOPY);	// ���½�
	::StretchBlt(compDC_,       0, 20, 20, cy - 40, imageDC_,  0, 20, 20,  1, SRCCOPY);	// ���
	::StretchBlt(compDC_, cx - 20, 20, 20, cy - 40, imageDC_, 23, 20, 20,  1, SRCCOPY);	// �ұ�
	::StretchBlt(compDC_,      20,  0, cx - 40, 20, imageDC_, 20,  0,  1, 20, SRCCOPY);	// �ϱ�
	::StretchBlt(compDC_, 20, cy - 20, cx - 40, 20, imageDC_, 20, 39,  1, 20, SRCCOPY);	// �±�

	::FillRect(compDC_, &CRect(20, 20, cx-20, cy-20), color1_);
	::FillRect(compDC_, &CRect(LT_CORNER_CX1, LT_CORNER_CY1, cx-RB_CORNER_CX2, cy-RB_CORNER_CY2-BUTTON_CY-BUTTON_INTER_CY), fill1_);
	::FillRect(compDC_, &CRect(LT_CORNER_CX1, LT_CORNER_CY1+1, cx-RB_CORNER_CX2, cy-RB_CORNER_CY2-BUTTON_CY-BUTTON_INTER_CY-1), fill2_);
	::FillRect(compDC_, &CRect(LT_CORNER_CX1+1, LT_CORNER_CY1+1, cx-RB_CORNER_CX2-1, cy-RB_CORNER_CY2-BUTTON_CY-BUTTON_INTER_CY-1), color2_);
	

	rcText.left += LT_CORNER_CX1 + LT_CORNER_CX2 + (icon_ ?  ICON_CX + ICON_INTER_CX: 0);
	rcText.right += LT_CORNER_CX1 + LT_CORNER_CX2 + (icon_ ?  ICON_CX + ICON_INTER_CX: 0);
	rcText.top += LT_CORNER_CY1 + LT_CORNER_CY2;
	rcText.bottom += LT_CORNER_CY1 + LT_CORNER_CY2;

	if ("" != caption_)
		::DrawText(compDC_, caption_.c_str(), caption_.length(), &CRect(20, 8, cx - 40, LT_CORNER_CY1), DT_SINGLELINE|DT_WORD_ELLIPSIS);
	
	::DrawText(compDC_, text_.c_str(), text_.length(), &rcText, DT_CENTER|DT_WORDBREAK);

	btnClose_.Initial(cx-30, 10, 13, 13, 0, 61, &xtDC_, "�˳�");

	if (1 == btnNum)
	{
		btn1_.Initial(nB1, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny1, &xtDC_, btn1Name);
		btn2_.ShowWindow(SW_HIDE);
		btn3_.ShowWindow(SW_HIDE);
	}
	else if (2 == btnNum)
	{
		btn1_.Initial(nB1, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny1, &xtDC_, btn1Name);
		btn2_.Initial(nB2, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny2, &xtDC_, btn2Name);
		btn3_.ShowWindow(SW_HIDE);
	}
	else if (3 == btnNum)
	{
		btn1_.Initial(nB1, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny1, &xtDC_, btn1Name);
		btn2_.Initial(nB2, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny2, &xtDC_, btn2Name);
		btn3_.Initial(nB3, cy-RB_CORNER_CY2-BUTTON_CY, 77, 21, 53, ny3, &xtDC_, btn3Name);
	}

	switch (type_ & 0x070)
	{
	case MB_ICONEXCLAMATION:	// ��ʾͼ��Ϊ��̾��
		{
			icon_ = TRUE;
			BitBlt(compDC_, LT_CORNER_CX1+LT_CORNER_CX2, LT_CORNER_CY1+LT_CORNER_CY2, 32, 32, imageDC_, 0, 76, SRCCOPY);
			break;
		}
	case MB_ICONQUESTION:		// ��ʾͼ��Ϊ�ʺ�
		{
			icon_ = TRUE;
			BitBlt(compDC_, LT_CORNER_CX1+LT_CORNER_CX2, LT_CORNER_CY1+LT_CORNER_CY2, 32, 32, imageDC_, 0, 144, SRCCOPY);
			break;
		}
	case MB_ICONSTOP:			// ��ʾͼ��Ϊ����
		{
			icon_ = TRUE;
			BitBlt(compDC_, LT_CORNER_CX1+LT_CORNER_CX2, LT_CORNER_CY1+LT_CORNER_CY2, 32, 32, imageDC_, 0, 110, SRCCOPY);
			break;
		}
	}
}

void CMessageDlg::OnBnClickedBtnClose()
{
	OnCancel();
	return_ = IDCLOSE;
}

void CMessageDlg::OnBnClickedBtn1()
{
	OnCancel();

	if (type_ & MB_OKCANCEL)				// ��ʾȷ����ȡ����ť
	{
		return_ = IDOK;
	}
	else if (type_ & MB_ABORTRETRYIGNORE)	// ��ʾ��ֹ�����ԡ����԰�ť
	{
		return_ = IDABORT;
	}
	else if (type_ & MB_YESNOCANCEL)		// ��ʾ�ǡ���ȡ����ť
	{
		return_ = IDYES;
	}
	else if (type_ & MB_YESNO)				// ��ʾ�ǡ���ť
	{
		return_ = IDYES;
	}
	else if (type_ & MB_RETRYCANCEL)		// ��ʾ���Ժ�ȡ����ť
	{
		return_ = IDTRYAGAIN;
	}
	else if (type_ & MB_CANCELTRYCONTINUE)	// ��ʾȡ�������ԡ�������ť
	{
		return_ = IDCANCEL;
	}
	else									// ֻ��ʾȷ����ť
	{
		return_ = IDOK;
	}
}

void CMessageDlg::OnBnClickedBtn2()
{
	OnCancel();

	if (type_ & MB_OKCANCEL)				// ��ʾȷ����ȡ����ť
	{
		return_ = IDCANCEL;
	}
	else if (type_ & MB_ABORTRETRYIGNORE)	// ��ʾ��ֹ�����ԡ����԰�ť
	{
		return_ = IDRETRY;
	}
	else if (type_ & MB_YESNOCANCEL)		// ��ʾ�ǡ���ȡ����ť
	{
		return_ = IDYES;
	}
	else if (type_ & MB_YESNO)				// ��ʾ�ǡ���ť
	{
		return_ = IDNO;
	}
	else if (type_ & MB_RETRYCANCEL)		// ��ʾ���Ժ�ȡ����ť
	{
		return_ = IDCANCEL;
	}
	else if (type_ & MB_CANCELTRYCONTINUE)	// ��ʾȡ�������ԡ�������ť
	{
		return_ = IDRETRY;
	}
}

void CMessageDlg::OnBnClickedBtn3()
{
	OnCancel();

	if (type_ & MB_ABORTRETRYIGNORE)		// ��ʾ��ֹ�����ԡ����԰�ť
	{
		return_ = IDIGNORE;
	}
	else if (type_ & MB_YESNOCANCEL)		// ��ʾ�ǡ���ȡ����ť
	{
		return_ = IDCANCEL;
	}
	else if (type_ & MB_CANCELTRYCONTINUE)	// ��ʾȡ�������ԡ�������ť
	{
		return_ = IDCONTINUE;
	}
}
