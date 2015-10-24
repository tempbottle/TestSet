#pragma once
#include "UI/Ctrl/XTDialog.h"
#include "UI/Ctrl/XTButton.h"
#include "UI/Ctrl/XTComboBox.h"
#include "UI/Ctrl/XTRichEdit.h"
#include "UI/Ctrl/XTListCtrl.h"
#include "UI/Ctrl/XTTreeCtrl.h"
#include "UI/Html/DemoView.h"


class CDlg_Skin_Dlg2 : public CXTDialog
{
    DECLARE_DYNAMIC(CDlg_Skin_Dlg2)

public:
    CDlg_Skin_Dlg2(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CDlg_Skin_Dlg2();

    // �Ի�������
    enum { IDD = IDD_DLG_UI_HA };

private:
    //��ҳ���水ť
    CXTButton		m_btnMoney;
    CXTButton		m_btnShop;
    CXTButton		m_btnWeb;
    CXTButton		m_btnForum;
    CXTButton		m_btnSet;
    CXTButton		m_btnExit;
    CXTButton		m_btnTitleMin;
    CXTButton		m_btnTitleMax;
    CXTButton		m_btnTitleClose;
    CXTButton		m_btnBack;
    CXTButton		m_btnGo;
    CXTButton		m_btnRefresh;
    CXTButton		m_btnStop;
    CXTButton		m_btnHome;
    CXTButton		m_btnHelp;
    //������水ť
    CXTButton		m_btnPlaything;
    CXTButton		m_btnAdd;
    CXTButton		m_btnFind;
    CXTButton		m_btnTeach;
    CXTButton		m_btnCustomerserver;
    CXTButton		m_btnSetRoom;
    CXTButton		m_btnExitRoom;
    CXTButton		m_btnSend;
    CXTButton		m_btnSendShape;
    CXTButton		m_btnSetText;
    //�����ؼ�
    CXTListCtrl		m_lstUser;
    CXTComboBox		m_cbbInput;
    //CDemoView		m_thmWeb;
    CXTTreeCtrl		m_treRoom;
    CXTRichEdit		m_rieText;

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    afx_msg void OnTimer(UINT nIDEvent);

    DECLARE_MESSAGE_MAP()
public:
    void SetSkin(int nImage);
    afx_msg void OnBnClickedHaBtTitleClose();
    afx_msg void OnBnClickedHaBtTitleMoney();
    afx_msg void OnBnClickedHaBtTitleShop();
};
