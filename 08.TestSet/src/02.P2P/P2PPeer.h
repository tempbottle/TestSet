
#pragma once
#include "P2PProtocol.h"
#include "P2PDataInfo.h"
#include "03.Network/IOCP/PackQueue.h"
#include "03.Network/IOCP/PrePack.h"
#include <map>
#include <list>

namespace XT_P2P
{
    class P2PPeer;

    typedef std::map<std::string, P2PPeer*> P2P_PEER_MAP;
    typedef std::list<P2PPeer*>             P2P_PEER_LIST;
    typedef std::list<P2P_ADDR>             P2P_PEER_ADDR_LIST;

    /**
     *\class  P2PPeer
     *\brief  ������Ҫ��P2P��Peer
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PPeer
    {
    public:
        P2PPeer();
        ~P2PPeer();

    private:
        void                *client_;       // �ͻ��˶���
        UINT                clientIndex_;   // �ͻ��˶�������
        XT_IOCP::BuffPool   *buffPool_;     // �ڴ��
        XT_IOCP::PackQueue  *sendQueue_;    // ���Ͷ���
        P2PPeerDataInfo     dataInfo_;      // ͳ��������Ϣ

    public:
        void*     getClient() { return client_; }
        UINT      getClientIndex() { return clientIndex_; }
        UINT      getLastRecvTime() { return dataInfo_.getLastRecvTime(); }
        USHORT    getRecvSpeed() { return dataInfo_.getRecvSpeed(); }
        UINT      getId() { return dataInfo_.getId(); }
        UINT      getState() { return dataInfo_.getState(); }
        UINT      getWanIp() { return dataInfo_.getWanIp(); }
        UINT      getLanIp() { return dataInfo_.getLanIp(); }
        USHORT    getWanPort() { return dataInfo_.getWanPort(); }
        USHORT    getLanPort() { return dataInfo_.getLanPort(); }
        P2P_ADDR* getAddr() { return dataInfo_.getAddr(); }
        P2PPeerDataInfo* getData() { return &dataInfo_; }

        void setClient(void *client) { client_ = client; }
        void setClientIndex(UINT index) { clientIndex_ = index; }
        void setSendQueue(XT_IOCP::PackQueue *queue) { sendQueue_ = queue; }
        void setId(UINT id) { dataInfo_.setId(id); }
        void setState(UINT state) { dataInfo_.setState(state); }
        void setAddr(P2P_ADDR *addr) { dataInfo_.setAddr(addr); }
        void setLanIp(UINT ip) { dataInfo_.setLanIp(ip); }
        void setWanIp(UINT ip) { dataInfo_.setWanIp(ip); }
        void setLanPort(USHORT port) { dataInfo_.setLanPort(port); }
        void setWanPort(USHORT port) { dataInfo_.setWanPort(port); }
        void addRecvPack(UINT size) { dataInfo_.addRecvPack(size); }
        void addSendPack(UINT size) { dataInfo_.addSendPack(size); }

        /**
         *\fn         int send(XT_IOCP::PACK *pack)
         *\brief      ��������
         *\param[in]  pack  Ҫ���͵����ݰ���
         *\return     0�ɹ�������ʧ��
         */
        int send(XT_IOCP::PACK *pack);

        /**
         *\fn         int send(XT_IOCP::PACK *pack, UINT ip, USHORT port)
         *\brief      ��������
         *\param[in]  pack  Ҫ���͵����ݰ���
         *\param[in]  ip    Ҫ���͵���IP��ַ��
         *\param[in]  port  Ҫ���͵��Ķ˿ڵ�ַ��
         *\return     0�ɹ�������ʧ��
         */
        int send(XT_IOCP::PACK *pack, UINT ip, USHORT port);

        /**
         *\fn         int sendReq(LPCTSTR md5, UINT64 begin, UINT size)
         *\brief      ��������������������
         *\param[in]  md5   ������ļ���
         *\param[in]  begin �������ݵ�λ�á�
         *\param[in]  size  �������ݵĴ�С��
         *\return     0�ɹ�������ʧ��
         */
        int sendReq(LPCTSTR md5, UINT64 begin, UINT size);
    };
}