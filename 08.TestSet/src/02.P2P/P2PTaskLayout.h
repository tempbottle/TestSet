
#pragma once
#include "P2PBase.h"
#include "P2PTask.h"
#include "03.Network/IOCP/BuffPool.h"
#include "08.Algorithm/base/XTXml.h"
#include <map>
#include <list>


namespace XT_P2P
{
    class P2PTask;

    typedef std::map<std::string, P2PTask*> P2PTaskMap;

    /**
     *\class  P2PTaskLayout
     *\brief  ������Ҫ��P2P�����
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PTaskLayout : public P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PTaskLayout()
         *\brief      ȱʡ���캯����
         */
        P2PTaskLayout();

        /**
         *\fn         ~P2PTaskLayout()
         *\brief      ����������
         */
        ~P2PTaskLayout();

    public:
        /**
         *\fn         int send(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int send(P2PMsg *msg);

        /**
         *\fn         int recv(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recv(P2PMsg *msg);

    private:
        P2PTaskMap          uploadTaskMap_;     // �ϴ�����
        P2PTaskMap          downloadTaskMap_;   // ��������

        void                *commandClient_;    // ����socket
        XT_IOCP::BuffPool   *buffPool_;         // �ڴ��

        XT_XML::xml         peerListXml;        // PEER�б�XML

        LPCTSTR             rootPath_;          // �ļ���·��

    protected:
        /**
         *\fn         int Init(P2PMsg *msg)
         *\brief      ��ʼ����
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int Init(P2PMsg *msg);

        /**
         *\fn         int Stop()
         *\brief      ��ʼ����
         *\param[in]  �ޡ�
         *\return     �ޡ�
         */
        void Stop();

        /**
         *\fn         int addUploadTask(P2PMsg *msg)
         *\brief      ��������ϴ��������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int addUploadTask(P2PMsg *msg);

        /**
         *\fn         int addDownloadTask(P2PMsg *msg)
         *\brief      ������������������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int addDownloadTask(P2PMsg *msg);

        /**
         *\fn         int recvDownloadReq(P2PMsg *msg)
         *\brief      �յ�Peer���������ļ���
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvDownloadReq(P2PMsg *msg);

        /**
         *\fn         int recvDownloadRsp(P2PMsg *msg)
         *\brief      �յ�Peer��Ӧ�����ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvDownloadRsp(P2PMsg *msg);

        /**
         *\fn         int getTaskDataInfo(P2PMsg *msg)
         *\brief      �õ�����ͳ�Ƶ����ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getTaskDataInfo(P2PMsg *msg);

        /**
         *\fn         int getLayoutDataInfo(P2PMsg *msg)
         *\brief      �õ���������ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getLayoutDataInfo(P2PMsg *msg);

        /**
         *\fn         int procTaskUpload(P2PMsg *msg)
         *\brief      ���������ϴ���
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int procTaskUpload(P2PMsg *msg);

        /**
         *\fn         int procTaskDownload(P2PMsg *msg)
         *\brief      �����������ء�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int procTaskDownload(P2PMsg *msg);
    };

}