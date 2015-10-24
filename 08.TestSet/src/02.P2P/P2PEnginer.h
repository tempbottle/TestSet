
#pragma once
#include "P2PTaskLayout.h"
#include "03.Network/IOCP/BuffPool.h"
#include "03.Network/IOCP/PackQueue.h"


namespace XT_P2P
{
    class P2PLayoutBase;

    /**
     *\class  P2PEnginer
     *\brief  ������Ҫ��P2P����
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PEnginer : public P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PEnginer()
         *\brief      ȱʡ���캯����
         */
        P2PEnginer();

        /**
         *\fn         ~P2PEnginer()
         *\brief      ����������
         */
        ~P2PEnginer();

    protected:
        CRITICAL_SECTION    csUsed_;	            // �ٽ���

        XT_IOCP::BuffPool   *buffPool_;             // �����

        XT_IOCP::PackQueue  *appQueue_;             // Ӧ�ö���

        P2PLayoutBase       *layoutList_;           // ����

        TCHAR               rootPath_[MAX_PATH];    // �ļ���·��
        UINT                procUploadTaskTime_;    // �����ϴ�����ʱ��
        UINT                procDownloadTaskTime_;  // ������������ʱ��
        UINT                procPeerConnection_;    // ����ͻ�����ͨ

        TCHAR               fileListMd5_[MAX_PATH]; // �ļ��б�md5
        UINT                fileListSize_;          // �ļ��б��С

    public:
        /**
         *\fn         int recv(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        virtual int recv(P2PMsg *msg);

        /**
         *\fn         int recvVersionInfoReq(P2PMsg *msg)
         *\brief      ��������õ��汾��Ϣ
         *\param[in]  P2PMsg * msg
         *\return     ��������ֵ
         */
        int recvVersionInfoReq(P2PMsg *msg);

        /**
         *\fn         int recvVersionInfoRsp(P2PMsg *msg)
         *\brief      ����Ӧ��õ��汾��Ϣ
         *\param[in]  P2PMsg * msg
         *\return     ��������ֵ
         */
        int recvVersionInfoRsp(P2PMsg *msg);

    public:
        /**
         *\fn         int init(XT_IOCP::BuffPool *buffPool, XT_IOCP::PackQueue *appQueue, LPCTSTR rootPath)
         *\brief      ��ʼ��P2P���档
         *\param[in]  buffPool      �����ָ�롣
         *\param[in]  appQueue      Ӧ�ö���ָ�롣
         *\param[in]  rootPath      �ļ���·����
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int init(XT_IOCP::BuffPool *buffPool, XT_IOCP::PackQueue *appQueue, LPCTSTR rootPath);

        /**
         *\fn         void stop()
         *\brief      ֹͣ��
         *\param[in]  �ޡ�
         *\return     �ޡ�
         */
        void stop();

        /**
         *\fn         int addListen(LPCTSTR ip, USHORT port)
         *\brief      ���һ�����ؼ�����
         *\param[in]  ip            IP��ַ��
         *\param[in]  port          �˿ںš�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int addListen(LPCTSTR ip, USHORT port);

        /**
         *\fn         int setVersionInfo(LPCTSTR md5, UINT size)
         *\brief      ���ð汾��Ϣ(�ļ��б���Ϣ)
         *\param[in]  LPCTSTR md5   �ļ��б�md5��
         *\param[in]  UINT size     �ļ��б��С
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int setVersionInfo(LPCTSTR md5, UINT size);

        /**
         *\fn         getVersionInfo(UINT projectId, LPCTSTR ip, int port)
         *\brief      �õ����µ��ļ��б���Ϣ(md5,��С)��
         *\param[in]  projectId     ����ID��
         *\param[in]  ip            ������IP��ַ��
         *\param[in]  port          �������˿ںš�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getVersionInfo(UINT projectId, LPCTSTR ip, USHORT port);

        /**
         *\fn         getPeerListInfo(LPCTSTR ip, int port)
         *\brief      �õ�Peer�б���Ϣ(md5,��С)��
         *\param[in]  ip            ������IP��ַ��
         *\param[in]  port          �������˿ںš�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getPeerListInfo(LPCTSTR ip, USHORT port);

        /**
         *\fn         int addUploadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
         *\brief      ���һ���ϴ�����
         *\param[in]  md5           Ҫ�ϴ����ļ���MD5��(Сд32���ַ���)��
         *\param[in]  size          Ҫ�ϴ����ļ��Ĵ�С(��λΪ�ֽ�)��
         *\param[in]  filename      ���ϴ����ļ���ŵ�����ļ�����
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int addUploadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename);

        /**
         *\fn         int addDownloadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
         *\brief      ���һ����������
         *\param[in]  md5           Ҫ���ص��ļ���MD5��(Сд32���ַ���)��
         *\param[in]  size          Ҫ���ص��ļ��Ĵ�С(��λΪ�ֽ�)��
         *\param[in]  fullFilename  �����ص��ļ���ŵ�����ļ�����
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int addDownloadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename);

        /**
         *\fn         void RunBackgroundThread()
         *\brief      ִ�к�̨�̡߳�
         *\return     ��
         */
        void RunBackgroundThread();

        /**
         *\fn         static unsigned __stdcall BackgroundThread(void *param)
         *\brief      ��̨�߳���Ҫ�ǳ�ʼ�����ã��������塣
         *\return     ��
         */
        static unsigned __stdcall BackgroundThread(void *param);

        /**
         *\fn         void BackgroundThreadFun()
         *\brief      ��̨�߳���������
         *\return     ��
         */
        void BackgroundThreadFun();
    };

}