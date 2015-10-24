#include "StdAfx.h"
#include ".\srv_nat.h"
#include "Dlg\Dlg_NAT.h"

CSrv_NAT::CSrv_NAT(void)
    :m_lMyPubIp(0),
    m_wMyPubPort(0),
    m_lMyPriIp(0),
    m_wMyPriPort(0),
    m_lServerIp(0),
    m_wServerPort(0),
    m_wMyRoomNum(0),
    m_dwMyId(0),
    m_dwAudioId(0),
    m_dwVideoId(0),
    m_hCommThread(NULL),
    m_hSvrHeartBeatThread(NULL),
    m_pMyUdpCC(NULL),
    m_hLockRoomMap(NULL),
    m_hLockUserMap(NULL),
    m_pDlg(NULL),
    m_pList(NULL)
{
    m_RoomMap.clear();
    m_UserMap.clear();

    InitializeCriticalSection(&m_csCheckNum);
}

CSrv_NAT::~CSrv_NAT(void)
{
    DeleteCriticalSection(&m_csCheckNum);

    CloseHandle(m_hLockRoomMap);
    CloseHandle(m_hLockUserMap);

    DWORD nCode = 0;
    for (int i = 10 ; i > 0 ; i--)
    {		

        if (m_hCommThread != NULL)
        {
            if (!GetExitCodeThread(m_hCommThread, &nCode) || nCode != STILL_ACTIVE)
            {
                CloseHandle(m_hCommThread);
                m_hCommThread = NULL;
            }
        }

        if (m_hSvrHeartBeatThread != NULL)
        {
            if (!GetExitCodeThread(m_hSvrHeartBeatThread, &nCode) || nCode != STILL_ACTIVE)
            {
                CloseHandle(m_hSvrHeartBeatThread);
                m_hSvrHeartBeatThread = NULL;
            }
        }

        Sleep(50);
    }

    if (m_hCommThread != NULL)
    {
        TerminateThread(m_hCommThread, 0);
        g_ProFile.m_Log.println(_T("WARNING: Terminating CSvr_NAT.CommThread."));
        CloseHandle(m_hCommThread);
        m_hCommThread = NULL;
    }

    if (m_hSvrHeartBeatThread != NULL)
    {
        TerminateThread(m_hSvrHeartBeatThread, 0);
        g_ProFile.m_Log.println(_T("WARNING: Terminating CSvr_NAT.SvrHeartBeat."));
        CloseHandle( m_hSvrHeartBeatThread);
        m_hSvrHeartBeatThread = NULL;
    }

    CloseHandle(m_hExit);
    m_hExit = NULL;
}

// bSrv �Ƿ��Ƿ�����
BYTE CSrv_NAT::Init(LPCTSTR pszIp, const WORD wPort, BOOL bSrv, CDlg_NAT *pDlg)
{
    if (NULL == pszIp || NULL == pDlg) return 1;

    CSrv::Init(CMD_MYSELF, 2014 * 2);

    g_ProFile.m_Log.println(_T("NAT::Init IP=%s,Port=%d"), pszIp, wPort);

    m_pDlg = pDlg;
    m_pList = &(pDlg->m_lstUser);
    m_lMyPriIp = g_ProFile.m_dwIP;
    m_wMyPriPort = htons(wPort);


    if (bSvr)
    {
        UINT nThreadId = 0;
        m_hLockRoomMap = CreateEvent(NULL, TRUE, TRUE, NULL);
        m_hSvrHeartBeatThread = (HANDLE)_beginthreadex(NULL, 0, SvrHeartBeatThread, (void*)this, 0, &nThreadId);
    }
    else
    {
        m_hLockUserMap = CreateEvent(NULL, TRUE, TRUE, NULL);
        //UINT nThreadId = 0;
        //m_hCommThread = (HANDLE)_beginthreadex(NULL, 0, CommThread, (void*)this, 0, &nThreadId);
    }

    m_AudioList.Init(&m_BuffPool, 50);
    m_VideoList.Init(&m_BuffPool, 50);
    m_RecvMap.Init(this, pDlg, &m_BuffPool);

    // ��ʹ���ط��ṹ
    if (!m_ResendQueue.Init(this, &m_BuffPool)) return 2;

    if (0 != CSrv::Start()) return 3;

    if (0 != m_IOCP.AddUdpListen(pszIp, wPort, &m_pMyUdpCC)) return 4;

    return 0;
}

// ��������
void CSrv_NAT::Lock(HANDLE hOject)
{
    WaitForSingleObject(hOject, INFINITE);
    ResetEvent(hOject);
}

// ����
void CSrv_NAT::UnLock(HANDLE hOject)
{
    SetEvent(hOject);
}

WORD CSrv_NAT::GetCheckNum(LPBYTE byData, DWORD dwDataLen)
{
    WORD wCheckSum = 0;

    for(DWORD i = 0; i < dwDataLen; i++)
        wCheckSum += (byData[i] << 1);

    return wCheckSum;
}

// �򷿼��ڵ������û�Ⱥ��
bool CSrv_NAT::S_Broadcast(WORD wRoom, WORD wTimes, PPACK pPack)
{
    /*	if (pPack->wCheckSum != GetCheckNum(pPack))
    {
    TRACE("Broadcast check num Error %d\n", pPack->dwId);
    m_BufferPool.Free(pPack);
    return false;
    }
    */
    int nCount = 0;
    USER_MAP::iterator delete_user_iter;

    ROOM_MAP::iterator room_iter = m_RoomMap.find(wRoom);
    if (room_iter != m_RoomMap.end())
    {
        USER_MAP* pUserMap = (USER_MAP*)&((*room_iter).second);
        if (NULL != pUserMap)
        {
            nCount = (int)pUserMap->size();
            if (0 != nCount)
            {
                USER_MAP::iterator user_iter = pUserMap->begin();
                for (; user_iter != pUserMap->end(); ++user_iter)
                {
                    for (WORD i = 0; i < wTimes; i++)
                    {
                        m_QueueOut.AddPack(m_pMyUdpCC, (*user_iter).second.lPublicIp, (*user_iter).second.wPublicPort, pPack->GetPackLen(), pPack);
                    }
                }

                return true;
            }
        }
    }

    m_BuffPool.Free(pPack);
    return false;
}

// ת��ĳ����Ϣ��ĳ���û�
bool CSrv_NAT::S_Transmit(WORD wRoom, DWORD dwUserId, WORD wTimes, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("Transmit check num Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    int nCount = 0;
    USER_MAP::iterator delete_user_iter;

    ROOM_MAP::iterator room_iter = m_RoomMap.find(wRoom);
    if (room_iter != m_RoomMap.end())
    {
        USER_MAP* pUserMap = (USER_MAP*)&((*room_iter).second);
        if (NULL != pUserMap)
        {
            nCount = (int)pUserMap->size();
            if (0 != nCount)
            {
                USER_MAP::iterator user_iter = pUserMap->find(dwUserId);
                if (user_iter != pUserMap->end())
                {

                    for (WORD i = 0; i < wTimes; i++)
                    {
                        m_QueueOut.AddPack(m_pMyUdpCC, (*user_iter).second.lPublicIp, (*user_iter).second.wPublicPort, 
                            pPack->GetPackLen(), pPack);

                        ::Sleep(2);
                    }

                    return true;
                }
            }
        }
    }

    m_BuffPool.Free(pPack);
    return false;
}

// �޸��б�
BOOL CSrv_NAT::ModList(DWORD dwUserId, WORD wCol, LPCTSTR pszInfo)
{
    if (NULL == pszInfo) return false;

    TCHAR szUserId[20] = {_T("")};
    dwUserId = _tcstol(szUserId, NULL, 10);

    LVFINDINFO  info;
    memset(&info, 0, sizeof(info));
    info.flags = LVFI_STRING;
    info.psz   = szUserId;
    int nIndex = m_pList->FindItem(&info);

    if(-1 == nIndex) return false;

    return m_pList->SetItemText(nIndex, wCol, pszInfo);
}

// ����б�
bool CSrv_NAT::AddList(DWORD dwUserId, ULONG lPubIP, WORD wPubPort, ULONG lPriIP, WORD wPriPort, LPCTSTR pszInfo)
{
    if(NULL == pszInfo) return false;

    TCHAR szTemp[50] = {_T("")};
    dwUserId = _tcstol(szTemp, NULL, 10);

    struct in_addr addr = {0};
    LVITEM lvi		={0};
    lvi.mask		= LVIF_TEXT;
    lvi.iItem		= 0;
    lvi.iSubItem	= 0;
    lvi.pszText		= szTemp;
    lvi.cchTextMax	= 30;
    lvi.iImage		= -1;

    LVFINDINFO info = {0};
    info.flags		= LVFI_STRING;
    info.psz		= szTemp;

    int nIndex = m_pList->FindItem(&info);

    if(-1 == nIndex)
        nIndex = m_pList->InsertItem(&lvi);	

    addr.S_un.S_addr = lPubIP;
    m_pList->SetItemText(nIndex, 1, inet_ntoa(addr));

    _itot(ntohs(wPubPort), szTemp, 10);
    m_pList->SetItemText(nIndex, 2, szTemp );

    addr.S_un.S_addr = lPriIP;
    m_pList->SetItemText(nIndex, 3, inet_ntoa(addr));

    _itot(ntohs(wPriPort), szTemp, 10);
    m_pList->SetItemText(nIndex, 4, szTemp);

    m_pList->SetItemText(nIndex, 5, pszInfo);

    return true;
}

// ������,�����߳�(����û��Ƿ�ʱ)
unsigned CSrv_NAT::SvrHeartBeatThread(LPVOID pParam)
{
    if( NULL == pParam ) return -1;

    CSrv_NAT *pThis = reinterpret_cast<CSrv_NAT*>(pParam);
    DWORD dwNowTime = 0;
    std::list<DWORD> DeleteUserIdList;

    try
    {
        while (NULL != pThis->m_hExit)
        {
            if (WaitForSingleObject(pThis->m_hExit, 1000) == WAIT_OBJECT_0) break;			

            pThis->Lock(pThis->m_hLockRoomMap);

            dwNowTime = ::GetTickCount();

            int nCou = (int)pThis->m_RoomMap.size();

            ROOM_MAP::iterator room_iter = pThis->m_RoomMap.begin();
            for (; room_iter != pThis->m_RoomMap.end(); ++room_iter)
            {
                DeleteUserIdList.clear();

                USER_MAP *pUserMap = (USER_MAP*)&((*room_iter).second);
                if (NULL == pUserMap) continue;

                int nCount = (int)pUserMap->size();

                USER_MAP::iterator user_iter = pUserMap->begin();
                for (; user_iter != pUserMap->end(); ++user_iter)
                {
                    PUSER_INFO pUser = (PUSER_INFO)&((*user_iter).second);
                    if (NULL == pUser)
                    {
                        TRACE("PUSER_INFO NULL\n");
                        continue;
                    }

                    DWORD dwtime = dwNowTime - pUser->dwAckTime;

                    if ((dwNowTime - pUser->dwAckTime) > 5*1000)
                    {
                        //DeleteUserIdList.push_back(pUser->dwUserId);

                        //pThis->PS_NotfiyUserOutRoom(pUser->dwUserId, pUser->wRoomNum, pUserMap);

                        g_ProFile.m_Log.println(_T("������ʱ,ID=%d,nowTime;%d,ackTime;%d"), pUser->dwUserId, dwNowTime, pUser->dwAckTime);
                    }
                }

                while (!DeleteUserIdList.empty())			
                {
                    pUserMap->erase((DWORD)DeleteUserIdList.front());
                    DeleteUserIdList.pop_front();
                }
            }

            pThis->UnLock(pThis->m_hLockRoomMap);
        }
    }
    catch(...)
    {
        TRACE("HeartBeatThread Error\n");
        g_ProFile.m_Log.println(_T("HeartBeatThread ErrorCode:%d"), ::GetLastError());
    }
    return 0;
}

// �ͻ���,����-�����߳�
unsigned CSrv_NAT::CommThread(LPVOID pParam)
{
    if( NULL == pParam ) return -1;

    CSrv_NAT* pThis	 = reinterpret_cast<CSrv_NAT*>(pParam);
    int nTimes		 = 0;
    int nToUserTimes = 60;

    PPACK pBeatPack = (PPACK)pThis->m_BuffPool.GetBuff(0xFFFFFFFF);
    pBeatPack->dwFlag	  = PACKFLAG;
    pBeatPack->wProtoNum  = PT_U_S_HEART_ANSWER;
    pBeatPack->dwId       = 1;
    pBeatPack->dwAckId	  = 0;
    pBeatPack->wCheckSum  = 0;
    pBeatPack->dwDataLen  = sizeof(USERID_ROOMNUM);
    PUSERID_ROOMNUM pInfo = (PUSERID_ROOMNUM)&(pBeatPack->szData[0]);
    pInfo->dwUserId		  = pThis->m_dwMyId;
    pInfo->wRoomNum		  = pThis->m_wMyRoomNum;
    pBeatPack->SetCheckNum();

    PPACK pCommPack = (PPACK)pThis->m_BuffPool.GetBuff(0xFFFFFFFF);
    pCommPack->dwFlag	  = PACKFLAG;
    pCommPack->wProtoNum  = PT_U_U_HELLO_REQUEST;
    pCommPack->dwId       = 2;
    pCommPack->dwAckId	  = pThis->m_dwMyId;
    pCommPack->dwDataLen  = 0;
    pCommPack->szData[0]  = 0;
    pCommPack->SetCheckNum();

    PPACK pAckPack  = (PPACK)pThis->m_BuffPool.GetBuff(0xFFFFFFFF);
    pAckPack->dwFlag	  = PACKFLAG;
    pAckPack->wProtoNum	  = PT_ACK_MSG;
    pAckPack->dwId		  = 3;
    pAckPack->dwAckId	  = 0;
    pAckPack->dwDataLen	  = 0;
    pAckPack->szData[0]   = 0;
    pAckPack->SetCheckNum();

    try
    {
        while (NULL != pThis->m_hExit)
        {
            if (WaitForSingleObject(pThis->m_hExit, 1000) == WAIT_OBJECT_0) break;

            //------------------------------------------------------------------------
            // �����������������
            pThis->m_QueueOut.AddPack(pThis->m_pMyUdpCC, pThis->m_lServerIp, pThis->m_wServerPort, pBeatPack->GetPackLen(), pBeatPack);
            //------------------------------------------------------------------------

            pThis->Lock(pThis->m_hLockUserMap);

            USER_MAP::iterator user_iter = pThis->m_UserMap.begin();
            for (; user_iter != pThis->m_UserMap.end(); ++user_iter)
            {
                PUSER_INFO pUser = (PUSER_INFO)&((*user_iter).second);
                if (NULL == pUser) continue;

                if (COMMUNICED != pUser->byCommState && IN_LAN != pUser->byCommState)	// ��ͻ��˷�����������
                {
                    if( UNCOMMUNICATION == pUser->byCommState )	// ĩ����
                    {
                        pUser->byCommState = COMMUNICING;
                        pThis->ModList(pUser->dwUserId, 5, _T("������"));
                    }

                    for (int i = 0; i < 2; i++)
                    {						
                        pThis->m_QueueOut.AddPack(pThis->m_pMyUdpCC, pUser->lPublicIp, pUser->wPublicPort, sizeof(PACK), pCommPack);
                        ::Sleep(2);
                    }
                }
                else if (nTimes == nToUserTimes)	// ���Ѿ�ֱͨ�ĺ;������е��û���һ��
                {
                    for (int i = 0; i < 2; i++)
                    {
                        pThis->m_QueueOut.AddPack(pThis->m_pMyUdpCC, pUser->lPublicIp, pUser->wPublicPort, sizeof(PACK), pAckPack);
                        ::Sleep(2);
                    }
                }
            }

            pThis->UnLock(pThis->m_hLockUserMap);

            if (nTimes++ == nToUserTimes) nTimes=0;
        }
    }
    catch(...)
    {
        TRACE("CommThread Error\n");
        g_ProFile.m_Log.println(_T("CommThread ErrorCode:%d"), ::GetLastError());
    }
    return 0;
}

// ������,����Ӧ��
bool CSrv_NAT::PS_HeartBeatAnswer(PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PS_HeartBeatAnswer checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    PUSERID_ROOMNUM pUserIdRoomNum = (PUSERID_ROOMNUM)&(pPack->szData[0]);

    ROOM_MAP::iterator room_iter = m_RoomMap.find(pUserIdRoomNum->wRoomNum);
    if (room_iter != m_RoomMap.end())
    {
        USER_MAP::iterator user_iter = (*room_iter).second.find(pUserIdRoomNum->dwUserId);
        if (user_iter != (*room_iter).second.end())
        {
            (*user_iter).second.dwAckTime = ::GetTickCount();
        }
    }

    m_BuffPool.Free(pPack);
    return true;
}

// ������,Э�鴦��
void CSrv_NAT::ProcessPack(PCLIENTCONTEXT pClient, DWORD dwIP, WORD wPort, DWORD dwDataLen, PBYTE pbyBuf)
{
    if (NULL == pbyBuf) return;

    PPACK pPack = (PPACK)pbyBuf;

    try
    {
        switch (pPack->wProtoNum)
        {
        case PT_U_S_LOGON_SERVER_REQUEST:	// �ͻ��˵�½������
            {
                PS_UserLogonServer(dwIP, wPort, pPack);
                break;
            }
        case PT_S_U_INTO_ROOM_USER:			// һ���û����뷿��
        case PT_S_U_IN_ROOM_USER:			// ������ԭ�е��û�
            {
                PU_UserIntoRoom(dwIP, wPort, pPack);
                break;
            }
        case PT_U_S_INTO_ROOM_ACK:			// һ���û����뷿����Ϣ��Ӧ��
        case PT_U_S_IN_ROOM_ACK:			// ������ԭ�е��û���Ϣ��Ӧ��
            {
                PS_UserIntoRoomAck(dwIP, wPort, pPack);
                break;
            }
        case PT_U_S_OUT_ROOM_USER:			// �������û�������Ҳ�����Ǽ�鵽û���յ�����ȷ���϶�Ϊ���û����˳�
            {
                PS_NotfiyUserOutRoom(pPack);
                break;
            }
        case PT_S_U_OUT_ROOM_USER:			// �ͻ����յ��û��˳���Ϣ��ɾ���û����������ȷ��
            {
                PU_UserOutRoom(dwIP, wPort, pPack);
                break;
            }
        case PT_U_U_HELLO_REQUEST:			// һ�ͻ�������һ�ͻ����ʹ���������Ϣ
            {
                PU_HelloRequest(dwIP, wPort, pPack);
                break;
            }
        case PT_U_U_HELLO_ANSWER:			// ���ͻ��յ�����Ϣʱ˵���Ѿ������ɹ���
            {
                PU_HelloAnswer(dwIP, wPort, pPack);
                break;
            }
        case PT_ACK_MSG:
            {
                m_BuffPool.Free((LPBYTE)pPack);
                break;
            }
        case PT_U_S_HEART_ANSWER:			// �������յ�����
            {
                PS_HeartBeatAnswer(pPack);
                break;
            }
        case PT_U_U_TEXT:					// �ͻ���֮��
            {
                PSU_ShowText(dwIP, wPort, pPack);
                break;
            }
        case PT_U_S_AUDIO:					// �������յ���Ƶ����ʼ�򷿼��ڵ��˷ַ�
            {
                PS_BroadcastAudio(pPack);
                break;
            }
        case PT_U_S_VIDEO:					// �������յ���Ƶ����ʼ�򷿼��ڵ��˷ַ�
            {
                PS_BroadcastVideo(pPack);
                break;
            }
        case PT_S_U_VIDEO:					// �ͻ����յ���Ƶ
        case PT_U_U_VIDEO:
            {
                m_RecvMap.AddVideoData(pPack);
                break;
            }
        case PT_S_U_AUDIO:					// �ͻ����յ���Ƶ
        case PT_U_U_AUDIO:
            {
                m_RecvMap.AddAudioData(pPack);
                break;
            }
        case PT_U_S_RESEND:					// ͨ��������ת��
            {
                PS_Resend(pPack);
                break;
            }
        case PT_S_U_RESEND:					// �����û�����ô�ĳ��Ϣ
        case PT_U_U_RESEND:
            {
                PU_Resend(pPack);
                break;
            }
        default:
            {
                m_BuffPool.Free(pPack);
                TRACE("error code\n");
                break;
            }
        }
    }
    catch(...)
    {
        g_ProFile.m_Log.println(_T("CSvr_NAT::ProcessPack ErrorCode:%d"), ::GetLastError());
    }
}

// �ͻ���,��½������
bool CSrv_NAT::U_LogonServer(LPCTSTR pszIp, WORD wPort, UINT nUserId, WORD wRoomNum)
{
    ////////////////////////////////////////////////////
    m_dwMyId = nUserId = ::GetTickCount();
    ////////////////////////////////////////////////////

    m_lServerIp = inet_addr(pszIp);
    m_wServerPort = htons(wPort);
    m_wMyRoomNum = wRoomNum;

    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->wProtoNum = PT_U_S_LOGON_SERVER_REQUEST;
    pPack->dwId		 = m_PackId.GetPackId();
    pPack->dwAckId	 = 0;	
    pPack->dwDataLen = sizeof(LOGON_INFO);
    LOGON_INFO *pLogonInfo	 = (LOGON_INFO*)&(pPack->szData[0]);
    pLogonInfo->dwUserId	 = nUserId;
    pLogonInfo->wRoomNum	 = wRoomNum;
    pLogonInfo->lPrivateIp	 = m_lMyPriIp;
    pLogonInfo->wPrivatePort = m_wMyPriPort;
    pPack->SetCheckNum();

    m_QueueOut.AddPack(m_pMyUdpCC, m_lServerIp, m_wServerPort, pPack->GetPackLen(), pPack);
    m_ResendQueue.AddPack(nUserId, m_dwMyId, m_pMyUdpCC, m_lServerIp, m_wServerPort, pPack);

    return true;
}

// ������,�����û���½
bool CSrv_NAT::PS_UserLogonServer(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PS_UserLogonServer checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    LOGON_INFO *pInUserInfo  = (LOGON_INFO*)&(pPack->szData[0]);

    // ׼�����뷿��Map�ڵ�����
    USER_INFO InUserInfo    = {0};
    InUserInfo.lPublicIp	= dwIP;
    InUserInfo.wPublicPort	= wPort;
    InUserInfo.lPrivateIp	= pInUserInfo->lPrivateIp;
    InUserInfo.wPrivatePort	= pInUserInfo->wPrivatePort;
    InUserInfo.dwUserId		= pInUserInfo->dwUserId;
    InUserInfo.wRoomNum		= pInUserInfo->wRoomNum;
    InUserInfo.dwAckTime  	= ::GetTickCount();
    InUserInfo.byCommState	= UNCOMMUNICATION;

    DWORD dwAckMsgId = pPack->dwId;

    // ���û�������
    ROOM_MAP::iterator room_iter = m_RoomMap.find(InUserInfo.wRoomNum);
    if (m_RoomMap.end() == room_iter)
    {
        USER_MAP UserMap;
        UserMap.clear();
        m_RoomMap.insert(ROOM_MAP::value_type(InUserInfo.wRoomNum, UserMap));

        room_iter = m_RoomMap.find(InUserInfo.wRoomNum);
    }

    if (m_RoomMap.end() != room_iter)
    {
        USER_MAP *pUserMapInRoom = (USER_MAP*)&((*room_iter).second);
        if(NULL != pUserMapInRoom)
        {
            AddList(InUserInfo.dwUserId, dwIP, wPort, InUserInfo.lPrivateIp, InUserInfo.wPrivatePort, _T(""));

            USER_MAP::iterator user_iter= pUserMapInRoom->find(InUserInfo.dwUserId);
            if(user_iter != pUserMapInRoom->end())
            {
                (*user_iter).second = InUserInfo;	// ��������

                m_BuffPool.Free(pPack);
                return false;
            }
            else
            {
                USER_MAP_INSERT pr = pUserMapInRoom->insert(USER_MAP::value_type(InUserInfo.dwUserId, InUserInfo));
                if (!pr.second)
                {
                    g_ProFile.m_Log.println(_T("���벻�ɹ����û�ID:%d�������:%d"), InUserInfo.dwUserId, InUserInfo.wRoomNum);
                }
            }

            // �����û���Ϣ
            int nUserCountInRoom		= (int)pUserMapInRoom->size();		
            int nSendUserInfoCountOnce	= 1024 / sizeof(USER_INFO);// һ����ֻ�ܷ��Ͳ�����1024���ֽ�
            int nSaveUserInfoCount		= 0;
            bool bFindNewUser			= false;
            bool bFree					= false;

            // ���½�����û�����ԭ���û�����Ϣ, Ӧ��ԭ���Ļ�����
            pPack->dwFlag     = PACKFLAG;
            pPack->wCheckSum  = 0;
            pPack->dwId       = 0;
            pPack->dwAckId	  = dwAckMsgId;
            pPack->wProtoNum  = PT_S_U_INTO_ROOM_USER;
            pPack->dwDataLen  = 0;		

            user_iter = pUserMapInRoom->begin();
            for (; user_iter != pUserMapInRoom->end(); ++user_iter)
            {
                PUSER_INFO pUser = (PUSER_INFO)&((*user_iter).second);
                if(NULL != pUser)
                {
                    if (pUser->dwUserId != InUserInfo.dwUserId)
                    {
                        // �򷿼���ԭ���û������½�����û�����Ϣ����
                        PPACK pNewPack = (PPACK)m_BuffPool.GetBuff();
                        pNewPack->dwFlag    = PACKFLAG;
                        pNewPack->wProtoNum = PT_S_U_INTO_ROOM_USER;
                        pNewPack->dwId      = m_PackId.GetPackId();
                        pNewPack->dwAckId   = dwAckMsgId;
                        pNewPack->dwDataLen = sizeof(USER_INFO);
                        memcpy(&(pNewPack->szData[0]), &InUserInfo, pNewPack->dwDataLen);
                        pNewPack->SetCheckNum();

                        m_QueueOut.AddPack(m_pMyUdpCC, pUser->lPublicIp, pUser->wPublicPort, pNewPack->GetPackLen(), pNewPack);
                        m_ResendQueue.AddPack(1, pNewPack->dwId, m_pMyUdpCC, pUser->lPublicIp, pUser->wPublicPort, pNewPack);

                        struct in_addr addr ={0};
                        addr.s_addr = pUser->lPublicIp;
                        g_ProFile.m_Log.println(_T("�򷿼���ԭ���û�����,IP:%s,Port:%d,MsgId:%d"), inet_ntoa(addr), ntohs(pUser->wPublicPort),pNewPack->dwId);
                    }

                    // ���淿�����û�����Ϣ��Ȼ�����½�����û����ͣ������Լ�
                    if (nSaveUserInfoCount < nSendUserInfoCountOnce)
                    {
                        memcpy(&(pPack->szData[pPack->dwDataLen]), (void*)pUser, sizeof(USER_INFO));
                        pPack->dwDataLen += sizeof(USER_INFO);
                        nSaveUserInfoCount++;
                    }
                    else if (nSaveUserInfoCount >=  nSendUserInfoCountOnce)
                    {
                        // ��������
                        pPack->dwId = m_PackId.GetPackId();
                        pPack->SetCheckNum();
                        m_QueueOut.AddPack(m_pMyUdpCC, dwIP, wPort, pPack->GetPackLen(), pPack);
                        m_ResendQueue.AddPack(1, pPack->dwId, m_pMyUdpCC, dwIP, wPort, pPack);

                        // �����ݲ�����һ���µĻ�����
                        pPack = (PPACK)m_BuffPool.GetBuff();
                        pPack->dwFlag    = PACKFLAG;
                        pPack->wCheckSum = 0;
                        pPack->dwId      = 0;
                        pPack->dwAckId	 = dwAckMsgId;
                        pPack->wProtoNum = PT_S_U_INTO_ROOM_USER;
                        pPack->dwDataLen = sizeof(USER_INFO);
                        memcpy(&(pPack->szData[0]), (void*)pUser, sizeof(USER_INFO));
                        nSaveUserInfoCount    = 1;
                    }
                }
            }// end for

            // ���½�����û�����
            if (nSaveUserInfoCount > 0)
            {
                pPack->dwId = m_PackId.GetPackId();
                pPack->SetCheckNum();
                m_QueueOut.AddPack(m_pMyUdpCC, dwIP, wPort, pPack->GetPackLen(), pPack);
                m_ResendQueue.AddPack(1, pPack->dwId, m_pMyUdpCC, dwIP, wPort, pPack);
                return true;
            }
        }// end if
    }

    m_BuffPool.Free(pPack);
    return false;
}

// �ͻ���,���û����뷿��
bool CSrv_NAT::PU_UserIntoRoom(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PU_UserIntoRoom checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USER_INFO* pUserInfo = (USER_INFO*)&(pPack->szData[0]);

    DWORD dwAckMsgId = pPack->dwId;
    int nCount = pPack->dwDataLen / sizeof(USER_INFO);

    m_ResendQueue.RemovePack(pPack->dwAckId);// ȷ�϶Է��Ѿ��յ���¼��Ϣ

    USER_INFO* pTempUserInfo = pUserInfo;

    for (int i = 0; i < nCount; i++)
    {
        if (m_dwMyId == pTempUserInfo->dwUserId)
        {
            m_lMyPubIp   = pTempUserInfo->lPublicIp;
            m_wMyPubPort = pTempUserInfo->wPublicPort;
            m_wMyRoomNum = pTempUserInfo->wRoomNum;

            UINT nThreadId = 0;
            m_hCommThread = (HANDLE)_beginthreadex(NULL, 0, CommThread, (void*)this, 0, &nThreadId);
        }
        pTempUserInfo++;
    }

    USER_MAP_INSERT pr;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lMyPubIp == pUserInfo->lPublicIp)
            pUserInfo->byCommState = IN_LAN;

        pUserInfo->dwAckTime = ::GetTickCount();
        pr = m_UserMap.insert(USER_MAP::value_type(pUserInfo->dwUserId, *pUserInfo));
        if (!pr.second)
        {
            g_ProFile.m_Log.println(_T("���û�����ʧ�� �û�ID:%d"), pUserInfo->dwUserId);
        }
        else
        {
            TCHAR szTemp[100] = {_T("")};
            if (pUserInfo->byCommState == IN_LAN)
                _tcscpy(szTemp, _T("������"));
            else
                _tcscpy(szTemp, _T("ĩͨ"));

            AddList(pUserInfo->dwUserId, pUserInfo->lPublicIp, pUserInfo->wPublicPort, 
                pUserInfo->lPrivateIp, pUserInfo->wPrivatePort, szTemp);
        }

        pUserInfo++;
    }

    // ����Ӧ���Ѿ��յ��ڷ����ڵ��û���Ϣ
    pPack->dwFlag    = PACKFLAG;
    pPack->dwId      = m_PackId.GetPackId();
    pPack->dwAckId	 = dwAckMsgId;
    pPack->wProtoNum = PT_U_S_IN_ROOM_ACK;
    pPack->dwDataLen = sizeof(USERID_ROOMNUM);	
    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);
    pUserIdRoomNum->dwUserId = m_dwMyId;
    pUserIdRoomNum->wRoomNum = m_wMyRoomNum;
    pPack->SetCheckNum();

    m_QueueOut.AddPack(m_pMyUdpCC, dwIP, wPort, pPack->GetPackLen(), pPack);
    m_ResendQueue.AddPack(m_dwMyId, pPack->dwId, m_pMyUdpCC, dwIP, wPort, pPack);

    return true;
}

// ������,���뷿��Ӧ��
bool CSrv_NAT::PS_UserIntoRoomAck(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PS_UserIntoRoomAck checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);

    m_ResendQueue.RemovePack(pUserIdRoomNum->dwUserId, PT_S_U_INTO_ROOM_USER);

    ROOM_MAP::iterator room_iter = m_RoomMap.find(pUserIdRoomNum->wRoomNum);
    if (room_iter != m_RoomMap.end())
    {
        USER_MAP::iterator user_iter = (*room_iter).second.find(pUserIdRoomNum->dwUserId);
        if (user_iter != (*room_iter).second.end())
        {
            (*user_iter).second.dwAckTime = ::GetTickCount();		
        }
    }

    m_BuffPool.Free(pPack);
    return true;
}


// �ͻ���,�����˳�����
bool CSrv_NAT::U_RequestOutRoom(void)
{
    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->dwAckId	 = 0;
    pPack->dwId      = m_PackId.GetPackId();
    pPack->wProtoNum = PT_U_S_OUT_ROOM_USER;
    pPack->dwDataLen = (WORD)sizeof(USERID_ROOMNUM);
    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);
    pUserIdRoomNum->dwUserId = m_dwMyId;
    pUserIdRoomNum->wRoomNum = m_wMyRoomNum;
    pPack->SetCheckNum();

    for (int i = 3; i > 0; i--)
    {
        m_QueueOut.AddPack(m_pMyUdpCC, m_lServerIp, m_wServerPort, pPack->GetPackLen(), pPack);
        ::Sleep(2);
    }

    return true;
}


// ������,֪ͨ�����ͻ������û��˳�
bool CSrv_NAT::PS_NotfiyUserOutRoom(PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PS_NotfiyUserOutRoom checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);

    pPack->wProtoNum = PT_S_U_OUT_ROOM_USER;

    ModList(pUserIdRoomNum->dwUserId, 5, _T("����"));

    S_Broadcast(pUserIdRoomNum->wRoomNum, 1, pPack);
    return false;
}

// ������,֪ͨ�����ͻ������û��˳�(�������̵߳���,�û������������߳�ɾ��)
bool CSrv_NAT::S_NotfiyUserOutRoom(DWORD dwUserId, WORD wRoomNum, USER_MAP* pUserMap)
{
    if (NULL == pUserMap) return false;

    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->dwAckId	 = 0;
    pPack->dwId      = m_PackId.GetPackId();
    pPack->wProtoNum = PT_S_U_OUT_ROOM_USER;
    pPack->dwDataLen = (WORD)sizeof(USERID_ROOMNUM);
    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);
    pUserIdRoomNum->dwUserId = dwUserId;
    pUserIdRoomNum->wRoomNum = wRoomNum;
    pPack->SetCheckNum();

    ModList(dwUserId, 5, _T("����"));

    int nCount = (int)pUserMap->size();
    USER_MAP::iterator delete_user_iter;	

    if (0 != nCount)
    {
        // �򷿼��ڵ�������֪ͨ���û��˳�
        USER_MAP::iterator user_iter = pUserMap->begin();
        for (; user_iter != pUserMap->end(); ++user_iter)
        {
            pPack->dwId = m_PackId.GetPackId();

            for (int i = 0; i < 2; i++)
            {
                m_QueueOut.AddPack(m_pMyUdpCC, (*user_iter).second.lPublicIp, (*user_iter).second.wPublicPort, 
                    pPack->GetPackLen(), pPack);

                ::Sleep(2);
            }
        }
        return true;
    }
    else
    {
        m_BuffPool.Free(pPack);
        return false;
    }
}

// �ͻ���,�յ�ĳ�û��˳�
bool CSrv_NAT::PU_UserOutRoom(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PU_UserOutRoom checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USERID_ROOMNUM *pUserIdRoomNum = (USERID_ROOMNUM*)&(pPack->szData[0]);

    m_UserMap.erase(pUserIdRoomNum->dwUserId);

    ModList(pUserIdRoomNum->dwUserId, 5, _T("����"));

    m_RecvMap.DelUser(pUserIdRoomNum->dwUserId);

    m_BuffPool.Free(pPack);
    return true;
}


// �ͻ���,�յ���ĳ���û�������,˵������û��Ѿ�ֱͨ
bool CSrv_NAT::PU_HelloRequest(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PU_HelloRequest checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USER_MAP::iterator user_iter = m_UserMap.find(pPack->dwAckId);
    if (m_UserMap.end() != user_iter)
    {
        (*user_iter).second.byCommState = COMMUNICED;
        ModList(pPack->dwAckId, 5, _T("ֱͨ"));

        // �ظ�Ӧ��		
        pPack->dwId      = m_PackId.GetPackId();
        pPack->wProtoNum = PT_U_U_HELLO_ANSWER;
        pPack->dwAckId   = m_dwMyId;

        m_QueueOut.AddPack(m_pMyUdpCC, dwIP, wPort, pPack->GetPackLen(), pPack);		
        return true;
    }
    else
    {
        m_BuffPool.Free(pPack);
        return false;
    }	
}

// �ͻ���,����Ӧ��,˵���Ѿ�ֱͨ
bool CSrv_NAT::PU_HelloAnswer(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PU_HelloRequest checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    USER_MAP::iterator user_iter = m_UserMap.find(pPack->dwAckId);
    if (m_UserMap.end() != user_iter)
    {
        (*user_iter).second.byCommState = COMMUNICED;
        ModList(pPack->dwAckId, 5, _T("ֱͨ"));
    }

    m_BuffPool.Free(pPack);
    return true;
}

// �ͻ���,�����ı�
bool CSrv_NAT::U_SendText(LPCTSTR pszIp, WORD wPort, LPCSTR pszText)
{
    if (NULL == pszText) return false;

    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->dwAckId	 = 0;
    pPack->dwId      = m_PackId.GetPackId();
    pPack->wProtoNum = PT_U_U_TEXT;
    pPack->dwDataLen = (WORD)strlen(pszText);	
    memcpy(&(pPack->szData[0]), pszText, pPack->dwDataLen);
    pPack->SetCheckNum();

    m_QueueOut.AddPack(m_pMyUdpCC, inet_addr(pszIp), htons(wPort), pPack->GetPackLen(), pPack);
    return true;
}

// �ͻ���,������,�����ı�
bool CSrv_NAT::PSU_ShowText(DWORD dwIP, WORD wPort, PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PSU_ShowText checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    pPack->szData[pPack->dwDataLen] = '\0';

    struct in_addr addr;
    addr.S_un.S_addr = dwIP;

    CString strAddr;
    strAddr.Format(_T("����:%s:%d"), inet_ntoa(addr), ntohs(wPort));	
    ::MessageBox(NULL, A2T(pPack->szData), strAddr, MB_OK);

    m_BuffPool.Free(pPack);
    return true;
}

// �ͻ���,������Ƶ
bool CSrv_NAT::U_SendVideo(DWORD dwUserId, WORD wProtoNum, BYTE *pbyVideo, WORD wDataLen)
{
    if (NULL == pbyVideo) return false;

    //-------------------------------------------------------------------
    // �õ����ն����ַ
    ULONG lIP = 0;
    WORD  wPort = 0;

    if (0 == dwUserId)
    {
        lIP = m_lServerIp;
        wPort = m_wServerPort;
    }
    else
    {
        USER_MAP::iterator user_iter = m_UserMap.find(dwUserId);
        if (user_iter != m_UserMap.end())
        {
            if((*user_iter).second.byCommState == IN_LAN)
            {
                lIP = (*user_iter).second.lPrivateIp;
                wPort = (*user_iter).second.wPrivatePort;
            }
            else if((*user_iter).second.byCommState == COMMUNICED)
            {
                lIP = (*user_iter).second.lPublicIp;
                wPort = (*user_iter).second.wPublicPort;
            }
        }
    }

    if (lIP == 0)
    {
        TRACE(_T("Send IP Error\n"));
        return false;
    }

    //-------------------------------------------------------------------
    // �ָ����ݰ�

    WORD wCheckNum = GetCheckNum((LPBYTE)pbyVideo, wDataLen);

    DWORD dwVideoId = m_dwVideoId;
    BYTE* pData = pbyVideo;
    int nSubPackHeaderLen = sizeof(SUBPACKINFO);
    int nPackNum = wDataLen / MY_MTU;
    int nLastPackLen = wDataLen % MY_MTU;
    if (nLastPackLen > 0) nPackNum++;
    else nLastPackLen = MY_MTU;

    for (int i = 0; i < nPackNum; i++)
    {
        PPACK pPack = (PPACK)m_BuffPool.GetBuff();
        pPack->dwFlag = PACKFLAG;
        pPack->wCheckSum    = wCheckNum;//0;
        pPack->dwId         = m_dwVideoId++;
        pPack->dwAckId		 = m_dwMyId;		
        pPack->wProtoNum    = wProtoNum;
        PSUBPACKINFO pSubInfo = (PSUBPACKINFO)&(pPack->szData[0]);
        pSubInfo->ucSubPackNum   = nPackNum;
        pSubInfo->ucSubPackId    = i;
        pSubInfo->wRoomNum       = m_wMyRoomNum;
        pSubInfo->dwVideoId		 = dwVideoId;

        if (i == nPackNum - 1)	// ���һ����
        {
            pPack->dwDataLen = (WORD)nLastPackLen + nSubPackHeaderLen - 1;
            memcpy(&(pSubInfo->ucData[0]), pData, nLastPackLen);			
        }
        else
        {
            pPack->dwDataLen = (WORD)MY_MTU + nSubPackHeaderLen - 1;
            memcpy(&(pSubInfo->ucData[0]), pData, MY_MTU);
        }

        //////////////////////////////////////////////////////////////////
        //m_RecvMap.AddVideoData(pPack);
        //////////////////////////////////////////////////////////////////

        m_QueueOut.AddPack(m_pMyUdpCC, lIP, wPort, pPack->GetPackLen(), pPack);
        m_VideoList.AddLimitNode(pPack->dwId, (BYTE*)pPack);

        pData += MY_MTU;
    }

    return true;
}

// �ͻ���,������Ƶ
bool CSrv_NAT::U_SendAudio(DWORD dwUserId, WORD wProtoNum, UCHAR *pAudio, int nDataLen)
{
    if (NULL == pAudio) return false;

    //-------------------------------------------------------------------
    // �õ����ն����ַ
    ULONG lIP = 0;
    WORD  wPort = 0;

    if (0 == dwUserId)
    {
        lIP = m_lServerIp;
        wPort = m_wServerPort;
    }
    else
    {
        USER_MAP::iterator user_iter = m_UserMap.find(dwUserId);
        if (user_iter != m_UserMap.end())
        {
            if((*user_iter).second.byCommState == IN_LAN)
            {
                lIP = (*user_iter).second.lPrivateIp;
                wPort = (*user_iter).second.wPrivatePort;
            }
            else if((*user_iter).second.byCommState == COMMUNICED)
            {
                lIP = (*user_iter).second.lPublicIp;
                wPort = (*user_iter).second.wPublicPort;
            }
        }
    }

    if (lIP == 0)
    {
        TRACE(_T("Send IP Error\n"));
        return false;
    }

    //-------------------------------------------------------------------

    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->dwId      = m_dwAudioId++;
    pPack->dwAckId	 = m_dwMyId;
    pPack->wProtoNum = wProtoNum;
    pPack->dwDataLen = nDataLen;
    pPack->wCheckSum = m_wMyRoomNum;
    memcpy(&(pPack->szData[0]), pAudio, nDataLen);

    //////////////////////////////////////////////////////////////////
    //m_RecvMap.AddAudioData(pPack);
    //////////////////////////////////////////////////////////////////

    m_QueueOut.AddPack(m_pMyUdpCC, lIP, wPort, pPack->GetPackLen(), pPack);
    m_AudioList.AddLimitNode(pPack->dwId, (BYTE*)pPack);

    return true;
}

// ������,�ڷ����ڷַ���Ƶ
bool CSrv_NAT::PS_BroadcastAudio(PPACK pPack)
{
    pPack->wProtoNum = PT_S_U_AUDIO;

    return S_Broadcast(pPack->wCheckSum, 1, pPack);
}

// ������,�ڷ����ڷַ���Ƶ
bool CSrv_NAT::PS_BroadcastVideo(PPACK pPack)
{
    PSUBPACKINFO pSubInfo = (PSUBPACKINFO)&(pPack->szData[0]);

    pPack->wProtoNum = PT_S_U_VIDEO;

    return S_Broadcast(pSubInfo->wRoomNum, 1, pPack);
}

// �ͻ���,����ô�����
bool CSrv_NAT::U_RequestResend(DWORD dwUserId, DWORD dwPackId, WORD wType)
{
    ULONG lIP = 0;
    WORD  wPort = 0;

    WORD  wProtoNum = PT_U_U_RESEND;

    if (0 == dwUserId)
    {
        lIP = m_lServerIp;
        wPort = m_wServerPort;
    }
    else
    {
        USER_MAP::iterator user_iter = m_UserMap.find(dwUserId);
        if (user_iter != m_UserMap.end())
        {
            if((*user_iter).second.byCommState == IN_LAN)
            {
                lIP = (*user_iter).second.lPrivateIp;
                wPort = (*user_iter).second.wPrivatePort;
            }
            else if((*user_iter).second.byCommState == COMMUNICED)
            {
                lIP = (*user_iter).second.lPublicIp;
                wPort = (*user_iter).second.wPublicPort;
            }
            else
            {
                lIP = m_lServerIp;
                wPort = m_wServerPort;

                wProtoNum = PT_U_S_RESEND;
            }
        }
    }

    if (lIP == 0)
    {
        TRACE(_T("U_RequestResend IP Error\n"));
        return false;
    }

    PPACK pPack = (PPACK)m_BuffPool.GetBuff();
    pPack->dwFlag    = PACKFLAG;
    pPack->dwId      = m_PackId.GetPackId();
    pPack->dwAckId	 = m_dwMyId;
    pPack->wProtoNum = wProtoNum;
    pPack->dwDataLen = sizeof(RESENDPACK);
    PRESENDPACK pResendPack  = (PRESENDPACK)&(pPack->szData[0]);
    pResendPack->wRoomNum	 = m_wMyRoomNum;
    pResendPack->dwRecvUserId= dwUserId;
    pResendPack->dwPackId    = dwPackId;
    pResendPack->wPackType    = wType;
    pPack->SetCheckNum();

    m_QueueOut.AddPack(m_pMyUdpCC, lIP, wPort, pPack->GetPackLen(), pPack);

    return true;
}

// �ͷ���,ת��
bool CSrv_NAT::PS_Resend(PPACK pPack)
{
    PRESENDPACK pResendPack = (PRESENDPACK)&(pPack->szData[0]);

    pPack->wProtoNum = PT_S_U_RESEND;

    return S_Transmit(pResendPack->wRoomNum, pResendPack->dwRecvUserId, 1, pPack);
}

// �ͻ���,������ת
bool CSrv_NAT::PU_Resend(PPACK pPack)
{
    if (pPack->wCheckSum != pPack->GetCheckNum())
    {
        TRACE("PU_Resend checknum Error %d\n", pPack->dwId);
        m_BuffPool.Free(pPack);
        return false;
    }

    PRESENDPACK pResendPack = (PRESENDPACK)&(pPack->szData[0]);

    ULONG lIP = 0;
    WORD wPort = 0;
    bool bToServer = false;

    USER_MAP::iterator user_iter = m_UserMap.find(pPack->dwAckId);
    if (user_iter != m_UserMap.end())
    {
        if((*user_iter).second.byCommState == IN_LAN)
        {
            lIP = (*user_iter).second.lPrivateIp;
            wPort = (*user_iter).second.wPrivatePort;
        }
        else if((*user_iter).second.byCommState == COMMUNICED)
        {
            lIP = (*user_iter).second.lPublicIp;
            wPort = (*user_iter).second.wPublicPort;
        }
        else
        {
            lIP = m_lServerIp;
            wPort = m_wServerPort;
            bToServer = true;
        }
    }
    else
    {
        m_BuffPool.Free(pPack);
        return false;
    }

    if (pResendPack->wPackType == TYPE_VIDEO)
    {
        PPACK pPackVideo = (PPACK)m_VideoList.GetNode(pResendPack->dwPackId);
        if (NULL == pPackVideo)
        {
            TRACE("No V %d\n", pResendPack->dwPackId);
            m_BuffPool.Free(pPackVideo);
            return false;
        }
        else
        {
            if (bToServer)
                pPackVideo->wProtoNum = PT_U_S_VIDEO;
            else
                pPackVideo->wProtoNum = PT_U_U_VIDEO;

            //pPackVideo->wCheckSum = 123;
            //TRACE("Resend V %d\n", pPackVideo->dwId);

            m_QueueOut.AddPack(m_pMyUdpCC, lIP, wPort, pPackVideo->GetPackLen(), pPackVideo);
        }
    }
    else if (pResendPack->wPackType == TYPE_AUDIO)
    {
        PPACK pPackAudio = (PPACK)m_AudioList.GetNode(pResendPack->dwPackId);
        if (NULL == pPackAudio)
        {
            TRACE("No A %d\n", pResendPack->dwPackId);
            m_BuffPool.Free(pPackAudio);
            return false;
        }
        else
        {
            if (bToServer)
                pPackAudio->wProtoNum = PT_U_S_AUDIO;
            else
                pPackAudio->wProtoNum = PT_U_U_AUDIO;

            pPackAudio->wCheckSum = 123;

            //TRACE("resend A %d\n", pPackAudio->dwId);
            m_QueueOut.AddPack(m_pMyUdpCC, lIP, wPort, pPackAudio->GetPackLen(), pPackAudio);
        }
    }
    else
    {
        TRACE("PU_Resend TYPE_COMMAND\n");
        m_BuffPool.Free(pPack);
        return false;
    }

    m_BuffPool.Free(pPack);
    return true;
}

