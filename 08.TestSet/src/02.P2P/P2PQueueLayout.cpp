
#include "StdAfx.h"
#include "P2PQueueLayout.h"
#include "P2PProtocol.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PQueueLayout()
	 *\brief      ȱʡ���캯����
	 */
	P2PQueueLayout::P2PQueueLayout()
	{
#ifdef _DEBUG
        recvQueue_.name_ = _T("recvQueue_");
        sendQueue_.name_ = _T("sendQueue_");
#endif // _DEBUG
	}

	/**
	 *\fn         ~P2PQueueLayout()
	 *\brief      ����������
	 */
	P2PQueueLayout::~P2PQueueLayout()
	{
	}

    /**
     *\fn         int send(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PQueueLayout::send(P2PMsg *msg)
    {
        if (NULL == next_ || NULL == msg) return P2P_ERR_QUEUE_SEND_INPUT;

        addSendPack(msg->size_);

        switch (msg->id_)
        {
        case P2P_INIT:
            {
                return Init(msg);
            }
        case P2P_STOP:
            {
                Stop();
                return next_->send(msg);
            }
        case P2P_LAYOUT_DATA_INFO:
            {
                return getLayoutDataInfo(msg);
            }
        default:
            {
                return next_->send(msg);
            }
        }
    }

    /**
     *\fn         int recv(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PQueueLayout::recv(P2PMsg *msg)
    {
        if (NULL == pre_ || NULL == msg) return P2P_ERR_QUEUE_RECV_INPUT;

        addRecvPack(msg->size_);

        return pre_->recv(msg);
    }

    /**
     *\fn         int Init(P2PMsg *msg)
     *\brief      �����ʼ�����
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PQueueLayout::Init(P2PMsg *msg)
    {
        P2PInsideMsg *init = (P2PInsideMsg*)msg;

        init->setQueue(&sendQueue_, &recvQueue_);

        int ret = next_->send(init);

        if (0 != ret) return ret;

        recvQueue_.Start();
        sendQueue_.Start();

        recvMsgProc_.setPre(pre_);
        recvMsgProc_.setBuffPool((XT_IOCP::BuffPool*)init->getBuffPool());
        recvMsgProc_.setRecvQueue((XT_IOCP::PackQueue*)init->getRecvQueue());

        ret = recvMsgProc_.Start();

        if (0 != ret) return ret;

        sendMsgProc_.setIocp((XT_IOCP::IOCP*)init->getIocp());
        sendMsgProc_.setBuffPool((XT_IOCP::BuffPool*)init->getBuffPool());
        sendMsgProc_.setSendQueue((XT_IOCP::PackQueue*)init->getSendQueue());

        ret = sendMsgProc_.Start();

        if (0 != ret) return ret;

        return 0;
    }

    /**
     *\fn         int Stop()
     *\brief      ��ʼ����
     *\param[in]  �ޡ�
     *\return     �ޡ�
     */
    void P2PQueueLayout::Stop()
    {
        sendQueue_.Stop();
        recvQueue_.Stop();
        recvMsgProc_.Stop();
        sendMsgProc_.Stop();
    }

    /**
     *\fn         int getLayoutDataInfo(P2PMsg *msg)
     *\brief      �õ���������ݡ�
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PQueueLayout::getLayoutDataInfo(P2PMsg *msg)
    {
        P2PInsideMsg *dataInfo = (P2PInsideMsg*)msg;

        P2P_DATA_PTR_LIST *dataList = (P2P_DATA_PTR_LIST*)dataInfo->getLayoutList();

        dataList->push_back(getDataInfo());

        return next_->send(msg);
    }
}