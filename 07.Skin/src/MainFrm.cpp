// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainFrm.h"

#pragma comment(lib, "dulib.lib")   // �����


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
    return FALSE;
}

CMainFrame::CMainFrame()
    :wndManager_(NULL)
{
    currentPath_[0] = _T('\0');
}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);
    SetMenu(NULL);

    GetCurrentPath();

    LoadSkin();

    return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    // unregister message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);
    bHandled = FALSE;
    return 1;
}

LRESULT CMainFrame::OnDirectUINotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    duBEGIN_MESSAGE_MAP(CMainFrame, wParam, lParam)
        duON_MESSAGE(windows_min,   DUM_BTNCLICK, &CMainFrame::OnMinClick)
        duON_MESSAGE(windows_close, DUM_BTNCLICK, &CMainFrame::OnCloseClick)
    duEND_MESSAGE_MAP()
    return 0;
}

LRESULT CMainFrame::OnMinClick(duPlugin * /*pPlugin*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    ShowWindow(SW_SHOWMINIMIZED);
    InvalidateRect(NULL,TRUE);
    return 0;
}

LRESULT CMainFrame::OnCloseClick(duPlugin * /*pPlugin*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    PostMessage(WM_CLOSE, NULL, NULL);
    return 0;
}

/**
 *\fn         GetCurrentPath()
 *\brief      �õ�����ĵ�ǰ·����
 *\return     �ޡ�
 */
void CMainFrame::GetCurrentPath()
{
    TCHAR path[MAX_PATH+1] = _T("");
    GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);    
    PathRemoveFileSpec(path);
    _tcscpy_s(currentPath_, MAX_PATH, path);
}

/**
 *\fn         LoadSkin()
 *\brief      ����DirectUIƤ����
 *\return     �Ƿ�ɹ�,0�ɹ�������ʧ��
 */
int CMainFrame::LoadSkin()
{
    TCHAR path[MAX_PATH+1] = _T("");
    _stprintf_s(path, MAX_PATH, _T("%s\\skin\\DefaultSkin\\FileUpdate\\FileUpdateSkin.zip"), currentPath_);

    wndManager_ = LoadStyleZipW(path);
    if (NULL == wndManager_)
    {
        ATLTRACE(_T("���ؽ����ʧ��,%s"), path);
        return -1;
    }

    ATLTRACE(_T("���ؽ����ɹ�,%s"), path);

    WindowManager_Attach(wndManager_, m_hWnd, _T("Update_Window"));

    duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_hWnd, _T("headerctrl_0"));
    pHeaderCtrl->InsertItem(0, _T("�ļ�����"), FALSE, 280, NULL);
    pHeaderCtrl->InsertItem(1, _T("�ٶ�"),     FALSE,  75, NULL);
    pHeaderCtrl->InsertItem(2, _T("���ؽ���"), FALSE, 150, NULL);

    duListCtrl *listCtrl = (duListCtrl*)GetPluginByName(m_hWnd, _T("listctrl_0"));

    for (int i = 0; i < 10; i++)
    {
        duLayout *pLayout = (duLayout*)listCtrl->InsertLine(0, _T("listctrl_0_template_"));
        if (!pLayout)
        {
            ;
        }
    }

    return 0;
}