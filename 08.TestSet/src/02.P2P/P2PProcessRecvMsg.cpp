
#include "StdAfx.h"
#include "P2PProcessRecvMsg.h"
#include "P2PProtocol.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PProcessRecvMsg()
	 *\brief      ȱʡ���캯����
	 */
	P2PProcessRecvMsg::P2PProcessRecvMsg()
        :recvQueue_(NULL),
        buffPool_(NULL),
        pre_(NULL),
        running_(false)
	{
	}

	/**
	 *\fn         ~P2PProcessRecvMsg()
	 *\brief      ����������
	 */
	P2PProcessRecvMsg::~P2PProcessRecvMsg()
	{
	}

    /**
     *\fn         int Start()
     *\brief      ��ʼ������Ϣ��
     *\param[in]  �ޡ�
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PProcessRecvMsg::Start()
    {
        if (running_) return -1;

        running_ = true;

        unsigned int threadID = 0;
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, &threadID);

        if ((NULL == thread) || (NULL == pre_) || (NULL == buffPool_) || (NULL == recvQueue_))
        {
            running_ = false;
            return P2P_ERR_QUEUE_RECV_START;
        }

        return 0;
    }

    /**
     *\fn         int Stop()
     *\brief      ֹͣ��
     *\param[in]  �ޡ�
     *\return     �ޡ�
     */
    void P2PProcessRecvMsg::Stop()
    {
        running_ = false;
    }

    /**
	 *\fn         RecvThread(void *pParam)
	 *\brief      �����̡߳�
	 *\param[in]  param  �̲߳�����
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	unsigned P2PProcessRecvMsg::RecvThread(void *param)
	{
        if (NULL == param) return P2P_ERR_QUEUE_RECV_THREAD_INPUT;

        P2PProcessRecvMsg *pThis = reinterpret_cast<P2PProcessRecvMsg*>(param);

        try
        {
            return pThis->RecvThreadFunc();
        }
        catch (...)
        {
            LOG(ERROR) << _T("P2PProcessRecvMsg::RecvThread error:") << ::GetLastError();
            return P2P_ERR_QUEUE_RECV_THREAD_EXCEPT;
        }
	}

	/**
	 *\fn         RecvThreadFunc()
	 *\brief      ʵ�ִ�������
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
    int P2PProcessRecvMsg::RecvThreadFunc()
    {
        short times = 0;
        short waitTime = 1;
        void *client = NULL;
        void *buf = NULL;
        int   len = 0;
        int   ip = 0;
        short port = 0;

        while (running_)
        {
            if (recvQueue_->Get(&client, &buf, len, ip, port))
            {
                ProcessPack(client, buf, len, ip, port);  // �������䴦��(���ݰ������нڵ�)
                waitTime = 0;
            }
            else
            {
                ::Sleep(waitTime); // ���������CPU��100%
                if (++times > 10000)
                {
                    times = 0;
                    waitTime = 1;
                }
            }
        }

        return 0;
    }

    /**
	 *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
	 *\brief      �������ݰ�
	 *\param[in]  client	�ͻ��˶���
	 *\param[in]  buf		���ݡ�
	 *\param[in]  len		���ݳ���
	 *\param[in]  ip		�Է�IP(UDP)��
	 *\param[in]  port		�Է��˿�(UDP)��
	 */
	void P2PProcessRecvMsg::ProcessPack(void *client, void *buf, int len, int ip, short port)
    {
        XT_IOCP::PPACK pack = (XT_IOCP::PPACK)buf;

        dataInfo_.addRecvPack(len);

        bool myMsg = false;

        switch (pack->protoId)
        {
        case P2P_GET_VERSION_REQ:
        case P2P_GET_VERSION_RSP:
        case P2P_GET_PEER_LIST_REQ:
        case P2P_GET_PEER_LIST_RSP:
        case P2P_GET_DATA_REQ:
        case P2P_GET_DATA_RSP:
        case P2P_HELLO_REQ:
        case P2P_HELLO_RSP: 
            {
                myMsg = true;
                break;
            }
        case CLIENT_CLOSE_SOCKET:
        default:
            {
                TRACE("P2PProcessMsg::ProcessPack CLIENT_CLOSE_SOCKET\n");
                buffPool_->Free(pack);
                return;
            }
        }

        if (!myMsg) return;

        P2PInsideMsg msg;
        msg.setRecvMsg(pack->protoId, pack, len, client, ip, port);
        pre_->recv(&msg);
    }
}