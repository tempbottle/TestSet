#pragma once


// CDlg_NakNet �Ի���

class CDlg_RakNet : public CDialog
{
	DECLARE_DYNAMIC(CDlg_RakNet)

public:
	CDlg_RakNet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_RakNet();

// �Ի�������
	enum { IDD = IDD_DLG_RAKNET };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRaknetBtnRun();
};
