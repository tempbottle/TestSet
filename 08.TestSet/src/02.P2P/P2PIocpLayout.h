
#pragma once
#include "P2PBase.h"
#include "03.Network/IOCP/IOCP.h"

namespace XT_P2P
{
    /**
     *\class  P2PIocpLayout
     *\brief  ������Ҫ��P2P��IOCP��ɶ˿������շ���
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PIocpLayout : public P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PIocpLayout()
         *\brief      ȱʡ���캯����
         */
        P2PIocpLayout();

        /**
         *\fn         ~P2PIocpLayout()
         *\brief      ����������
         */
        ~P2PIocpLayout();

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
        XT_IOCP::IOCP iocp_; // ��ɶ˿�

    protected:
        /**
         *\fn         int Init(P2PMsg *msg)
         *\brief      ��ʼ����ɶ˿ڡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int Init(P2PMsg *msg);

        /**
         *\fn         int Stop()
         *\brief      ��ʼ����
         *\param[in]  �ޡ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int Stop();

        /**
         *\fn         int AddListen(P2PMsg *msg)
         *\brief      �ڱ������һ������(����UDP����)��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int AddListen(P2PMsg *msg);

        /**
         *\fn         int getLayoutDataInfo(P2PMsg *msg)
         *\brief      �õ���������ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getLayoutDataInfo(P2PMsg *msg);
    };

}