
#pragma once
#include "resource.h"
#include "UI\XTDC.h"
#include "UI\XTButton.h"
#include "UI\XTCheckBox.h"
#include "UI\XTComboBox.h"
#include "UI\XTBackForth.h"
#include "UI\XTOutBarCtrl.h"
#include "Srv\Clt_Chat.h"


class CDlg_Chat_Clt : public CDialog
{
    DECLARE_DYNAMIC(CDlg_Chat_Clt)

public:
	CDlg_Chat_Clt(CWnd* pParent = NULL);	// ��׼���캯��
	~CDlg_Chat_Clt();

	enum { IDD = IDD_DLG_CHAT_CLT };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	HMODULE			m_hResource;
	HDC				m_hDC;
	HDC				m_hCompDC;
	HDC				m_hBmpDC;
	CXTDC			m_xtDC;
	CXTButton		m_btnMin;
	CXTButton		m_btnExit;
	CXTButton		m_btnLogin;
	CXTCheckBox		m_btnSavePass;
	CXTCheckBox		m_btnAutoLogin;	
	CXTComboBox		m_ComboBoxUser;
	CEdit			m_edPass;
	CXTBackForth	m_Progress;

	CXTOutBarCtrl	m_wndOutlookBar;

private:
	CClt_Chat   	m_Client;
	USERINFO_MAP	*m_pUserInfoMap;
	ID_ARRAY		*m_pFriendIdArray;
	CStringArrayEx	*m_pLoginUserInfo;	    // �Ѿ���½�����û���Ϣ��������XML�ļ���

	ID_ARRAY		m_OnlineUserIdArray;	// �ؼ�����ʾ�������û�˳��
    ID_ARRAY		m_OnlineFriendIdArray;	// �ؼ�����ʾ�����ߺ���˳��
	DWORD			m_dwMyId;
	int				m_nOnlineFriends;

	bool			m_bLoginDlg;
	bool			m_bDlgMin;

private:
	void ShowLoginDlg(int nCmdShow);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	LRESULT OnComboBoxSel(WPARAM wParam, LPARAM lParam);
	LRESULT OnProgressOk(WPARAM wParam, LPARAM lParam);
	LRESULT OnOutlookCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnAddFriendFail(WPARAM wParam, LPARAM lParam);
	LRESULT OnDelFriendFail(WPARAM wParam, LPARAM lParam);
	LRESULT OnSendTextFail(WPARAM wParam, LPARAM lParam);
	LRESULT OnNoFriendText(WPARAM wParam, LPARAM lParam);
	LRESULT OnDelChatDlg(WPARAM wParam, LPARAM lParam);

public:
	void CloseSocket(void);
	void LoginSuccess(ID_ARRAY	*pFriendIdArray, USERINFO_MAP *pUserInfoMap);
	void LoginFail(DWORD dwError);
	void OnlineUserInfo(DWORD dwUserId, CStringArrayEx *pLoginUserInfo);
	void UserExit(DWORD dwUserId);
	void AddFriendSuccess(DWORD dwFriendId);
	void DelFriendSuccess(DWORD dwFriendId);	
	void StartActiveUserIcon(DWORD dwUserId);
	void StopActiveUserIcon(DWORD dwUserId);

public:
	afx_msg void OnBtnClickedMin();
	afx_msg void OnBtnClickedExit();
	afx_msg void OnBtnClickedLogin();
	afx_msg void OnBtnClickedBtAutoLogin();
	afx_msg void OnBtnClickedBtSavePassword();
	afx_msg void OnEnLinkRicheditMessage(NMHDR *pNMHDR, LRESULT *pResult);
};
