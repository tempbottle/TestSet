
#include "StdAfx.h"
#include "P2PEnginer.h"
#include "P2PBase.h"
#include "P2PTaskLayout.h"
#include "P2PPeerLayout.h"
#include "P2PQueueLayout.h"
#include "P2PIocpLayout.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PEnginer()
	 *\brief      ȱʡ���캯����
	 */
	P2PEnginer::P2PEnginer()
        :buffPool_(NULL),
        appQueue_(NULL),
        layoutList_(NULL),
        procUploadTaskTime_(0),
        procDownloadTaskTime_(0),
        procPeerConnection_(0),
        fileListSize_(0)
	{
        rootPath_[0] = _T('\0');
        fileListMd5_[0] = _T('\0');
		InitializeCriticalSection(&csUsed_);
	}

	/**
	 *\fn         ~P2PEnginer()
	 *\brief      ����������
	 */
	P2PEnginer::~P2PEnginer()
	{
		DeleteCriticalSection(&csUsed_);

        if (NULL == layoutList_) return;

        P2PTaskLayout *task = (P2PTaskLayout*)layoutList_;
        P2PPeerLayout *peer = (P2PPeerLayout*)task->getNext();
        P2PQueueLayout *queue = (P2PQueueLayout*)peer->getNext();
        P2PIocpLayout *iocp = (P2PIocpLayout*)queue->getNext();

        delete task;
        delete peer;
        delete queue;
        delete iocp;
	}

    /**
     *\fn         int init(XT_IOCP::BuffPool *buffPool, XT_IOCP::PackQueue *appQueue, LPCTSTR rootPath)
     *\brief      ��ʼ��P2P���档
     *\param[in]  buffPool      �����ָ�롣
     *\param[in]  appQueue      Ӧ�ö���ָ�롣
     *\param[in]  rootPath      �ļ���·����
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::init(XT_IOCP::BuffPool *buffPool, XT_IOCP::PackQueue *appQueue, LPCTSTR rootPath)
    {
        if (NULL == buffPool || NULL == appQueue || NULL == rootPath) return -1;

        _tcscpy_s(rootPath_, ARRAYSIZE(rootPath_), rootPath);

        buffPool_ = buffPool;
        appQueue_ = appQueue;

        P2PTaskLayout *task = new P2PTaskLayout();
        P2PPeerLayout *peer = new P2PPeerLayout();
        P2PQueueLayout *queue = new P2PQueueLayout();
        P2PIocpLayout *iocp = new P2PIocpLayout();

        // ���������ɴ�
        this->setNext(task);
        task->setPre(this);
        task->setNext(peer);
        peer->setPre(task);
        peer->setNext(queue);
        queue->setPre(peer);
        queue->setNext(iocp);
        iocp->setPre(queue);
        layoutList_ = task;

        // ��ʼ������
        P2PInsideMsg initMsg;
        initMsg.setInit(XT_IOCP::CMD_TYPE_MYSELF, buffPool_, rootPath_);
        int ret = layoutList_->send(&initMsg);

        RunBackgroundThread();

        return ret;
    }

    /**
     *\fn         void stop()
     *\brief      ֹͣ��
     *\param[in]  �ޡ�
     *\return     �ޡ�
     */
    void P2PEnginer::stop()
    {
        if (NULL == layoutList_) return;

        P2PMsg msg(P2P_STOP);
        layoutList_->send(&msg);
    }

    /**
     *\fn         int addListen(LPCTSTR ip, USHORT port)
     *\brief      ���һ�����ؼ�����
     *\param[in]  ip            IP��ַ��
     *\param[in]  port          �˿ںš�
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::addListen(LPCTSTR ip, USHORT port)
    {
        if (NULL == ip || 0 == port || NULL == layoutList_) return -1;

        P2PInsideMsg msg;

        msg.setListen(ip, port);

        return layoutList_->send(&msg);
    }

    /**
     *\fn         int setVersionInfo(LPCTSTR md5, UINT size)
     *\brief      ���ð汾��Ϣ(�ļ��б���Ϣ)
     *\param[in]  LPCTSTR md5   �ļ��б�md5��
     *\param[in]  UINT size     �ļ��б��С
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::setVersionInfo(LPCTSTR md5, UINT size)
    {
        if (NULL == md5) return -1;

        fileListSize_ = size;
        _tcscpy_s(fileListMd5_, ARRAYSIZE(fileListMd5_), md5);

        return 0;
    }

    /**
     *\fn         int recvVersionInfoReq(P2PMsg *msg)
     *\brief      ��������õ��汾��Ϣ
     *\param[in]  P2PMsg * msg
     *\return     ��������ֵ
     */
    int P2PEnginer::recvVersionInfoReq(P2PMsg *msg)
    {
        P2PInsideMsg *req = (P2PInsideMsg*)msg;

        req->setId(P2P_GET_VERSION_RSP);

        XT_IOCP::PACK *pack = (XT_IOCP::PACK*)req->getRecvPack();

        pack->clear(P2P_GET_VERSION_RSP);
        pack->dataLen = sizeof(P2PGetVersionRsp);

        P2PGetVersionRsp *data = (P2PGetVersionRsp*)pack->getData();

        data->setData(fileListMd5_, fileListSize_);

        return layoutList_->send(msg);
    }

    /**
     *\fn         int recvVersionInfoRsp(P2PMsg *msg)
     *\brief      ����Ӧ��õ��汾��Ϣ
     *\param[in]  P2PMsg * msg
     *\return     ��������ֵ
     */
    int P2PEnginer::recvVersionInfoRsp(P2PMsg *msg)
    {
        P2PInsideMsg *rsp = (P2PInsideMsg*)msg;

        XT_IOCP::PACK *pack = (XT_IOCP::PACK*)rsp->getRecvPack();

        P2PGetVersionRsp *data = (P2PGetVersionRsp*)pack->getData();

        memcpy(pack, data, sizeof(P2PGetVersionRsp));

        appQueue_->Add(NULL, pack, 0, 0, 0);

        return 0;
    }

    /**
     *\fn         getVersionInfo(UINT projectId, LPCTSTR ip, int port)
     *\brief      �õ����µ��ļ��б���Ϣ(md5,��С)��
     *\param[in]  projectId     ����ID��
     *\param[in]  ip            ������IP��ַ��
     *\param[in]  port          �������˿ںš�
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::getVersionInfo(UINT projectId, LPCTSTR ip, USHORT port)
    {
        if (NULL == ip || 0 == port || NULL == layoutList_) return -1;

        P2PGetVersionReq msg(projectId, ip, port);

        return layoutList_->send(&msg);
    }

    /**
     *\fn         getPeerListInfo(LPCTSTR ip, int port, LPCTSTR filename)
     *\brief      �õ�Peer�б���Ϣ(md5,��С)��
     *\param[in]  ip            ������IP��ַ��
     *\param[in]  port          �������˿ںš�
     *\param[in]  filename      �ļ�����
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::getPeerListInfo(LPCTSTR ip, USHORT port)
    {
        if (NULL == ip || 0 == port || NULL == layoutList_) return -1;

        P2PGetPeerListReq msg(ip, port);

        return layoutList_->send(&msg);
    }

    /**
     *\fn         int addUploadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
     *\brief      ���һ���ϴ�����
     *\param[in]  md5           Ҫ�ϴ����ļ���MD5��(Сд32���ַ���)��
     *\param[in]  size          Ҫ�ϴ����ļ��Ĵ�С(��λΪ�ֽ�)��
     *\param[in]  filename      ���ϴ����ļ���ŵ�����ļ�����
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::addUploadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
    {
        if (NULL == md5 || 0 == size || NULL == filename || NULL == layoutList_) return -1;

        P2PInsideMsg msg;
        
        msg.setAddUpload(md5, size, filename);

        return layoutList_->send(&msg);
    }

    /**
     *\fn         int addDownloadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
     *\brief      ���һ����������
     *\param[in]  md5           Ҫ���ص��ļ���MD5��(Сд32���ַ���)��
     *\param[in]  size          Ҫ���ص��ļ��Ĵ�С(��λΪ�ֽ�)��
     *\param[in]  filename      �����ص��ļ���ŵ�����ļ�����
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::addDownloadTask(LPCTSTR md5, UINT64 size, LPCTSTR filename)
    {
        if (NULL == md5 || 0 == size || NULL == filename || NULL == layoutList_) return -1;

        P2PInsideMsg msg;

        msg.setAddDownload(md5, size, filename);

        return layoutList_->send(&msg);
    }

    /**
     *\fn         int recv(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PEnginer::recv(P2PMsg *msg)
    {
        if (NULL == appQueue_) return -1;

        switch (msg->id_)
        {
        case P2P_GET_VERSION_REQ:
            {
                recvVersionInfoReq(msg);
                break;
            }
        case P2P_GET_VERSION_RSP:
            {
                recvVersionInfoRsp(msg);
                break;
            }
        default:
            {
                appQueue_->Add(NULL, msg, 0, 0, 0);
                break;
            }
        }

        return 0;
    }

    /**
     *\fn         void RunBackgroundThread()
     *\brief      ִ�к�̨�̡߳�
     *\return     ��
     */
    void P2PEnginer::RunBackgroundThread()
    {
        UINT threadId = 0;
        _beginthreadex(NULL, 0, BackgroundThread, this, 0, &threadId);
    }
    
    /**
     *\fn         void BackgroundThread(void *param)
     *\brief      ��̨�߳���Ҫ�ǳ�ʼ�����ã��������塣
     *\return     ��
     */
    unsigned P2PEnginer::BackgroundThread(void *param)
    {
        if (NULL == param) return 1;
    
        P2PEnginer *pthis = (P2PEnginer*)param;
        pthis->BackgroundThreadFun();
        return 0;
    }
    
    /**
     *\fn         void BackgroundThreadFun()
     *\brief      ��̨�߳���������
     *\return     ��
     */
    void P2PEnginer::BackgroundThreadFun()
    {
        do 
        {
            UINT nowTime = GetTickCount();

            if (nowTime > procUploadTaskTime_)
            {
                procUploadTaskTime_ = nowTime + MINUTE(1); // 1����

                P2PMsg msg(P2P_TASK_PROC_UPLOAD);
                layoutList_->send(&msg); // �����ϴ�����
            }

            if (nowTime > procDownloadTaskTime_)
            {
                procDownloadTaskTime_ = nowTime + SECOND(10); // 5��

                P2PInsideMsg msg(P2P_TASK_PROC_DOWNLOAD);
                layoutList_->send(&msg); // ������������
            }

            if (nowTime > procPeerConnection_)
            {
                procPeerConnection_ = nowTime + SECOND(1); // 1��

                P2PMsg msg(P2P_PEER_HELLO);
                layoutList_->send(&msg); // ����ͻ�������
            }

            Sleep(100);

        } while (true);
    }
}