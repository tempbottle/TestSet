// src\Dlg\Dlg_Skin_Dlg3.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestSet.h"
#include "src\Dlg\Dlg_Skin_Dlg3.h"
#include ".\dlg_skin_dlg3.h"


// CDlg_Skin_Dlg3 �Ի���

IMPLEMENT_DYNAMIC(CDlg_Skin_Dlg3, CDialog)
CDlg_Skin_Dlg3::CDlg_Skin_Dlg3(CWnd* pParent /*=NULL*/)
    : CDialog(CDlg_Skin_Dlg3::IDD, pParent)
{
    m_hDC = NULL;
    m_hCompDC = NULL;
    m_hBmpDC = NULL;
}

CDlg_Skin_Dlg3::~CDlg_Skin_Dlg3()
{
}

void CDlg_Skin_Dlg3::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX,IDC_GA_MIN,				m_btnMin);
    DDX_Control(pDX,IDC_GA_CLOSE,			m_btnClose);
    DDX_Control(pDX,IDC_GA_SET,				m_btnSet);
    DDX_Control(pDX,IDC_GA_GAMEHELP,		m_btnHelp);
    DDX_Control(pDX,IDC_GA_CAMERA1,			m_btnCamera[0]);
    DDX_Control(pDX,IDC_GA_CAMERA2,			m_btnCamera[1]);
    DDX_Control(pDX,IDC_GA_CAMERA3,			m_btnCamera[2]);
    DDX_Control(pDX,IDC_GA_VIDICON1,		m_btnVidicon[0]);
    DDX_Control(pDX,IDC_GA_VIDICON2,		m_btnVidicon[1]);
    DDX_Control(pDX,IDC_GA_VIDICON3,		m_btnVidicon[2]);
    DDX_Control(pDX,IDC_GA_CAMERA_BIG,		m_btnCameraBig);
    DDX_Control(pDX,IDC_GA_VIDICON_BIG,		m_btnVidiconBig);
    DDX_Control(pDX,IDC_GA_SEND,			m_btnSend);
    DDX_Control(pDX,IDC_GA_SEND_SECRETINFO,	m_btnSendSecret);
    DDX_Control(pDX,IDC_GA_START,			m_btnStart);
    DDX_Control(pDX,IDC_GA_LIST1,			m_lstUser);
    DDX_Control(pDX,IDC_GA_MESSAGE,			m_cbMessage);
    DDX_Control(pDX,IDC_GA_SECRET_MSG,		m_cbSecretMessage);
}


BEGIN_MESSAGE_MAP(CDlg_Skin_Dlg3, CDialog)
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_GA_CLOSE, OnBnClickedClose)
    ON_BN_CLICKED(IDC_GA_SET, OnBnClickedSet)
END_MESSAGE_MAP()



BOOL CDlg_Skin_Dlg3::OnInitDialog()
{
    CDialog::OnInitDialog();

    //���������ʾ
    CRect Rect;
    GetWindowRect(&Rect);
    int x = Rect.left -(853 + 2 - Rect.Width()) / 2;
    int y = Rect.top  -(602 + 2 - Rect.Height()) / 2;
    if (x < 0)	x = 0;
    if (y < 0)	y = 0;
    MoveWindow(x, y, 853 + 2, 602 + 2);
    this->CenterWindow();

    //����Բ��
    HRGN hRgn = CreateRoundRectRgn(0, 0, 853 + 2, 602 + 2, 15, 15);
    SetWindowRgn(hRgn, TRUE);

    m_hDC = m_xtDC.Init(NULL, this->m_hWnd);
    m_hCompDC = m_xtDC.AddCompDC();
    m_hBmpDC = m_xtDC.AddPngDC(0, IDB_UI_PNG_GAME);
    ::BitBlt(m_hCompDC, 0, 0, 853, 602, m_hBmpDC, 0, 0, SRCCOPY);

    m_btnMin.Initial(791, 6, 20, 20, 339, 743, &m_xtDC, _T("��С��"));
    m_btnClose.Initial(819, 6, 20, 20, 400, 743, &m_xtDC, _T("�ر�"));
    m_btnSet.Initial(665, 2, 56, 26, 1, 743, &m_xtDC, _T("����"));
    m_btnHelp.Initial(721, 2, 56, 26, 170, 743, &m_xtDC, _T("����"));
    m_btnCamera[0].Initial(292, 235, 17, 17, 211, 771, &m_xtDC, _T("�����"));
    m_btnCamera[1].Initial(447, 235, 17, 17, 211, 771, &m_xtDC, _T("�����")) ;
    m_btnCamera[2].Initial(602, 235, 17, 17, 211, 771, &m_xtDC, _T("�����"));
    m_btnVidicon[0].Initial(311, 235, 17, 17, 159, 771, &m_xtDC, _T("¼���"));
    m_btnVidicon[1].Initial(466, 235, 17, 17, 159, 771, &m_xtDC, _T("¼���"));
    m_btnVidicon[2].Initial(621, 235, 17, 17, 159, 771, &m_xtDC, _T("¼���"));
    m_btnCameraBig.Initial(786, 318, 26, 26, 80, 771, &m_xtDC, _T("�����"));
    m_btnVidiconBig.Initial(816, 318, 26, 26, 1, 771,  &m_xtDC, _T("¼���"));
    m_btnStart.Initial(588, 534, 54, 54, 1, 604, &m_xtDC, _T("׼��"));
    m_btnSend.Initial(545, 564, 43, 25, 1, 716, &m_xtDC, _T("����"));
    m_btnSendSecret.Initial(815, 574, 24, 20, 391, 716, &m_xtDC, _T("����������Ϣ"));

    CString strHeads(_T("�û�ID,80;�û��ǳ�,110;����,30;��λ,30;����,30;����,30;����,30;���,50;"));
    m_lstUser.Initial(4, 312, 172, 274, 1, 799, &m_xtDC, strHeads);	

    CString strText;
    for (int i = 0; i < 250; i++)
    {
        strText.Format(_T("User %d"), i);
        m_lstUser.InsertItem(i, strText);
    }

    m_cbMessage.Initial(195, 567, 340, 100, 461, 743, &m_xtDC);
    m_cbMessage.AddString(_T("����ˮ��,��Ҫɱ�ң�����Ҳû��!"));
    m_cbMessage.AddString(_T("���Ǻ������"));
    m_cbMessage.AddString(_T("���Ϊ����Ʊ�����������Ǽ�ֵ��ʱ����!"));

    m_cbSecretMessage.Initial(669, 575, 135, 100, 461, 743, &m_xtDC);
    m_cbSecretMessage.AddString(_T("�ֵܿ��������!"));
    m_cbSecretMessage.AddString(_T("��û��ã�����һ���˰�!"));	
    m_cbSecretMessage.AddString(_T("��˭��?"));	
    m_cbSecretMessage.AddString(_T("��Ҫ����!�����Ǹ��ң���͸���ɱ"));
    m_cbSecretMessage.AddString(_T("��Ҫ�Ϸˣ���ȥ�������ǵ�ע����!"));
    m_cbSecretMessage.AddString(_T("ɱ˭��?"));
    m_cbSecretMessage.AddString(_T("һ�����ڸ��ӽ�����������!!"));

    //---------------------------------------------------------------------------------------

    HBITMAP hMaskBmp = ::CreateBitmap(160, 32, 1, 1, NULL);
    if (NULL == hMaskBmp) return FALSE;

    HDC hMaskDC = ::CreateCompatibleDC(m_hDC);
    if (NULL == hMaskDC) return FALSE;

    HBITMAP hOldMaskBmp = (HBITMAP)::SelectObject(hMaskDC, hMaskBmp);

    ::SetBkColor(m_hBmpDC, RGB(96, 123, 159));

    ::BitBlt(hMaskDC, 0, 0, 160, 32, m_hBmpDC, 464, 771, SRCCOPY);

    ::BitBlt(m_hCompDC, 355, 288, 32, 32, m_hBmpDC, 464, 771, SRCINVERT);
    ::BitBlt(m_hCompDC, 355, 288, 32, 32, hMaskDC, 0, 0, SRCAND);
    ::BitBlt(m_hCompDC, 355, 288, 32, 32, m_hBmpDC, 464, 771, SRCINVERT);

    ::SelectObject(hMaskDC, hOldMaskBmp);

    ::DeleteDC(hMaskDC);
    ::DeleteObject((HGDIOBJ)hMaskBmp);

    return TRUE;
}

void CDlg_Skin_Dlg3::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������
    }
    else
    {
        if (m_hDC == NULL || m_hCompDC == NULL)	return;

        ::BitBlt(m_hDC, 0, 0, 853, 602, m_hCompDC, 0, 0, SRCCOPY);

        CDialog::OnPaint();
    }
}

void CDlg_Skin_Dlg3::OnMouseMove(UINT nFlags, CPoint point)
{
    if (point.y < 100)
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

    CDialog::OnMouseMove(nFlags, point);
}
void CDlg_Skin_Dlg3::OnBnClickedClose()
{
    OnCancel();
}

void CDlg_Skin_Dlg3::OnBnClickedSet()
{
    //PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);	
}

