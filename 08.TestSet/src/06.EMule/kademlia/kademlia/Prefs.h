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

//------------------------------> xt
#include "../types.h"
#include "../io/BufferedFileIO.h"
#include "../utils/UInt128.h"
//------------------------------< xt


namespace Kademlia
{
	class CPrefs
	{
		public:
			CPrefs();
			~CPrefs();

			//-----------------------------------

			uint16 GetTcpPort(void) {return m_uTcpPort;}
			void SetTcpPort(uint16 uPort) { m_uTcpPort = uPort;}

			uint16 GetUdpPort(void) {return m_uUdpPort;}
			void SetUdpPort(uint16 uPort) { m_uUdpPort = uPort;}

			CString GetMyselfIp(void){ return m_strMyselfIp; }
			void SetMyselfIp(CString strIp){ m_strMyselfIp = strIp;	}

			//-----------------------------------

			void GetKadID(CUInt128 *puID) const;
			void GetKadID(CString *psID) const;
			void SetKadID(const CUInt128 &puID);
			CUInt128 GetKadID() const;
			void GetClientHash(CUInt128 *puID) const;
			void GetClientHash(CString *psID) const;
			void SetClientHash(const CUInt128 &puID);
			CUInt128 GetClientHash() const;
			uint32 GetIPAddress() const;
			void SetIPAddress(uint32 uVal);
			bool GetRecheckIP() const;
			void SetRecheckIP();
			void IncRecheckIP();
			bool HasHadContact() const;
			void SetLastContact();
			bool HasLostConnection() const;
			uint32 GetLastContact() const;
			bool GetFirewalled() const;
			void SetFirewalled();
			void IncFirewalled();
			uint8 GetTotalFile() const;
			void SetTotalFile(uint8 uVal);
			uint8 GetTotalStoreSrc() const;
			void SetTotalStoreSrc(uint8 uVal);
			uint8 GetTotalStoreKey() const;
			void SetTotalStoreKey(uint8 uVal);
			uint8 GetTotalSource() const;
			void SetTotalSource(uint8 uVal);
			uint8 GetTotalNotes() const;
			void SetTotalNotes(uint8 uVal);
			uint8 GetTotalStoreNotes() const;
			void SetTotalStoreNotes(uint8 uVal);
			uint32 GetKademliaUsers() const;
			void SetKademliaUsers(uint32 uVal);
			uint32 GetKademliaFiles() const;
			void SetKademliaFiles();
			bool GetPublish() const;
			void SetPublish(bool bVal);
			bool GetFindBuddy();
			void SetFindBuddy(bool bVal = true);
		private:
			void Init(LPCTSTR szFilename);
			void Reset();
			void SetDefaults();
			void ReadFile();
			void WriteFile();			
			CString	m_sFilename;
			time_t m_tLastContact;
			CUInt128 m_uClientID;
			CUInt128 m_uClientHash;
			uint32 m_uIP;
			uint32 m_uIPLast;
			uint32 m_uRecheckip;
			uint32 m_uFirewalled;
			uint32 m_uKademliaUsers;
			uint32 m_uKademliaFiles;
			uint8 m_uTotalFile;
			uint8 m_uTotalStoreSrc;
			uint8 m_uTotalStoreKey;
			uint8 m_uTotalSource;
			uint8 m_uTotalNotes;
			uint8 m_uTotalStoreNotes;
			bool m_bPublish;
			bool m_bFindBuddy;
			bool m_bLastFirewallState;

			//------------------------------------->
			uint8  m_uVersion;
			CString m_strMyselfIp;
			uint16 m_uTcpPort;
			uint16 m_uUdpPort;
			//-------------------------------------<
	};
}





