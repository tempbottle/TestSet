
#pragma once

#include "RtpPacket.h"
#include "AsfPacket.h"
#include "AsfHeadObject.h"


class AsfReader
{
public:
	CFile	fp;					//�ļ����
	CString filename;
	AsfHeadObject *pHeadObj;

	//QWORD	packetCounts;		//packet ����
	//WORD	packetSize;			//�ļ��İ���С
	QWORD	packetInitOffset;

public:
	AsfReader(void);
	~AsfReader(void);
	int Initial(CString  filename); //��ʼ�� �ļ������������ȥ �� Ԫ����

	int Locate(WORD i);
	BYTE* ReadNextPacket();
	RtpPacket* ReadNextRtpPacket();	//��ȡ��һ��Packet ��������ת��ΪRtpPacket

private:
	DWORD PacketsRead;

    //��Packetת����RtpPacket �ĺ���
	static RtpPacket* Packet2Rtp(AsfPacket* pa); 		
};


