/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       AddinWnd.h
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS�������ʵ��
*************************************************/

#include "stdafx.h"
#include "AddinWnd.h"
#include "TerminalWnd.h"
#include "BrowseWnd.h"

#pragma comment(lib,"Comctl32.lib") // ImageList_LoadImage

#define S_L     5   // ��߿���
#define S_R     5   // �ұ߿���
#define S_T     5   // �ϱ߿���
#define S_B     5   // �±߿���
#define S_MT    30  // �����м����

typedef void (*setParamCallback)(int id, void *param);
void saveFileCallback(void *param, const char *filename); // BrowseWnd�ж���


CAddinWnd::CAddinWnd()
    :hImageList_(NULL),
    hCursorDragWnd_(NULL),
    hCursorDragFile_(NULL),
    font_(NULL),
    wnd_(NULL),
    msgMap_(NULL),
    open_file_cb_(NULL),
    output_text_cb_(NULL),
    config_(NULL),
    serverInfo_(NULL),
    serverCount_(0),
    showServerIndex_(-1),
    connectServerCount_(0)
{
    projectName_[0] = '\0';
    memset(connectServer_, 0, sizeof(connectServer_));

    LOGFONT lf = {0};
    lf.lfHeight = 14;
    strcpy_s(lf.lfFaceName, "Lucida Console");//Courier New
    font_ = ::CreateFontIndirect(&lf);
}

CAddinWnd::~CAddinWnd()
{
    destroy();

    for (int i = 0; i < connectServerCount_; i++)
    {
        if (SUB_TYPE_TERMINAL == connectServer_[i]->type_)
        {
            delete (CTerminalWnd*)connectServer_[i];
        }
        else
        {
            delete (CBrowseWnd*)connectServer_[i];
        }
    }

    if (NULL != serverInfo_)
    {
        delete[] serverInfo_;
    }
}

/**
 *\fn           void destroy()
 *\brief        �ͷŴ������Դ
 *\return       void ��
 */
void CAddinWnd::destroy()
{
    if (comboServer_.IsWindow())
    {
        comboServer_.DestroyWindow();
    }

    if (comboConnect_.IsWindow())
    {
        comboConnect_.DestroyWindow();
    }

    for (int i = 0; i < connectServerCount_; i++)
    {
        connectServer_[i]->destroy();
    }
}

/**
 *\fn           void setParam(int id, void *data)
 *\brief        ���ò���
 *\param[in]    int id ���
 *\param[in]    void * data ����
 *\return       void ��
 */
void CAddinWnd::setParam(int id, void *data)
{
    switch (id)
    {
    case 1: // ������Ŀ����
        {
            strcpy_s(projectName_, (const char*)data);
            break;
        }
    case 2: // ��������ص�
        {
            open_file_cb_ = (ssh_recv_data_callback)data; // ����ssh�������ݻص�
            break;
        }
    case 3: // ���ô��ļ��ص�
        {
            output_text_cb_ = (ssh_recv_data_callback)data; // ����ssh�������ݻص�
            break;
        }
    case 4: // ���������ļ�
        {
            config_ = (Config*)data;
            addServer();
            break;
        }
    case 5: // �������ûص�
        {
            setParamCallback setParamCb = (setParamCallback)data;
            setParamCb(0, saveFileCallback);
            break;
        }
    }
}

/**
 *\fn           int create(HINSTANCE instance, CMessageMap *map, HWND wnd, 
 *\                        int icon, int cursorDrag, int cursorCopy)
 *\brief        ��������
 *\param[in]    HINSTANCE instance ʵ�����
 *\param[in]    CMessageMap * map ��Ϣѭ��
 *\param[in]    HWND wnd ������
 *\param[in]    int icon ͼ��
 *\param[in]    int cursorDrag �϶�ʱͼ��
 *\param[in]    int cursorCopy ����ʱͼ��
 *\return       int 0�ɹ�,����ʧ��
 */
int CAddinWnd::create(HINSTANCE instance, CMessageMap *map, HWND wnd, 
    int icon, int cursorDrag, int cursorCopy)
{
    if (NULL == map) return -1;

    wnd_ = wnd;
    msgMap_ = map;

    hCursorDragWnd_ = ::LoadCursor(instance, MAKEINTRESOURCE(cursorDrag));//IDC_CURSOR_DRAG_HZ
    hCursorDragFile_ = ::LoadCursor(instance, MAKEINTRESOURCE(cursorCopy));//IDC_CURSOR_COPY
    hImageList_ = ImageList_LoadImage(instance, MAKEINTRESOURCE(icon), //IDB_BITMAP_ICON
        16, 0, RGB(0,0,255), IMAGE_BITMAP, ILC_MIRROR);

    RECT rc = {0};
    comboServer_.m_pObject = map;
    comboServer_.m_dwMsgMapID = 1;
    comboServer_.m_lpszClassName = WC_COMBOBOX;
    comboServer_.Create(wnd, rc, _T(""), WS_VISIBLE|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|CBS_SORT);
    comboServer_.SetFont(font_);

    comboConnect_.m_pObject = map;
    comboConnect_.m_dwMsgMapID = 2;
    comboConnect_.m_lpszClassName = WC_COMBOBOX;
    comboConnect_.Create(wnd, rc, _T(""), WS_VISIBLE|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|CBS_SORT);
    comboConnect_.SetFont(font_);

    return 0;
}

/**
 *\fn           int addServer()
 *\brief        ��ӷ�����
 *\return       int ��
 */
int CAddinWnd::addServer()
{
    char text[512] = {0};
    serverCount_ = config_->getServerCount();

    serverInfo_ = new ServerInfo[serverCount_];

    for (int i = 0; i < serverCount_; i++)
    {
        config_->getServerParam(i, serverInfo_[i].serverAddr, serverInfo_[i].serverPort, 
            serverInfo_[i].username, serverInfo_[i].password, serverInfo_[i].port_map_count);
    }

    comboServer_.SetRedraw(FALSE);

    int index = 0;

    for (int i = 0; i < serverCount_; i++)
    {
        _snprintf_s(text, sizeof(text)-1, "ssh://%s@%s:%d", serverInfo_[i].username, 
            serverInfo_[i].serverAddr, serverInfo_[i].serverPort);

        index = comboServer_.SendMessage(CB_ADDSTRING, 0, (LPARAM)text);
        comboServer_.SendMessage(CB_SETITEMDATA, index, (LPARAM)i);
    }

    for (int i = 0; i < serverCount_; i++)
    {
        _snprintf_s(text, sizeof(text)-1, "sftp://%s@%s:%d", serverInfo_[i].username, 
            serverInfo_[i].serverAddr, serverInfo_[i].serverPort);

        index = comboServer_.SendMessage(CB_ADDSTRING, 0, (LPARAM)text);
        comboServer_.SendMessage(CB_SETITEMDATA, index, (LPARAM)i);
    }

    comboServer_.SetRedraw(TRUE);

    return 0;
}

/**
 *\fn           int connectServer()
 *\brief        ���ӷ�����
 *\return       int ��
 */
int CAddinWnd::connectServer()
{
    if (-1 != showServerIndex_)
    {
        connectServer_[showServerIndex_]->showWindow(FALSE);
    }

    char text[512] = {0};
    int curSel = comboServer_.SendMessage(CB_GETCURSEL, 0, 0L);
    comboServer_.SendMessage(CB_GETLBTEXT, curSel, LPARAM(text));
    int serverId = comboServer_.SendMessage(CB_GETITEMDATA, curSel, NULL);

    showServerIndex_ = comboConnect_.SendMessage(CB_ADDSTRING, 0, (LPARAM)text);
    comboConnect_.SendMessage(CB_SETCURSEL, showServerIndex_, NULL);

    void *data = NULL;
    if (curSel >= serverCount_)
    {
        addTerminal(serverId, &data);
    }
    else
    {
        addBrowse(serverId, &data);
    }

    comboConnect_.SendMessage(CB_SETITEMDATA, showServerIndex_, (LPARAM)data);

    connectServerCount_++;

    for (int i = 0; i < connectServerCount_; i++)
    {
        connectServer_[i] = (CSubWnd*)comboConnect_.SendMessage(CB_GETITEMDATA, i, NULL);
    }

    return 0;
}

/**
 *\fn           int showServer()
 *\brief        ��ʾ������
 *\return       int ��
 */
int CAddinWnd::showServer()
{
    int curSel = comboConnect_.SendMessage(CB_GETCURSEL, 0, 0L);

    if (curSel != showServerIndex_)
    {
        if (-1 != showServerIndex_)
        {
            connectServer_[showServerIndex_]->showWindow(FALSE);
        }

        connectServer_[curSel]->showWindow(TRUE);

        showServerIndex_ = curSel;
    }

    return 0;
}

/**
 *\fn           int addBrowse(int serverId, void **data)
 *\brief        ��������
 *\param[in]    int serverId ������ID
 *\param[out]   void * * data ����
 *\return       int 0�ɹ�,����ʧ��
 */
int CAddinWnd::addBrowse(int serverId, void **data)
{
    const char *temp = config_->getTempPath();
    const char *path = config_->getRemotePath(projectName_);

    RECT rc = {0};
    rc.left = S_L;
    rc.right = wndWidth_ - S_R;
    rc.top = S_T + S_MT;
    rc.bottom = wndHeight_ - S_B;

    PServerInfo info = &(serverInfo_[serverId]);

    CBrowseWnd *browse = new CBrowseWnd;
    browse->create(msgMap_, 4, wnd_, &rc, treeWidth_, hImageList_, hCursorDragWnd_, hCursorDragFile_);
    browse->setParam(0, (void*)temp);
    browse->setParam(1, (void*)open_file_cb_);
    browse->setParam(2, (void*)output_text_cb_);
    browse->setSftpParam(info->serverAddr, (unsigned short)info->serverPort, info->username, info->password, (void*)browse);
    browse->runSftp(path);

    *data = browse;
    return 0;
}

/**
 *\fn           int addTerminal(int serverId, void **data)
 *\brief        ����ն�
 *\param[in]    int serverId ������ID
 *\param[out]   void * * data ����
 *\return       int 0�ɹ�,����ʧ��
 */
int CAddinWnd::addTerminal(int serverId, void **data)
{
    RECT rc = {0};
    rc.left = S_L;
    rc.right = wndWidth_ - S_R;
    rc.top = S_T + S_MT;
    rc.bottom = wndHeight_ - S_B;

    PServerInfo info = &(serverInfo_[serverId]);

    CTerminalWnd *term = new CTerminalWnd;
    term->create(msgMap_, 3, wnd_, &rc);
    term->setSShParam(info->serverAddr, (unsigned short)info->serverPort, info->username, info->password, term);
    term->runSSh();

    *data = term;
    return 0;
}

/**
 *\fn           void resizeWnd(int cx, int cy)
 *\brief        ���ô�����С
 *\param[in]    int cx ��
 *\param[in]    int cy ��
 *\return       void ��
 */
void CAddinWnd::resizeWnd(int cx, int cy)
{
    wndWidth_ = cx;
    wndHeight_ = cy;
    treeWidth_ = (cx - S_L - S_ML - S_R) / 2;

    comboConnect_.SetWindowPos(NULL, S_L, S_T, treeWidth_, 280, SWP_NOZORDER | SWP_NOACTIVATE);
    comboServer_.SetWindowPos(NULL, S_L + treeWidth_ + S_ML, S_T, treeWidth_, 280, SWP_NOZORDER | SWP_NOACTIVATE);

    for (int i = 0; i < connectServerCount_; i++)
    {
        connectServer_[i]->setWndRect(S_L, S_T + S_MT, cx - S_L - S_ML - S_R, cy - S_T - S_MT - S_B);
    }
}

/**
 *\fn           void mouseMove(int x, int y, int type)
 *\brief        ����ƶ�
 *\param[in]    int x X��
 *\param[in]    int y Y��
 *\param[in]    int type 0-����,1-����,2-̧��
 *\return       void ��
 */
void CAddinWnd::mouseMove(int x, int y, int type)
{
    if (showServerIndex_ < 0) return;

    connectServer_[showServerIndex_]->mouseMove(x, y, type);
}

/**
 *\fn           void onPaint()
 *\brief        ��ͼ
 *\return       void ��
 */
void CAddinWnd::onPaint()
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    term->paint();
}

/**
 *\fn           void setFocus(bool focus)
 *\brief        ���ý���
 *\param[in]    bool focus �Ƿ��н���
 *\return       void ��
 */
void CAddinWnd::setFocus(bool focus)
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    if (NULL == term) return;

    term->setFocus(focus);
}

/**
 *\fn           void scroll(int type, int data)
 *\brief        ������Ļ
 *\param[in]    int type ����
 *\param[in]    int data ��ֵ
 *\return       void ��
 */
void CAddinWnd::scroll(int type, int data)
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    term->scroll(type, data);
}

/**
 *\fn           void mouseWheel(int data)
 *\brief        ��껬�ֹ���
 *\param[in]    int data ��ֵ
 *\return       void ��
 */
void CAddinWnd::mouseWheel(int data)
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    term->mouseWheel(data);
}

/**
 *\fn           void onChar(int key, int type)
 *\brief        ����һ���ַ�
 *\param[in]    int key �ַ�
 *\param[in]    int type ����
 *\return       void ��
 */
void CAddinWnd::onChar(int key, int type)
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    term->onChar(key, type);
}

/**
 *\fn           void onChar(int key)
 *\brief        ����һ����
 *\param[in]    int key ��
 *\return       void ��
 */
void CAddinWnd::onKeyDown(int key)
{
    if (showServerIndex_ < 0) return;

    CTerminalWnd *term = (CTerminalWnd*)connectServer_[showServerIndex_];

    term->onKeyDown(key);
}

/**
 *\fn           void sortList(int id)
 *\brief        �б�����
 *\param[in]    int id ��ID
 *\return       void ��
 */
void CAddinWnd::sortList(int id)
{
    if (showServerIndex_ < 0) return;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    browse->sortList(id);
}

/**
 *\fn           void addFile(HWND wnd)
 *\brief        ���һ�����ļ�
 *\param[in]    HWND wnd ����
 *\return       void ��
 */
void CAddinWnd::addFile(HWND wnd)
{
    if (showServerIndex_ < 0) return;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    if (NULL != browse && browse->isList(wnd))
    {
        browse->addFile();
    }
}

/**
 *\fn           void openFile(HWND wnd, int id)
 *\brief        ���ļ�����
 *\param[in]    HWND wnd
 *\param[in]    int id ��ID
 *\return       void ��
 */
void CAddinWnd::openFile(HWND wnd, int id)
{
    if (showServerIndex_ < 0) return;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    if (NULL != browse && browse->isList(wnd))
    {
        browse->openFile(id);
    }
}

/**
 *\fn           void startDragFile()
 *\brief        ��ʼ�϶��ļ�
 *\return       void ��
 */
void CAddinWnd::startDragFile()
{
    if (showServerIndex_ < 0) return;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    browse->startDragFile();
}

/**
 *\fn        int updateFileList(HWND wnd, HTREEITEM hItem)
 *\brief     ���¸ýڵ���ļ��б�
 *\param[in] HWND wnd ����
 *\param[in] HTREEITEM hItem ���ڵ�
 *\return    int 0�ɹ�,����ʧ��
 */
void CAddinWnd::updateFileList(HWND wnd, HTREEITEM item)
{
    if (showServerIndex_ < 0) return;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    if (NULL != browse && browse->isTree(wnd))
    {
        browse->updateFileList(item);
    }
}

/**
 *\fn           BOOL renameDir(const char *name)
 *\brief        ������
 *\param[in]    const char *name ������
 *\return       BOOL TRUE�ɹ�,����ʧ��
 */
BOOL CAddinWnd::renameDir(const char *name)
{
    if (showServerIndex_ < 0) return FALSE;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    if (NULL != browse)
    {
        return browse->renameDir(name);
    }

    return FALSE;
}

/**
 *\fn           BOOL renameFile(int item, int subItem, const char *name)
 *\brief        ������
 *\param[in]    int item �б�ڵ�ID
 *\param[in]    int subItem �б��ӽڵ�ID
 *\param[in]    const char * name ������
 *\return       BOOL TRUE�ɹ�,����ʧ��
 */
BOOL CAddinWnd::renameFile(int item, int subItem, const char *name)
{
    if (showServerIndex_ < 0) return FALSE;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    if (NULL != browse)
    {
        return browse->renameFile(item, subItem, name);
    }

    return FALSE;
}

/**
 *\fn           BOOL deleteSelectDir()
 *\brief        ɾ����ǰѡ��Ŀ¼
 *\return       BOOL TRUE�ɹ�,����ʧ��
 */
BOOL CAddinWnd::deleteSelectDir()
{
    if (showServerIndex_ < 0) return FALSE;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    return browse->deleteSelectDir();
}

/**
 *\fn           BOOL deleteSelectFile()
 *\brief        ɾ����ǰѡ���ļ�
 *\return       BOOL TRUE�ɹ�,����ʧ��
 */
BOOL CAddinWnd::deleteSelectFile()
{
    if (showServerIndex_ < 0) return FALSE;

    CBrowseWnd *browse = (CBrowseWnd*)connectServer_[showServerIndex_];

    return browse->deleteSelectFile();
}
