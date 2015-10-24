
#include "stdafx.h"
#include "AVSend.h"
#include "Network\PackQueue.h"
#include "Network\BuffPool.h"
#include "Network\PrePack.h"
#include <math.h>


namespace XT_IOCP
{

	CAVSend::CAVSend()
		:mtuSize_(1000),
		maxSize_(10000),
		channceId_(0),
		lastTime_(0),
		packId_(1),
		audioProto_(0),
		videoProto_(0),
		addrSize_(0),
		addrSize6_(0),
		outputQueue_(NULL),
		buffPool_(NULL)
	{
		addrSize_ = sizeof(SOCKADDR_IN);
		addrSize6_ = sizeof(SOCKADDR_IN6);
	}

	CAVSend::~CAVSend()
	{
		DelAllSend();
	}

	// bIPV6          [in] �Ƿ���IPV6
	// dwChanId       [in] ͨ��ID
	// dwMTU          [in] MTU
	// m_dwBufLen     [in] ��Ļ�������С
	// wAudioProtoNum [in] ��ƵЭ���
	// wVideoProtoNum [in] ��ƵЭ���
	// pQueueOut      [in] ���Ͷ���
	// pSubBuffPool   [in] С�Ļ�����
	void CAVSend::Init(int channceId, int mtuSize, int buffSize,
		int audioProto, int videoProto,
		PackQueue *outputQueue, BuffPool *buffPool)
	{
		mtuSize_     = mtuSize;
		channceId_   = channceId;
		audioProto_  = audioProto;
		videoProto_  = videoProto;
		outputQueue_ = outputQueue;
		buffPool_    = buffPool;
		maxSize_     = buffSize - sizeof(PACK);
	}

	void CAVSend::AddAudioSender(PCLIENT client, int ip, unsigned short port)
	{
		int count = audioSenders_.size();

		for (int i = 0; i < count; i++)
		{
			if ((audioSenders_[i].port == port) && (audioSenders_[i].ip == ip))
				return;
		}

		SenderInfo info = {client, ip, port};
		audioSenders_.push_back(info);
	}

	void CAVSend::AddVideoSender(PCLIENT client, int ip, unsigned short port)
	{
		int count = videoSenders_.size();

		for (int i = 0; i < count; i++)
		{
			if ((videoSenders_[i].port == port) && (videoSenders_[i].ip == ip))
				return;
		}

		SenderInfo info = {client, ip, port};
		videoSenders_.push_back(info);
	}

	void CAVSend::DelAudioSend(PCLIENT client, int ip, unsigned short port)
	{
		SENDINFOERS::iterator iter = audioSenders_.begin();

		for (; iter != audioSenders_.end(); ++iter)
		{
			if ((iter->port == port) && (iter->ip == ip))
			{
				audioSenders_.erase(iter);
				return;
			}
		}
	}

	void CAVSend::DelVideoSend(PCLIENT client, int ip, unsigned short port)
	{
		SENDINFOERS::iterator iter = videoSenders_.begin();

		for (; iter != videoSenders_.end(); ++iter)
		{
			if ((iter->port == port) && (iter->ip == ip))
			{
				videoSenders_.erase(iter);
				return;
			}
		}
	}

	void CAVSend::DelAllSend(void)
	{
		audioSenders_.clear();
		videoSenders_.clear();
	}

	// ������Ƶ����,G729 ѹ����Ϊ60���ֽ�
	void CAVSend::SendAudio(char *data, int len)
	{
		DWORD dwTime = ::GetTickCount();
		m_dwLastTime = (dwTime == m_dwLastTime)? ++dwTime : dwTime;

		PPACK pPack = (PPACK)m_pSubBuffPool->Get(0xFFFFFFFF);
		pPack->Clear();
		pPack->wProtoNum = m_wAudioProto;
		pPack->dwId      = m_dwPackId++;
		pPack->dwAckId	 = m_dwChanId;  // ��Ƶ����Ƶ��ͬһͨ����,�Է�����ͬʱ���ն�·����Ƶ
		pPack->dwDataLen = dwLen + 8;
		PPACKEX pPackEx   = (PPACKEX)pPack->byData;
		pPackEx->dwTime  = dwTime;
		memcpy((void*)&pPack->byData[8], pbyData, dwLen);
		pPack->SetCheckNum();

		m_AVList.AddTail(pPack->dwId, pPack);

		for (int i = 0; i < m_AudioSend.GetCount(); i++)
		{
			m_pQueueOut->Add(m_AudioSend[i].pClient, pPack->GetPackLen(), pPack, m_AudioSend[i].Addr);
		}

		DelPack(100);
	}

	// ������Ƶ����
	void CAVSend::SendVideo(char *data, int len)
	{
		if (dwLen > m_dwMaxData) return;

		DWORD dwTime = ::GetTickCount();
		m_dwLastTime = (dwTime == m_dwLastTime)? ++dwTime : dwTime;

		WORD wCheckNum = _tagPACKHEAD::GetCheckNum(pbyData, dwLen);
		WORD wBlcokNum = (WORD)ceil(dwLen / (float)m_dwMTU);

		for (WORD i = 0; i < wBlcokNum; i++)
		{
			PPACK pBlcok = (PPACK)m_pSubBuffPool->Get(0xFFFFFFFF);
			pBlcok->Clear();
			pBlcok->wProtoNum     = m_wVideoProto;
			pBlcok->dwId          = m_dwPackId++;                                   // ��ʱ��ID
			pBlcok->wSubId        = i;                                              // �����
			pBlcok->dwAckId	      = m_dwChanId;                                     // ͨ����
			pBlcok->dwDataLen     = (i==wBlcokNum-1) ? (dwLen-i*m_dwMTU):m_dwMTU;   // �����ݳ�
			PBLOCKEX pBlockEx     = (PBLOCKEX)pBlcok->byData;                       // ����Ϣ
			pBlockEx->dwFrameLen  = dwLen;                                          // �����ݳ�
			pBlockEx->wFrameCheck = wCheckNum;                                      // ������У��
			pBlockEx->wBlockCount = wBlcokNum;                                      // ���������Ŀ���
			pBlockEx->dwTime      = dwTime;                                         // ���ŵ�ʱ��
			memcpy(++pBlockEx, &pbyData[i*m_dwMTU], pBlcok->dwDataLen);
			pBlcok->dwDataLen     += sizeof(BLOCKEX);
			pBlcok->SetCheckNum();

			m_AVList.AddTail(pBlcok->dwId, pBlcok);

			for (int j = 0; j < m_VideoSend.GetCount(); j++)
			{
				m_pQueueOut->Add(m_VideoSend[j].pClient, pBlcok->GetPackLen(), pBlcok, m_VideoSend[j].Addr);
			}
		}

		DelPack(100);
	}

	// ɾ������İ�
	void CAVSend::DelPack(int num)
	{
		int nNum = avData_.getCount() - num;
		for (int i = 0; i < nNum; i++)
		{
			m_pSubBuffPool->SetTimes(m_AVList.GetHead(TRUE), 0);
		}
	}

	// dwId [in] ˳��ID
	void CAVSend::ResendAV(PCLIENT pClient, DWORD dwId, UINT64 *pnIP, WORD wPort)
	{
		PPACK pPack = (PPACK)m_AVList.GetNode(dwId);
		if (NULL != pPack)
			m_pQueueOut->Add(pClient, pPack->GetPackLen(), pPack, pnIP, wPort);
	}
}