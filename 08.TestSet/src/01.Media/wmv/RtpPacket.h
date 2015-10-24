#pragma once


/*
	RtpPacket �����ڲ���RtpPacket ��Ϊ�޸ĺͻ�ȡrtpPacket ����س�Ա�ṩ
	�������������Rtp�Ĵ�С�����ڴ棬�������Ӧ��������һ�� Rtppacket ��

*/

class RtpPacket
{
public:
    enum
    {
        RTP_VERSION_OFFSET	    =	0,
        RTP_PAYLOADTYPE_OFFSET	=	1,
        RTP_SEQ_NUM_OFFSET		=	2,
        RTP_TIME_STAMP_OFFSET	=	4,
        RTP_SSRC_OFFSET			=	8,

        RTP_HEAD_LEN			=	12,
        RTP_PAYLOAD_OFFSET      =   12,

        RTP_VERSION_CONST	    =	0x80,	
        RTP_PAYLOADTYPE_CONST   =	0xe0,
        RTP_SSRC_CONST			=	3202321672
    };

    BYTE * buffer;
    WORD  bufferSize;

    BYTE * payload;
    WORD  payloadSize;

    WORD	trackID;				//��Asf�ļ��� û���ṩ
    BOOL	isKeyFrame;				//��Asf �ļ����� ��һ��Ľ���

public:
    RtpPacket( WORD );				//����Ϊrtppacket �Ĵ�С
    ~RtpPacket(void);
    int SetPayLoad(const BYTE * pData,DWORD size );

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
	BYTE	GetVersion();
	int		SetVersion( BYTE );
	BYTE	GetPayLoadType();
	int		SetPayLoadType( BYTE );
	WORD	GetSeqNum();
	int		SetSeqNum( WORD );
	DWORD	GetTimeStamp();
	int		SetTimeStamp( DWORD );
	DWORD   GetSSRC() ;
	int     SetSSRC( DWORD );

#ifdef  DEBUG
	int		PacketOut() ;
	static int Test();
#endif
	BOOL SetFirstFlag(void);
};
