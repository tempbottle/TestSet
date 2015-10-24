
#ifndef _INCLUDED_SYSTEMTRAY_H_
#define _INCLUDED_SYSTEMTRAY_H_

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// ������CSystemTray

class CSystemTray : public CWnd
{
public:
    //�˵���ԴID
    UINT uIDtemp;

    //�����Ƿ���в˵�
    void ShowMenu(BOOL ShowMenuFlag);

// ���캯������������
public:
    CSystemTray();
    CSystemTray(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID);
    virtual ~CSystemTray();

    DECLARE_DYNAMIC(CSystemTray)

public:
    BOOL Enabled() { return m_bEnabled; }
    BOOL Visible() { return !m_bHidden; }

    // �������̴���
    BOOL Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID);

    // �ı�����������ʾ��Ϣ
    BOOL    SetTooltipText(LPCTSTR pszTooltipText);
    BOOL    SetTooltipText(UINT nID);
    CString GetTooltipText() const;

    // �ı�����ͼ����ʾ
    BOOL  SetIcon(HICON hIcon);
    BOOL  SetIcon(LPCTSTR lpszIconName);
    BOOL  SetIcon(UINT nIDResource);
    BOOL  SetStandardIcon(LPCTSTR lpIconName);
    BOOL  SetStandardIcon(UINT nIDResource);
    HICON GetIcon() const;
    void  HideIcon();
    void  ShowIcon();
    void  RemoveIcon();
    void  MoveToRight();

    // ������ʵ��
    BOOL  SetIconList(UINT uFirstIconID, UINT uLastIconID); 
    BOOL  SetIconList(HICON* pHIconList, UINT nNumIcons); 
    BOOL  Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  StepAnimation();
    BOOL  StopAnimation();
    BOOL  SetImageList(CImageList *pImageList);

    // �ı�˵���Ĭ����
    void GetMenuDefaultItem(UINT& uItem, BOOL& bByPos);
    BOOL SetMenuDefaultItem(UINT uItem, BOOL bByPos);

    // �ı�����windows֪ͨ��Ϣ��
    BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
    CWnd* GetNotificationWnd() const;

    // Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
    void Initialise();

    BOOL            m_bEnabled;   // does O/S support tray icon?
    BOOL            m_bHidden;    // Has the icon been hidden?
    NOTIFYICONDATA  m_tnd;

    /*
      Public Type NOTIFYICONDATA
        cbSize As Long
        hwnd As Long
        uID As Long
        uFlags As Long
        uCallbackMessage As Long
        hIcon As Long
        szTip As String * 64
      End Type
    */

    CImageList  *m_pImageList;

    CArray<HICON, HICON> m_IconList;
    static UINT  m_nIDEvent;
    UINT_PTR     m_uIDTimer;
    int          m_nCurrentIcon;
    COleDateTime m_StartTime;
    int          m_nAnimationPeriod;
    HICON        m_hSavedIcon;
    UINT         m_DefaultMenuItemID;
    BOOL         m_DefaultMenuItemByPos;

protected:
    afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
};

#endif
