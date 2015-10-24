
#pragma once
#include "Network/Srv.h"

namespace XT_IOCP
{

	typedef struct _tagMMS_CMD
	{
		DWORD	dwFlag1;		// 0x00000001
		DWORD	dwFlag2;		// 0xb00bface
		DWORD	dwDataLen;		// "MMS "�����ݳ�
		DWORD	dwFlag3;		// "MMS "
		DWORD	dwAlign1;		// ��8�ֽڶ���
		DWORD	dwSeqNum;		// ���
		UINT64	uTimeStamp;		// ʱ���
		DWORD	dwAlign2;		// ��8�ֽڶ���
		USHORT	sCmdNum;		// �����
		USHORT	sDirect;		// ���ͷ���,0x03ָ���ͻ��˷���������,0x04ָ�������������ͻ���
		char	szData[1];		// ��Ϣ����
	}MMS_HEAD,*PMMS_HEAD;

	class CSrv_MMS : public CSrv
	{
	public:
		CSrv_MMS(void);
		~CSrv_MMS(void);

	private:    
		HANDLE   m_hRtspStreamThread;
		PCLIENT  m_pClient;

		void ProcessPack(PCLIENT pClient, DWORD dwDataLen, PVOID pvBuf, UINT64 nIP[2], WORD wPort);

		void SetU32(BYTE **pBuf, DWORD dwValue);
		void SetStr(BYTE **pBuf, LPCSTR szStr);

	public:
		int Start(LPCTSTR pszIP, const int nPort);
		int Connect(LPCTSTR pszIP, const int nPort);
		int Login(void);
	};
}