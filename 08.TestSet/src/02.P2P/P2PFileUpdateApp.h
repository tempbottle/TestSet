
#ifndef _FILE_UPDATE_H_
#define _FILE_UPDATE_H_
#pragma once

#include <map>
#include <list>
#include <string>
#include "P2PApplication.h"
#include "P2PFileOperator.h"


namespace XT_P2P
{
    class P2PFileUpdateApp : public P2PApplication
    {
    public:
        P2PFileUpdateApp();
        ~P2PFileUpdateApp();

    protected:
        TCHAR           rootPath_[MAX_PATH];
        TCHAR           fileListMd5_[MAX_PATH]; // ��ǰ���ļ��б�md5��
        UINT            fileListSize_;          // ��ǰ���ļ��б��С

        FILE_INFO_LIST  updateList_;            // Ҫ���µ��ļ��б�

        /**
         *\fn         int GetUpdateFileList(LPCTSTR rootPath, FILE_INFO_LIST &updateList)
         *\brief      �õ���Ҫ���µ��ļ��б�
         *\param[in]  LPCTSTR rootPath              ��·��
         *\param[in]  FILE_INFO_LIST & updateList   ��Ҫ���µ��ļ��б�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int GetUpdateFileList(LPCTSTR rootPath, FILE_INFO_LIST &updateList);

        /**
         *\fn         int GetErrorFileList(LPCTSTR rootPath, FILE_INFO_LIST &updateList)
         *\brief      �õ�����У�������ļ��б�
         *\param[in]  LPCTSTR rootPath              ��·��
         *\param[in]  FILE_INFO_LIST & updateList   ��Ҫ���µ��ļ��б�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int GetErrorFileList(LPCTSTR rootPath, FILE_INFO_LIST &updateList);

        /**
         *\fn         int DownloadUpdateFile(FILE_INFO_LIST &updateList)
         *\brief      ������Ҫ���µ��ļ�
         *\param[in]  FILE_INFO_LIST & updateList   ��Ҫ���µ��ļ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int DownloadUpdateFile(FILE_INFO_LIST &updateList);

    public:

        /**
         *\fn         bool haveUpdate()
         *\brief      �Ƿ����°汾��Ҫ����
         *\return     �Ƿ��и���
         */
        bool haveUpdate() { return (0 != updateList_.size()); }

        /**
         *\fn         int decompressFilelist(LPCTSTR listList)
         *\brief      ��ѹ�ļ��б�
         *\param[in]  LPCTSTR listList �ļ��б���
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int decompressFilelist(LPCTSTR listList);

        /**
         *\fn         int addUploadTaskList(LPCTSTR rootPath)
         *\brief      ����ϴ�����
         *\param[in]  LPCTSTR rootPath �ļ���Ŀ¼
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int addUploadTaskList(LPCTSTR rootPath);

        /**
         *\fn         int run(UINT projectId, LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port)
         *\brief      ����p2p�ļ����¿ͻ���
         *\param[in]  LPCTSTR projectId ����ID
         *\param[in]  LPCTSTR rootPath  ��Ŀ¼(�ļ��б����ڵ�Ŀ¼)
         *\param[in]  LPCTSTR ip        ��������IP��ַ
         *\param[in]  LPCTSTR port      �������Ķ˿ڵ�ַ
         *\return     �Ƿ�ɹ�
         */
        int run(UINT projectId, LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port);

        /**
	     *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
	     *\brief      �������ݰ���
	     *\param[in]  client  �ͻ��ˡ�
	     *\param[in]  buf     ���ݡ�
	     *\param[in]  len     ���ݳ���
	     *\param[in]  ip	  IP��ַ(UDP)��
	     *\param[in]  port    �˿ڵ�ַ(UDP)��
	     */
	    void ProcessPack(void *client, void *buf, int len, int ip, short port);

        /**
         *\fn         void procGetVersion(LPCTSTR md5, UINT size)
         *\brief      ����õ��ļ��б���Ϣ��Ϣ
         *\param[in]  LPCTSTR md5   ���µ��ļ��б��md5
         *\param[in]  UINT size     ���µ��ļ��б��
         *\return     ��
         */
        void procGetVersion(LPCTSTR md5, UINT size);

        /**
         *\fn         void procDownload(LPCTSTR md5, UINT size)
         *\brief      �����ļ����������Ϣ
         *\param[in]  LPCTSTR md5   ��������ļ���md5
         *\param[in]  UINT size     ��������ļ��Ĵ�С
         *\return     ��
         */
        void procDownload(LPCTSTR md5, UINT size);
    };

} // namespace XT_P2P

#endif // _FILE_UPDATE_H_