
#include "StdAfx.h"
#include "P2PPeer.h"
#include "03.Network/IOCP/BuffPool.h"


namespace XT_P2P
{
    P2PPeer::P2PPeer()
        :client_(NULL),
        clientIndex_(0),
        sendQueue_(NULL),
        buffPool_(NULL)
    {
    }

    P2PPeer::~P2PPeer()
    {
    }

    /**
     *\fn         int send(XT_IOCP::PACK *pack)
     *\brief      ��������
     *\param[in]  pack  Ҫ���͵����ݰ���
     *\return     0�ɹ�������ʧ��
     */
    int P2PPeer::send(XT_IOCP::PACK *pack)
    {
        if (NULL == pack) return -1;

        if (dataInfo_.getState() == PEER_STATE_LAN_CONNECTED)
        {
            return send(pack, dataInfo_.getLanIp(), dataInfo_.getLanPort());
        }
        else if (dataInfo_.getState() == PEER_STATE_WAN_CONNECTED)
        {
            return send(pack, dataInfo_.getWanIp(), dataInfo_.getWanPort());
        }
        else
        {
            TRACE(_T("peer sendReq don't connect"));
            return P2P_ERR_PEER_DONT_CONNECT;
        }
    }

    /**
     *\fn         int send(XT_IOCP::PACK *pack, UINT ip, USHORT port)
     *\brief      ��������
     *\param[in]  pack  Ҫ���͵����ݰ���
     *\param[in]  ip    Ҫ���͵���IP��ַ��
     *\param[in]  port  Ҫ���͵��Ķ˿ڵ�ַ��
     *\return     0�ɹ�������ʧ��
     */
    int P2PPeer::send(XT_IOCP::PACK *pack, UINT ip, USHORT port)
    {
        if (NULL == pack || NULL == sendQueue_) return -1;

        sendQueue_->Add(client_, pack, pack->getPackLen(), ip, port);

        addSendPack(pack->getPackLen());

        return 0;
    }

    /**
     *\fn         int sendReq(LPCTSTR md5, UINT64 begin, UINT size)
     *\brief      ��������������������
     *\param[in]  md5   ������ļ���
     *\param[in]  begin �������ݵ�λ�á�
     *\param[in]  size  �������ݵĴ�С��
     *\return     0�ɹ�������ʧ��
     */
    int P2PPeer::sendReq(LPCTSTR md5, UINT64 begin, UINT size)
    {
        XT_IOCP::PACK *pack = (XT_IOCP::PACK*)buffPool_->Get(1);

        pack->clear(P2P_GET_DATA_REQ);
        pack->dataLen = sizeof(P2PGetDataReq);

        P2PGetDataReq *req = (P2PGetDataReq*)pack->getData();
        _tcscpy_s(req->md5_, ARRAYSIZE(req->md5_), md5);
        req->questCount_ = 1;
        req->position_.begin_ = begin;
        req->position_.size_ = size;

        return send(pack);
    }
}