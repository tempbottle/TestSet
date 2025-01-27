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
#include "stdafx.h"
//#include "emule.h"
#include "ClientUDPSocket.h"
/*
#include "Packets.h"
#include "DownloadQueue.h"
#include "Statistics.h"
#include "PartFile.h"
#include "SharedFileList.h"
#include "UploadQueue.h"
#include "UpDownClient.h"
#include "Preferences.h"
#include "OtherFunctions.h"
#include "SafeFile.h"
#include "ClientList.h"
#include "Listensocket.h"
#include <zlib/zlib.h>
#include "kademlia/kademlia/Kademlia.h"
#include "kademlia/net/KademliaUDPListener.h"
#include "kademlia/io/IOException.h"
#include "IPFilter.h"
#include "Log.h"
#include "EncryptedDatagramSocket.h"
#include "Sockets.h"	// VC-kernel[2006-11-24]:
//#include "NatTraversal/NatBridge.h"	// VC-kernel[2006-11-24]:

#include "FirewallOpener.h" //Added by thilon on 2006.10.18,For FireWall

//  added by yunchenn
#include "NatTraversal/NatThread.h"
#include "StatForServer.h"
*/
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CNatThread * natthread;
// CClientUDPSocket

using namespace ATL;

CClientUDPSocket::CClientUDPSocket()
{
	m_bWouldBlock = false;
	m_port=0;
	//	m_nCreateState = 0;
}

CClientUDPSocket::~CClientUDPSocket()
{
	//    theApp.uploadBandwidthThrottler->RemoveFromAllQueues(this); // ZZ:UploadBandWithThrottler (UDP)

	POSITION pos = controlpacket_queue.GetHeadPosition();
	while (pos){
		UDPPack* p = controlpacket_queue.GetNext(pos);
		delete p->packet;
		delete p;
	}

}

void CClientUDPSocket::OnReceive(int nErrorCode)
{
	//	if(theApp.m_app_state != APP_STATE_RUNNING) return;
	if (!Kademlia::CKademlia::IsRunning()) return;

	if (nErrorCode)
	{
		//		if (thePrefs.GetVerbose())
		//			DebugLogError(_T("Error: Client UDP socket, error on receive event: %s"), GetErrorMessage(nErrorCode, 1));
		TRACE(_T("Error: Client UDP socket, error on receive event: %d"), nErrorCode);
	}

	BYTE buffer[5000];
	SOCKADDR_IN sockAddr = {0};
	int iSockAddrLen = sizeof sockAddr;
	int nRealLen = ReceiveFrom(buffer, sizeof buffer, (SOCKADDR*)&sockAddr, &iSockAddrLen);
	if(buffer[0]==OP_VC_NAT_HEADER)
	{
		//		if(theApp.natthread)
		//			theApp.natthread->ProcessPacket(buffer+1, nRealLen-1, sockAddr.sin_addr.S_un.S_addr, sockAddr.sin_port);
		return;
	}
	//	if (!(theApp.ipfilter->IsFiltered(sockAddr.sin_addr.S_un.S_addr) || theApp.clientlist->IsBannedClient(sockAddr.sin_addr.S_un.S_addr)))
	{
		BYTE* pBuffer = buffer;
		int nPacketLen = nRealLen;//DecryptReceivedClient(buffer, nRealLen, &pBuffer, sockAddr.sin_addr.S_un.S_addr);
		if (nPacketLen >= 1)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState())

				CString strError;
			try
			{
				switch (pBuffer[0])
				{
				case OP_EMULEPROT:
					{
						if (nPacketLen >= 2)
							ProcessPacket(pBuffer+2, nPacketLen-2, pBuffer[1], sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
						else
							throw CString(_T("eMule packet too short"));
						break;
					}
				case OP_KADEMLIAPACKEDPROT:
					{
						//						theStats.AddDownDataOverheadKad(nPacketLen);
						if (nPacketLen >= 2)
						{
							uint32 nNewSize = nPacketLen*10+300;
							byte* unpack = new byte[nNewSize];
							uLongf unpackedsize = nNewSize-2;
							int iZLibResult = uncompress(unpack+2, &unpackedsize, pBuffer+2, nPacketLen-2);
							if (iZLibResult == Z_OK)
							{
								unpack[0] = OP_KADEMLIAHEADER;
								unpack[1] = pBuffer[1];
								try
								{
									Kademlia::CKademlia::ProcessPacket(unpack, unpackedsize+2, ntohl(sockAddr.sin_addr.S_un.S_addr), ntohs(sockAddr.sin_port));
								}
								catch(...)
								{
									delete[] unpack;
									throw;
								}
							}
							else
							{
								delete[] unpack;
								CString strError;
								strError.Format(_T("Failed to uncompress Kad packet: zip error: %d (%hs)"), iZLibResult, zError(iZLibResult));
								throw strError;
							}
							delete[] unpack;
						}
						else
							throw CString(_T("Kad packet (compressed) too short"));
						break;
					}
				case OP_KADEMLIAHEADER:
					{
						//						theStats.AddDownDataOverheadKad(nPacketLen);
						if (nPacketLen >= 2)
							Kademlia::CKademlia::ProcessPacket(pBuffer, nPacketLen, ntohl(sockAddr.sin_addr.S_un.S_addr), ntohs(sockAddr.sin_port));
						else
							throw CString(_T("Kad packet too short"));
						break;
					}
					//  added by yunchenn
				case OP_VC_NAT_HEADER:
					{
						/*if(theApp.serverconnect->clientid==0)  break;
						// VC-kernel[2006-11-24]:
						if(theApp.serverconnect->IsLowID()){*/
						//						if(theApp.natthread)
						//								theApp.natthread->ProcessPacket(pBuffer+1, nPacketLen-1, sockAddr.sin_addr.S_un.S_addr, sockAddr.sin_port);
						/*}else{
						theApp.natbridge->Process((char*)pBuffer+1, nPacketLen-1, sockAddr.sin_addr.S_un.S_addr, sockAddr.sin_port);
						}*/

						break;
					}
					//case OP_VC_NAT_HEADER+1:
					//	if(theApp.natthread)
					//		theApp.natthread->ProcessPacket2((uchar)(pBuffer+1), pBuffer+2, nPacketLen-2, sockAddr.sin_addr.S_un.S_addr, sockAddr.sin_port);
					//	break;
				default:
					{
						CString strError;
						strError.Format(_T("Unknown protocol 0x%02x"), pBuffer[0]);
						throw strError;
					}
				}
			}
			catch(CFileException* error)
			{
				error->Delete();
				strError = _T("Invalid packet received");
			}
			catch(CMemoryException* error)
			{
				error->Delete();
				strError = _T("Memory exception");
			}
			catch(CString error)
			{
				strError = error;
			}
			catch(Kademlia::CIOException* error)
			{
				error->Delete();
				strError = _T("Invalid packet received");
			}
			catch(CException* error)
			{
				error->Delete();
				strError = _T("General packet error");
			}
#ifndef _DEBUG
			catch(...)
			{
				strError = _T("Unknown exception");
				ASSERT(0);
			}
#endif
			/*			if (thePrefs.GetVerbose() && !strError.IsEmpty())
			{
			CString strClientInfo;
			CUpDownClient* client;
			if (pBuffer[0] == OP_EMULEPROT)
			client = theApp.clientlist->FindClientByIP_UDP(sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
			else
			client = theApp.clientlist->FindClientByIP_KadPort(sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
			if (client)
			strClientInfo = client->DbgGetClientInfo();
			else
			strClientInfo.Format(_T("%s:%u"), ipstr(sockAddr.sin_addr), ntohs(sockAddr.sin_port));

			DebugLogWarning(_T("Client UDP socket: prot=0x%02x  opcode=0x%02x  sizeaftercrypt=%u realsize=%u  %s: %s"), pBuffer[0], pBuffer[1], nPacketLen, nRealLen, strError, strClientInfo);
			}
			*/		}
		else if (nPacketLen == SOCKET_ERROR)
		{
			DWORD dwError = WSAGetLastError();
			if (dwError == WSAECONNRESET)
			{
				// Depending on local and remote OS and depending on used local (remote?) router we may receive
				// WSAECONNRESET errors. According some KB articles, this is a special way of winsock to report 
				// that a sent UDP packet was not received by the remote host because it was not listening on 
				// the specified port -> no eMule running there.
				//
				// TODO: So, actually we should do something with this information and drop the related Kad node 
				// or eMule client...
				;
			}
			else
			{
				TRACE(_T("CClientUDPSocket::OnReceive Error %d\n"), dwError);
			}
			/*			if (thePrefs.GetVerbose() && dwError != WSAECONNRESET)
			{
			CString strClientInfo;
			if (iSockAddrLen > 0 && sockAddr.sin_addr.S_un.S_addr != 0 && sockAddr.sin_addr.S_un.S_addr != INADDR_NONE)
			strClientInfo.Format(_T(" from %s:%u"), ipstr(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
			DebugLogError(_T("Error: Client UDP socket, failed to receive data%s: %s"), strClientInfo, GetErrorMessage(dwError, 1));
			}
			*/			
		}
	}
}

bool CClientUDPSocket::ProcessPacket(const BYTE* packet, UINT size, uint8 opcode, uint32 ip, uint16 port)
{
	/*	if (!CSourceExchangeNAT::ProcessUDPPacket(packet, size, opcode, ip, port))
	{
	return true;
	}
	*/
	switch(opcode)
	{
	case OP_REASKCALLBACKUDP:
		{
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_ReaskCallbackUDP", NULL, NULL, ip);
			DebugRecv("OP_ReaskCallbackUDP", ip, port);
			/*			theStats.AddDownDataOverheadOther(size);
			CUpDownClient* buddy = theApp.clientlist->GetBuddy();
			if( buddy )
			{
			if( size < 17 || buddy->socket == NULL )
			break;
			if (!md4cmp(packet, buddy->GetBuddyID()))
			{
			PokeUInt32(const_cast<BYTE*>(packet)+10, ip);
			PokeUInt16(const_cast<BYTE*>(packet)+14, port);
			Packet* response = new Packet(OP_EMULEPROT);
			response->opcode = OP_REASKCALLBACKTCP;
			response->pBuffer = new char[size];
			memcpy(response->pBuffer, packet+10, size-10);
			response->size = size-10;
			if (thePrefs.GetDebugClientTCPLevel() > 0)
			DebugSend("OP__ReaskCallbackTCP", buddy);
			theStats.AddUpDataOverheadFileRequest(response->size);
			buddy->socket->SendPacket(response);
			}
			}
			*/			break;
		}
	case OP_REASKFILEPING:
		{
			DebugRecv("OP_ReaskFilePing", ip, port);

			/*			theStats.AddDownDataOverheadFileRequest(size);
			CSafeMemFile data_in(packet, size);
			uchar reqfilehash[16];
			data_in.ReadHash16(reqfilehash);
			CKnownFile* reqfile = theApp.sharedfiles->GetFileByID(reqfilehash);

			bool bSenderMultipleIpUnknown = false;
			CUpDownClient* sender = theApp.uploadqueue->GetWaitingClientByIP_UDP(ip, port, true, &bSenderMultipleIpUnknown);
			if (!reqfile)
			{
			//if (thePrefs.GetDebugClientUDPLevel() > 0) {
			//DebugRecv("OP_ReaskFilePing", NULL, reqfilehash, ip);
			DebugRecv("OP_ReaskFilePing", ip, port);
			//DebugSend("OP__FileNotFound", NULL);
			//}

			Packet* response = new Packet(OP_FILENOTFOUND,0,OP_EMULEPROT);
			theStats.AddUpDataOverheadFileRequest(response->size);
			if (sender != NULL)
			SendPacket(response, ip, port, sender->ShouldReceiveCryptUDPPackets(), sender->GetUserHash());
			else
			SendPacket(response, ip, port, false, NULL);
			break;
			}
			if (sender)
			{
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_ReaskFilePing", sender, reqfilehash);			

			//Make sure we are still thinking about the same file
			if (md4cmp(reqfilehash, sender->GetUploadFileID()) == 0)
			{
			sender->AddAskedCount();
			sender->SetLastUpRequest();
			//I messed up when I first added extended info to UDP
			//I should have originally used the entire ProcessExtenedInfo the first time.
			//So now I am forced to check UDPVersion to see if we are sending all the extended info.
			//For now on, we should not have to change anything here if we change
			//anything to the extended info data as this will be taken care of in ProcessExtendedInfo()
			//Update extended info. 
			if (sender->GetUDPVersion() > 3)
			{
			sender->ProcessExtendedInfo(&data_in, reqfile);
			}
			//Update our complete source counts.
			else if (sender->GetUDPVersion() > 2)
			{
			uint16 nCompleteCountLast= sender->GetUpCompleteSourcesCount();
			uint16 nCompleteCountNew = data_in.ReadUInt16();
			sender->SetUpCompleteSourcesCount(nCompleteCountNew);
			if (nCompleteCountLast != nCompleteCountNew)
			{
			reqfile->UpdatePartsInfo();
			}
			}
			CSafeMemFile data_out(128);
			if(sender->GetUDPVersion() > 3)
			{
			if (reqfile->IsPartFile())
			((CPartFile*)reqfile)->WritePartStatus(&data_out);
			else
			data_out.WriteUInt16(0);
			}
			data_out.WriteUInt16((uint16)(theApp.uploadqueue->GetWaitingPosition(sender)));
			if (thePrefs.GetDebugClientUDPLevel() > 0)
			DebugSend("OP__ReaskAck", sender);
			Packet* response = new Packet(&data_out, OP_EMULEPROT);
			response->opcode = OP_REASKACK;
			theStats.AddUpDataOverheadFileRequest(response->size);
			SendPacket(response, ip, port, sender->ShouldReceiveCryptUDPPackets(), sender->GetUserHash());
			}
			else
			{
			DebugLogError(_T("Client UDP socket; ReaskFilePing; reqfile does not match"));
			TRACE(_T("reqfile:         %s\n"), DbgGetFileInfo(reqfile->GetFileHash()));
			TRACE(_T("sender->GetRequestFile(): %s\n"), sender->GetRequestFile() ? DbgGetFileInfo(sender->GetRequestFile()->GetFileHash()) : _T("(null)"));
			}
			}
			else
			{
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_ReaskFilePing", NULL, reqfilehash, ip);
			DebugRecv("OP_ReaskFilePing", ip, port);

			// Don't answer him. We probably have him on our queue already, but can't locate him. Force him to establish a TCP connection
			if (!bSenderMultipleIpUnknown){
			if (((uint32)theApp.uploadqueue->GetWaitingUserCount() + 50) > thePrefs.GetQueueSize())
			{
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugSend("OP__QueueFull", NULL);
			DebugSend("OP__QueueFull", 0, 0);
			Packet* response = new Packet(OP_QUEUEFULL,0,OP_EMULEPROT);
			theStats.AddUpDataOverheadFileRequest(response->size);
			SendPacket(response, ip, port, false, NULL); // we cannot answer this one encrypted since we dont know this client
			}
			}
			else{
			//DebugLogWarning(_T("UDP Packet received - multiple clients with the same IP but different UDP port found. Possible UDP Portmapping problem, enforcing TCP connection. IP: %s, Port: %u"), ipstr(ip), port); 
			TRACE(_T("UDP Packet received - multiple clients with the same IP but different UDP port found. Possible UDP Portmapping problem, enforcing TCP connection. IP: %s, Port: %u"), ipstr(ip), port); 
			}
			}
			*/			break;
		}
	case OP_QUEUEFULL:
		{
			DebugRecv("OP_QueueFull", ip, port);
			/*			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = theApp.downloadqueue->GetDownloadClientByIP_UDP(ip, port, true);
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_QueueFull", sender, NULL, ip);
			

			if (sender && sender->UDPPacketPending()){
			sender->SetRemoteQueueFull(true);
			sender->UDPReaskACK(0);
			}
			else if (sender != NULL)
			//DebugLogError(_T("Received UDP Packet (OP_QUEUEFULL) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			TRACE(_T("Received UDP Packet (OP_QUEUEFULL) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			*/			break;
		}
	case OP_REASKACK:
		{
			DebugRecv("OP_ReaskAck", ip, port);
			/*			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = theApp.downloadqueue->GetDownloadClientByIP_UDP(ip, port, true);
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_ReaskAck", sender, NULL, ip);
			
			if (sender && sender->UDPPacketPending()){
			CSafeMemFile data_in(packet, size);
			if ( sender->GetUDPVersion() > 3 )
			{
			sender->ProcessFileStatus(true, &data_in, sender->GetRequestFile());
			}
			uint16 nRank = data_in.ReadUInt16();
			sender->SetRemoteQueueFull(false);
			sender->UDPReaskACK(nRank);
			sender->AddAskedCountDown();
			}
			else if (sender != NULL)
			DebugLogError(_T("Received UDP Packet (OP_REASKACK) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			*/
			break;
		}
	case OP_FILENOTFOUND:
		{
			DebugRecv("OP_FileNotFound", ip, port);
			/*			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = theApp.downloadqueue->GetDownloadClientByIP_UDP(ip, port, true);
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_FileNotFound", sender, NULL, ip);			
			if (sender && sender->UDPPacketPending()){
			sender->UDPReaskFNF(); // may delete 'sender'!
			sender = NULL;
			}
			else if (sender != NULL)
			//DebugLogError(_T("Received UDP Packet (OP_FILENOTFOUND) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			TRACE(_T("Received UDP Packet (OP_FILENOTFOUND) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			*/
			break;
		}
	case OP_PORTTEST:
		{
			//if (thePrefs.GetDebugClientUDPLevel() > 0)
			//	DebugRecv("OP_PortTest", NULL, NULL, ip);
			DebugRecv("OP_PortTest", ip, port);
			/*
			theStats.AddDownDataOverheadOther(size);
			if (size == 1){
			if (packet[0] == 0x12){
			bool ret = theApp.listensocket->SendPortTestReply('1', true);
			AddDebugLogLine(true, _T("UDP Portcheck packet arrived - ACK sent back (status=%i)"), ret);
			}
			}
			*/			break;
		}
	default:
		TRACE(_T("Unknown client UDP packet\n"));
		/*			theStats.AddDownDataOverheadOther(size);
		if (thePrefs.GetDebugClientUDPLevel() > 0)
		{
		CUpDownClient* sender = theApp.downloadqueue->GetDownloadClientByIP_UDP(ip, port, true);
		//Debug(_T("Unknown client UDP packet: host=%s:%u (%s) opcode=0x%02x  size=%u\n"), ipstr(ip), port, sender ? sender->DbgGetClientInfo() : _T(""), opcode, size);
		TRACE(_T("Unknown client UDP packet: host=%s:%u (%s) opcode=0x%02x  size=%u\n"), ipstr(ip), port, sender ? sender->DbgGetClientInfo() : _T(""), opcode, size);
		}
		*/			return false;
	}
	return true;
}

void CClientUDPSocket::OnSend(int nErrorCode)
{
	//	if(theApp.m_app_state != APP_STATE_RUNNING) return;
	if (nErrorCode){
		//		if (thePrefs.GetVerbose())
		//DebugLogError(_T("Error: Client UDP socket, error on send event: %s"), GetErrorMessage(nErrorCode, 1));
		TRACE(_T("Error: Client UDP socket, error on send event: %d"), nErrorCode);
		return;
	}

	// ZZ:UploadBandWithThrottler (UDP) -->
	sendLocker.Lock();
	m_bWouldBlock = false;

	//		if(!controlpacket_queue.IsEmpty()) {
	//        theApp.uploadBandwidthThrottler->QueueForSendingControlPacket(this);
	//    }

	sendLocker.Unlock();
	// <-- ZZ:UploadBandWithThrottler (UDP)
}

//SocketSentBytes CClientUDPSocket::SendControlData(uint32 maxNumberOfBytesToSend, uint32 /*minFragSize*/){ // ZZ:UploadBandWithThrottler (UDP)
/*// ZZ:UploadBandWithThrottler (UDP) -->
// NOTE: *** This function is invoked from a *different* thread!
sendLocker.Lock();

uint32 sentBytes = 0;
// <-- ZZ:UploadBandWithThrottler (UDP)

while (!controlpacket_queue.IsEmpty() && !IsBusy() && sentBytes < maxNumberOfBytesToSend){ // ZZ:UploadBandWithThrottler (UDP)
UDPPack* cur_packet = controlpacket_queue.GetHead();

if( GetTickCount() - cur_packet->dwTime < UDPMAXQUEUETIME )
{
uint32 nLen = cur_packet->packet->size+2;
uchar* sendbuffer = new uchar[nLen];
memcpy(sendbuffer,cur_packet->packet->GetUDPHeader(),2);
memcpy(sendbuffer+2,cur_packet->packet->pBuffer,cur_packet->packet->size);

if (cur_packet->bEncrypt && theApp.GetPublicIP() > 0){
nLen = EncryptSendClient(&sendbuffer, nLen, cur_packet->achTargetClientHash);
DEBUG_ONLY(  DebugLog(_T("Sent obfuscated UDP packet to clientIP: %s"), ipstr( cur_packet->dwIP)) );
}

if (!SendTo((char*)sendbuffer, nLen, cur_packet->dwIP, cur_packet->nPort)){
sentBytes += nLen; // ZZ:UploadBandWithThrottler (UDP)

controlpacket_queue.RemoveHead();
delete cur_packet->packet;
delete cur_packet;
}
delete[] sendbuffer;
}
else
{
controlpacket_queue.RemoveHead();
delete cur_packet->packet;
delete cur_packet;
}
}

// ZZ:UploadBandWithThrottler (UDP) -->
if(!IsBusy() && !controlpacket_queue.IsEmpty()) {
theApp.uploadBandwidthThrottler->QueueForSendingControlPacket(this);
}
sendLocker.Unlock();

SocketSentBytes returnVal = { true, 0, sentBytes };
return returnVal;
// <-- ZZ:UploadBandWithThrottler (UDP)
}
*/
int CClientUDPSocket::SendTo(char* lpBuf,int nBufLen,uint32 dwIP, uint16 nPort){
	// NOTE: *** This function is invoked from a *different* thread!
	const BYTE* pucIP = (BYTE*)&dwIP;
	CString strIP;
	strIP.ReleaseBuffer(_stprintf(strIP.GetBuffer(3+1+3+1+3+1+3), _T("%u.%u.%u.%u"), pucIP[0], pucIP[1], pucIP[2], pucIP[3]));

	uint32 result = CAsyncSocket::SendTo(lpBuf,nBufLen,nPort,strIP);//ipstr(dwIP));
	if (result == (uint32)SOCKET_ERROR){
		uint32 error = GetLastError();
		if (error == WSAEWOULDBLOCK){
			m_bWouldBlock = true;
			return -1;
		}
		//if (thePrefs.GetVerbose())
		//DebugLogError(_T("Error: Client UDP socket, failed to send data to %s:%u: %s"), ipstr(dwIP), nPort, GetErrorMessage(error, 1));
		TRACE(_T("Error: Client UDP socket, failed to send data to %d:%u: %d\n"), dwIP, nPort, error);
	}
	return 0;
}

bool CClientUDPSocket::SendPacket(Packet* packet, uint32 dwIP, uint16 nPort, bool bEncrypt, const uchar* pachTargetClientHash)
{
	//-------------------------------------------------------------

	uint32 nLen = packet->size+2;
	uchar* sendbuffer = new uchar[nLen];
	memcpy(sendbuffer, packet->GetUDPHeader(),2);
	memcpy(sendbuffer+2, packet->pBuffer, packet->size);

	if (bEncrypt)
	{
		//nLen = EncryptSendClient(&sendbuffer, nLen, pachTargetClientHash);
		TRACE(_T("Sent Encrypt UDP packet to client"));
	}

	if (!SendTo((char*)sendbuffer, nLen, dwIP, nPort))
	{
		delete packet;
	}

	delete[] sendbuffer;

	return true;
	//-------------------------------------------------------------
}

bool CClientUDPSocket::Create()
{
	//DWORD dwId;

	//CreateThread(NULL, 0, PrivateThread, this, 0, &dwId);
	//while(!m_nCreateState) Sleep(100);
	//
	//return m_nCreateState>0;
	return PrivateCreate();
}

bool CClientUDPSocket::Rebind()
{
//	if (thePrefs.GetUDPPort() == m_port)
//		return false;
	Close();
	return Create();
}

bool CClientUDPSocket::PrivateCreate()
{
	bool ret = true;

	//if (thePrefs.GetUDPPort())
	if (Kademlia::CKademlia::GetPrefs()->GetUdpPort())
	{
		ret = CAsyncSocket::Create(Kademlia::CKademlia::GetPrefs()->GetUdpPort(),
			SOCK_DGRAM,
			FD_READ | FD_WRITE, 
			Kademlia::CKademlia::GetPrefs()->GetMyselfIp()/*thePrefs.GetBindAddrW()*/	) != FALSE;	//Changed by thilon on 2006.10.19
		if (ret)
			//m_port = thePrefs.GetUDPPort();
			m_port = Kademlia::CKademlia::GetPrefs()->GetUdpPort();
	}

	if (ret)
	{
		//m_port = thePrefs.GetUDPPort();
		m_port = Kademlia::CKademlia::GetPrefs()->GetUdpPort();
	}

	//	m_nCreateState = ret? 1 : -1;
	return ret;

}

//DWORD WINAPI CClientUDPSocket::PrivateThread(LPVOID param)
//{
//#ifndef _AFXDLL 
//#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE 
//#define _afxSockThreadState AfxGetModuleThreadState() 
//	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState; 
//	if (pState->m_pmapSocketHandle == NULL) 
//		pState->m_pmapSocketHandle = new CMapPtrToPtr; 
//	if (pState->m_pmapDeadSockets == NULL) 
//		pState->m_pmapDeadSockets = new CMapPtrToPtr; 
//	if (pState->m_plistSocketNotifications == NULL) 
//		pState->m_plistSocketNotifications = new CPtrList; 
//#endif
//	CClientUDPSocket * socket=(CClientUDPSocket*)param;
//	socket->PrivateCreate();
//
//	MSG msg;
//	while(GetMessage(&msg, NULL, 0, 0))
//	{
//		DispatchMessage(&msg);
//	}
//	delete theApp.clientudp;		theApp.clientudp = NULL;
//	return 0;
//}
