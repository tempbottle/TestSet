/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       TerminalWnd.h
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS������ն˴��嶨��
*************************************************/

#pragma once
#include "SubWnd.h"
#include "Terminal.h"

extern "C"
{
#include "ssh2/ssh2.h"
#include "ssh2/direct_tcpip.h"
};

class CTerminalWnd  : public CSubWnd
{
public:
    CTerminalWnd();
    ~CTerminalWnd();

private:
    bool setSshSize_;           // �Ƿ�����ssh�Ĵ�С

    int accumDelta_;            // ������ʹ��
    int deltaPerLine_;          // ������ʹ��

    my_ssh_param ssh_param_;    // SSH����

    CTerminal term_;            // ���մ���

    CContainedWindow edit_;     // EDIT�ؼ�

public:
    /**
     *\fn           void create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc)
     *\brief        ��������
     *\param[in]    CMessageMap * map ��Ϣѭ��
     *\param[in]    int msgMapId ��Ϣѭ��ID
     *\param[in]    HWND wnd ������
     *\param[in]    RECT * rc ����λ��
     *\return       void ��
     */
    void create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc);

    /**
     *\fn           void destroy()
     *\brief        �ͷŴ���
     *\return       void ��
     */
    void destroy();

    /**
     *\fn           void showWindow(int cmd)
     *\brief        ��ʾ����
     *\param[in]    int cmd ����SW_SHOW��ʾ,SW_HIDE����
     *\return       void ��
     */
    void showWindow(int cmd);

    /**
     *\fn           void setWndRect(int x, int y, int cx, int cy)
     *\brief        ���� ����λ��
     *\param[in]    int x x��λ��
     *\param[in]    int y y��λ��
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\return       void ��
     */
    void setWndRect(int x, int y, int cx, int cy);

    /**
     *\fn           void setTermRect()
     *\brief        �����ն˴�С
     *\return       void ��
     */
    void setTermRect();

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
    void setSShParam(const char *host, unsigned short port, 
        const char *username, const char *password, void *param);

    /**
     *\fn           void runSSh()
     *\brief        ����SSH
     *\return       void ��
     */
    void runSSh();

    /**
     *\fn           void setFocus(bool focus)
     *\brief        ���ý���
     *\param[in]    bool focus �Ƿ��н���
     *\return       void ��
     */
    void setFocus(bool focus);

    /**
     *\fn           void paint()
     *\brief        ��ͼ
     *\return       void ��
     */
    void paint();

    /**
     *\fn           void scroll(int type, int data)
     *\brief        ������Ļ
     *\param[in]    int type ����
     *\param[in]    int data ��ֵ
     *\return       void ��
     */
    void scroll(int type, int data);

    /**
     *\fn           void mouseWheel(int data)
     *\brief        ��껬�ֹ���
     *\param[in]    int data ��ֵ
     *\return       void ��
     */
    void mouseWheel(int data);

    /**
     *\fn           void onChar(int key, int type)
     *\brief        ����һ���ַ�
     *\param[in]    int key �ַ�
     *\param[in]    int type ����
     *\return       void ��
     */
    void onChar(int key, int type);

    /**
     *\fn           void onChar(int key)
     *\brief        ����һ����
     *\param[in]    int key ��
     *\return       void ��
     */
    void onKeyDown(int key);

    /**
     *\fn           void output(const char *text, int textLen)
     *\brief        �������
     *\param[in]    const char * text ����
     *\param[in]    int textLen ���ֳ�
     *\return       void ��
     */
    void output(const char *text, int textLen);
};


