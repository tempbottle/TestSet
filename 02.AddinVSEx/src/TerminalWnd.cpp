/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       TerminalWnd.cpp
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS������ն˴���ʵ��
*************************************************/

#include "stdafx.h"
#include "TerminalWnd.h"

/**
 *\fn        void ssh_recv_data_cb(void *param, const char *data, int data_len)
 *\brief     ssh�������ݻص�
 *\param[in] void * param �û��Զ������
 *\param[in] const char * data ���յ�������
 *\param[in] int data_len ���յ������ݳ�
 *\return    int 0�ɹ�,����ʧ��
 */
int ssh_recv_data_cb(void *param, const char *data, int data_len)
{
    if (NULL == param) return -1;

    CTerminalWnd *termWnd = (CTerminalWnd*)param;

    termWnd->setTermRect();
    termWnd->output(data, data_len);

    return 0;
}


CTerminalWnd::CTerminalWnd()
    :setSshSize_(false),
    accumDelta_(0),
    deltaPerLine_(0)
{
    type_ = SUB_TYPE_TERMINAL;
    memset(&ssh_param_, 0, sizeof(ssh_param_));
}

CTerminalWnd::~CTerminalWnd()
{
    destroy();
}

/**
 *\fn           void create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc)
 *\brief        ��������
 *\param[in]    CMessageMap * map ��Ϣѭ��
 *\param[in]    int msgMapId ��Ϣѭ��ID
 *\param[in]    HWND wnd ������
 *\param[in]    RECT * rc ����λ��
 *\return       void ��
 */
void CTerminalWnd::create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc)
{
    edit_.m_pObject = map;
    edit_.m_dwMsgMapID = msgMapId;
    edit_.m_lpszClassName = WC_EDIT;

    edit_.Create(wnd, rc, _T(""), WS_VISIBLE | WS_CHILD | ES_MULTILINE |
        WS_VSCROLL | ES_AUTOVSCROLL, 
        WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES);

    term_.Init(edit_.m_hWnd);
}

/**
 *\fn           void destroy()
 *\brief        �ͷŴ���
 *\return       void ��
 */
void CTerminalWnd::destroy()
{
    if (edit_.IsWindow())
    {
        edit_.DestroyWindow();
    }

    char data[] = "exit\r\n";
    ssh_send_data(&ssh_param_, data, strlen(data));
    //ssh_param_.exit = TRUE;
}

/**
 *\fn           void showWindow(int cmd)
 *\brief        ��ʾ����
 *\param[in]    int cmd ����SW_SHOW��ʾ,SW_HIDE����
 *\return       void ��
 */
void CTerminalWnd::showWindow(int cmd)
{
    edit_.ShowWindow(cmd);
}

/**
 *\fn           void setWndRect(int x, int y, int cx, int cy)
 *\brief        ���� ����λ��
 *\param[in]    int x x��λ��
 *\param[in]    int y y��λ��
 *\param[in]    int cx ��
 *\param[in]    int cy ��
 *\return       void ��
 */
void CTerminalWnd::setWndRect(int x, int y, int cx, int cy)
{
    int charCx = 0;
    int charCy = 0;

    term_.setWndRect(cx, cy);
    term_.getTermRect(charCx, charCy);

    ssh_set_size(&ssh_param_, charCx, charCy); /*��������������ô����С��Ϣ*/

    edit_.SetWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

/**
 *\fn           void setTermRect()
 *\brief        �����ն˴�С
 *\return       void ��
 */
void CTerminalWnd::setTermRect()
{
    if (setSshSize_ || NULL == ssh_param_.channel) return;

    int xCount = 0;
    int yCount = 0;

    term_.getTermRect(xCount, yCount);
    ssh_set_size(&ssh_param_, xCount, yCount);

    setSshSize_ = true;
}

/**
 *\fn           void setFocus(bool focus)
 *\brief        ���ý���
 *\param[in]    bool focus �Ƿ��н���
 *\return       void ��
 */
void CTerminalWnd::setFocus(bool focus)
{
    term_.setFocus(focus);

    if (focus)
    {
        edit_.HideCaret(); // ����ʾϵͳ���
    }
}

/**
 *\fn           void paint()
 *\brief        ��ͼ
 *\return       void ��
 */
void CTerminalWnd::paint()
{
    term_.drawWndDc();
}

/**
 *\fn           void scroll(int type, int data)
 *\brief        ������Ļ
 *\param[in]    int type ����
 *\param[in]    int data ��ֵ
 *\return       void ��
 */
void CTerminalWnd::scroll(int type, int data)
{
    term_.scrollScreen(type, data);
}

/**
 *\fn           void mouseWheel(int data)
 *\brief        ��껬�ֹ���
 *\param[in]    int data ��ֵ
 *\return       void ��
 */
void CTerminalWnd::mouseWheel(int data)
{
    if (deltaPerLine_ == 0)
    {
        deltaPerLine_ = 40;
    }

    accumDelta_ += (short)data;

    while (accumDelta_ >= deltaPerLine_)
    {
        scroll(SB_LINEUP, 0);

        accumDelta_ -= deltaPerLine_;
    }

    while (accumDelta_ <= -deltaPerLine_)
    {
        scroll(SB_LINEDOWN, 0);

        accumDelta_ += deltaPerLine_;
    }
}

/**
 *\fn           void onChar(int key, int type)
 *\brief        ����һ���ַ�
 *\param[in]    int key �ַ�
 *\param[in]    int type ����
 *\return       void ��
 */
void CTerminalWnd::onChar(int key, int type)
{
    if (0x03 == key && 0x2e0001 == type) // Ctrl+C
    {
        char data[4] = "";
        data[0] = 27; // Esc
        data[1] = 91; // [
        data[2] = 03; // ETX

        ssh_send_data(&ssh_param_, data, 3);
    }
    else if (0x16 == key && 0x2f0001 == type) // Ctrl+V
    {
        if (!OpenClipboard(/*wnd*/edit_.m_hWnd)) return;

        HANDLE hBuffer = ::GetClipboardData(CF_TEXT);

        char *buffer = (char*)GlobalLock(hBuffer);

        ssh_send_data(&ssh_param_, buffer, strlen(buffer));

        GlobalUnlock(hBuffer);

        CloseClipboard();
    }
    else
    {
        if (0x0d == key)
        {
            term_.setNewCmd();
        }

        char data[2] = "";
        data[0] = (char)key;
        data[1] = 0;

        ssh_send_data(&ssh_param_, data, 1);
    }

    // �����ѶϿ�,����
    if (ssh_param_.exit)
    {
        _beginthread(ssh_thread_func, 0, &ssh_param_);
    }
}

/**
 *\fn           void onChar(int key)
 *\brief        ����һ����
 *\param[in]    int key ��
 *\return       void ��
 */
void CTerminalWnd::onKeyDown(int key)
{
    char data[4] = "";
    data[0] = 27; // Esc
    data[1] = 91; // [

    switch (key)
    {
    case VK_UP:    data[2] = 65; break;
    case VK_DOWN:  data[2] = 66; break;
    case VK_LEFT:  data[2] = 68; break;
    case VK_RIGHT: data[2] = 67; break;
    default: return;
    }

    ssh_send_data(&ssh_param_, data, 3);
}

/**
 *\fn           void setSShParam(const char *host, unsigned short port, 
 *\                               const char *username, const char *password, 
 *\                               void *param)
 *\brief        ����SFTP�Ĳ���
 *\param[in]    const char * host 
 *\param[in]    unsigned short port 
 *\param[in]    const char * username 
 *\param[in]    const char * password 
 *\param[in]    void * param 
 *\return       void 0�ɹ�,����ʧ��
 */
void CTerminalWnd::setSShParam(const char *host, unsigned short port, 
    const char *username, const char *password, void *param)
{
    ssh_param_.type = SSH_TYPE_SSH;
    ssh_param_.recv_cb = ssh_recv_data_cb;
    ssh_param_.param = param;
    ssh_param_.server_addr = host;
    ssh_param_.server_port = port;
    ssh_param_.username = username;
    ssh_param_.password = password;
}

/**
 *\fn           void runSSh()
 *\brief        ����SSH
 *\return       void ��
 */
void CTerminalWnd::runSSh()
{
    _beginthread(ssh_thread_func, 0, &ssh_param_);

//     int type = 0;
//     int local = 0;
//     int remote = 0;

//     // ���ö˿�ӳ��
//     for (int i = 0; i < curComboData_->portMappingCount; i++)
//     {
//         if (0 != config_->getServerPortMap(curComboData_->serverId, i, type, local, remote))
//         {
//             continue;
//         }
// 
//         p_my_ssh_param param = (p_my_ssh_param)malloc(sizeof(my_ssh_param));
//         *param = curComboData_->ssh_param;
//         param->session = NULL;
//         param->channel = NULL;
//         param->param1 = (void*)local;
//         param->param2 = (void*)remote;
//         param->recv_cb = printf_data_cb;
// 
//         if (0 == type)
//         {
//             _beginthread(port_map_thread_func, 0, param);
//         }
//         else
//         {
//             free(param);
//         }
//     }
}

/**
 *\fn           void output(const char *text, int textLen)
 *\brief        �������
 *\param[in]    const char * text ����
 *\param[in]    int textLen ���ֳ�
 *\return       void ��
 */
void CTerminalWnd::output(const char *text, int textLen)
{
    term_.outputText(text, textLen);
}
