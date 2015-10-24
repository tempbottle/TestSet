#pragma once

/*
AsfPacket �����ڽ�����asf�ļ��ж�����packet ԭ���ݡ�
�õ�timestamp  ��duration ��payload
AsfPacket �಻�޸�ԭ���ݣ��������ڴ�ķ�����ͷš�
*/

class AsfPacket
{
public:
    const BYTE* buf;		//packet ����ָ��
    DWORD PacketSize;		//packet ��С�� ����ͬһ���ļ�PacketSize �Ǹ�����

    BYTE ErrorFlags;		//packet���ݵĵ�һ��

    const BYTE *ErrorData;
    BYTE  ErrorDataLen;

    BYTE LenFlags;

    BYTE PropFlags;

    DWORD	PacketLen;
    DWORD	Sequence;
    DWORD	PadLen;
    DWORD	Time;
    WORD	Duration;
    const	BYTE*	Payload;
    DWORD   PayloadSize;			//�������������
public:
    AsfPacket( const BYTE * , DWORD );
	BOOL	Initial();

public:
    ~AsfPacket(void);
	static AsfPacket* CreatePacket( const BYTE*, DWORD );
};
