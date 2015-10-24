/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       BrowseWnd.h
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS�����������嶨��
*************************************************/

#pragma once
#include "SubWnd.h"
#include <map>
#include <string>

extern "C"
{
#include "ssh2/ssh2.h"
#include "ssh2/sftp.h"
#include "ssh2/direct_tcpip.h"
};

#define S_ML 5  // �����б�ؼ��м䱣��
#define S_MB 5  // ����·���ؼ��м䱣��

typedef std::map<std::string, int>              mapStrInt;
typedef std::map<std::string, std::string>      mapStrStr;
typedef std::map<std::string, HTREEITEM>        mapStrHtreeitem;

class CBrowseWnd : public CSubWnd
{
public:
    CBrowseWnd();
    ~CBrowseWnd();

private:
    int x_;                                 // ����λ��
    int y_;                                 // ����λ��
    int wndWidth_;                          // �����
    int wndHeight_;                         // �����
    int treeWidth_;                         // ���ؼ���

    bool dragWnd_;                          // �Ƿ������϶��ؼ�
    bool dragFile_;                         // �Ƿ������϶��ļ�
    bool processExpand_;                    // �Ƿ���treeviewչ����Ϣ

    ssh_recv_data_callback open_file_cb_;   // ��VS�д��ļ��ص�
    ssh_recv_data_callback output_text_cb_; // ��״̬����ʾ��Ϣ

    my_ssh_param ssh_param_;                // SSH����

    const char *tempPath_;                  // �ٵ�Ŀ¼
    mapStrInt mapExtImage_;                 // "�ļ�����"->hImageList_�е�ID
    mapStrStr mapLocalRemoteFile;           // �����ļ���Զ���ļ�

private:
    HWND wnd_;                              // ������
    HIMAGELIST hImageList_;                 // ͼ���б�
    HCURSOR hCursorDragWnd_;                // �϶�����ʱ���
    HCURSOR hCursorDragFile_;               // �϶��ļ�ʱ���

    CContainedWindow tree_;                 // ���Ϳؼ�
    CContainedWindow list_;                 // �б�ؼ�
    CContainedWindow path_;                 // EDIT�ؼ�,��ǰ·��

public:
    static CBrowseWnd *cur_;                // ��ǰ��ʾ��

public:
    /**
     *\fn           void create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc, 
     *\                         int treeWidth, HIMAGELIST imageList, 
     *\                         HCURSOR dragWnd, HCURSOR dragFile)
     *\brief        ����������
     *\param[in]    CMessageMap * map ��Ϣѭ��
     *\param[in]    int msgMapId ��Ϣѭ��ID
     *\param[in]    HWND wnd ������
     *\param[in]    RECT * rc λ��
     *\param[in]    int treeWidth ���ؼ���
     *\param[in]    HIMAGELIST imageList ͼƬ�б�
     *\param[in]    HCURSOR dragWnd �϶�������
     *\param[in]    HCURSOR dragFile �϶��ļ����
     *\return       void ��
     */
    void create(CMessageMap *map, int msgMapId, HWND wnd, RECT *rc, 
        int treeWidth, HIMAGELIST imageList, HCURSOR dragWnd, HCURSOR dragFile);

    /**
     *\fn           void destroy()
     *\brief        �ͷŴ���
     *\return       void ��
     */
    void destroy();

    /**
     *\fn           bool isTree(HWND wnd)
     *\brief        �Ƿ������ؼ�
     *\param[in]    HWND wnd ����
     *\return       bool 
     */
    bool isTree(HWND wnd) { return (wnd == tree_.m_hWnd); }

    /**
     *\fn           bool isList(HWND wnd)
     *\brief        �Ƿ����б�ؼ�
     *\param[in]    HWND wnd ����
     *\return       bool 
     */
    bool isList(HWND wnd) { return (wnd == list_.m_hWnd); }

    /**
     *\fn           bool inDragWndRect(int x, int y)
     *\brief        ���϶���������
     *\param[in]    int x ���λ��
     *\param[in]    int y ���λ��
     *\return       bool false����,true��
     */
    bool inDragWndRect(int x, int y);

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
     *\fn           void mouseMove(int x, int y, int type)
     *\brief        ����ƶ�
     *\param[in]    int x Xλ��
     *\param[in]    int y Yλ��
     *\param[in]    int type 0-����,1-��갴��,2-���̧��
     *\return       void ��
     */
    void mouseMove(int x, int y, int type);

    /**
     *\fn           void startDragFile()
     *\brief        ��ʼʩ���ļ�
     *\return       void ��
     */
    void startDragFile();

    /**
     *\fn           void dragFile(int x, int y)
     *\brief        �϶��ļ�
     *\param[in]    int x ���λ��
     *\param[in]    int y ���λ��
     *\return       void ��
     */
    void dragFile(int x, int y);

    /**
     *\fn           void setParam(int id, void *data)
     *\brief        ���ò���
     *\param[in]    int id ���
     *\param[in]    void * data ����
     *\return       void ��
     */
    void setParam(int id, void *data);

    /**
     *\fn           void setSftpParam(const char *host, unsigned short port, 
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
    void setSftpParam(const char *host, unsigned short port, 
        const char *username, const char *password, void *param);

    /**
     *\fn           void runSftp(const char *path)
     *\brief        ����SFTP�ͻ���
     *\param[in]    const char * path 
     *\return       void ��
     */
    void runSftp(const char *path);

    /**
     *\fn        int updateFileList(HTREEITEM hItem)
     *\brief     ���¸ýڵ���ļ��б�
     *\param[in] HTREEITEM hItem ���ڵ�
     *\return    int 0�ɹ�,����ʧ��
     */
    int updateFileList(HTREEITEM hItem);

    /**
     *\fn           void sortList(int id)
     *\brief        �б�����
     *\param[in]    int id ��ID
     *\return       void ��
     */
    void sortList(int id);

    /**
     *\fn           int sortList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
     *\brief        �б�����
     *\param[in]    LPARAM lParam1 ����1
     *\param[in]    LPARAM lParam2 ����2
     *\param[in]    LPARAM lParamSort �������
     *\return       int 0�ɹ�,����ʧ��
     */
    int sortList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    /**
     *\fn           int uploadFile(const char *filename)
     *\brief        �ϴ��ļ�
     *\param[in]    const char * filename �ļ���
     *\return       int 0�ɹ�,����ʧ��
     */
    int uploadFile(const char *filename);

    /**
     *\fn           bool isLink(HTREEITEM hItem)
     *\brief        �ýڵ��Ƿ�������
     *\param[in]    HTREEITEM hItem ���ڵ�
     *\return       bool �Ƿ�������
     */
    bool isLink(HTREEITEM hItem);

    /**
     *\fn           int addListItem(const char *filename, const char *size, 
                                    const char *month, const char *day, const char *time, 
                                    const char *mod, const char *user, const char *group, 
                                    void *param, bool isLink)
     *\brief        ����б�
     *\param[in]    const char * filename �ļ���
     *\param[in]    const char * size ��С
     *\param[in]    const char * month ��
     *\param[in]    const char * day ��
     *\param[in]    const char * time ʱ��
     *\param[in]    const char * mod ����
     *\param[in]    const char * user �û�
     *\param[in]    const char * group ��
     *\param[in]    void * param ����
     *\param[in]    bool isLink �Ƿ�������
     *\return       int 0�ɹ�,����ʧ��
     */
    int addListItem(const char *filename, const char *size, 
        const char *month, const char *day, const char *time,
        const char *mod, const char *user, const char *group,
        void *param, bool isLink);

    /**
     *\fn           HTREEITEM addTreeItem(HTREEITEM hParent, const char *text, bool link)
     *\brief        ��ӽڵ�
     *\param[in]    HTREEITEM hParent ���ڵ�
     *\param[in]    const char * text �ڵ�����
     *\param[in]    bool link �Ƿ�������
     *\return       HTREEITEM �½ڵ�
     */
    HTREEITEM addTreeItem(HTREEITEM hParent, const char *text, bool link);

    /**
     *\fn           HTREEITEM findTreeItem(HTREEITEM hParent, const char *text)
     *\brief        ���ҽڵ�
     *\param[in]    HTREEITEM hParent ���ڵ�
     *\param[in]    const char * text �ڵ�����
     *\return       HTREEITEM �ڵ�
     */
    HTREEITEM findTreeItem(HTREEITEM hParent, const char *text);

    /**
     *\fn           HTREEITEM createItem(const char *path)
     *\brief        �������õ����ڵ�
     *\param[in]    const char * path ·��
     *\return       HTREEITEM 0�ɹ�,����ʧ��
     */
    HTREEITEM createItem(const char *path);

    /**
     *\fn        int getPath(HTREEITEM hItem, char *path)
     *\brief     �õ��ýڵ��·��
     *\param[in] HTREEITEM hItem ���ڵ�
     *\param[in] char * path ·��
     *\return    int 0�ɹ�,����ʧ��
     */
    int getPath(HTREEITEM hItem, char *path);

    /**
     *\fn        int getSubtree(HTREEITEM hItem, mapStrHtreeitem &map)
     *\brief     �õ��ýڵ�������ӽڵ�
     *\param[in] HTREEITEM hItem �ڵ�
     *\param[in] mapStrHtreeitem & map �ӽڵ�,����->�ڵ���
     *\return    int 0�ɹ�,����ʧ��
     */
    int getSubtree(HTREEITEM hItem, mapStrHtreeitem &map);

    /**
     *\fn           const char* getFileExt(const char *filename)
     *\brief        �õ��ļ�����չ��
     *\param[in]    const char * filename �ļ���
     *\return       const char* ��չ��
     */
    const char* getFileExt(const char *filename);

    /**
     *\fn           int getFileExtId(const char *filename)
     *\brief        �õ��ļ���չ��ID
     *\param[in]    const char * filename �ļ���
     *\return       int ��չ��ID
     */
    int getFileExtId(const char *filename);

    /**
     *\fn           void addFile()
     *\brief        ���һ�����ļ�
     *\return       void ��
     */
    void addFile();

    /**
     *\fn           void openFile(int id)
     *\brief        ���ļ�
     *\param[in]    int id �б�ID
     *\return       void 0�ɹ�,����ʧ��
     */
    void openFile(int id);

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
     *\return       BOOL 0�ɹ�,����ʧ��
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


