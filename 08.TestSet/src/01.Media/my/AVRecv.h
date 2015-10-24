
#pragma once
#include <afxtempl.h>
#include "Common/KeyList.h"
//#include "Network/DefineClientContext.h"

typedef struct tagClientContext*  PCLIENT;
typedef struct tagPACKHEAD*       PPACK;
typedef struct _tagPackEx*        PPACKEX;
typedef struct _tagBlockEx*       PBLOCKEX;
class CPackQueue;
class CBuffPool;
class CAVPlay;

typedef struct _tagRECVINFO
{
    DWORD          dwWaitId;    // ˳��ID,ϣ�����յ���֡ID
    DWORD          dwToPlayId;  // ˳��ID,�Ѿ����͵�AVPlay��ID
    CKeyList       AVList;      // ����Ƶ���ݿ�
    //ADDR           Addr;        // ��ַ
	int		ip;
	short   port;
    PCLIENT pClient;
}RECVINFO, *PRECVINFO;

typedef CMap<DWORD, DWORD, PRECVINFO, PRECVINFO>    RECVINFO_MAP;
typedef CArray<PPACK, PPACK>                        PPACK_ARR;


/**
    ���յ���������ϳɲ������ݾͷ��͸�AVPlay,���䲥��,
    ��Ƶ����Ƶ��ID����һ��˳���
**/
class CAVRecv
{
public:
    CAVRecv();
    ~CAVRecv();

protected:
    DWORD           m_dwBit[32];    // ������λֵ
    DWORD           m_dwMTU;        // ��С���䵥Ԫ
    WORD            m_wResendProto; // �ش�Э���,����Ƶ��ͬһ��
    WORD            m_wTimes;       // ����Ƿ��ж���
    UINT            m_nAddrSize;    // IPV4��ַ�ṹ��С
    UINT            m_nAddrSize6;   // IPV6��ַ�ṹ��С
    BOOL            m_bIPV6;        // IPV6��ַ

    CAVPlay         *m_pAVPlay;     // ���Ŷ���
    CPackQueue      *m_pQueueOut;   // ���Ͷ���,�û������ش�����
    CBuffPool       *m_pBigBuffPool;// ��Ļ�����
    CBuffPool       *m_pSubBuffPool;// С�Ļ�����

    RECVINFO_MAP    m_mapRecvAV;    // ���յ�������Ƶ��
    PPACK_ARR       m_arrPack;      // ��Ƶ�Ӱ�����

    enum {AUDIO = 1, VIDEO, VIDEO_BIG};

protected:
    void MakeFrame(DWORD dwChanId, PRECVINFO pRecvInfo);
    void QuestResend(DWORD dwChanId, PRECVINFO pRecvInfo);
    void SendResend(DWORD dwChanId, DWORD dwId, PCLIENT pClient, UINT64 *pnIP, WORD wPort);

public:
    void Init(BOOL bIPV6, DWORD dwMTU, WORD wResendProto, CAVPlay *pAVPlay, 
              CPackQueue *pQueueOut, CBuffPool *pBigBuffPool, CBuffPool *pSubBuffPool);
    void AddRecver(DWORD dwChanId, PCLIENT pClient, UINT64 *pnIP, WORD wPort);
    void DelRecver(DWORD dwChanId);
    void RecvAV(PPACK pPack, WORD wType);
};