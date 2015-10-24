
#include "stdafx.h"
#include "Dlg_Chat_Srv.h"
#include "Dlg/Dlg_Chat_Clt.h"


IMPLEMENT_DYNAMIC(CDlg_Chat_Srv, CDialog)
CDlg_Chat_Srv::CDlg_Chat_Srv(CWnd* pParent /*=NULL*/)
    :CDialog(CDlg_Chat_Srv::IDD, pParent),
    m_bRun(FALSE),
    m_dwLast(0)
{	
}

CDlg_Chat_Srv::~CDlg_Chat_Srv()
{
    for (int i = 0; i < m_arrClient.GetCount(); i++)
    {
        m_arrClient[i]->DestroyWindow();
        delete m_arrClient[i];
    }
}

void CDlg_Chat_Srv::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    
    DDX_Control(pDX, IDC_CHAT_BTN_START, m_btnStart);
}

BEGIN_MESSAGE_MAP(CDlg_Chat_Srv, CDialog)
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_SYSCOMMAND()
    ON_BN_CLICKED(IDC_CHAT_BTN_START, OnBnClickedStart)
    ON_BN_CLICKED(IDC_CHAT_BTN_TEST, OnBnClickedTest)
    ON_BN_CLICKED(IDC_CHAT_BTN_CLIENT, OnBnClickedClient)
END_MESSAGE_MAP()

BOOL CDlg_Chat_Srv::OnInitDialog()
{
    CDialog::OnInitDialog();

    int nError = m_Server.Init();
    if (0 != nError)
    {
        CString strError(_T("��ʹ������:"));
        strError.AppendFormat(_T("%d"), nError);
        MessageBox(strError, _T("����"));
        m_btnStart.EnableWindow(FALSE);
    }

    return TRUE;
}

void CDlg_Chat_Srv::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������
    }
    else
    {
        CDialog::OnPaint();
    }
}

void CDlg_Chat_Srv::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_CLOSE)//�ر�
    {
        if (IDCANCEL == MessageBox(_T("�����Ҫ�˳���?"), _T("�˳���ʾ"), MB_OKCANCEL))
            return;
    }

    CDialog::OnSysCommand(nID, lParam);
}

void CDlg_Chat_Srv::OnTimer(UINT nIDEvent)
{
    DWORD dwNow = m_Server.GetNum();
    g_ProFile.m_Log.println(_T("%d"), dwNow - m_dwLast);
    m_dwLast = dwNow;

    CDialog::OnTimer(nIDEvent);
}

void CDlg_Chat_Srv::OnBnClickedStart()
{
    if (!m_bRun)
    {
        int nError = m_Server.Start();
        if (0 == nError)
        {
            m_bRun = TRUE;
            m_btnStart.SetWindowText(_T("ֹͣ����"));
        }
        else
        {
            CString strError(_T("��������ʱ���ִ���:"));
            strError.AppendFormat(_T("%d"), nError);
            MessageBox(strError, _T("����"));
        }
    }
    else
    {
        if (IDCANCEL == MessageBox(_T("�����Ҫֹͣ������?"), _T("��ʾ"), MB_OKCANCEL))	return;

        m_Server.Stop();
        m_bRun = FALSE;
        m_btnStart.SetWindowText(_T("��������"));
    }
}

void CDlg_Chat_Srv::OnBnClickedTest()
{
    this->SetTimer(1, 1000, NULL);
    m_Server.Test();
}

void CDlg_Chat_Srv::OnBnClickedClient()
{
    CDlg_Chat_Clt *pCltDlg = new CDlg_Chat_Clt();
    pCltDlg->Create(IDD_DLG_CHAT_CLT);
    m_arrClient.Add(pCltDlg);
}
