#pragma once

#include "Srv\Srv_MMS.h"
#include "Player\WMPPlayer.h"

// CDlg_MMS �Ի���

class CDlg_MMS : public CDialog
{
	DECLARE_DYNAMIC(CDlg_MMS)

public:
	CDlg_MMS(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_MMS();

// �Ի�������
	enum { IDD = IDD_DLG_MMS };

private:
	XT_IOCP::CSrv_MMS	m_SrvMMS;
	CWMPPlayer	m_MediaPlayer;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButStartServer();
	afx_msg void OnBnClickedButStartClient();
};
