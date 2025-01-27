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
 
 
This work is based on the java implementation of the Kademlia protocol.
Kademlia: Peer-to-peer routing based on the XOR metric
Copyright (C) 2002  Petar Maymounkov [petar@post.harvard.edu]
http://kademlia.scs.cs.nyu.edu
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

#include "stdafx.h"
#include "./Contact.h"
#include "../kademlia/Prefs.h"
#include "../kademlia/Kademlia.h"
//#include "../utils/MiscUtils.h"
#include "../OpCodes.h"				//#include "../../OpCodes.h"
//#include "../../emule.h"
//#include "../../emuledlg.h"
//#include "../../kademliawnd.h"

//#include "../../IP2Country.h"
//extern CIP2Country g_IP2Country;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Kademlia;

CContact::~CContact()
{
	if (m_bGuiRefs)
	{
		//theApp.emuledlg->kademliawnd->ContactRem(this);
		CKademlia::GetListCtrl()->ContactRem(this);
		CKademlia::GetHistogramCtrl()->ContactRem(this);
	}
}

CContact::CContact()
{
	m_uClientID = 0;
	m_uIp = 0;
	m_uUdpPort = 0;
	m_uTcpPort = 0;
	m_uVersion = 0;
	InitContact();
}

CContact::CContact(const CUInt128 &uClientID, uint32 uIp, uint16 uUdpPort, uint16 uTcpPort, uint8 uVersion)
{
	m_uClientID = uClientID;
	CKademlia::GetPrefs()->GetKadID(&m_uDistance);
	m_uDistance.Xor(uClientID);
	m_uIp = uIp;
	m_uUdpPort = uUdpPort;
	m_uTcpPort = uTcpPort;
	m_uVersion = uVersion;
	InitContact();
}

CContact::CContact(const CUInt128 &uClientID, uint32 uIp, uint16 uUdpPort, uint16 uTcpPort, const CUInt128 &uTarget, uint8 uVersion)
{
	m_uClientID = uClientID;
	m_uDistance.SetValue(uTarget);
	m_uDistance.Xor(uClientID);
	m_uIp = uIp;
	m_uUdpPort = uUdpPort;
	m_uTcpPort = uTcpPort;
	m_uVersion = uVersion;
	InitContact();
}

void CContact::InitContact()
{
	m_byType = 3;
	m_tExpires = 0;
	m_tLastTypeSet = time(NULL);
	m_bGuiRefs = 0;
	m_uInUse = 0;
	m_tCreated = time(NULL);
	m_bCheckKad2 = true;

	//-------------------------------

	try
	{
		Kademlia::CKademlia::GetIP2Country()->GetCountryFromIP(m_uIp, m_wCountryFlag, m_strCountryName);
	}
	catch(...)
	{
		TRACE(_T("m_pIP2Country->GetCountryFromIP %d"), ::GetLastError());
	}

}

void CContact::GetClientID(CUInt128 *puId) const
{
	puId->SetValue(m_uClientID);
}

void CContact::GetClientID(CString *psId) const
{
	m_uClientID.ToHexString(psId);
}

void CContact::SetClientID(const CUInt128 &uClientID)
{
	m_uClientID = uClientID;
	CKademlia::GetPrefs()->GetKadID(&m_uDistance);
	m_uDistance.Xor(uClientID);
}

void CContact::GetDistance(CUInt128 *puDistance) const
{
	puDistance->SetValue(m_uDistance);
}

void CContact::GetDistance(CString *psDistance) const
{
	m_uDistance.ToBinaryString(psDistance);
}

//---------------------------------------------------->xt
void CContact::GetHexDistance(CString *psDistance) const
{
	m_uDistance.ToHexString(psDistance);
}
//----------------------------------------------------<

CUInt128 CContact::GetDistance() const
{
	return m_uDistance;
}

uint32 CContact::GetIPAddress() const
{
	return m_uIp;
}

void CContact::GetIPAddress(CString *psIp) const
{
	//CMiscUtils::IPAddressToString(m_uIp, psIp);
	psIp->Format(_T("%ld.%ld.%ld.%ld"),
	                ((m_uIp >> 24) & 0xFF),
	                ((m_uIp >> 16) & 0xFF),
	                ((m_uIp >>  8) & 0xFF),
	                ((m_uIp      ) & 0xFF) );
}

void CContact::SetIPAddress(uint32 uIp)
{
	m_uIp = uIp;
}

uint16 CContact::GetTCPPort() const
{
	return m_uTcpPort;
}

void CContact::GetTCPPort(CString *psPort) const
{
	psPort->Format(_T("%ld"), m_uTcpPort);
}

void CContact::SetTCPPort(uint16 uPort)
{
	m_uTcpPort = uPort;
}

uint16 CContact::GetUDPPort() const
{
	return m_uUdpPort;
}

void CContact::GetUDPPort(CString *psPort) const
{
	psPort->Format(_T("%ld"), m_uUdpPort);
}

void CContact::SetUDPPort(uint16 uPort)
{
	m_uUdpPort = uPort;
}

byte CContact::GetType() const
{
	return m_byType;
}

void CContact::CheckingType()
{
	if(time(NULL) - m_tLastTypeSet < 10 || m_byType == 4)
		return;

	m_tLastTypeSet = time(NULL);

	m_tExpires = time(NULL) + MIN2S(2);
	m_byType++;
	
	//------------------------------> xt
	//theApp.emuledlg->kademliawnd->ContactRef(this);
	CKademlia::GetListCtrl()->ContactRef(this);
	//------------------------------< xt
}

void CContact::UpdateType()
{
#pragma warning(disable : 4244)
	uint32 uHours = (time(NULL)-m_tCreated)/HR2S(1);
#pragma warning(default : 4244)
	switch(uHours)
	{
		case 0:
			m_byType = 2;
			m_tExpires = time(NULL) + HR2S(1);
			break;
		case 1:
			m_byType = 1;
			m_tExpires = (time_t)(time(NULL) + HR2S(1.5));
			break;
		default:
			m_byType = 0;
			m_tExpires = time(NULL) + HR2S(2);
	}
	//------------------------------> xt
	//theApp.emuledlg->kademliawnd->ContactRef(this);
	CKademlia::GetListCtrl()->ContactRef(this);
	//------------------------------< xt
}

CUInt128 CContact::GetClientID() const
{
	return m_uClientID;
}

bool CContact::GetGuiRefs() const
{
	return m_bGuiRefs;
}

void CContact::SetGuiRefs(bool bRefs)
{
	m_bGuiRefs = bRefs;
}

bool CContact::InUse()
{
	return (m_uInUse>0);
}

void CContact::IncUse()
{
	m_uInUse++;
}

void CContact::DecUse()
{
	if(m_uInUse)
		m_uInUse--;
	else
		ASSERT(0);
}

time_t CContact::GetCreatedTime() const
{
	return m_tCreated;
}

time_t CContact::GetExpireTime() const
{
	return m_tExpires;
}

time_t CContact::GetLastTypeSet() const
{
	return m_tLastTypeSet;
}

uint8 CContact::GetVersion() const
{
	return m_uVersion;
}

void CContact::SetVersion(uint8 uVersion)
{
	m_uVersion = uVersion;
}

bool CContact::CheckIfKad2()
{
	if(m_bCheckKad2)
	{
		m_bCheckKad2 = false;
		return true;
	}
	return false;
}