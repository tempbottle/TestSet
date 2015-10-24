
#pragma once
#include "P2PBase.h"
#include "P2PPeer.h"
#include "P2PPeerList.h"


namespace XT_P2P
{
#define P2P_CLIENT_COUNT    10

    /**
     *\class  P2PPeerLayout
     *\brief  ������Ҫ��P2P��Peer���
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PPeerLayout : public P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PPeerLayout()
         *\brief      ȱʡ���캯����
         */
        P2PPeerLayout();

        /**
         *\fn         ~P2PPeerLayout()
         *\brief      ����������
         */
        ~P2PPeerLayout();

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
        P2P_PEER_LIST       usedPeerList_;      // ���ߵ��û�
        P2P_PEER_LIST       freePeerList_;      // �Ѿ����ߵ��û�
        UINT                peerCountUseClient_[P2P_CLIENT_COUNT];
        void                *client_[P2P_CLIENT_COUNT];
        P2P_ADDR            myAddr_;            // �ҵĵ�ַ
        P2PPeerList         peerListInfo_;      // �����������Ѿ�������ص��û�
        UINT                peerId_;            // �ͻ���ID

        XT_IOCP::BuffPool   *buffPool_;         // �ڴ��
        XT_IOCP::PackQueue  *sendQueue_;        // ���Ͷ���

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
         *\fn         P2PPeer* createPeer(P2P_ADDR *addr)
         *\brief      ����һ���ͻ��˶���
         *\param[in]  addr          �ͻ��˵�ַ��
         *\return     �ͻ��ˡ�
         */
        P2PPeer* createPeer(P2P_ADDR *addr);

        /**
         *\fn         int AddListen(P2PMsg *msg)
         *\brief      �ڱ������һ������(����UDP����)��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int AddListen(P2PMsg *msg);

        /**
         *\fn         int getConnectedPeer(P2PMsg *msg)
         *\brief      ���Ѿ���ͨ�Ŀͻ��ˡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getConnectedPeer(P2PMsg *msg);

        /**
         *\fn         int getPeerData(P2PMsg *msg)
         *\brief      �õ��ͻ�����Ϣ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getPeerData(P2PMsg *msg);

        /**
         *\fn         int getLayoutDataInfo(P2PMsg *msg)
         *\brief      �õ���������ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int getLayoutDataInfo(P2PMsg *msg);

        /**
         *\fn         int SendData(XT_IOCP::PACK *pack, UINT ip, USHORT port)
         *\brief      �������ݡ�
         *\param[in]  pack          ���ݰ���
         *\param[in]  ip            Ҫ���͵���IP��ַ��
         *\param[in]  port          Ҫ���͵��Ķ˿ڵ�ַ��
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int SendData(XT_IOCP::PACK *pack, UINT ip, USHORT port);

        /**
         *\fn         int sendVersionReq(P2PMsg *msg)
         *\brief      ��������������µ��ļ��б���Ϣ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendVersionReq(P2PMsg *msg);

        /**
         *\fn         int sendVersionRsp(P2PMsg *msg)
         *\brief      ��ͻ��˷������µ��ļ��б���Ϣ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendVersionRsp(P2PMsg *msg);

        /**
         *\fn         int sendGetPeerListReq(P2PMsg *msg)
         *\brief      �����������Peer�б�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendGetPeerListReq(P2PMsg *msg);

        /**
         *\fn         int recvGetPeerListReq(P2PMsg *msg)
         *\brief      ��������������Peer�б�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvGetPeerListReq(P2PMsg *msg);

        /**
         *\fn         int recvGetPeerListRsp(P2PMsg *msg)
         *\brief      �����������Peer�б�Ӧ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvGetPeerListRsp(P2PMsg *msg);

        /**
         *\fn         int sendHelloReq(P2PMsg *msg)
         *\brief      �ͻ�����ͨ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendHelloReq(P2PMsg *msg);

        /**
         *\fn         int recvHelloReq(P2PMsg *msg)
         *\brief      �յ�������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvHelloReq(P2PMsg *msg);

        /**
         *\fn         int recvHelloRsp(P2PMsg *msg)
         *\brief      �յ���Ӧ��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int recvHelloRsp(P2PMsg *msg);

        /**
         *\fn         int sendDownloadReq(P2PMsg *msg)
         *\brief      ��Peer���������ļ�(һ�����1M)��
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendDownloadReq(P2PMsg *msg);

        /**
         *\fn         int sendDownloadRsp(P2PMsg *msg)
         *\brief      ���Ϳͻ�����������ݡ�
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int sendDownloadRsp(P2PMsg *msg);
    };

}