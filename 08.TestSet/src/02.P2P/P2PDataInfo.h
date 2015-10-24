
#pragma once
#include "P2PProtocol.h"
#include <map>
#include <list>

namespace XT_P2P
{
    class P2PDataInfo;
    typedef std::list<P2PDataInfo*>             P2P_DATA_PTR_LIST;
    

    typedef struct _tagReqDataInfo
    {
        LPCTSTR md5_;   // ������ļ�
        UINT64  begin_; // �ļ���ʼλ��
        UINT    size_;  // ���ݴ�С

    }REQ_DATA_INFO;
    
    typedef std::list<REQ_DATA_INFO>            P2P_REQ_DATA_LIST;
    typedef std::map<void*, P2P_REQ_DATA_LIST>  P2P_PEER_REQ_MAP;

    /**
     *\class  P2PDataInfo
     *\brief  ������Ҫ��P2Pͳ����Ϣ
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PDataInfo
    {
    public:
        /**
         *\fn         P2PStatisticalInfo()
         *\brief      ȱʡ���캯����
         */
        P2PDataInfo();

        /**
         *\fn         ~P2PStatisticalInfo()
         *\brief      ����������
         */
        ~P2PDataInfo();

    protected:
        UINT recvPackSizeTemp_; // һ���ڽ��յ����ݴ�С
        UINT sendPackSizeTemp_; // һ���ڷ��͵����ݴ�С
        UINT lastRecvTimeTemp_; // ������յ����һ������ʱ��
        UINT lastSendTimeTemp_; // ���뷢�͵����һ������ʱ��

        UINT recvPackCount_;    // ���յ��İ�����
        UINT recvPackSize_;     // ���յ��İ��ܴ�С(�ֽ�)
        UINT sendPackCount_;    // ���͵İ�����
        UINT sendPackSize_;     // ���͵İ��ܴ�С(�ֽ�)

        UINT firstRecvTime_;    // ��һ�ν�������ʱ��
        UINT firstSendTime_;    // ��һ�η�������ʱ��
        UINT lastRecvTime_;     // ���һ�ν�������ʱ��
        UINT lastSendTime_;     // ���һ�η�������ʱ��

        float recvSpeed_;       // ���͵��ٶ�
        float sendSpeed_;       // ���յ��ٶ�

    public:
        /**
         *\fn         void addRecvPack(int size)
         *\brief      ���յ�һ�����ݰ�������ͳ������
         *\param[in]  int size ���յ����ݳ�
         *\return     ��
         */
        void addRecvPack(int size);

        /**
         *\fn         void addSendPack(int size)
         *\brief      ����һ�����ݰ�������ͳ������
         *\param[in]  int size ���͵����ݳ�
         *\return     ��
         */
        void addSendPack(int size);
        
        /**
         *\fn         float getRecvSpeed()
         *\brief      �õ��������ݵ��ٶ�
         *\return     �������ݵ��ٶ�
         */
        float getRecvSpeed();

        /**
         *\fn         float getSendSpeed()
         *\brief      �õ��������ݵ��ٶ�
         *\return     �������ݵ��ٶ�
         */
        float getSendSpeed();

        UINT getRecvPackCount() { return recvPackCount_; }
        UINT getRecvPackSize() { return recvPackSize_; }
        UINT getSendPackCount() { return sendPackCount_; }
        UINT getSendPackSize() { return sendPackSize_; }

        UINT getFirstRecvTime() { return firstRecvTime_; }
        UINT getFirstSendTime() { return firstSendTime_; }
        UINT getLastRecvTime() { return lastRecvTime_; }
        UINT getLastSendTime() { return lastSendTime_; }
    };

    enum
    {
        TASK_STATE_UNKNOW,          // δ֪
        TASK_STATE_WAIT_DOWNLOAD,   // �ȴ�����
        TASK_STATE_SENDED_REQUEST,  // �Ѿ���������������
        TASK_STATE_DOWNLOADING,     // ��������
        TASK_STATE_DOWNLOADED,      // �Ѿ��������
        TASK_STATE_WAIT_UPLOAD,     // �ȴ��ϴ�
        TASK_STATE_UPLOADING,       // �����ϴ�
        TASK_STATE_UPLOADED,        // �Ѿ��ϴ����
    };

    class P2PTaskDataInfo : public P2PDataInfo
    {
    public:
        P2PTaskDataInfo();
        ~P2PTaskDataInfo();

    protected:
        TCHAR   md5_[MAX_PATH];     // MD5
        TCHAR   filename_[MAX_PATH];// �ļ���        
        UINT64  fileSize_;          // �ļ���С
        UINT64  finishSize_;        // ���ش�С
        UINT    state_;             // ��ǰ״̬

    public:
        void setMd5(LPCTSTR md5) { _tcscpy_s(md5_, ARRAYSIZE(md5_), md5); }
        void setFilname(LPCTSTR filename) { _tcscpy_s(filename_, ARRAYSIZE(filename_), filename); }
        void setState(UINT state) { state_ = state; }
        void setFileSize(UINT64 size) { fileSize_ = size; }
        void addFinishSize(UINT64 size) { finishSize_ += size; }

        LPCTSTR getMd5() { return md5_; }
        LPCTSTR getFilename() { return filename_; }
        UINT    getState() { return state_; }
        UINT64  getFileSize() { return fileSize_; }
        UINT64  getFinishSize() { return finishSize_; }
        
        LPCTSTR getStateStr();
    };

    enum
    {
        PEER_STATE_UNKNOW = 0,
        PEER_STATE_LAN_RECVED    = 1,   // �����ɽ�������
        PEER_STATE_LAN_SENDED    = 2,   // �����ɷ�������
        PEER_STATE_LAN_CONNECTED = 3,   // ��������ͨ
        PEER_STATE_WAN_RECVED    = 4,   // �����ɽ�������
        PEER_STATE_WAN_SENDED    = 8,   // �����ɷ�������
        PEER_STATE_WAN_CONNECTED = 12,  // ��������ͨ
        PEER_STATE_BREAK         = 16,  // �Ѿ�����
    };

    class P2PPeerDataInfo : public P2PDataInfo
    {
    public:
        P2PPeerDataInfo();
        ~P2PPeerDataInfo();

    protected:
        UINT        id_;    // �ͻ���id
        UINT        state_; // �û���ͨ״̬
        P2P_ADDR    addr_;  // �û���ַ

    public:
        void setId(UINT id) { id_ = id; }
        UINT getId() { return id_; }

        void setState(UINT state) { state_ = state; }
        UINT getState() { return state_; }

        void setLanIp(UINT ip) { addr_.lanIp_ = ip; }
        void setWanIp(UINT ip) { addr_.wanIp_ = ip; }
        void setLanPort(USHORT port) { addr_.lanPort_ = port; }
        void setWanPort(USHORT port) { addr_.wanPort_ = port; }
        void setAddr(P2P_ADDR *addr) { addr_ = *addr; }

        UINT      getWanIp() { return addr_.wanIp_; }
        UINT      getLanIp() { return addr_.lanIp_; }
        USHORT    getWanPort() { return addr_.wanPort_; }
        USHORT    getLanPort() { return addr_.lanPort_; }
        P2P_ADDR* getAddr() { return &addr_; }
        LPCTSTR   getStateStr();
    };
}