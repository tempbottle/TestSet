
#include "StdAfx.h"
#include "P2PIocpLayout.h"
#include "P2PProtocol.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PIocpLayout()
	 *\brief      ȱʡ���캯����
	 */
	P2PIocpLayout::P2PIocpLayout()
	{
	}

	/**
	 *\fn         ~P2PIocpLayout()
	 *\brief      ����������
	 */
	P2PIocpLayout::~P2PIocpLayout()
	{
	}

    /**
     *\fn         int send(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PIocpLayout::send(P2PMsg *msg)
    {
        if (NULL == msg) return P2P_ERR_IOCP_SEND_INPUT;

        addSendPack(msg->size_);

        switch (msg->id_)
        {
        case P2P_INIT:
            {
                return Init(msg);
            }
        case P2P_STOP:
            {
                return Stop();
            }
        case P2P_ADD_LISTEN:
            {
                return AddListen(msg);
            }
        case P2P_LAYOUT_DATA_INFO:
            {
                return getLayoutDataInfo(msg);
            }
        default:
            {
                return P2P_ERR_MSG_DISCARDED;
            }
        }
    }

    /**
     *\fn         int recv(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PIocpLayout::recv(P2PMsg *msg)
    {
        if (NULL == pre_ || NULL == msg) return P2P_ERR_IOCP_RECV_INPUT;

        addRecvPack(msg->size_);

        return pre_->recv(msg);
    }

    /**
     *\fn         int Init(P2PMsg *msg)
     *\brief      ��ʼ����ɶ˿ڡ�
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PIocpLayout::Init(P2PMsg *msg)
    {
        if (NULL == msg) return P2P_ERR_IOCP_INIT_INPUT;

        P2PInsideMsg *init = (P2PInsideMsg*)msg;

        init->setIocp(&iocp_);

        return iocp_.Init(init->getCmdType(), 
            (XT_IOCP::PackQueue*)init->getRecvQueue(), 
            (XT_IOCP::PackQueue*)init->getSendQueue(), 
            (XT_IOCP::BuffPool*)init->getBuffPool());
    }

    /**
     *\fn         int Stop()
     *\brief      ��ʼ����
     *\param[in]  �ޡ�
     *\return     �ޡ�
     */
    int P2PIocpLayout::Stop()
    {
        iocp_.Stop();
        return 0;
    }

    /**
     *\fn         int AddListen(P2PMsg *msg)
     *\brief      �ڱ������һ������(����UDP����)��
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PIocpLayout::AddListen(P2PMsg *msg)
    {
        if (NULL == msg) return P2P_ERR_IOCP_ADD_PEER_INPUT;

        P2PInsideMsg *addPeer = (P2PInsideMsg*)msg;

        XT_IOCP::PCLIENT client = NULL;

        int ret = iocp_.AddUdpListen(addPeer->getListenIp(), addPeer->getListenPort(), &client);

        if (0 == ret)
        {
            addPeer->setListenClient(client);

            TRACE("addUdpListen ip:%s port:%d client:%d\n", addPeer->getListenIp(), addPeer->getListenPort(), client);
        }

        return ret;
    }

    /**
     *\fn         int getLayoutDataInfo(P2PMsg *msg)
     *\brief      �õ���������ݡ�
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PIocpLayout::getLayoutDataInfo(P2PMsg *msg)
    {
        P2PInsideMsg *dataInfo = (P2PInsideMsg*)msg;

        P2P_DATA_PTR_LIST *dataList = (P2P_DATA_PTR_LIST*)dataInfo->getLayoutList();

        dataList->push_back(getDataInfo());

        return 0;
    }
}