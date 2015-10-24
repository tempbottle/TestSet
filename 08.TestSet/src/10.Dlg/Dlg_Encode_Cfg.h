
#pragma once

#include "resource.h"
#include "wmv\Encode\mspropshellwnd.h"
#include "wmv\Encode\wmencode.h"
#include "wmv\Encode\mspropshell.h"

class CDlg_Encode_Cfg : public CDialog
{
    DECLARE_DYNAMIC(CDlg_Encode_Cfg)

public:
    CDlg_Encode_Cfg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CDlg_Encode_Cfg();

    // �Ի�������
    enum { IDD = IDD_DLG_ENCODE_CONFIG };

    CMSPropShell	m_PropShellConfig;
    IWMEncoder*		m_pEncoder;
    IMSPropShell*	pPropShell;

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    void SetEncoder(IWMEncoder* pEncoder);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
};
