/*
Copyright (C)2003 Barry Dunne (http://www.emule-project.net)
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// Note To Mods //
/*
Please do not change anything here and release it..
There is going to be a new forum created just for the Kademlia side of the client..
If you feel there is an error or a way to improve something, please
post it in the forum first and let us look at it.. If it is a real improvement,
it will be added to the offical client.. Changing something without knowing
what all it does can cause great harm to the network if released in mass form..
Any mod that changes anything within the Kademlia side will not be allowed to advertise
there client on the eMule forum..
*/

#pragma once

#include "../routing/Maps.h"
#include "../io/BufferedFileIO.h"


//class CSafeMemFile;
struct SSearchTerm;

namespace Kademlia
{
	struct TrackPackets_Struct{
		uint32 dwIP;
		uint32 dwInserted;
		uint8  byOpcode;
	};

	class CKademliaUDPListener
	{
			friend class CSearch;
		public:
			void Bootstrap(LPCTSTR uIP, uint16 uUDPPort, bool bKad2);
			void Bootstrap(uint32 uIP, uint16 uUDPPort, bool bKad2);
			void FirewalledCheck(uint32 uIP, uint16 uUDPPort);
			void SendMyDetails(byte byOpcode, uint32 uIP, uint16 uUDPPort, bool bKad2);
			void SendPublishSourcePacket(CContact* pContact, const CUInt128& uTargetID, const CUInt128& uContactID, const TagList& tags);
			void SendNullPacket(byte byOpcode, uint32 uIP, uint16 uUDPPort);
			virtual void ProcessPacket(const byte* pbyData, uint32 uLenData, uint32 uIP, uint16 uUDPPort);
			void SendPacket(const byte* pbyData, uint32 uLenData, uint32 uDestinationHost, uint16 uDestinationPort);
			void SendPacket(const byte *pbyData, uint32 uLenData, byte byOpcode, uint32 uDestinationHost, uint16 uDestinationPort);
			void SendPacket(CSafeMemFile* pfileData, byte byOpcode, uint32 uDestinationHost, uint16 uDestinationPort);
		private:
			void AddContact (const byte* pbyData, uint32 uLenData, uint32 uIP, uint16 uUDPPort, uint16 uTCPPort, bool bUpdate);
			void AddContact_KADEMLIA2 (const byte* pbyData, uint32 uLenData, uint32 uIP, uint16 uUDPPort, bool bUpdate);
			void AddContacts(const byte* pbyData, uint32 uLenData, uint16 uNumContacts, bool bUpdate);
			static SSearchTerm* CreateSearchExpressionTree(CSafeMemFile& fileIO, int iLevel);
			static void Free(SSearchTerm* pSearchTerms);
			void Process_KADEMLIA_BOOTSTRAP_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_BOOTSTRAP_REQ (uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_BOOTSTRAP_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA2_BOOTSTRAP_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_HELLO_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_HELLO_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_HELLO_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_HELLO_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_SEARCH_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_SEARCH_KEY_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_SEARCH_SOURCE_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_SEARCH_RES (const byte* pbyPacketData, uint32 uLenPacket);
			void Process_KADEMLIA2_SEARCH_RES (const byte* pbyPacketData, uint32 uLenPacket);
			void Process_KADEMLIA_PUBLISH_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_PUBLISH_KEY_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_PUBLISH_SOURCE_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_PUBLISH_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA2_PUBLISH_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA_SEARCH_NOTES_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_SEARCH_NOTES_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_SEARCH_NOTES_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA_PUBLISH_NOTES_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA2_PUBLISH_NOTES_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_PUBLISH_NOTES_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA_FIREWALLED_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_FIREWALLED_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);
			void Process_KADEMLIA_FIREWALLED_ACK_RES (uint32 uLenPacket);
			void Process_KADEMLIA_FINDBUDDY_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_FINDBUDDY_RES (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP, uint16 uUDPPort);
			void Process_KADEMLIA_CALLBACK_REQ (const byte* pbyPacketData, uint32 uLenPacket, uint32 uIP);

			void AddTrackedPacket(uint32 dwIP, uint8 byOpcode);
			bool IsOnTrackList(uint32 dwIP, uint8 byOpcode, bool bDontRemove = false);
			CList<TrackPackets_Struct> listTrackedRequests;
	};
}
