
#include "stdafx.h"
#include "ResendQueue.h"
#include "Network\Srv.h"

/*
�߳�    ״̬    �˳���

�ط�    ����    200
�ط�    ǿ��    201
�ط�    �쳣    202

*/


CResendQueue::CResendQueue()
    :m_pSrv(NULL),
    m_pBuffPool(NULL),
    m_hExit(NULL),
    m_hThread(NULL),
    m_bInit(FALSE)
{
    InitializeCriticalSection(&m_csUsedFlag);// ��ʼ���ٽ�����Ϊ�˴�ȡm_mapAckPack
}

CResendQueue::~CResendQueue()
{
    UnInit();                               // ֹͣ�߳� 
    DeleteCriticalSection(&m_csUsedFlag);   // ɾ���ٽ���
}

int CResendQueue::Init(CSrv *pSrv, CBuffPool *pBuffPool)
{
    if (NULL == pSrv) return -1;

    if (m_bInit) return -2;
    
    m_pSrv = pSrv;
    m_pBuffPool = pBuffPool;

    SECURITY_ATTRIBUTES sat = {0};
    sat.nLength = sizeof(SECURITY_ATTRIBUTES);
    sat.lpSecurityDescriptor = NULL;
    sat.bInheritHandle = TRUE;

    m_hExit = CreateEvent(&sat, TRUE, FALSE, NULL);// ��������¼�
    ::ResetEvent(m_hExit);

    UINT nThreadID = 0;
    m_hThread = (HANDLE)_beginthreadex(NULL, 0, CheckPackThread, this, 0, &nThreadID);// ��������߳�
    if (m_hThread == NULL) return -3;

    m_bInit = TRUE;
    return 0;
}

void CResendQueue::UnInit(void)
{
    ::SetEvent(m_hExit);

    EnterCriticalSection(&m_csUsedFlag);
    m_mapAckPack.RemoveAll();   // ɾ���б��е�����
    LeaveCriticalSection(&m_csUsedFlag);

    if (m_bInit)                // ֹͣ�̣߳��������
    {
        if (m_hThread != NULL)
        {
            int i = 10;
            for (; i > 0 ; i--)
            {
                DWORD nCode;
                if (!GetExitCodeThread(m_hThread, &nCode)) break;
                if (nCode != STILL_ACTIVE) break;
                Sleep(50);
            }

            if (i == 0)
            {
                TerminateThread(m_hThread, 201);
                TRACE(_T("WARNING: Terminating CResendQueue.WorkThread."));
            }
            CloseHandle(m_hThread);
        }

        m_bInit = FALSE;
    }

    CloseHandle(m_hExit);
    m_hExit = NULL;
}

//�̺߳���
unsigned CResendQueue::CheckPackThread(LPVOID pParam)
{
    if (NULL == pParam) return -1;

    CResendQueue *pThis = reinterpret_cast<CResendQueue*>(pParam);
    VERIFY(pThis);

    try
    {
        while(pThis->m_hExit)
        {
            if (WaitForSingleObject(pThis->m_hExit, 100) == WAIT_OBJECT_0) break;

            pThis->CheckPack();     // ��ʱ���
        }
    }
    catch(...)
    {
        g_ProFile.m_pLog->println(LOG_EXCEPT, _T("CheckPackThread ErrorCode:%d"), ::GetLastError());
        return 202;
    }
    return 200;
}

// ����Ƿ��г�ʱ����Ϣ�������ʱ���ط��˰�
void CResendQueue::CheckPack()
{
    DWORD dwNowTime = 0;
    DWORD dwKey = 0;
    ACKPACK AckPack = {0};

    EnterCriticalSection(&m_csUsedFlag); // ��ס�ٽ���

    POSITION pos = m_mapAckPack.GetStartPosition();
    while (pos != NULL)
    {
        m_mapAckPack.GetNextAssoc(pos, dwKey, AckPack);

        dwNowTime = ::GetTickCount();
        if (dwNowTime >= AckPack.dwNextTime)
        {
            AckPack.wSendTimes++;
            AckPack.dwNextTime = dwNowTime + RESEND_TIME;

            if (AckPack.wSendTimes < MAX_SEND_TIMES)
            {
                m_pSrv->ResendUdpPack(AckPack.pClient, AckPack.dwDataLen, AckPack.pData, AckPack.Addr.Int64, AckPack.Addr.wPort);
                m_mapAckPack.SetAt(dwKey, AckPack);     //��Ҫ���޸�ʱ��
            }
            else
            {
                m_pSrv->DelResendUdpPack(AckPack.pData);
                m_mapAckPack.RemoveKey(dwKey);
            }
        }       
    }
    LeaveCriticalSection(&m_csUsedFlag);
}

BOOL CResendQueue::Add(DWORD dwUserId, DWORD dwPackId, PPACK pPack, PCLIENT pClient, ADDR &Addr)
{
    if(NULL == pPack) return FALSE;

    EnterCriticalSection(&m_csUsedFlag);

    m_pBuffPool->SetTimes(pPack, 0xFFFFFFFF);

    ACKPACK AckPack = {0};
    AckPack.dwUserId   = dwUserId;
    AckPack.dwPackId   = dwPackId;
    AckPack.dwDataLen  = pPack->GetPackLen();
    AckPack.dwNextTime = GetTickCount() + RESEND_TIME;
    AckPack.wProtoNum  = pPack->wProtoNum;    
    AckPack.pClient    = pClient;
    AckPack.pData      = pPack;
    AckPack.Addr       = Addr;

    m_mapAckPack.SetAt(dwPackId, AckPack);

    LeaveCriticalSection(&m_csUsedFlag);

    return TRUE;
}

BOOL CResendQueue::Add(DWORD dwUserId, DWORD dwPackId, PPACK pPack, PCLIENT pClient, UINT64 *pnIP, WORD wPort)
{
    if(NULL == pPack) return FALSE;

    EnterCriticalSection(&m_csUsedFlag);

    m_pBuffPool->SetTimes(pPack, 0xFFFFFFFF);

    ACKPACK AckPack = {0};
    AckPack.dwUserId   = dwUserId;
    AckPack.dwPackId   = dwPackId;
    AckPack.dwDataLen  = pPack->GetPackLen();
    AckPack.dwNextTime = GetTickCount() + RESEND_TIME;
    AckPack.wProtoNum  = pPack->wProtoNum;    
    AckPack.pClient    = pClient;
    AckPack.pData      = pPack;
    AckPack.Addr.Int64[0] = pnIP[0];
    AckPack.Addr.Int64[1] = pnIP[1];
    AckPack.Addr.wPort    = wPort;

    m_mapAckPack.SetAt(dwPackId, AckPack);

    LeaveCriticalSection(&m_csUsedFlag);

    return TRUE;
}

// �Ѿ��õ�ȷ�ϣ�����Ӷ�����ɾ��
BOOL CResendQueue::Del(DWORD dwPackId)
{
    EnterCriticalSection(&m_csUsedFlag);

    ACKPACK AckPack = {0};
    while (m_mapAckPack.Lookup(dwPackId, AckPack))
    {
        m_pBuffPool->SetTimes(AckPack.pData, 0);
        m_mapAckPack.RemoveKey(dwPackId);
    }

    LeaveCriticalSection(&m_csUsedFlag);

    return TRUE;
}

BOOL CResendQueue::Del(DWORD dwUserId, WORD wProtoNum)
{
    CArray<DWORD, DWORD> DelPackList;
    DWORD dwKey = 0;
    ACKPACK AckPack = {0};

    EnterCriticalSection(&m_csUsedFlag);

    POSITION pos = m_mapAckPack.GetStartPosition();
    while (pos != NULL)
    {
        m_mapAckPack.GetNextAssoc(pos, dwKey, AckPack);

        if (AckPack.dwUserId == dwUserId && AckPack.wProtoNum == wProtoNum)
        {
            m_pBuffPool->SetTimes(AckPack.pData, 0);
            DelPackList.Add(dwKey);
        }
    }

    for (int i = 0; i < DelPackList.GetCount(); i++)
        m_mapAckPack.RemoveKey(DelPackList[i]);

    DelPackList.RemoveAll();

    LeaveCriticalSection(&m_csUsedFlag);

    return TRUE;
}
