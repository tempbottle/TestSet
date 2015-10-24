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

#include "StdAfx.h"
#include "DataPacket.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  SHOW_DATA_PACKET 1

//////////////////////////////////////////////////////////////////////

/* Packet types */
#define RDT_ASMACTIION_PACKET               0xff00
#define RDT_BANDWIDTHREPORT_PACKET          0xff01
#define RDT_ACK_PACKET                      0xff02
#define RDT_RTTREQUEST_PACKET               0xff03
#define RDT_RTTRESPONSE_PACKET              0xff04
#define RDT_CONGESTION_PACKET               0xff05
#define RDT_STREAMEND_PACKET                0xff06
#define RDT_REPORT_PACKET                   0xff07
#define RDT_LATENCYREPORT_PACKET            0xff08
#define RDT_TRANSPORTINFO_PACKET            0xff09
#define RDT_TRANSPORTINFORESPONSE_PACKET    0xff0a
#define RDT_BWPROBING_PACKET                0xff0b



#define RTP_TIME_OFFSET		3

#define RDT_FLAG_OFFSET     3
#define RDT_TIME_OFFSET		4
#define RDT_PAD_OFFSET		8
#define RDT_HEAD_LEN        10






//
// CPNPacket Ĭ�ϵĹ��캯������������
//
CDataPacket::CDataPacket() : CPacket( PROTOCOL_DATA )
{

}

CDataPacket::~CDataPacket()
{

}


//����һ��������
CDataPacket*		CDataPacket::MakeEmptyPacket()
{
	CDataPacket * pPacket = new CDataPacket();
	
	BYTE pEmptyData[8];
	BYTE pData[10];
	pData[0] = 0x42;
	pData[1] = 0x00;
	pData[2] = 0x00;
	pData[3] = 0x41;
	pData[4] = 0x00;
	pData[5] = 0x00;
	pData[6] = 0x00;
	pData[7] = 0x00;
	pData[8] = 0x00;
	pData[9] = 0x00;

	pEmptyData[0] = '$';
	pEmptyData[1] = 0;
	*(unsigned short * )(pEmptyData + 2) = htons(10);

	pPacket->Write( pEmptyData, 4 );
	pPacket->Write( pData, 10);

	return pPacket;
}

//////////////////////////////////////////////////////////////////////////

//
//	���RTP���ͽ�����Ӧ�Ĵ���
//
CDataPacket *	CDataPacket::HandleRTPDataPacket(BYTE * pBuf)
{
	CDataPacket * pPacket = new CDataPacket();

	pPacket->m_nDataType	= RTP_PACKET;

	//OVER TCP HEADER
	WORD nDataLen		= ntohs( *(WORD *)(pBuf + 2) );
	pPacket->m_nDataLen	= nDataLen;
	pPacket->m_nChanID	= * (pBuf+1);

	//д����
	pPacket->Write(pBuf, nDataLen + 4);

	pPacket->m_pData	= pPacket->m_pBuffer + 4;

	//RTP DATA HEADER
	pPacket->m_nSeqNum		= ntohs( *(WORD *)( pPacket->m_pData + 1) );
	pPacket->m_nTimeStamp	= ntohl( *(UINT32 *)( pPacket->m_pData + 3) );

	//�Ƿ�Ϊ�ؼ�֡
	pPacket->m_bKeyFrame	= !(0x01 & pPacket->m_pData[3]);

	//����ʱ�����������ݰ�������һ��
	pPacket->m_nSecond		= pPacket->m_nTimeStamp / 1000;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 0)
		pPacket->m_nPayloadType = AUDIO_TYPE;

	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = VIDEO_TYPE;	

	return pPacket;
}

//
//	���RDT���ͽ�����Ӧ�Ĵ���
//
CDataPacket *	CDataPacket::HandleRDTDataPacket(BYTE * pBuf)
{
	CDataPacket * pPacket = new CDataPacket();

	pPacket->m_nDataType	= RDT_PACKET;

	WORD nDataLen		= ntohs( *(WORD *)(pBuf + 2) );
	pPacket->m_nDataLen	= nDataLen;

	//д����
	pPacket->Write(pBuf, nDataLen + 4);
	
	pPacket->m_pData		= pPacket->m_pBuffer + 4;

	pPacket->m_nChanID		= ((* pPacket->m_pData) & 0x3e) >> 1;
	pPacket->m_nSeqNum		= ntohs( *(WORD *)( pPacket->m_pData + 1) );
	
	//�Ƿ�Ϊ�ؼ�֡
	if( 0x01 & pPacket->m_pData[3] )
		pPacket->m_bKeyFrame	=  0;
	else
		pPacket->m_bKeyFrame	=  1;
	
	pPacket->m_nTimeStamp	= ntohl( *(UINT32 *)( pPacket->m_pData + 4) );

	//����ʱ�����������ݰ�������һ��
	pPacket->m_nSecond		= pPacket->m_nTimeStamp / 1000;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 0)
		pPacket->m_nPayloadType = AUDIO_TYPE;

	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = VIDEO_TYPE;	

	return pPacket;
}

//
//	
//
CDataPacket *	CDataPacket::HandleAsmActionPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_ASMACTIION_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleBandwidthReportPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_BANDWIDTHREPORT_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleAckPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_ACK_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleRTTRequestPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_RTTREQUEST_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleRTTResponsePacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_RTTRESPONSE_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleCongestionPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_CONGESTION_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleStreamEndPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_STREAMEND_PACKET");
	
	CDataPacket * pPacket = new CDataPacket();

	pPacket->m_nDataType	= END_PACKET;

	WORD nDataLen		= ntohs( *(WORD *)(pBuf + 2) );
	pPacket->m_nDataLen	= nDataLen;

	pPacket->m_nChanID	= ((* (pBuf + 4)) & 0x7c) >> 2;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 0)
		pPacket->m_nPayloadType = AUDIO_TYPE;
	
	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = VIDEO_TYPE;	


	return pPacket;
}

//
//
//
CDataPacket *	CDataPacket::HandleReportPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_REPORT_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleLatencyReportPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_LATENCYREPORT_PACKET");

	CDataPacket * pPacket	= new CDataPacket();

	pPacket->m_nDataType	= RDT_PACKET;

	//OVER TCP HEADER
	WORD nDataLen		=  *(WORD *)(pBuf + 2);
	nDataLen			=  ntohs(nDataLen);
	pPacket->m_nDataLen	=  nDataLen - 9;

	//Save OVER TCP HEADER
	pPacket->Write(pBuf, 4);

	//Skip  Latency Report Packet Header
	pBuf = pBuf + 4 + 9;

	//д����
	pPacket->Write(pBuf, nDataLen - 9);
	
	//Modify OVER TCP HEADER
	*(WORD *)(pPacket->m_pBuffer + 2) = htons( nDataLen - 9);

	pPacket->m_pData		= pPacket->m_pBuffer + 4;

	pPacket->m_nChanID	=  ( *pPacket->m_pData & 0x3e) >> 1;
	pPacket->m_nSeqNum		= ntohs( *(WORD *)( pPacket->m_pData + 1) );

	//�Ƿ�Ϊ�ؼ�֡
	if( 0x01 & pPacket->m_pData[3] )
		pPacket->m_bKeyFrame	=  0;
	else
		pPacket->m_bKeyFrame	=  1;

	pPacket->m_nTimeStamp	= ntohl( *(UINT32 *)( pPacket->m_pData + 4) );

	//����ʱ�����������ݰ�������һ��
	pPacket->m_nSecond	= pPacket->m_nTimeStamp / 1000;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 0)
		pPacket->m_nPayloadType = AUDIO_TYPE;

	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = VIDEO_TYPE;	

	return pPacket;	

}

//
//
//
CDataPacket *	CDataPacket::HandleTransportInfoPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_TRANSPORTINFO_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleTransportInfoResponsePacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_TRANSPORTINFORESPONSE_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleBwpRobingPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"RDT_BWPROBING_PACKET");

	return NULL;
}

//
//
//
CDataPacket *	CDataPacket::HandleUnKnownPacket(BYTE * pBuf)
{
	//theApp.Message(MSG_DATAPACKET,"UNKOWN_PACKET");

	return NULL;
}


//////////////////////////////////////////////////////////////////////

/*  OVER TCP  Header
 *	|	$	   | StreamID |	Data Length		|   RTP  HEADER | RTP DATA |
 *	|  byte    | byte     |   word          |   RDT  HEADER | RDT DATA |
 *
 *  RTP		  Header
 *	| PayLoad Type (BYTE) | Seq Num         |  TimeStamp               | 
 *  | byte                | word  16Bit     |  32 Bit                  |
 * 
 *
 *  RDT       Header      
 *  | FLAG + STREAM ID |  PACKET TYPE (WORD) (SeqNum) |  flag (BYTE)    |timestamp  32 Bit |
 *  |
 */


//
//	��Buffer�й���һ��DATA��
//
CDataPacket* CDataPacket::CreatePacket(BYTE * pData, DATA_TYPE nDataType)
{
	ASSERT( *pData == '$' );

	//�����ṩ�����ͽ�������	
	switch( nDataType )
	{
	case  RTP_PACKET:
		return HandleRTPDataPacket(pData);
	
	case  RDT_PACKET:		
		{
			//����RDT���͵����ݰ������ݲ�ͬ��RDT���ͽ�����Ӧ�Ĵ���
			UINT16  packetType		=  ntohs( *(UINT16 *)( pData + 5) );

			if (packetType < 0xff00)
			{
				return HandleRDTDataPacket(pData);
			}
			else
			{
				switch (packetType)
				{
				case RDT_ASMACTIION_PACKET:
					return HandleAsmActionPacket(pData);
					
				case RDT_BANDWIDTHREPORT_PACKET:
					return HandleBandwidthReportPacket(pData);
					
				case RDT_ACK_PACKET:
					return HandleAckPacket(pData);
					
				case RDT_RTTREQUEST_PACKET:
					return HandleRTTRequestPacket(pData);
					
				case RDT_RTTRESPONSE_PACKET:
					return HandleRTTResponsePacket(pData);
					
				case RDT_CONGESTION_PACKET:
					return HandleCongestionPacket(pData);
					
				case RDT_STREAMEND_PACKET:
					return HandleStreamEndPacket(pData);
					
				case RDT_REPORT_PACKET:
					return HandleReportPacket(pData);
					
				case RDT_LATENCYREPORT_PACKET:
					return HandleLatencyReportPacket(pData);
					
				case RDT_TRANSPORTINFO_PACKET:
					return HandleTransportInfoPacket(pData);
					
				case RDT_TRANSPORTINFORESPONSE_PACKET:
					return HandleTransportInfoResponsePacket(pData);
					
				case RDT_BWPROBING_PACKET:
					return HandleBwpRobingPacket(pData);

				default:
					return HandleUnKnownPacket(pData);
				}
			}
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////

/*  OVER TCP  Header
 *	|	$	   | StreamID |	Data Length		|   RTP  HEADER | RTP DATA |
 *	|  byte    | byte     |   word          |   RDT  HEADER | RDT DATA |
 *
 *  RTP		  Header
 *	| PayLoad Type (BYTE) | Seq Num         |  TimeStamp               | 
 *  | byte                | word  16Bit     |  32 Bit                  |
 * 
 *
 *  RDT       Header      
 *  | FLAG + STREAM ID |  PACKET TYPE (WORD) (SeqNum) |  flag (BYTE)    |timestamp  32 Bit |
 *  |
 */
#define CHAN_ID_CONST 0x02
BOOL	CDataPacket::SetFirstFlag()
{
	*(m_pData+RtpPacket::RTP_PAYLOAD_OFFSET)|=0x80;
	return TRUE;
}
CDataPacket *	CDataPacket::CreatePacket(RtpPacket *  packet ){
	if ( packet == NULL ) return NULL;

	CDataPacket * pPacket = new CDataPacket();

	pPacket->m_nDataType = RTP_PACKET;

	
	pPacket->m_nDataLen	= packet->bufferSize;				//���ݳ���ΪRDT ͷ���Ⱥ�ý�����ݳ���

	

	BYTE headBuf[4];
	memset( headBuf, 0, 4 );										//д��OVER TCP ͷ
	pPacket->Write( headBuf, 4);


	pPacket->Write(packet->buffer, packet->bufferSize );						//д��ý������
	
	pPacket->m_pData		= pPacket->m_pBuffer + 4;				//����ָ��ָ��RTP ͷ
	
	
	pPacket->m_nChanID		= 0;							//����Stream Id 
	
	
	//�Ƿ�Ϊ�ؼ�֡
	pPacket->m_bKeyFrame		= packet->isKeyFrame;
	
	pPacket->m_nTimeStamp = packet->GetTimeStamp();
	

	//����ʱ�����������ݰ�������һ��
	pPacket->m_nSecond		= pPacket->m_nTimeStamp / 1000;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = AUDIO_TYPE;

	if( pPacket->m_nChanID == 2)
		pPacket->m_nPayloadType = VIDEO_TYPE;	
															//�������ֽ�˳������buffer�е���Ӧ�ֶ�
	pPacket->SetDefault();
	pPacket->SetTimeStamp( pPacket->m_nTimeStamp );
	pPacket->SetChanID(CHAN_ID_CONST );
	pPacket->SetDataLen ( pPacket->m_nDataLen );
	//pPacket->SetFlags ( frame->flags );

	return pPacket;
	
}
CDataPacket *	CDataPacket::CreatePacket( rmff_frame_t * frame) {	//��rm frame �д���һ��Data packet

	
	if ( frame == NULL ) return NULL;

	CDataPacket * pPacket = new CDataPacket();

	pPacket->m_nDataType = RDT_PACKET;

	
	pPacket->m_nDataLen	= RDT_HEAD_LEN + frame->size;				//���ݳ���ΪRDT ͷ���Ⱥ�ý�����ݳ���

	

	BYTE headBuf[14];
	memset( headBuf, 0, 14 );										//д��OVER TCP ͷ,д��RDT ͷ
	pPacket->Write( headBuf, 14);


	pPacket->Write(frame->data, frame->size );						//д��ý������
	
	pPacket->m_pData		= pPacket->m_pBuffer + 4;				//����ָ��ָ��RDT ͷ
	
	
	pPacket->m_nChanID		= frame->id;							//����Stream Id 
	
	
	//�Ƿ�Ϊ�ؼ�֡
	pPacket->m_bKeyFrame		= frame->flags & 0x01;
	
	pPacket->m_nTimeStamp = frame->timecode;
	

	//����ʱ�����������ݰ�������һ��
	pPacket->m_nSecond		= pPacket->m_nTimeStamp / 1000;

	//��ʼ��PayLoad ����
	if( pPacket->m_nChanID == 0)
		pPacket->m_nPayloadType = AUDIO_TYPE;

	if( pPacket->m_nChanID == 1)
		pPacket->m_nPayloadType = VIDEO_TYPE;	
															//�������ֽ�˳������buffer�е���Ӧ�ֶ�
	pPacket->SetDefault();
	//pPacket->SetTimeStamp( pPacket->m_nTimeStamp );
	pPacket->SetChanID( pPacket->m_nChanID );
	pPacket->SetDataLen ( pPacket->m_nDataLen );
	//pPacket->SetFlags ( frame->flags );

	return pPacket;
}

//CDataPacket *	CDataPacket::CreatePacket(  RtpPacket* frame ) {	//��rm frame �д���һ��Data packet
//
//	
//	if ( frame == NULL ) return NULL;
//
//	CDataPacket * pPacket = new CDataPacket();
//
//	pPacket->m_nDataType = RTP_PACKET;
//
//	
//	pPacket->m_nDataLen	= RTP_HEAD_LEN + frame->size;				//���ݳ���ΪRDT ͷ���Ⱥ�ý�����ݳ���
//
//	
//
//	BYTE headBuf[4];
//	memset( headBuf, 0, 4 );										//д��OVER TCP ͷ
//	pPacket->Write( headBuf, 4);
//
//
//	pPacket->Write(frame->data, frame->size );						//д��ý������
//	
//	pPacket->m_pData		= pPacket->m_pBuffer + 4;				//����ָ��ָ��RDT ͷ
//	
//	
//	pPacket->m_nChanID		= frame->id;							//����Stream Id 
//	
//	
//	//�Ƿ�Ϊ�ؼ�֡
//	pPacket->m_bKeyFrame		= frame->flags & 0x01;
//	
//	pPacket->m_nTimeStamp = frame->timecode;
//	
//
//	//����ʱ�����������ݰ�������һ��
//	pPacket->m_nSecond		= pPacket->m_nTimeStamp / 1000;
//
//	//��ʼ��PayLoad ����
//	if( pPacket->m_nChanID == 0)
//		pPacket->m_nPayloadType = AUDIO_TYPE;
//
//	if( pPacket->m_nChanID == 1)
//		pPacket->m_nPayloadType = VIDEO_TYPE;	
//															//�������ֽ�˳������buffer�е���Ӧ�ֶ�
//	pPacket->SetDefault();
//	pPacket->SetTimeStamp( pPacket->m_nTimeStamp );
//	pPacket->SetChanID( pPacket->m_nChanID );
//	pPacket->SetDataLen ( pPacket->m_nDataLen );
//	pPacket->SetFlags ( frame->flags );
//
//	return pPacket;
//}

void CDataPacket::SetDefault() {							//����ȱʡ�ֶ� ���ڣϣ֣ţҡ��ԣãС�ͷ��
	m_pBuffer[0] = '$';
	if( m_nDataType == RDT_PACKET ) {

		*( WORD* )( m_pData + RDT_PAD_OFFSET ) = 0;
	}
	//*(UINT16 *)(buf + 12) = 0;
}

void CDataPacket::SetTimeStamp( UINT32 time) {				//����buffer�е�Timestamp �ֶΣ�����RTPͷ��RDTͷ
	if( m_nDataType == RTP_PACKET ) {
		*(UINT32*) ( m_pData + RTP_TIME_OFFSET ) = htonl( time );
	
	}
	if( m_nDataType == RDT_PACKET ) {
		*(UINT32*)( m_pData + RDT_TIME_OFFSET ) = htonl( time );
	}
}

void CDataPacket::SetChanID( BYTE chanID) {					//����StreamId�ֶΣ�����OVER TCP ͷ��RDT ͷ				
	*(m_pBuffer+1) = chanID;
	if(  m_nDataType == RDT_PACKET ) {						//stream ID �ĸ�ʽ
		// *( m_pData )&= 0xc1 ;								// | ++-- ---+ |
		//chanID <<=1 ;
		//chanID &= 0x3e ;
		//*( m_pData ) |= chanID;

		char m	= chanID;
		m = m << 1;
		m |= 0x40;
		*( m_pData ) = m;
	}
}

void CDataPacket::SetDataLen ( WORD len ) {					//����DataLen �ֶΣ���OVER TCP ͷ��
	*(WORD*)( m_pBuffer + 2) = htons( len );
}

void CDataPacket::SetFlags( BYTE flags) {					//����Flag�ֶ� ����RDTͷ�� 
	ASSERT (m_nDataType == RDT_PACKET ) ;
	BYTE  m;
	if( flags )
		m = 0;
	else
		m = 1;

	m |= 0xc0;
	*(m_pData + RDT_FLAG_OFFSET) = m;
}








//
//	����һ����ͬ���ݵ�DataPacket
//
CDataPacket* CDataPacket::Clone()
{
	CDataPacket* pPacket		= new CDataPacket();

	//��ʼ��ͷ��
	pPacket->m_nSeqNum			= m_nSeqNum;
	pPacket->m_nTimeStamp		= m_nTimeStamp;
	pPacket->m_nChanID			= m_nChanID;
	pPacket->m_nPayloadType		= m_nPayloadType;
	pPacket->m_nSecond			= m_nSecond;
	pPacket->m_nDataType		= m_nDataType;	
	pPacket->m_bKeyFrame		= m_bKeyFrame;

	//����������
	pPacket->Write( &m_pBuffer, (DWORD)m_nBuffer );

	return pPacket;		
}

//
//	�����ݷŵ�һ��Buffer��
//
void CDataPacket::ToBuffer(CBuffer* pBuffer) const
{
	pBuffer->Add( m_pBuffer, m_nLength );
}

CDataPacket * CDataPacket::ReadFromPacket( CPacket * pIn )
{
	//�ж�ͷ���ĳ����Ƿ���
	if( pIn->GetRemaining() < sizeof(DATAPACKET_HEADER) ) return NULL;


    //PEEKͷ����Ϣ
	DATAPACKET_HEADER  *pHeader = ( DATAPACKET_HEADER * ) (pIn->m_pBuffer + pIn->m_nPosition);

	//����ܵĳ����Ƿ���
	if( pIn->GetRemaining() < pHeader->m_nTotalLength ) return NULL;


	//����һ���µ�DataPacket
	CDataPacket *pDataPacket = new CDataPacket();

	//��ȡͷ����Ϣ
	DATAPACKET_HEADER header;
	pIn->Read( (BYTE*)&header, sizeof(DATAPACKET_HEADER));

	pDataPacket->m_nSeqNum		=	header.m_nSeqNum;
	pDataPacket->m_nTimeStamp	=	header.m_nTimeStamp;
	pDataPacket->m_nSecond		=	header.m_nSecond;
	pDataPacket->m_nChanID		=	header.m_nChanID;
	pDataPacket->m_nPayloadType	=	(PAYLOAD_TYPE)header.m_nPayloadType;
	pDataPacket->m_nDataType	=	(DATA_TYPE)header.m_nDataType;
	pDataPacket->m_nDataLen		=	header.m_nDataLen;
	pDataPacket->m_bKeyFrame	=	header.m_bKeyFrame;


	//��������Ϣ
	pDataPacket->Write( pIn->m_pBuffer + pIn->m_nPosition, header.m_nTotalLength - sizeof(DATAPACKET_HEADER));

	//��ȡ���ݺ��ƶ�ָ��λ��
	pIn->m_nPosition += header.m_nTotalLength - sizeof(DATAPACKET_HEADER);

	//����DataPacket������ָ��
	pDataPacket->m_pData		=	pDataPacket->m_pBuffer + 4;

	return pDataPacket;

}

//
//	�����ṹ�����ݷ���Buffer��
//
void	CDataPacket::MakePacket(CPacket * pOut )
{
	//���㳤��
	WORD	nLen = (WORD)( m_nLength + sizeof(DATAPACKET_HEADER));

	//дͷ��
	DATAPACKET_HEADER header;

	header.m_nTotalLength	=	nLen;		//ͷ�����ݳ��Ⱥ�ý�����ݳ���
	header.m_nSeqNum		=   m_nSeqNum;
	header.m_nTimeStamp		=	m_nTimeStamp;
	header.m_nSecond		=	m_nSecond;
	header.m_nChanID		=	m_nChanID;
	header.m_nPayloadType	=	m_nPayloadType;
	header.m_nDataType		=	m_nDataType;
	header.m_nDataLen		=	m_nDataLen;
	header.m_bKeyFrame		=   m_bKeyFrame;

	pOut->Write( (BYTE *)&header, sizeof(DATAPACKET_HEADER) );

	//д����
	pOut->Write( m_pBuffer, m_nLength );

}

//
//	��ȡ���ͣ���Ƶ�� ���� ��Ƶ�� 
//
PAYLOAD_TYPE CDataPacket::GetPayLoadType() const
{
	return m_nPayloadType;
}

//
//	���� RTP Packet OR RDT Packet
//
DATA_TYPE CDataPacket::GetDataType() const
{
	return m_nDataType;
}

//
//	���ڵڼ���
//
DWORD CDataPacket::GetSecond() const
{
	return m_nSecond;
}


//
//	ʱ���
//
DWORD CDataPacket::GetTimeStamp() const
{
	return m_nTimeStamp;
}

//
//	���к�
//
WORD CDataPacket::GetSeqNum() const
{
	return m_nSeqNum;
}

//
//	�Ƿ�Ϊ�ؼ�֡
//
BYTE CDataPacket::IsKeyFrame() const
{
	return m_bKeyFrame;
}


//
//	ȡ�����ݵ�ָ��(��ȥͷ��)
//
BYTE* CDataPacket::GetData() const
{
	return m_pData;
}

//
//	ȡ�ó�ȥͷ�������ݵĳ���
//
WORD CDataPacket::GetDataLen() const
{
	return m_nDataLen;
}

//
//	�������
//
void CDataPacket::SetSeqNum(WORD nSeqNum)
{
	m_nSeqNum = nSeqNum;
	
	//�޸�RTP���е����кţ���д��
	if(m_nDataType == RTP_PACKET) 
	{
		*(WORD*)(m_pData +RtpPacket::RTP_SEQ_NUM_OFFSET) = htons( m_nSeqNum);
	}
	if(m_nDataType == RDT_PACKET)
	{
		*(WORD * ) (m_pData + 1) = htons( m_nSeqNum );
	}

}

//
//����Channel ID
//
WORD CDataPacket::GetChanID()
{
	return m_nChanID;
}

//////////////////////////////////////////////////////////////////////

//
//	CPNPacket ��ӡ�������ݣ���������
//
void CDataPacket::SmartDump() const
{
	CString strPlayLoad;
	
	switch( m_nPayloadType )
	{
	case VIDEO_TYPE:
		strPlayLoad = "Video";
		break;
	case AUDIO_TYPE:
		strPlayLoad = "Audio";
		break;
	default:
		strPlayLoad = "Unkown";
	}

#if SHOW_DATA_PACKET	
	/*theApp.Message(MSG_DATAPACKET,"Type:%s, Seq = %lu, timestamp = %lu, KeyFrame = %d, Length=%d",
								strPlayLoad,
								this->m_nSeqNum,
								this->m_nTimeStamp,
								this->m_bKeyFrame,
								this->m_nDataLen);*/
		
			CString str;
				
					for( int i = 0; i<32; i++)
					{
						
						str.AppendFormat("%02x ",this->m_pBuffer[i]);
						if( i%16 == 15 ) str.AppendFormat( "\n");
					}
					theApp.Message(MSG_DEFAULT,str);
#endif

}

