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

#include "stdafx.h"
#include "./Kademlia.h"
#include "./defines.h"
#include "./Prefs.h"
#include "./Indexed.h"
#include "../net/KademliaUDPListener.h"
#include "../routing/RoutingZone.h"
#include "../opcodes.h"
#include "../StringConversion.h"
//#include "./SearchManager.h"
//#include "../routing/contact.h"
//#include "../../emule.h"
//#include "../../preferences.h"
//#include "../../emuledlg.h"
//#include "../../Log.h"
#include "../MD4.h"
//  added by yunchenn
//#include "../../ClientList.h"
//#include "Search.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Kademlia;

CKademlia	*CKademlia::m_pInstance = NULL;
EventMap	CKademlia::m_mapEvents;
time_t		CKademlia::m_tNextSearchJumpStart;
time_t		CKademlia::m_tNextSelfLookup;
time_t		CKademlia::m_tStatusUpdate;
time_t		CKademlia::m_tBigTimer;
time_t		CKademlia::m_tNextFirewallCheck;
time_t		CKademlia::m_tNextFindBuddy;
time_t		CKademlia::m_tBootstrap;
time_t		CKademlia::m_tConsolidate;
time_t		CKademlia::m_tRecvData=0;
bool		CKademlia::m_bRunning = false;


//----------------------------------------------------->
CString		CKademlia::m_strCofigPath = _T("res\\eMule\\");
//-----------------------------------------------------<

CKademlia::CKademlia()
{}

BOOL CKademlia::Start(CKadContactListCtrl *pListCtrl, 
					  CKadSearchListCtrl *pSearchListCtrl, 
					  CKadContactHistogramCtrl *pHistogramCtrl,
					  CXTIP2Country *pIP2Country,
					  LPCTSTR tszIP,
					  WORD wTcpPort,
					  WORD wUdpPort
					  )
{
	AddDebugLogLine(false, _T("Starting Kademlia"));

	// Create a new default pref object.

	CString strIP(tszIP);
	
	CPrefs *pPrefs = new CPrefs();

	if (NULL != pPrefs)
	{

		//---------------------------------------->
		if (NULL == tszIP || 0 == _tcscmp(_T(""), tszIP))
		{
			strIP = g_ProFile.m_strIP;
		}

		pPrefs->SetMyselfIp(strIP);
		pPrefs->SetTcpPort(wTcpPort);
		pPrefs->SetUdpPort(wUdpPort);

		TRACE(_T("LocalIp:%s UdpPort:%u TcpPort:%u\n"), strIP, pPrefs->GetUdpPort(), pPrefs->GetTcpPort());
		//----------------------------------------<

		return Start(pPrefs, pListCtrl, pSearchListCtrl, pHistogramCtrl, pIP2Country);
	}

	return FALSE;
}

BOOL CKademlia::Start(CPrefs *pPrefs, 
					  CKadContactListCtrl *pListCtrl, 
					  CKadSearchListCtrl *pSearchListCtrl, 
					  CKadContactHistogramCtrl *pHistogramCtrl,
					  CXTIP2Country *pIP2Country)
{
	if (NULL == pListCtrl) return FALSE;

	try
	{
		// If we already have a instance, something is wrong. 
		if (m_pInstance)
		{
			delete pPrefs;
			ASSERT(m_pInstance->m_bRunning);
			ASSERT(m_pInstance->m_pPrefs);
			return FALSE;
		}

		// Make sure a prefs was passed in..
		if (!pPrefs) return FALSE;

		//AddDebugLogLine(false, _T("Starting Kademlia"));

		m_tRecvData = time(NULL);

		// Init jump start timer.
		m_tNextSearchJumpStart = time(NULL);

		m_tNextUpdateWholeKBucket = time(NULL);	//ADDED by VC-fengwen 2007/08/27 : restart后重新更新K桶，以避免upnp成功后restart kad，中断了更新K桶的操作。
		//MODIFIED by VC-fengwen 2007/08/13 <begin> : 引入快速填充K桶机制，FindNodeComplete在Process_UpdateWholeKBucket之后即可。
		// Force a FindNodeComplete within the first 3 minutes.
		//m_tNextSelfLookup = time(NULL) + MIN2S(3);
		m_tNextSelfLookup = time(NULL) /*+ MIN2S(3)*/;
		//MODIFIED by VC-fengwen 2007/08/13 <end> : 引入快速填充K桶机制，FindNodeComplete在Process_UpdateWholeKBucket之后即可。
		// Init status timer.
		m_tStatusUpdate = time(NULL);
		// Init big timer for Zones
		m_tBigTimer = time(NULL);
		// First Firewall check is done on connect, init next check.
		m_tNextFirewallCheck = time(NULL) + (HR2S(1));
		// Find a buddy after the first 5mins of starting the client.
		// We wait just in case it takes a bit for the client to determine firewall status..
		m_tNextFindBuddy = time(NULL) + (MIN2S(5));
		// Init contact consolidate timer;
		m_tConsolidate = time(NULL) + (MIN2S(45));
		// Init bootstrap time.
		m_tBootstrap = 0;
		// Init our random seed.
		srand((UINT)time(NULL));
		// Create our Kad objects.
		m_pInstance = new CKademlia();
		m_pInstance->m_pPrefs = pPrefs;
		m_pInstance->m_pUDPListener = NULL;
		m_pInstance->m_pRoutingZone = NULL;
		m_pInstance->m_pUDPSocket = NULL;
		m_pInstance->m_pListCtrl = pListCtrl;
		m_pInstance->m_pSearchListCtrl = pSearchListCtrl;
		m_pInstance->m_pHistogramCtrl = pHistogramCtrl;
		m_pInstance->m_pIP2Country = pIP2Country;
		m_pInstance->m_pIndexed = new CIndexed();
		m_pInstance->m_pRoutingZone = new CRoutingZone();
		m_pInstance->m_pUDPListener = new CKademliaUDPListener();
		m_pInstance->m_pUDPSocket = new CClientUDPSocket();
		if (!m_pInstance->m_pUDPSocket->Create())
		{
			AfxMessageBox(_T("UDP Port was used!"));
			
			//exit(0);
			Stop();
			return FALSE;
		}

		if (!InitUnicode(AfxGetInstanceHandle()))
		{
			AfxMessageBox(_T("Init Unicode Faile!"));
			
			//exit(1);
			Stop();
			return FALSE;
		}

		// Mark Kad as running state.
		m_bRunning = true;

		return TRUE;
	}
	catch (CException *e)
	{
		// Although this has never been an issue, maybe some just in case code
		// needs to be created here just in case things go real bad.. But if things
		// went real bad, the entire client most like is in bad shape, so this may
		// not be something to worry about as the client most likely will crap out anyway.
		TCHAR err[512];
		e->GetErrorMessage(err, 512);
		AddDebugLogLine( false, _T("Start Kademlia Error %s"), err);
		e->Delete();
	}

	return FALSE;
}

void CKademlia::Stop()
{
	// Make sure we are running to begin with.
	//if( !m_bRunning )
	//	return;

	AddDebugLogLine(false, _T("Stopping Kademlia"));

	// Mark Kad as being in the stop state to make sure nothing else is used.
	m_bRunning = false;

	// Remove all active searches.
	CSearchManager::StopAllSearches();

	// Delete all Kad Objects.
	if (NULL != m_pInstance->m_pUDPListener)
	delete m_pInstance->m_pUDPListener;
	m_pInstance->m_pUDPListener = NULL;

	if (NULL != m_pInstance->m_pRoutingZone)
	delete m_pInstance->m_pRoutingZone;
	m_pInstance->m_pRoutingZone = NULL;

	if (NULL != m_pInstance->m_pIndexed)
	delete m_pInstance->m_pIndexed;
	m_pInstance->m_pIndexed = NULL;

	if (NULL != m_pInstance->m_pPrefs)
	delete m_pInstance->m_pPrefs;
	m_pInstance->m_pPrefs = NULL;

	if (NULL != m_pInstance->m_pUDPSocket)
	m_pInstance->m_pUDPSocket->Close();
	delete m_pInstance->m_pUDPSocket;
	m_pInstance->m_pUDPSocket = NULL;

	if (NULL != m_pInstance)
	delete m_pInstance;
	m_pInstance = NULL;

	// Make sure all zones are removed.
	m_mapEvents.clear();
}

void CKademlia::Process()
{
	if( m_pInstance == NULL || !m_bRunning)
		return;
	bool bUpdateUserFile = false;
	uint32 uMaxUsers = 0;
	uint32 uTempUsers = 0;
	uint32 uLastContact = 0;
	time_t tNow = time(NULL);
	ASSERT(m_pInstance->m_pPrefs != NULL);
	uLastContact = m_pInstance->m_pPrefs->GetLastContact();
	CSearchManager::UpdateStats();
	if( m_tStatusUpdate <= tNow )
	{
		bUpdateUserFile = true;
		m_tStatusUpdate = MIN2S(1) + tNow;
	}
	if( m_tNextFirewallCheck <= tNow)
		RecheckFirewalled();

	//ADDED by VC-fengwen 2007/08/08 <begin> :
	// Process_UpdateWholeKBucket 应放在Find self 之前，因为Find self 会带上NodeComplete标记以表示节点更新完毕。
	Process_UpdateWholeKBucket();
	//ADDED by VC-fengwen 2007/08/08 <end> :
	
	if (m_tNextSelfLookup <= tNow)
	{
		CSearchManager::FindNode(m_pInstance->m_pPrefs->GetKadID(), true);
		m_tNextSelfLookup = HR2S(4) + tNow;
	}
	if (m_tNextFindBuddy <= tNow)
	{
		m_pInstance->m_pPrefs->SetFindBuddy();
		m_tNextFindBuddy = MIN2S(5) + m_tNextFirewallCheck;
	}

	for (EventMap::const_iterator itEventMap = m_mapEvents.begin(); itEventMap != m_mapEvents.end(); ++itEventMap)
	{
		CRoutingZone* pZone = itEventMap->first;
		if( bUpdateUserFile )
		{
			uTempUsers = pZone->EstimateCount();
			if( uMaxUsers < uTempUsers )
				uMaxUsers = uTempUsers;
		}
		if (m_tBigTimer <= tNow)
		{
			if( pZone->m_tNextBigTimer <= tNow )
			{
				if(pZone->OnBigTimer())
				{
					pZone->m_tNextBigTimer = HR2S(1) + tNow;
					m_tBigTimer = SEC(10) + tNow;
				}
			}
			else
			{
				if( uLastContact && ( (tNow - uLastContact) > (KADEMLIADISCONNECTDELAY-MIN2S(5))))
				{
					if(pZone->OnBigTimer())
					{
						pZone->m_tNextBigTimer = HR2S(1) + tNow;
						m_tBigTimer = SEC(10) + tNow;
					}
				}
			}
		}
		if (pZone->m_tNextSmallTimer <= tNow)
		{
			pZone->OnSmallTimer();
			pZone->m_tNextSmallTimer = MIN2S(1) + tNow;
		}
	}

	// This is a convenient place to add this, although not related to routing
	if (m_tNextSearchJumpStart <= tNow)
	{
		CSearchManager::JumpStart();
		m_tNextSearchJumpStart = SEARCH_JUMPSTART + tNow;
	}

	// Try to consolidate any zones that are close to empty.
	if (m_tConsolidate <= tNow)
	{
		uint32 uMergedCount = m_pInstance->m_pRoutingZone->Consolidate();
		if(uMergedCount)
			AddDebugLogLine(false, _T("Kad merged %u Zones"), uMergedCount);
		m_tConsolidate = MIN2S(45) + tNow;
	}

	//Update user count only if changed.
	if( bUpdateUserFile )
	{
		if( uMaxUsers != m_pInstance->m_pPrefs->GetKademliaUsers())
		{
			m_pInstance->m_pPrefs->SetKademliaUsers(uMaxUsers);
			m_pInstance->m_pPrefs->SetKademliaFiles();
//			theApp.emuledlg->ShowUserCount();
		}
	}
}

void CKademlia::AddEvent(CRoutingZone *pZone)
{
	m_mapEvents[pZone] = pZone;
}

void CKademlia::RemoveEvent(CRoutingZone *pZone)
{
	m_mapEvents.erase(pZone);
}

bool CKademlia::IsConnected()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->HasHadContact();
	return false;
}

bool CKademlia::IsFirewalled()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetFirewalled();
	return true;
}

//  added by yunchenn 2006.12.13
bool CKademlia::StartFindBuddy()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
	{
		//TODO: Kad buddies won'T work with RequireCrypt, so it is disabled for now but should (and will)
		//be fixed in later version

		//  force finding buddy here
		Kademlia::CKademlia::GetPrefs()->SetFindBuddy();

		//----------------------------------> xt

		//if( theApp.clientlist->GetBuddyStatus()== Disconnected && Kademlia::CKademlia::GetPrefs()
		//	&& Kademlia::CKademlia::GetPrefs()->GetFindBuddy() && !thePrefs.IsClientCryptLayerRequired())

//		if( g_clientlist.GetBuddyStatus()== Disconnected && Kademlia::CKademlia::GetPrefs()
//			&& Kademlia::CKademlia::GetPrefs()->GetFindBuddy())

		//----------------------------------< xt

		{
			//We are a firewalled client with no buddy. We have also waited a set time 
			//to try to avoid a false firewalled status.. So lets look for a buddy..
#ifdef _DEBUG_KAD_
			Debug( _T("Now PrepareLookup FINDBUDDY! \r\n") );
#endif 
			if( !Kademlia::CSearchManager::PrepareLookup(Kademlia::CSearch::FINDBUDDY, true, Kademlia::CUInt128(true).Xor(Kademlia::CKademlia::GetPrefs()->GetKadID())) )
			{
				//This search ID was already going. Most likely reason is that
				//we found and lost our buddy very quickly and the last search hadn't
				//had time to be removed yet. Go ahead and set this to happen again
				//next time around.
				Kademlia::CKademlia::GetPrefs()->SetFindBuddy();
				return true;
			}
		}
	}
	return false;
}

uint32 CKademlia::GetKademliaUsers()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetKademliaUsers();
	return 0;
}

uint32 CKademlia::GetKademliaFiles()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetKademliaFiles();
	return 0;
}

uint32 CKademlia::GetTotalStoreKey()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetTotalStoreKey();
	return 0;
}

uint32 CKademlia::GetTotalStoreSrc()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetTotalStoreSrc();
	return 0;
}

uint32 CKademlia::GetTotalStoreNotes()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetTotalStoreNotes();
	return 0;
}

uint32 CKademlia::GetTotalFile()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetTotalFile();
	return 0;
}

uint32 CKademlia::GetIPAddress()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetIPAddress();
	return 0;
}

void CKademlia::ProcessPacket(const byte *pbyData, uint32 uLenData, uint32 uIP, uint16 uPort)
{
	m_tRecvData = time(NULL);

	if( m_pInstance && m_pInstance->m_pUDPListener )
		m_pInstance->m_pUDPListener->ProcessPacket( pbyData, uLenData, uIP, uPort);
}

bool CKademlia::GetPublish()
{
	if( m_pInstance && m_pInstance->m_pPrefs )
		return m_pInstance->m_pPrefs->GetPublish();
	return 0;
}

void CKademlia::Bootstrap(LPCTSTR szHost, uint16 uPort, bool bKad2)
{
	if( m_pInstance && m_pInstance->m_pUDPListener && !IsConnected() && time(NULL) - m_tBootstrap > MIN2S(1) )
		m_pInstance->m_pUDPListener->Bootstrap( szHost, uPort, bKad2 );
}

void CKademlia::Bootstrap(uint32 uIP, uint16 uPort, bool bKad2)
{
	if( m_pInstance && m_pInstance->m_pUDPListener && !IsConnected() && time(NULL) - m_tBootstrap > MIN2S(1) )
		m_pInstance->m_pUDPListener->Bootstrap( uIP, uPort, bKad2 );
}

void CKademlia::RecheckFirewalled()
{
	if( m_pInstance && m_pInstance->GetPrefs() )
	{
		// Something is forcing a new firewall check
		// Stop any new buddy requests, and tell the client
		// to recheck it's IP which in turns rechecks firewall.
		m_pInstance->m_pPrefs->SetFindBuddy(false);
		m_pInstance->m_pPrefs->SetRecheckIP();
		// Always set next buddy check 5 mins after a firewall check.
		m_tNextFindBuddy = MIN2S(5) + m_tNextFirewallCheck;
		m_tNextFirewallCheck = HR2S(1) + time(NULL);
	}
}

CPrefs *CKademlia::GetPrefs()
{
	if (m_pInstance == NULL || m_pInstance->m_pPrefs == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pPrefs;
}

CKademliaUDPListener *CKademlia::GetUDPListener()
{
	if (m_pInstance == NULL || m_pInstance->m_pUDPListener == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pUDPListener;
}

CRoutingZone *CKademlia::GetRoutingZone()
{
	if (m_pInstance == NULL || m_pInstance->m_pRoutingZone == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pRoutingZone;
}

CIndexed *CKademlia::GetIndexed()
{
	if ( m_pInstance == NULL || m_pInstance->m_pIndexed == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pIndexed;
}

//----------------------------------------------------------------------->

CClientUDPSocket *CKademlia::GetUDPSocket()
{
	if (m_pInstance == NULL || m_pInstance->m_pUDPSocket == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pUDPSocket;
}

CKadContactListCtrl *CKademlia::GetListCtrl()
{
	if (m_pInstance == NULL || m_pInstance->m_pListCtrl == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pListCtrl;
}

CKadSearchListCtrl *CKademlia::GetSearchListCtrl()
{
	if (m_pInstance == NULL || m_pInstance->m_pSearchListCtrl == NULL)
	{
		ASSERT(0);
		return NULL;
	}

	return m_pInstance->m_pSearchListCtrl;
}

CKadContactHistogramCtrl *CKademlia::GetHistogramCtrl()
{
	if (m_pInstance == NULL || m_pInstance->m_pHistogramCtrl == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pHistogramCtrl;
}

CXTIP2Country *CKademlia::GetIP2Country()
{
	if (m_pInstance == NULL || m_pInstance->m_pIP2Country == NULL)
	{
		ASSERT(0);
		return NULL;
	}
	return m_pInstance->m_pIP2Country;
}

//-----------------------------------------------------------------------<

bool CKademlia::IsRunning()
{
	return m_bRunning;
}

void KadGetKeywordHash(const CStringA& rstrKeywordA, Kademlia::CUInt128* pKadID)
{
	CMD4 md4;
	md4.Add((byte*)(LPCSTR)rstrKeywordA, rstrKeywordA.GetLength());
	md4.Finish();
	pKadID->SetValueBE(md4.GetHash());

	TRACE(_T("KadGetKeywordHash\n"));
}

CStringA KadGetKeywordBytes(const CStringW& rstrKeywordW)
{
	return CStringA(wc2utf8(rstrKeywordW));
}

void KadGetKeywordHash(const CStringW& rstrKeywordW, Kademlia::CUInt128* pKadID)
{
	KadGetKeywordHash(KadGetKeywordBytes(rstrKeywordW), pKadID);
}

bool CKademlia::LostConnection()
{
	//  根本没启动过Kad，所以也没断开连接的说法
	if(!m_pInstance || !m_pInstance->m_pPrefs)
		return false;

	//  5分钟都没收到任何数据，网络断了，应该重新开始
	if(m_tRecvData && time(NULL)-m_tRecvData > 60 * 5)
		return true;
	return false;
}

time_t CKademlia::m_tNextUpdateWholeKBucket = 0;
void CKademlia::Process_UpdateWholeKBucket()
{
	time_t tNow = time(NULL);

	if (0 == m_tNextUpdateWholeKBucket
		|| tNow > m_tNextUpdateWholeKBucket)
	{
		m_tNextUpdateWholeKBucket = tNow + HR2S(2);

		UpdateWholeKBucket();
	}
}

void CKademlia::UpdateWholeKBucket()
{
	int i;
	for (i = 0; i < (1 << (KBASE)); i++)
	{
		CUInt128	uDistance((ULONG)i);
		uDistance.ShiftLeft(128 - KBASE);
		CUInt128 uRandom(uDistance, 128 - KBASE);
		uRandom.Xor(CKademlia::GetPrefs()->GetKadID());
		CSearchManager::FindNode(uRandom, false);
	}
	for (i = 0; i < (128 - KBASE); i++)
	{
		CUInt128	uDistance((ULONG)1);
		uDistance.ShiftLeft(i);
		CUInt128 uRandom(uDistance, 128 - i);
		uRandom.Xor(CKademlia::GetPrefs()->GetKadID());
		CSearchManager::FindNode(uRandom, false);
	}
}
