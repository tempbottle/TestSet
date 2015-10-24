
#pragma once
#include "P2PDataInfo.h"
#include "Base/MD5.h"
#include <list>
#include <bitset>

namespace XT_P2P
{

#define SECTION_SIZE    M(1)

    typedef std::list<UINT>     P2P_UINT_LIST;
    typedef std::bitset<1024>   P2P_RECV_PACK;

    typedef struct _tagSection
    {
        UINT64          sectionBegin_;      // ��ǰ���ο�ʼ
        UINT            sectionSize_;       // ��ǰ���δ�С
        USHORT          packCount_;         // ��Ҫ���յİ�����(sectionSize_ / K(1))��ȡ��
        USHORT          recvPackCount_;     // �Ѿ����յ��İ�����
        P2P_RECV_PACK   recvPackBit_;       // �Ѿ����յ��İ�

        UINT            lastSendTime_;      // ���һ�η��������ʱ��
        UINT            lastRecvTime_;      // ���һ�ν������ݵ�ʱ��

        _tagSection()
            :sectionBegin_(0),
            sectionSize_(0),
            packCount_(0),
            recvPackCount_(0),
            lastSendTime_(0),
            lastRecvTime_(0)
        {
        }

    }TASK_SECTION, *PTASK_SECTION;

    typedef std::list<TASK_SECTION> P2P_SECTION_LIST;

    /**
     *\class  P2PTask
     *\brief  ������Ҫ��P2P������
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PTask
    {
    public:
        P2PTask();
        ~P2PTask();

    private:
        UINT                sectionCount_;          // ���θ���
        P2P_UINT_LIST       unreqSectionList_;      // ��û���������ݵ�����
        P2P_UINT_LIST       unrecvSectionList_;     // ��û�н��������ݵ�����
        P2P_SECTION_LIST    sectionInfo_;           // ������Ϣ

        HANDLE              file_;                  // �ļ����
        HANDLE              fileMap_;               // �ļ�ӳ�����
        UINT64              fileSeek_;              // �ļ��ĵ�ǰλ��
        UINT                buffSize_;              // ��������С
        BYTE                *buff_;                 // ������

        P2PTaskDataInfo     dataInfo_;              // ͳ������
        CMD5                md5_;                   // MD5

    protected:
        /**
         *\fn         int OpenFile(bool read, UINT64 begin);
         *\brief      ���ļ���
         *\param[in]  read          ���ļ���
         *\param[in]  begin         ӳ�俪ʼλ�á�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int OpenFile(bool read, UINT64 begin);

        /**
         *\fn         int MoveView(bool read, UINT64 begin);
         *\brief      �ƶ�ӳ��λ�á�
         *\param[in]  read          ���ļ���
         *\param[in]  begin         ӳ�俪ʼλ�á�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int MoveView(bool read, UINT64 begin);

    public:
        LPCTSTR getMd5() { return dataInfo_.getMd5(); }
        UINT64  getFileSize() { return dataInfo_.getFileSize(); }

        UINT getState() { return dataInfo_.getState(); }
        void setState(UINT state) { dataInfo_.setState(state); }
        P2PTaskDataInfo* getDataInfo() { return &dataInfo_; }

        void addRecvPack(int size) { dataInfo_.addRecvPack(size); }
        void addSendPack(int size) { dataInfo_.addSendPack(size); }

        /**
         *\fn         void Free()
         *\brief      �ͷ����ݡ�
         *\param[in]  �ޡ�
         *\return     �ޡ�
         */
        void Free();

        /**
         *\fn         setUpload(LPCTSTR md5, UINT64 fileSize, LPCTSTR filename)
         *\brief      �����ϴ���
         *\param[in]  md5           �ļ�Md5��
         *\param[in]  fileSize      �ļ���С��
         *\param[in]  filename      �ļ�ȫ·������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int setUpload(LPCTSTR md5, UINT64 fileSize, LPCTSTR filename);

        /**
         *\fn         setDownload(LPCTSTR md5, UINT64 fileSize, LPCTSTR filename)
         *\brief      �������ء�
         *\param[in]  md5           �ļ�Md5��
         *\param[in]  fileSize      �ļ���С��
         *\param[in]  filename      �ļ�ȫ·������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int setDownload(LPCTSTR md5, UINT64 fileSize, LPCTSTR filename);

        /**
         *\fn         int GetData(UINT64 begin, UINT size, void *buff)
         *\brief      �������ݡ�
         *\param[in]  begin         ��ʼλ�á�
         *\param[in]  size          ���ݴ�С��
         *\param[in]  buff          ���ݡ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int GetData(UINT64 begin, UINT size, void *buff);

        /**
         *\fn         int SaveData(UINT64 begin, UINT size, void *buff)
         *\brief      �������ݡ�
         *\param[in]  begin         ��ʼλ�á�
         *\param[in]  size          ���ݴ�С��
         *\param[in]  buff          ���ݡ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int SaveData(UINT64 begin, UINT size, void *buff);

        /**
         *\fn         int procUpload()
         *\brief      �����ϴ�����,5����û�н���������ر��ļ����ͷ��ڴ�
         *\return     ��
         */
        int procUpload();

        /**
         *\fn         int procDownload(P2P_PEER_REQ_MAP *peerReqMap)
         *\brief      ������������
         *\param[in]  peerIdReqMap �ͻ���ID�������б�
         *\return     ��
         */
        int procDownload(P2P_PEER_REQ_MAP *peerReqMap);
    };
}