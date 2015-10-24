/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       AddinWnd.h
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS������嶨��
*************************************************/

#pragma once
#include "SubWnd.h"
#include "SshConfig.h"

extern "C"
{
#include "ssh2/ssh2.h"
};


typedef struct _tagServerInfo
{
    char username[128];
    char password[128];
    char serverAddr[128];
    int serverPort;
    int port_map_count;

    _tagServerInfo()
    {
        username[0] = 0;
        password[0] = 0;
        serverAddr[0] = 0;
        serverPort = 0;
        port_map_count = 0;
    }

}ServerInfo, *PServerInfo;


class CAddinWnd
{
public:
    CAddinWnd();
    ~CAddinWnd();

private:
    HIMAGELIST hImageList_;                     // ͼ���б�
    HCURSOR hCursorDragWnd_;                    // �϶�����ʱ��ʾ�Ĺ��
    HCURSOR hCursorDragFile_;                   // �϶��ļ�ʱ��ʾ�Ĺ��
    HFONT font_;                                // ����

    HWND wnd_;                                  // ������
    CMessageMap *msgMap_;                       // ��Ϣѭ��

    CContainedWindow comboServer_;              // ComboBox�ؼ�,�����ӵķ�����
    CContainedWindow comboConnect_;             // ComboBox�ؼ�,��ǰ�Ѿ����ӵķ�����

private:
    ssh_recv_data_callback open_file_cb_;       // ��VS�д��ļ��ص�
    ssh_recv_data_callback output_text_cb_;     // ��״̬����ʾ��Ϣ

    char projectName_[128];                     // ��Ŀ��
    Config *config_;                            // �����ļ�

    PServerInfo serverInfo_;                    // ��������Ϣ

    CSubWnd *connectServer_[100];               // �Ѿ����ӵķ�����

    int serverCount_;                           // ����������
    int showServerIndex_;                       // ��ǰ��ʾ�Ĵ���
    int connectServerCount_;                    // �Ѿ����ӵķ���������

    int wndWidth_;                              // �����
    int wndHeight_;                             // �����
    int treeWidth_;                             // ���ؼ���

public:
    /**
     *\fn           void setParam(int id, void *data)
     *\brief        ���ò���
     *\param[in]    int id ���
     *\param[in]    void * data ����
     *\return       void ��
     */
    void setParam(int id, void *data);

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
    int create(HINSTANCE instance, CMessageMap *map, HWND wnd, 
        int icon, int cursorDrag, int cursorCopy);

    /**
     *\fn           void destroy()
     *\brief        �ͷŴ������Դ
     *\return       void ��
     */
    void destroy();

    /**
     *\fn           void resizeWnd(int cx, int cy)
     *\brief        ���ô�����С
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\return       void ��
     */
    void resizeWnd(int cx, int cy);

    /**
     *\fn           void mouseMove(int x, int y, int type)
     *\brief        ����ƶ�
     *\param[in]    int x X��
     *\param[in]    int y Y��
     *\param[in]    int type 0-����,1-����,2-̧��
     *\return       void ��
     */
    void mouseMove(int x, int y, int type);

    /**
     *\fn           int addServer()
     *\brief        ��ӷ�����
     *\return       int ��
     */
    int addServer();

    /**
     *\fn           int connectServer()
     *\brief        ���ӷ�����
     *\return       int ��
     */
    int connectServer();

    /**
     *\fn           int showServer()
     *\brief        ��ʾ������
     *\return       int ��
     */
    int showServer();

    /**
     *\fn           int addBrowse(int serverId, void **data)
     *\brief        ��������
     *\param[in]    int serverId ������ID
     *\param[out]   void * * data ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int addBrowse(int serverId, void **data);

    /**
     *\fn           int addTerminal(int serverId, void **data)
     *\brief        ����ն�
     *\param[in]    int serverId ������ID
     *\param[out]   void * * data ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int addTerminal(int serverId, void **data);

    /**
     *\fn           void setFocus(bool focus)
     *\brief        ���ý���
     *\param[in]    bool focus �Ƿ��н���
     *\return       void ��
     */
    void setFocus(bool focus);

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
     *\fn           void onPaint()
     *\brief        ��ͼ
     *\return       void ��
     */
    void onPaint();

    /**
     *\fn           void sortList(int id)
     *\brief        �б�����
     *\param[in]    int id ��ID
     *\return       void ��
     */
    void sortList(int id);

    /**
     *\fn           void addFile(HWND wnd)
     *\brief        ���һ�����ļ�
     *\param[in]    HWND wnd ����
     *\return       void ��
     */
    void addFile(HWND wnd);

    /**
     *\fn           void openFile(HWND wnd, int id)
     *\brief        ���ļ�����
     *\param[in]    HWND wnd
     *\param[in]    int id ��ID
     *\return       void ��
     */
    void openFile(HWND wnd, int id);

    /**
     *\fn           void startDragFile()
     *\brief        ��ʼ�϶��ļ�
     *\return       void ��
     */
    void startDragFile();

    /**
     *\fn        int updateFileList(HWND wnd, HTREEITEM hItem)
     *\brief     ���¸ýڵ���ļ��б�
     *\param[in] HWND wnd ����
     *\param[in] HTREEITEM hItem ���ڵ�
     *\return    int 0�ɹ�,����ʧ��
     */
    void updateFileList(HWND wnd, HTREEITEM item);

    /**
     *\fn           BOOL renameDir(const char *name)
     *\brief        ������
     *\param[in]    const char *name ������
     *\return       BOOL TRUE�ɹ�,����ʧ��
     */
    BOOL renameDir(const char *name);

    /**
     *\fn           BOOL renameFile(int item, int subItem, const char *name)
     *\brief        ������
     *\param[in]    int item �б�ڵ�ID
     *\param[in]    int subItem �б��ӽڵ�ID
     *\param[in]    const char * name ������
     *\return       BOOL TRUE�ɹ�,����ʧ��
     */
    BOOL renameFile(int item, int subItem, const char *name);

    /**
     *\fn           BOOL deleteSelectDir()
     *\brief        ɾ����ǰѡ��Ŀ¼
     *\return       BOOL TRUE�ɹ�,����ʧ��
     */
    BOOL deleteSelectDir();

    /**
     *\fn           BOOL deleteSelectFile()
     *\brief        ɾ����ǰѡ���ļ�
     *\return       BOOL TRUE�ɹ�,����ʧ��
     */
    BOOL deleteSelectFile();
};


