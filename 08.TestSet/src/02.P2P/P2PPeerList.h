
#pragma once
#include "P2PPeer.h"


namespace XT_P2P
{
    /**
     *\class  P2PPeerList
     *\brief  ������Ҫ��P2P��PEER�б�
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PPeerList
    {
    public:
        /**
         *\fn         P2PPeerList()
         *\brief      ȱʡ���캯����
         */
        P2PPeerList();

        /**
         *\fn         ~P2PPeerList()
         *\brief      ����������
         */
        ~P2PPeerList();

    protected:
        void   *buff_;
        UINT    size_;
        UINT    peerCount_;

        TCHAR   fileListMd5_[MAX_PATH];
        UINT    fileListSize_;

    public:
        int GetData(UINT ip, USHORT port, P2P_PEER_LIST &list);

        void setFileListMd5(LPCTSTR md5) { if (NULL != md5) _tcscpy_s(fileListMd5_, ARRAYSIZE(fileListMd5_), md5); }
        void setFileListSize(UINT size) { fileListSize_ = size; }

        void*   getBuff() { return buff_; }
        UINT    getPeerCount() { return peerCount_; }
        LPCTSTR getFileListMd5() { return fileListMd5_; }
        UINT    getFileListSize() { return fileListSize_; }
    };

}