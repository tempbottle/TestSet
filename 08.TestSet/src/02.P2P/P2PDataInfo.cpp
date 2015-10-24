
#include "StdAfx.h"
#include "P2PDataInfo.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PDataInfo()
	 *\brief      ȱʡ���캯����
	 */
	P2PDataInfo::P2PDataInfo()
        :recvPackSizeTemp_(0),
        sendPackSizeTemp_(0),
        lastRecvTimeTemp_(0),
        lastSendTimeTemp_(0),
        recvPackCount_(0),
        recvPackSize_(0),
        sendPackCount_(0),
        sendPackSize_(0),
        firstRecvTime_(0),
        firstSendTime_(0),
        lastRecvTime_(0),
        lastSendTime_(0),
        recvSpeed_(0.0),
        sendSpeed_(0.0)
	{
	}

	/**
	 *\fn         ~P2PDataInfo()
	 *\brief      ����������
	 */
	P2PDataInfo::~P2PDataInfo()
	{
	}

    /**
     *\fn         void addRecvPack(int size)
     *\brief      ���յ�һ�����ݰ�������ͳ������
     *\param[in]  int size ���յ����ݳ�
     *\return     ��
     */
    void P2PDataInfo::addRecvPack(int size)
    {
        UINT nowTime = GetTickCount();

        if (0 == firstRecvTime_)
        {
            firstRecvTime_ = nowTime;
            lastRecvTimeTemp_ = nowTime;
        }
        else if ((nowTime - lastRecvTimeTemp_) < 1000) // 1����
        {
            recvPackSizeTemp_ += size;
        }
        else
        {
            recvPackSizeTemp_ += size;
            recvSpeed_ = (float)recvPackSizeTemp_ / (nowTime - lastRecvTimeTemp_); // K/S
            lastRecvTimeTemp_ = nowTime;
        }

        lastRecvTime_ = nowTime;
        recvPackSize_ += size;
        ++recvPackCount_;
    }

    /**
     *\fn         void addSendPack(int size)
     *\brief      ����һ�����ݰ�������ͳ������
     *\param[in]  int size ���͵����ݳ�
     *\return     ��
     */
    void P2PDataInfo::addSendPack(int size)
    {
        UINT nowTime = GetTickCount();

        if (0 == firstSendTime_)
        {
            firstSendTime_ = nowTime;
            lastSendTimeTemp_ = nowTime;
        }
        else if ((nowTime - lastSendTimeTemp_) < 1000) // 1����
        {
            sendPackSizeTemp_ += size;
        }
        else
        {
            sendPackSizeTemp_ += size;
            sendSpeed_ = (float)sendPackSizeTemp_ / (nowTime - lastSendTimeTemp_); // K/S
            lastSendTimeTemp_ = nowTime;
        }

        lastSendTime_ = nowTime;
        sendPackSize_ += size;
        ++sendPackCount_;
    }

    /**
     *\fn         float getRecvSpeed()
     *\brief      �õ��������ݵ��ٶ�
     *\return     �������ݵ��ٶ�
     */
    float P2PDataInfo::getRecvSpeed()
    {
        if (recvPackSizeTemp_ > 0 && 0 == recvSpeed_)
        {
            recvSpeed_ = (float)recvPackSizeTemp_ / (GetTickCount() - lastRecvTimeTemp_);
        }

        return recvSpeed_;
    }

    /**
     *\fn         float getSendSpeed()
     *\brief      �õ��������ݵ��ٶ�
     *\return     �������ݵ��ٶ�
     */
    float P2PDataInfo::getSendSpeed()
    {
        if (sendPackSizeTemp_ > 0 && 0 == sendSpeed_)
        {
            sendSpeed_ = (float)sendPackSizeTemp_ / (GetTickCount() - lastSendTimeTemp_);
        }

        return sendSpeed_;
    }

    P2PTaskDataInfo::P2PTaskDataInfo()
        :fileSize_(0),
        finishSize_(0),
        state_(TASK_STATE_UNKNOW)
    {
        md5_[0] = _T('\0');
        filename_[0] = _T('\0');
    }

    LPCTSTR P2PTaskDataInfo::getStateStr()
    {
        switch (state_)
        {
        case XT_P2P::TASK_STATE_WAIT_DOWNLOAD:
            {
                return _T("�ȴ�����");
            }
        case XT_P2P::TASK_STATE_DOWNLOADING:
            {
                return _T("������");
            }
        case XT_P2P::TASK_STATE_DOWNLOADED:
            {
                return _T("�������");
            }
        case XT_P2P::TASK_STATE_WAIT_UPLOAD:
            {
                return _T("�ȴ��ϴ�");
            }
        case XT_P2P::TASK_STATE_UPLOADING:
            {
                return _T("�ϴ���");
            }
        case XT_P2P::TASK_STATE_UPLOADED:
            {
                return _T("�ϴ����");
            }
        default:
            {
                return _T("δ֪");
            }
        }
    }

    P2PTaskDataInfo::~P2PTaskDataInfo()
    {
    }

    P2PPeerDataInfo::P2PPeerDataInfo()
        :id_(0),
        state_(PEER_STATE_UNKNOW)
    {
    }

    P2PPeerDataInfo::~P2PPeerDataInfo()
    {
    }

    LPCTSTR P2PPeerDataInfo::getStateStr()
    {
        switch (state_)
        {
        case XT_P2P::PEER_STATE_UNKNOW:
            {
                return _T("δ֪");
            }
        case XT_P2P::PEER_STATE_LAN_RECVED:
            {
                return _T("�����ɽ�������");
            }
        case XT_P2P::PEER_STATE_LAN_SENDED:
            {
                return _T("�����ɷ�������");
            }
        case XT_P2P::PEER_STATE_LAN_CONNECTED:
            {
                return _T("��������ͨ");
            }
        case XT_P2P::PEER_STATE_WAN_RECVED:
            {
                return _T("�����ɽ�������");
            }
        case XT_P2P::PEER_STATE_WAN_SENDED:
            {
                return _T("�����ɷ�������");
            }
        case XT_P2P::PEER_STATE_WAN_CONNECTED:
            {
                return _T("��������ͨ");
            }
        case XT_P2P::PEER_STATE_BREAK:
            {
                return _T("�Ѿ��Ͽ�");
            }
        default:
            {
                return _T("δ֪");
            }
        }
    }
}