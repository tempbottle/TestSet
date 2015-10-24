
#pragma once
#include "P2PBase.h"
#include "P2PProcessSendMsg.h"
#include "P2PProcessRecvMsg.h"
#include "03.Network/IOCP/PackQueue.h"


namespace XT_P2P
{
    /**
     *\class  P2PQueueLayout
     *\brief  ������Ҫ��P2P�Ķ��л����
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PQueueLayout : public P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PQueueLayout()
         *\brief      ȱʡ���캯����
         */
        P2PQueueLayout();

        /**
         *\fn         ~P2PQueueLayout()
         *\brief      ����������
         */
        ~P2PQueueLayout();

    public:
        /**
         *\fn         int send(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        virtual int send(P2PMsg *msg);

        /**
         *\fn         int recv(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        virtual int recv(P2PMsg *msg);

    protected:
        XT_IOCP::PackQueue recvQueue_;   // ���ն���
        XT_IOCP::PackQueue sendQueue_;   // ���Ͷ���
        P2PProcessRecvMsg  recvMsgProc_; // ������Ϣ������
        P2PProcessSendMsg  sendMsgProc_; // ������Ϣ������
        
        /**
         *\fn         int init(P2PMsg *msg)
         *\brief      �����ʼ�����
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
         *\fn         int getLayoutDataInfo(P2PMsg *msg)
         *\brief      �õ���������ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getLayoutDataInfo(P2PMsg *msg);
    };
}