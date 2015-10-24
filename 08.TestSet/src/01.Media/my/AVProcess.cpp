
#include "stdafx.h"
#include "AVProcess.h"
#include <WS2tcpip.h>

namespace XT_IOCP
{

#define BUF_SIZE 76100

	CAVProcess::CAVProcess()
		:m_bIPV6(FALSE)
	{
	}

	CAVProcess::~CAVProcess()
	{
	}

	bool CAVProcess::GetAddrUdp(LPCTSTR sIP, WORD wPort, addrinfo **ppAddInfo)
	{
		CString sPort;
		sPort.Format("%d", wPort);

		struct addrinfo Info = {0};
		Info.ai_family = m_bIPV6?AF_INET6:AF_INET;
		Info.ai_socktype = SOCK_DGRAM;
		Info.ai_protocol = IPPROTO_UDP;

		if (getaddrinfo(sIP, sPort, &Info, ppAddInfo) == 0)
			return true;
		else
			return false;
	}

	void CAVProcess::Init(BOOL bIPV6, DWORD dwChanId, DWORD dwMTU,
		WORD  wAudioProto, WORD wVideoProto, WORD  wResendProto,
		PackQueue *pQueueOut, BuffPool *pSubBuffPool)
	{
		m_bIPV6 = bIPV6;

		m_BigBuffPool.SetSize(BUF_SIZE);
		m_AVSend.Init(bIPV6, dwChanId, dwMTU, BUF_SIZE, wAudioProto, wVideoProto, pQueueOut, pSubBuffPool);
		m_AVCapture.Init(&m_AVSend);
		m_AVPlay.Init(&m_BigBuffPool, pSubBuffPool);
		m_AVRecv.Init(bIPV6, dwMTU, wResendProto, &m_AVPlay, pQueueOut, &m_BigBuffPool, pSubBuffPool);
	}

	// �����Լ���Ƶ��λ��
	void CAVProcess::AddShow(HDC hDC, int x, int y)
	{
		m_AVCapture.AddShow(hDC, x, y);
	}

	void CAVProcess::DelShow(HDC hDC)
	{
		m_AVCapture.DelShow(hDC);
	}

	// ���һ��������
	void CAVProcess::AddRecver(DWORD dwChannId, 
		PCLIENT pClient, UINT64 *pnIP, WORD wPort,
		HDC hDC, int x, int y, 
		BOOL bShowBigVideo, int x1, int y1, int cx, int cy)
	{
		m_AVPlay.AddPlayer(dwChannId, hDC, x, y, bShowBigVideo, x1, y1, cx, cy);
		m_AVRecv.AddRecver(dwChannId, pClient, pnIP, wPort);
	}

	// ���һ������,ֱ�Ӵ��ļ����в���
	void CAVProcess::AddPlayer(DWORD dwChannId, LPCSTR strFile, HWND hWnd, int x, int y, 
		BOOL bShowBigVideo, int x1, int y1, int cx, int cy)
	{
		m_AVPlay.AddPlayer(dwChannId, strFile, hWnd, x, y, bShowBigVideo, x1, y1, cx, cy);
	}

	// ���һ��������
	void CAVProcess::AddSender(BOOL bAudio, BOOL bVideo, PCLIENT pClient, LPCTSTR strIP, WORD wPort)
	{
		struct addrinfo *pAddrInfo = NULL;
		if (GetAddrUdp(strIP, wPort, &pAddrInfo))
		{
			UINT64 nIP[2] = {0};
			WORD   wPort  = 0;

			if (m_bIPV6)
			{
				PSOCKADDR_IN6 pAddr6 = (PSOCKADDR_IN6)pAddrInfo->ai_addr;
				wPort = pAddr6->sin6_port;
				memcpy(&nIP, &pAddr6->sin6_addr, 16);
			}
			else
			{
				PSOCKADDR_IN pAddr = (PSOCKADDR_IN)pAddrInfo->ai_addr;
				wPort = pAddr->sin_port;
				nIP[0] = pAddr->sin_addr.S_un.S_addr;
			}

			if (bAudio) m_AVSend.AddAudioSend(pClient, nIP, wPort);
			if (bVideo) m_AVSend.AddVideoSend(pClient, nIP, wPort);

			freeaddrinfo(pAddrInfo);
		}
	}

	// ���һ��������
	// dwIP, wPort �����ֽ�˳��
	void CAVProcess::AddSender(BOOL bAudio, BOOL bVideo, PCLIENT pClient, UINT64 *pnIP, WORD wPort)
	{
		if (bAudio) m_AVSend.AddAudioSend(pClient, pnIP, wPort);
		if (bVideo) m_AVSend.AddVideoSend(pClient, pnIP, wPort);
	}

	// ɾ��������
	void CAVProcess::DelRecver(DWORD dwChannId)
	{
		m_AVRecv.DelRecver(dwChannId);
		m_AVPlay.DelPlayer(dwChannId);
	}

	// ɾ������
	void CAVProcess::DelPlayer(DWORD dwChannId)
	{
		m_AVPlay.DelPlayer(dwChannId);
	}

	// ɾ��������
	void CAVProcess::DelSender(BOOL bAudio, BOOL bVideo, PCLIENT pClient, LPCTSTR strIP, WORD wPort)
	{
		struct addrinfo *pAddrInfo = NULL;
		if (GetAddrUdp(strIP, wPort, &pAddrInfo))
		{
			UINT64 nIP[2] = {0};
			WORD   wPort  = 0;

			if (m_bIPV6)
			{
				PSOCKADDR_IN6 pAddr6 = (PSOCKADDR_IN6)pAddrInfo->ai_addr;
				wPort = pAddr6->sin6_port;
				memcpy(&nIP, &pAddr6->sin6_addr, 16);
			}
			else
			{
				PSOCKADDR_IN pAddr = (PSOCKADDR_IN)pAddrInfo->ai_addr;
				wPort = pAddr->sin_port;
				nIP[0] = pAddr->sin_addr.S_un.S_addr;
			}

			if (bAudio) m_AVSend.DelAudioSend(pClient, nIP, wPort);
			if (bVideo) m_AVSend.DelVideoSend(pClient, nIP, wPort);

			freeaddrinfo(pAddrInfo);
		}
	}

	// ɾ��������
	// dwIP, wPort �����ֽ�˳��
	void CAVProcess::DelSender(BOOL bAudio, BOOL bVideo, PCLIENT pClient, UINT64 *pnIP, WORD wPort)
	{
		if (bAudio) m_AVSend.DelAudioSend(pClient, pnIP, wPort);
		if (bVideo) m_AVSend.DelVideoSend(pClient, pnIP, wPort);
	}

	// ��ʼ������Ƶ����Ϊ�ļ�
	void CAVProcess::StartSaveFile(DWORD dwChanId, LPCSTR strFile)
	{
		m_AVPlay.StartSaveFile(dwChanId, strFile);
	}

	// ֹͣ������Ƶ����Ϊ�ļ�
	DWORD CAVProcess::StopSaveFile(DWORD dwChanId)
	{
		return m_AVPlay.StopSaveFile(dwChanId);
	}

	// ��ʼ��ʾ����Ƶ
	void CAVProcess::StartBigVideo(DWORD dwChanId)
	{
		m_AVPlay.StartShowBigVideo(dwChanId);
	}

	// ֹͣ��ʾ����Ƶ
	void CAVProcess::StopBigVideo(DWORD dwChanId)
	{
		m_AVPlay.StopShowBigVideo(dwChanId);
	}

	// �����Լ�����Ƶ
	void CAVProcess::StartSaveMyFile(LPCTSTR psFile)
	{
		m_AVCapture.StartSaveFile(psFile);
	}

	// �����Լ�����Ƶ
	void CAVProcess::StopSaveMyFile(void)
	{
		m_AVCapture.StopSaveFile();
	}

	// ���ò���
	// dwChanId  [in] ͨ����
	// ePlayType [in] ��������(PLAY,PAUSE,STOP,FORWARD,BACKWARD)
	void CAVProcess::SetPlay(DWORD dwChanId, BYTE ePlayType)
	{
		m_AVPlay.SetPlayFile(dwChanId, ePlayType);
	}

	// ����Ƶ֡�����BMP�ļ�
	void CAVProcess::SaveImage(DWORD dwChanId, LPCSTR strFile)
	{
		m_AVPlay.SaveImage(dwChanId, strFile);
	}

	// ��ʼ��������Ƶ
	void CAVProcess::Start(BOOL bAudio, BOOL bVideo)
	{
		m_AVCapture.Start(bAudio, bVideo);
	}

	// ֹͣ��������Ƶ
	void CAVProcess::Stop(BOOL bAudio, BOOL bVideo)
	{
		m_AVCapture.Stop(bAudio, bVideo);
	}

	// �յ�����Ƶ��
	void CAVProcess::AddAV(PPACK pPack, WORD wType)
	{
		m_AVRecv.RecvAV(pPack, wType);
	}

	// �ط�����Ƶ֡
	void CAVProcess::ResendAV(PCLIENT pClient, DWORD dwId, UINT64 *pnIP, WORD wPort)
	{
		m_AVSend.ResendAV(pClient, dwId, pnIP, wPort);
	}

	WORD CAVProcess::GetVideoSenderCount(void)
	{
		return m_AVSend.GetVideoCount();
	}
}