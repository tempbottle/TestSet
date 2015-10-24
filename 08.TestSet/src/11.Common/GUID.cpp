#include "stdafx.h"
#include "GUID.h"


/*

0                ~                127

1 2 3  4 5  6 7  8 9  A B C D E F *
E57153E1-EE6E-40C7-A6E0-9ED9A6937F1B

f81d4fae-7dec-11d0-a765-00a0c91e6bf6 


һ,ʱ���(��60λ)

ʱ�����(00~31,32λ),������f81d4fae
ʱ�����(32~47,16λ),������7dec
ʱ�����(52~63,12λ),������11d0��1d0

��,�汾��(48~51,��4λ),��7���ڵ��ǰ4λ,������11d0�ĵ�һ��1

0001 ��ʱ��Ϊ����
0010 ������DCE,Ƕ��ʽPOSIX��UID
0011 ������Ϊ����
0100 �����α�������

��,GUID��������(64~66,��3λ),λ�ڵ�9���ֽڵ�ǰ3λa765��a

000 ����,������������
100 �ڱ���ָ���ı���
101 
110 ����,΢��˾������
111 ����,Ϊ�����Ķ���

��,ʱ�����(67~79,��14λ)

ʱ����Ÿ�(67~71,5λ)
ʱ����ŵ�(72~78,8λ)

��,NODE��ַ��ʶ(80~127,48λ)

//------------------------------------------------------------------

һ,�汾Ϊ1ʱ
ʱ���,��1582-10-15 00:00:00.00��100����ļ���
ʱ����,�����ǰʱ��������֪,��1����,�����������
NODE,��������MAC��ַ,���û�����������

��,�汾Ϊ3ʱ
ʱ���,�����ֿռ����һ��60λ��
ʱ����,�����ֿռ����һ��14λ��
NODE,�����ֿռ����һ��48λ��

��,�汾Ϊ4ʱ
ʱ���,�������һ��60λ��
ʱ����,�������һ��14λ��
NODE,�������һ��48λ��

*/


CGUID::CGUID()
:m_LastTime(0),
m_wThisTick(GUIDS_PER_TICK)
{
}

CGUID::~CGUID()
{
}


/* get-current_time -- get time as 60 bit 100ns ticks since whenever.
Compensate for the fact that real clock resolution is
less than 100ns. */
void CGUID::GetCurTimestamp(GUIDTIME *pTimestamp)
{
    if (NULL == pTimestamp) return;

    GUIDTIME TimeNow = 0;

    while (1)
    {
        this->GetSystemTime(&TimeNow);

        /* if clock reading changed since last UUID generated... */
        if (m_LastTime != TimeNow)
        {
            /* reset count of uuids gen'd with this clock reading */
            m_wThisTick = 0;
            break;
        }

        if (m_wThisTick < GUIDS_PER_TICK)
        {
            m_wThisTick++;
            break;
        }
        /* going too fast for our clock; spin */
    }

    /* add the count of guids to low order bits of the clock reading */
    *pTimestamp = m_LastTime = TimeNow + m_wThisTick;
}

// �õ���1582-10-15 00:00:00.00��100����ļ���
void CGUID::GetSystemTime(GUIDTIME *pTimestamp)
{
    if (NULL == pTimestamp) return;

    ULARGE_INTEGER time = {0};

    GetSystemTimeAsFileTime((FILETIME*)&time);

    /* NT keeps time in FILETIME format which is 100ns ticks since
    Jan 1, 1601.  UUIDs use time in 100ns ticks since Oct 15, 1582.
    The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
    + 18 years and 5 leap days.
    */

    time.QuadPart +=
        (unsigned __int64) (1000*1000*10)       // seconds
        * (unsigned __int64) (60 * 60 * 24)       // days
        * (unsigned __int64) (17+30+31+365*18+5); // # of days

    *pTimestamp = time.QuadPart;
}

// ���õ�48λ���
void CGUID::GetRandomNode(BYTE *pbyData)
{
    RANDOM_NESS r;

    /* memory usage stats */
    GlobalMemoryStatus(&r.m);
    /* random system stats */
    GetSystemInfo(&r.s);
    /* 100ns resolution (nominally) time of day */
    GetSystemTimeAsFileTime(&r.t);
    /* high resolution performance counter */
    QueryPerformanceCounter(&r.pc);
    /* milliseconds since last boot */
    r.tc = GetTickCount();
    /* local host name  */
    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.szHostName, &r.l);

    memset(pbyData, 0, 6);

    m_MD5.Reset();
    m_MD5.Add(&r, sizeof(r));
    m_MD5.Finish();

    memcpy(pbyData, m_MD5.m_nState, 6);
};

// �õ�14λ�����
void CGUID::GetRandomClockSeq(WORD *pwData)
{
    GUIDTIME TimeNow = 0;
    GetSystemTime(&TimeNow);
    TimeNow /= GUIDS_PER_TICK;
    srand((unsigned int)(((TimeNow >> 32) ^ TimeNow)&0xffffffff));

    *pwData = (WORD)rand();
}

// �õ�1��GUID
void CGUID::FormatGUIDv1(tGUID *pGUID, GUID_DATA *pGuidData)
{
    // Construct a version 1 uuid with the information we've gathered
    // plus a few constants.

    pGUID->dwTimeLow = (DWORD)(pGuidData->ts & 0xFFFFFFFF);
    pGUID->wTimeMid = (WORD)((pGuidData->ts >> 32) & 0xFFFF);
    pGUID->wTimeHighAndVersion = (WORD)((pGuidData->ts >> 48) & 0x0FFF) | (1 << 12);
    pGUID->byClockSeqHighAndReserved = ((pGuidData->cs & 0x1F00) >> 8) | 0x80;
    pGUID->byClockSeqLow = pGuidData->cs & 0xFF;

    memcpy(&pGUID->Node, &pGuidData->node, 6);
}

// �õ�3��GUID
void CGUID::FormatGUIDv3(tGUID *pGUID, BYTE *pbyData)
{
    // Construct a version 3 uuid with the (pseudo-)random number
    // plus a few constants.

    tGUID HashGUID = {0};
    memcpy(pGUID, pbyData, 16);
    memcpy(&HashGUID, pbyData, 16);

    /* convert UUID to local byte order */
    pGUID->dwTimeLow = ntohl(HashGUID.dwTimeLow);
    pGUID->wTimeMid = ntohs(HashGUID.wTimeMid);
    pGUID->wTimeHighAndVersion = ntohs(HashGUID.wTimeHighAndVersion);

    /* put in the variant and version bits */
    pGUID->wTimeHighAndVersion &= 0x0FFF;
    pGUID->wTimeHighAndVersion |= (3 << 12);
    pGUID->byClockSeqHighAndReserved &= 0x1F;
    pGUID->byClockSeqHighAndReserved |= 0x80;
}

// �õ�4��GUID
void CGUID::FormatGUIDv4(tGUID *pGUID, WORD *pwData)
{
    // Construct a version 4 uuid with the all random number
    // plus a few constants.

    memcpy(pGUID, pwData, 16);

    /* put in the variant and version bits */
    pGUID->wTimeHighAndVersion &= 0x0FFF;
    pGUID->wTimeHighAndVersion |= (4 << 12);
    pGUID->byClockSeqHighAndReserved &= 0x1F;
    pGUID->byClockSeqHighAndReserved |= 0x80;
}

// �õ�1��GUID
BOOL CGUID::CreateGUIDv1(tGUID *pGUID)
{
    if (NULL == pGUID) return FALSE;

    GUID_DATA Data = {0};

    /* get current time */
    GetCurTimestamp(&Data.ts);

    /* get node ID */
    GetRandomNode((BYTE*)&Data.node);

    /* get clock sequence */
    GetRandomClockSeq(&Data.cs);

    /* format guid data */
    FormatGUIDv1(pGUID, &Data);

    return TRUE;
}

// �õ�3��GUID
BOOL CGUID::CraateGUIDv3(tGUID *pGUID, tGUID Nsid, void *pName, int nNameLen)
{
    if (NULL == pGUID) return FALSE;

    tGUID NetNsid = Nsid;      /* context GUID in network byte order */

    /* put name space ID in network byte order so it hashes the same
    no matter what endian machine we're on */

    NetNsid.dwTimeLow = htonl(Nsid.dwTimeLow);
    NetNsid.wTimeMid = htons(Nsid.wTimeMid);
    NetNsid.wTimeHighAndVersion = htons(Nsid.wTimeHighAndVersion);

    BYTE byData[16] = {0};

    m_MD5.Reset();
    m_MD5.Add(&NetNsid, sizeof(NetNsid));
    m_MD5.Add(pName, nNameLen);
    m_MD5.Finish();
   // memcpy(byData, m_MD5.getHash(), 16);

    /* the hash is in network byte order at this point */
    FormatGUIDv3(pGUID, byData);

    return TRUE;
}

// �õ�4��GUID
BOOL CGUID::CreateGUIDv4(tGUID *pGUID)
{
    if (NULL == pGUID) return FALSE;

    WORD wData[8] = {0};

    for (int i = 0; i < 8; i++)
    {
        wData[i] = rand();
    }
   
    /* format guid data */
    FormatGUIDv4(pGUID, wData);

    return TRUE;
}

#define MY_CHECK(f1, f2) if (f1 != f2) return f1 < f2 ? -1 : 1;

int CGUID::CompareGUID(tGUID *u1, tGUID *u2)
{
    MY_CHECK(u1->dwTimeLow, u2->dwTimeLow);
    MY_CHECK(u1->wTimeMid, u2->wTimeMid);
    MY_CHECK(u1->wTimeHighAndVersion, u2->wTimeHighAndVersion);
    MY_CHECK(u1->byClockSeqHighAndReserved, u2->byClockSeqHighAndReserved);
    MY_CHECK(u1->byClockSeqLow, u2->byClockSeqLow);

    for (int i = 0; i < 6; i++)
    {
        if (u1->Node.nodeID[i] < u2->Node.nodeID[i])
            return -1;
        if (u1->Node.nodeID[i] > u2->Node.nodeID[i])
            return 1;
    }

    return 0;
}