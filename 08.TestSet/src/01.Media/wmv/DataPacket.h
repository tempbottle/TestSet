/*********************************************************************
  * �ļ���	  :	DataPacket.h
  *	��������  :	ý�����ݰ��Ļ���
  *
  *
  *	����	  : �̱�
  * ��ǰ�汾  : Version 1.0
  * �޸ļ�¼  : 
  *
  *
  *
  *
*****************************************************************/

#if !defined	_DATAPACKET_H__
#define			_DATAPACKET_H__

#pragma once

#include "Packet.h"
#include "librmff.h"
#include "Rtppacket.h"
typedef struct _DATAPACKET_HEADER {

	WORD			m_nTotalLength;
	WORD			m_nSeqNum;
	DWORD			m_nTimeStamp;
	DWORD			m_nSecond;
	BYTE			m_nChanID;
	BYTE			m_nPayloadType;
	BYTE			m_nDataType;
	WORD			m_nDataLen;
	BYTE			m_bKeyFrame;
} DATAPACKET_HEADER;

typedef enum {
	VIDEO_TYPE = 1,
	AUDIO_TYPE = 2
} PAYLOAD_TYPE;

typedef enum{
	RTP_PACKET	= 11,
	RDT_PACKET	= 12,
	END_PACKET	= 13
} DATA_TYPE;

class CDataPacket : public CPacket
{
// Construction
public:
	CDataPacket();
	virtual ~CDataPacket();
	
// Attributes
public:

	//ͷ����Ϣ
	WORD			m_nSeqNum;
	UINT32			m_nTimeStamp;
	DWORD			m_nSecond;
	BYTE			m_nChanID;

	PAYLOAD_TYPE	m_nPayloadType;
	DATA_TYPE		m_nDataType;
	WORD			m_nDataLen;
	BYTE			m_bKeyFrame;

	//ʵ�ʵ���������
	BYTE*			m_pData;	//���� OVER TCP��ͷ��

// Operations
public:
	//////////////////////////////
	//��Buffer�й���һ��DATA��
	static CDataPacket*		CreatePacket(BYTE * pData, DATA_TYPE nDataType);

	static CDataPacket *	CreatePacket(rmff_frame_t * frame) ;//��rm frame�д���һ��DATA��
	static CDataPacket *	CreatePacket(RtpPacket *  packet );
	BOOL	SetFirstFlag();

private:

	virtual void SetTimeStamp ( UINT32 );						//����buffer�е�Timestamp �ֶΣ�����RTPͷ��RDTͷ
	virtual void SetChanID	  ( BYTE );							//����StreamId�ֶΣ�����OVER TCP ͷ��RDT ͷ
	virtual void SetDataLen	  (WORD );							//����DataLen �ֶΣ���OVER TCP ͷ��
	virtual void SetFlags	  ( BYTE );							//����Flag�ֶ� ����RDTͷ�� 
	virtual void SetDefault   ();								//����ȱʡ�ֶ� ���ڣϣ֣ţҡ��ԣãС�ͷ��
	///////////////////////////////
public:

	//��ȡ���ͣ���Ƶ�� ���� ��Ƶ�� 
	virtual PAYLOAD_TYPE	GetPayLoadType() const;

	//���� RTP Packet OR RDT Packet
	virtual DATA_TYPE		GetDataType() const;

	//���ڵڼ���
	virtual DWORD			GetSecond() const;

	//ʱ���
	virtual DWORD			GetTimeStamp() const;

	//���к�
	virtual WORD			GetSeqNum() const;

	//��ȡ���ݵ�ָ��(��ȥOVERTCP��ͷ��)
	virtual BYTE*			GetData() const;

	//��ȡ���ݵĳ���(��ȥOVERTCP��ͷ��)
	virtual WORD			GetDataLen() const;
	
	//�������
	virtual	void			SetSeqNum(WORD nSeqNum);

	//����Channel ID
	virtual WORD			GetChanID();

	//	�Ƿ�Ϊ�ؼ�֡
	virtual	BYTE			IsKeyFrame() const;

	//��ӡ���ݰ��Ĺؼ�������Ϣ
	virtual void			SmartDump() const;

	//����һ��������
	static CDataPacket*		MakeEmptyPacket();

	//����һ����ͬ���ݵ�DataPacket
	virtual	CDataPacket*	Clone();
	
	//���������ݴ����һ��Buffer��
	virtual void	ToBuffer(CBuffer* pBuffer) const;

	//��Buffer�лָ���DataPacket�Ľṹ
	static  CDataPacket * ReadFromPacket( CPacket * pIn);
	
	//�����ṹ�����ݷ���Buffer��
	virtual void	MakePacket(CPacket * pOut);

	//��������
	virtual inline void Delete()
	{
		delete this;
	}

private:
	//���RDT���ͽ�����Ӧ�Ĵ���

	static  CDataPacket *	HandleRTPDataPacket(BYTE * pBuf);

	static  CDataPacket *	HandleRDTDataPacket(BYTE * pBuf);
	static  CDataPacket *	HandleAsmActionPacket(BYTE * pBuf);
	static  CDataPacket *	HandleBandwidthReportPacket(BYTE * pBuf);
	static  CDataPacket *	HandleAckPacket(BYTE * pBuf);
	static  CDataPacket *	HandleRTTRequestPacket(BYTE * pBuf);
	static  CDataPacket *	HandleRTTResponsePacket(BYTE * pBuf);
	static  CDataPacket *	HandleCongestionPacket(BYTE * pBuf);
	static  CDataPacket *	HandleStreamEndPacket(BYTE * pBuf);
	static  CDataPacket *	HandleReportPacket(BYTE * pBuf);
	static  CDataPacket *	HandleLatencyReportPacket(BYTE * pBuf);
	static  CDataPacket *	HandleTransportInfoPacket(BYTE * pBuf);
	static  CDataPacket *	HandleTransportInfoResponsePacket(BYTE * pBuf);
	static  CDataPacket *	HandleBwpRobingPacket(BYTE * pBuf);
	static  CDataPacket *	HandleUnKnownPacket(BYTE * pBuf);

};


#endif 

