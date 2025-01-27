//this file is part of eMule
//Copyright (C)2002-2006 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once
//#include "UploadBandwidthThrottler.h" // ZZ:UploadBandWithThrottler (UDP)
//#include "EncryptedDatagramSocket.h"
//#include "UPnp.h"

#include <afxmt.h>
#include <afxsock.h>

//#include <atlsync.h>
#include "./types.h"
#include "./io/BufferedFileIO.h"
#include "./io/IOException.h"
#include "./kademlia/Kademlia.h"


class Packet;

#pragma pack(1)
struct UDPPack
{
	Packet* packet;
	uint32	dwIP;
	uint16	nPort;
	uint32	dwTime;
	bool	bEncrypt;
	uchar	achTargetClientHash[16];
	//uint16 nPriority; We could add a priority system here to force some packets.
};
#pragma pack()


struct SocketSentBytes {
    bool    success;
	uint32	sentBytesStandardPackets;
	uint32	sentBytesControlPackets;
};


#define OP_VC_NAT_HEADER		0xf1



class CClientUDPSocket : public CAsyncSocket//, public CEncryptedDatagramSocket, public ThrottledControlSocket // ZZ:UploadBandWithThrottler (UDP)
{
public:
	CClientUDPSocket();
	virtual ~CClientUDPSocket();

	bool	Create();
	bool	Rebind();
	uint16	GetConnectedPort()			{ return m_port; }
	bool	SendPacket(Packet* packet, uint32 dwIP, uint16 nPort, bool bEncrypt, const uchar* pachTargetClientHash);
//    SocketSentBytes  SendControlData(uint32 maxNumberOfBytesToSend, uint32 minFragSize); // ZZ:UploadBandWithThrottler (UDP)
protected:
	bool	ProcessPacket(const BYTE* packet, UINT size, uint8 opcode, uint32 ip, uint16 port);
	
	virtual void	OnSend(int nErrorCode);
	virtual void	OnReceive(int nErrorCode);

private:
	bool	PrivateCreate();
//	static DWORD WINAPI PrivateThread(LPVOID param);
//	int		m_nCreateState;
	int		SendTo(char* lpBuf,int nBufLen,uint32 dwIP, uint16 nPort);
    bool	IsBusy() const { return m_bWouldBlock; }

	bool	m_bWouldBlock;
	uint16	m_port;

	CTypedPtrList<CPtrList, UDPPack*> controlpacket_queue;

    CCriticalSection sendLocker; // ZZ:UploadBandWithThrottler (UDP)
};
