
#pragma once

#include "resource.h"
#include "Dlg_Encode_Cfg.h"
#include "wmv\Encode\mspropshellwnd.h"
#include "wmv\Encode\wmencode.h"
#include "wmv\Encode\mspropshell.h"

class CDlg_Encode : public CDialog
{
    DECLARE_DYNAMIC(CDlg_Encode)

public:
    CDlg_Encode(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CDlg_Encode();

    // �Ի�������
    enum { IDD = IDD_DLG_ENCODE };

private:
    CMSPropShell	m_PropShell;
    IWMEncoder*		m_pEncoder;

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedEncodeConfigure();
    afx_msg void OnBnClickedEncodeStart();
    afx_msg void OnBnClickedEncodeStop();
};
