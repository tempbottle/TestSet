#pragma once

#include "03.Network\Pinger.h"
#include "Dlg_Emule_KAD.h"
#include "Dlg_Emule_Search.h"


// CDlg_Emule �Ի���

class CDlg_Emule: public CDialog
{
    DECLARE_DYNAMIC(CDlg_Emule)

public:
    CDlg_Emule(CWnd* pParent = NULL); // ��׼���캯��
    virtual ~CDlg_Emule();

    // �Ի�������
    enum{ IDD = IDD_DLG_EMULE };

private:
    Pinger              m_Ping;

    CTabCtrl            m_Tab;
    CDialog             *m_pLastDlg;    // ����һ����ʾ�Ĵ���
    CDlg_Emule_KAD      m_DlgKAD;       // KAD����
    CDlg_Emule_Search   m_DlgSearch;    // ��ѯ����


protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedEmuleButPing();
    afx_msg void OnTcnSelchangeEmuleTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
