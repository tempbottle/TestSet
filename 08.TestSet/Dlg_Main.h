#pragma once

#include "11.Common\SystemTray.h"

enum
{
    DLG_FINGER,         // ָ��ʶ��
    DLG_MEDIA,          // ��ý��
    DLG_P2P,            // P2P�������
    DLG_NETWORK,        // ����
    DLG_UI,             // ����
    DLG_CPU,            // ����CPU
    DLG_EMULE,          // ��¿
    DLG_DATABASE,       // ���ݿ�
    DLG_ALGORITHM,      // �㷨
    DLG_ALL,            // �������
};


class CDlg_Main : public CDialog
{

public:
    CDlg_Main(CWnd* pParent = NULL);
    enum { IDD = IDD_MAIN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    CComboBox           comboMain_;        // ������
    CSystemTray         sysTray_;          // ������ͼ��

    CDialog             *curDlg_;         // �浱ǰ��ʾ�Ĵ���
    CDialog             *dlg_[DLG_ALL];   // ���д���

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    void SetTransparent(BYTE byTrans);
    CDialog* NewDlg(int nIndex, int &nID);

public:
    LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnMenuShow();
    afx_msg void OnMenuExit();
    afx_msg void OnMenuAutorun();
    afx_msg void OnMenuCloseMin();
    afx_msg void OnSelchangeComboMain();
    afx_msg void OnBnClickedMainBtnCreate();
};
