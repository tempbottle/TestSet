
#pragma once


namespace XT_P2P
{
    #define K(n)            (n*1024)
    #define M(n)            (K(n)*1024)
    #define G(n)            (M(n)*1024)

    #define MILLISECOND(n)  (n)             // ����
    #define SECOND(n)       (n*1000)        // ��
    #define MINUTE(n)       (SECOND(n)*60)  // ����
    #define HOUR(n)         (MINUTE(n)*60)  // Сʱ
    #define DAY(n)          (HOUR(n)*24)    // ��

    // �����
    enum
    {
        P2P_ERR_TASK_SEND_INPUT = -1999,    // ����㷢�����ݴ���
        P2P_ERR_TASK_RECV_INPUT,            // �����������ݴ���
        P2P_ERR_TASK_OPEN_FILE,             // �������ļ�����
        P2P_ERR_TASK_READ_FILE,             // ������ȡ�ļ�����
        P2P_ERR_TASK_DONOT_HAVE,            // ��������񲻴���
        P2P_ERR_TASK_HAD,                   // ����������Ѿ�����

        P2P_ERR_PEER_SEND_INPUT = -2999,    // �ͻ��˲㷢�����ݴ���
        P2P_ERR_PEER_RECV_INPUT,            // �ͻ��˲�������ݴ���
        P2P_ERR_PEER_DONT_CONNECT,          // ��˿ͻ���δ��ͨ

        P2P_ERR_QUEUE_SEND_INPUT = -3999,   // ���в㷢�����ݴ���
        P2P_ERR_QUEUE_RECV_INPUT,           // ���в�������ݴ���
        P2P_ERR_QUEUE_SEND_START,           // ���в����������̴߳���
        P2P_ERR_QUEUE_RECV_START,           // ���в����������̴߳���
        P2P_ERR_QUEUE_SEND_THREAD_INPUT,    // ���в㷢���̲߳�������
        P2P_ERR_QUEUE_RECV_THREAD_INPUT,    // ���в�����̲߳�������
        P2P_ERR_QUEUE_SEND_THREAD_EXCEPT,   // ���в㷢���߳��쳣
        P2P_ERR_QUEUE_RECV_THREAD_EXCEPT,   // ���в�����߳��쳣

        P2P_ERR_IOCP_SEND_INPUT = -4999,    // ��ɶ˿ڲ㷢�����ݴ���
        P2P_ERR_IOCP_RECV_INPUT,            // ��ɶ˿ڲ�������ݴ���
        P2P_ERR_IOCP_INIT_INPUT,            // ��ɶ˿ڲ��ʹ����������
        P2P_ERR_IOCP_ADD_PEER_INPUT,        // ��ɶ˿ڲ���ӿͻ���ʱ��������

        P2P_ERR_MSG_DISCARDED = -5000,      // δ֪����Ϣ
    };

    // �ڲ�Э��
    enum
    {
        P2P_INIT = 5000,                    // ��ʹ��P2P
        P2P_STOP,                           // ֹͣP2P
        P2P_GET_QUEUE,                      // �õ����в����ָ��
        P2P_ADD_LISTEN,                     // ��Ӽ����˿�
        P2P_LAYOUT_DATA_INFO,               // �õ���������

        P2P_PEER_ADD_PEER_LIST,             // �ͻ������PEER�б�
        P2P_PEER_GET_CONNECTED_PEER,        // �õ��Ѿ���ͨ�Ŀͻ���
        P2P_PEER_DATA_INFO,                 // �õ��ͻ���
        P2P_PEER_HELLO,                     // �ͻ��˴򶴣�����

        P2P_TASK_ADD_UPLOAD,                // ����ϴ�����
        P2P_TASK_ADD_DOWNLOAD,              // �����������
        P2P_TASK_PROC_UPLOAD,               // �����ϴ�����
        P2P_TASK_PROC_DOWNLOAD,             // ������������

        P2P_TASK_DOWNLOAD_RET,              // ���سɹ�0������ʧ��
        P2P_TASK_DATA_INFO,                 // ��ѯ����ͳ������
    };

    // �ⲿЭ��
    enum
    {
        P2P_GET_VERSION_REQ = 1000,         // �������µ��ļ��б���Ϣ
        P2P_GET_VERSION_RSP,                // Ӧ���ļ��б���Ϣ

        P2P_GET_PEER_LIST_REQ,              // ����Peer�б�,�õ��ļ�MD5,�ļ���С
        P2P_GET_PEER_LIST_RSP,              // Ӧ��ͻ��б�

        P2P_HELLO_REQ,                      // ������
        P2P_HELLO_RSP,                      // ��Ӧ��

        P2P_GET_DATA_REQ,                   // ������������
        P2P_GET_DATA_RSP,                   // Ӧ����������
    };

    /**
     *\class  P2PMsg
     *\brief  ������Ҫ��P2PЭ�����������
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PMsg
    {
    public:
        UINT id_;   // Э��ID
        UINT size_; // Э���С

        P2PMsg()
            :id_(0),
            size_(0)
        {
        }

        P2PMsg(int id)
            :id_(id),
            size_(0)
        {
        }
    };

    /**
     *\class  P2PInsideMsg
     *\brief  ������Ҫ��P2P�ڲ�Э��
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PInsideMsg : public P2PMsg
    {
    public:
        void   *param_[10];
        TCHAR  str50_[50];
        UINT64 int64_[2];

        P2PInsideMsg(int id)
        {
            id_ = id;
            size_ = 4;
            str50_[0] = _T('\0');
            memset(param_, 0, sizeof(void*)*10);
            memset(int64_, 0, sizeof(UINT64)*2);
        }

        P2PInsideMsg()
        {
            size_ = 0;
            str50_[0] = _T('\0');
            memset(param_, 0, sizeof(void*)*10);
            memset(int64_, 0, sizeof(UINT64)*2);
        }

        void setId(UINT id)
        {
            id_ = id;
        }

        // ��ʹ��
        void setInit(int cmdType, void *buffPool, LPCTSTR rootPath)
        {
            id_ = P2P_INIT;
            size_ = 4 + sizeof(void*)*5;
            param_[2] = (void*)cmdType;
            param_[3] = buffPool;
            param_[4] = (void*)rootPath;
        }
        
        void setQueue(void *sendQueue, void *recvQueue)
        {
            param_[0] = sendQueue;
            param_[1] = recvQueue;
        }
        
        void setIocp(void *iocp) { param_[5] = (void*)iocp; }

        // ��Ӽ����˿�
        void setListen(LPCTSTR ip, USHORT port)
        {
            id_ = P2P_ADD_LISTEN;
            size_ = 4 + sizeof(void*)*2+sizeof(TCHAR)*_tcslen(ip);
            _tcscpy_s(str50_, ARRAYSIZE(str50_), ip);
            setListenPort(port);
        }

        void setListenPort(USHORT port)
        {
            param_[0] = (void*)port;
        }

        void setListenClient(void *client)
        {
            param_[1] = client;
        }

        // ����ϴ�����
        void setAddUpload(LPCTSTR md5, UINT64 size, LPCTSTR filename)
        {
            id_ = P2P_TASK_ADD_UPLOAD;
            size_ = 4 + sizeof(UINT64) + sizeof(void*)*2;
            int64_[0] = size;
            param_[0] = (void*)md5;
            param_[1] = (void*)filename;
        }

        // �����������
        void setAddDownload(LPCTSTR md5, UINT64 size, LPCTSTR filename)
        {
            id_ = P2P_TASK_ADD_DOWNLOAD;
            size_ = 4 + sizeof(UINT64) + sizeof(void*)*2;
            int64_[0] = size;
            param_[0] = (void*)md5;
            param_[1] = (void*)filename;
        }

        // ��������
        void setRecvMsg(int id, void *pack, int size, void *client, int ip, int port)
        {
            id_ = id;
            size_ = 4 + sizeof(void*)*5;
            param_[0] = (void*)ip;
            param_[1] = (void*)port;
            param_[2] = (void*)pack;
            param_[3] = (void*)client;
            param_[4] = (void*)size;
        }

        // �������صĽ��,��P2P������Ӧ�ò㷢��
        void setResult(int ret, LPCTSTR md5, UINT64 fileSize)
        {
            id_ = P2P_TASK_DOWNLOAD_RET;
            size_ = sizeof(void*)*2+sizeof(TCHAR)*_tcslen(md5);
            param_[0] = (void*)ret;
            param_[1] = (void*)fileSize;
            _tcscpy_s(str50_, ARRAYSIZE(str50_), md5);
        }

        // �õ�ͳ����Ϣ
        void setGetFileInfo(int id, void *dataList)
        {
            id_ = id;
            size_ = 4 + sizeof(void*);
            param_[0] = (void*)dataList;
        }

        // �õ�ͳ����Ϣ
        void setGetTaskInfo(int id, void *dataList)
        {
            id_ = id;
            size_ = 4 + sizeof(void*);
            param_[1] = (void*)dataList;
        }

        // �õ�ͳ����Ϣ
        void setGetPeerInfo(int id, void *dataList)
        {
            id_ = id;
            size_ = 4 + sizeof(void*);
            param_[2] = (void*)dataList;
        }

        // �õ�ͳ����Ϣ
        void setGetLayoutInfo(int id, void *dataList)
        {
            id_ = id;
            size_ = 4 + sizeof(void*);
            param_[3] = (void*)dataList;
        }

        // ���������б�
        void setReqList(void *list)
        {
            id_ = P2P_TASK_PROC_DOWNLOAD;
            size_ = 4 + sizeof(void*);
            param_[0] = list;
        }

        int     getCmdType() { return (int)param_[2]; }
        void*   getIocp() { return param_[5]; }
        void*   getBuffPool() { return param_[3]; }
        void*   getSendQueue() { return param_[0]; }
        void*   getRecvQueue() { return param_[1]; }
        LPCTSTR getRootPath() { return (LPCTSTR)param_[4]; }

        LPCTSTR getListenIp() { return str50_; }
        USHORT  getListenPort() { return (USHORT)param_[0]; }
        void*   getListenClient() { return param_[1]; }

        UINT64  getUploadSize() { return (UINT64)int64_[0]; }
        LPCTSTR getUploadMd5() { return (LPCTSTR)param_[0]; }        
        LPCTSTR getUploadFilename() { return (LPCTSTR)param_[1]; }

        UINT64  getDownloadSize() { return (UINT64)int64_[0]; }
        LPCTSTR getDownloadMd5() { return (LPCTSTR)param_[0]; }        
        LPCTSTR getDownloadFilename() { return (LPCTSTR)param_[1]; }

        UINT    getRecvIp() { return (UINT)param_[0]; }
        USHORT  getRecvPort() { return (USHORT)param_[1]; }
        void*   getRecvPack() { return param_[2]; }
        void*   getRecvClient() { return param_[3]; }
        UINT    getRecvDataLen() { return (UINT)param_[4]; }

        UINT    getResultId() { return (UINT)param_[0]; }
        UINT    getResultSize() { return (UINT)param_[1]; }
        LPCTSTR getResultMd5() { return str50_; }

        void*   getFileList() { return param_[0]; }
        void*   getTaskList() { return param_[1]; }
        void*   getPeerList() { return param_[2]; }
        void*   getLayoutList() { return param_[3]; }

        void*   getReqList() { return param_[0]; }
    };

    /**
     *\class  P2PGetVersionReq
     *\brief  ������Ҫ��P2P�ⲿЭ��,�������µ��ļ��б���Ϣ
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PGetVersionReq : public P2PMsg
    {
    public:
        UINT projectId_;    // ����ID
        LPCTSTR ip_;        // IP��ַ
        USHORT  port_;      // �˿�

        P2PGetVersionReq(UINT projectId, LPCTSTR ip, USHORT port)
            :P2PMsg(P2P_GET_VERSION_REQ)
        {
            projectId_ = projectId;
            ip_ = ip;
            port_ = port;
        }
    };

    class P2PGetVersionRsp : public P2PMsg
    {
    public:
        TCHAR md5_[33];     // �ļ��б�md5��
        UINT  size_;        // �ļ��б��С

        P2PGetVersionRsp(LPCTSTR md5, UINT size)
            :P2PMsg(P2P_GET_VERSION_RSP)
        {
            setData(md5, size);
        }

        void setData(LPCTSTR md5, UINT size)
        {
            id_ = P2P_GET_VERSION_RSP;
            _tcscpy_s(md5_, ARRAYSIZE(md5_), md5);
            size_ = size;
        }
    };

    /**
     *\class  P2PGetPeerListReq
     *\brief  ������Ҫ��P2P�ⲿЭ��,����ͻ����б�
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PGetPeerListReq : public P2PMsg
    {
    public:
        LPCTSTR ip_;       // IP��ַ
        USHORT  port_;     // �˿�

        P2PGetPeerListReq(LPCTSTR ip, USHORT port)
            :P2PMsg(P2P_GET_PEER_LIST_REQ)
        {
            ip_ = ip;
            port_ = port;
        }
    };

    typedef struct _tagP2PAddr
    {
        UINT    wanIp_;     // ����IP
        USHORT  wanPort_;   // �����˿�
        UINT    lanIp_;     // ����IP
        USHORT  lanPort_;   // �����˿�

        _tagP2PAddr()
            :wanIp_(0),
            wanPort_(0),
            lanIp_(0),
            lanPort_(0)
        {
        }

    }P2P_ADDR, *PP2P_ADDR;

    /**
     *\class  P2PGetPeerListRsp
     *\brief  ������Ҫ��P2P�ⲿЭ��,Ӧ��ͻ����б�
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PGetPeerListRsp : public P2PMsg
    {
    public:
        UINT    myWanIp_;         // �û�����IP
        USHORT  myWanPort_;       // �û������˿�
        BYTE    peerCount_;       // �û�����

        P2PGetPeerListRsp()
            :P2PMsg(P2P_GET_PEER_LIST_RSP),
            myWanIp_(0),
            myWanPort_(0),
            peerCount_(0)
        {
        }

        void setData(UINT wanIp, USHORT wanPort, BYTE peerCount)
        {
            myWanIp_ = wanIp;
            myWanPort_ = wanPort;
            peerCount_ = peerCount;
        }
    };

    /**
     *\struct  _tagGetDataInfo
     *\brief  ������Ҫ��P2P�ⲿЭ��,�õ���������,�����λ�ú���������ݳ�
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    typedef struct _tagGetDataInfo
    {
        UINT64 begin_;  // �ļ���ʼλ��
        UINT   size_;   // ���ݴ�С

    }GET_DATA_INFO;

    /**
     *\class  P2PGetDataRsp
     *\brief  ������Ҫ��P2P�ⲿЭ��,�õ���������
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PGetDataReq : public P2PMsg
    {
    public:
        char            md5_[33];       // �ļ�MD5
        BYTE            questCount_;    // ��������
        USHORT          speed_;         // ��ǰ���յ��ٶ�(K/S)
        GET_DATA_INFO   position_;      // �����λ�ú�����(��questCount_��)

        P2PGetDataReq(LPCTSTR md5, UINT64 begin, UINT size, USHORT speed)
            :P2PMsg(P2P_GET_DATA_REQ)
        {
            setData(md5, begin, size, speed);
        }

        void setData(LPCTSTR md5, UINT64 begin, UINT size, USHORT speed)
        {
            id_ = P2P_GET_DATA_REQ;
          
            USES_CONVERSION;
            strcpy_s(md5_, ARRAYSIZE(md5_), T2A((LPTSTR)md5));

            speed_ = speed;
            questCount_ = 1;
            position_.begin_ = begin;
            position_.size_  = size;
        }

        void addReq(UINT64 begin, UINT size)
        {
            GET_DATA_INFO *req = &position_;

            for (int i = 0; i < questCount_; i++, req++);

            questCount_++;
            req->begin_ = begin;
            req->size_ = size;
        }
    };

    /**
     *\class  P2PGetDataRsp
     *\brief  ������Ҫ��P2P�ⲿЭ��,�õ�����Ӧ��
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PGetDataRsp : public P2PMsg
    {
    public:
        char   md5_[33];   // �ļ�MD5
        UINT   packCount_; // ���ص�һ�����ݰ�����
        UINT   packId_;    // ��ǰ���������е�λ��
        UINT64 begin_;     // �ļ���ʼλ��
        UINT   size_;      // ���ݴ�С
        void   *pack_;     // ���ݰ�
        UINT   ip_;        // ip��ַ
        USHORT port_;      // �˿ڵ�ַ

        P2PGetDataRsp(LPCTSTR md5, UINT64 begin, UINT size, 
            UINT packId, UINT packCount, void *pack, UINT ip, USHORT port)
            :P2PMsg(P2P_GET_DATA_RSP)
        {
            setData(md5, begin, size, packId, packCount, pack, ip, port);
        }

        void setData(LPCTSTR md5, UINT64 begin, UINT size, 
            UINT packId, UINT packCount, void *pack, UINT ip, USHORT port)
        {
            id_ = P2P_GET_DATA_RSP;
            begin_ = begin;
            size_ = size;
            pack_ = pack;
            packId_ = packId;
            packCount_ = packCount;
            ip_ = ip;
            port_ = port;

            USES_CONVERSION;
            strcpy_s(md5_, ARRAYSIZE(md5_), T2A((LPTSTR)md5));
        }
    };
}