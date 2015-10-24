
#pragma once

#include <afxtempl.h>
#include <vfw.h>
#include "H264\H264.h"
#include "Network\PreparePack.h"
#include "Common\AVList.h"

#define USER_COUNT  3

class CAvInfo
{
public:
    CRITICAL_SECTION    m_csAvInfo;     // �ٽ���
    CRITICAL_SECTION    m_csAvPlayV;    // �ٽ���
    DWORD               dwUserId;       // �û�ID
    WORD                wLocId;         // ռ�ô��ں�
    DWORD               dwWantVideoId;  // ϣ�����յ���ƵID
    DWORD               dwWantAudioId;  // ϣ�����յ���ƵID
    DWORD               dwPlayVideoId;  // ���ŵ���ƵID
    DWORD               dwPlayAudioId;  // ���ŵ���ƵID
    LONG                lLocX;          // ��Ƶ����x
    LONG                lLocY;          // ��Ƶ����y
    HDC                 hdc;            // HDC
    HDRAWDIB            hdib;           // ��λͼ
    BITMAPINFOHEADER*   pbmpheader;     // λͼ��Ϣ
    CH264               h264;           // H264ѹ��
    UCHAR               ucVideo[80000]; // ������һ֡��Ƶ����
    WORD                wTempSubNum;    // ���һ����Ƶ�Ӱ�����
    LPBYTE              pbyTempBuf[10]; // ��Ƶ������
    CAVList             AudioList;      // ���յ�����Ƶ
    CAVList             VideoList;      // ���յ�����Ƶ

    CAvInfo()
    {
        InitializeCriticalSection(&m_csAvInfo);
        InitializeCriticalSection(&m_csAvPlayV);
    }

    ~CAvInfo()
    {
        DeleteCriticalSection(&m_csAvInfo);
        DeleteCriticalSection(&m_csAvPlayV);
    }

    void ShowVideo(BYTE* pbyData, WORD wLen)
    {
        EnterCriticalSection(&m_csAvPlayV);

        BYTE *pDeCodeData = NULL;
        WORD wDeCodeDataLen = 0;

        pDeCodeData = h264.Decode(pbyData, wLen, wDeCodeDataLen);

        if (NULL != pDeCodeData)
            ::DrawDibDraw(hdib,hdc,lLocX,lLocY,-1,-1,pbmpheader,pDeCodeData,0,0,IMAGE_WIDTH,IMAGE_HEIGHT,DDF_SAME_DRAW);

        LeaveCriticalSection(&m_csAvPlayV);
    }

    DWORD GetPlayAudioId(void)
    {
        DWORD dwRetId = 0;
        EnterCriticalSection(&m_csAvInfo);
        dwRetId = dwPlayAudioId;
        LeaveCriticalSection(&m_csAvInfo);
        return dwRetId;
    }
    DWORD GetPlayVideoId(void)
    {
        DWORD dwRetId = 0;
        EnterCriticalSection(&m_csAvInfo);
        dwRetId = dwPlayVideoId;
        LeaveCriticalSection(&m_csAvInfo);
        return dwRetId;
    }
    void AdcPlayAudioId(void)
    {
        EnterCriticalSection(&m_csAvInfo);
        dwPlayAudioId++;
        LeaveCriticalSection(&m_csAvInfo);
    }   
    void AdcPlayVideoId(void)
    {
        EnterCriticalSection(&m_csAvInfo);
        dwPlayVideoId++;
        LeaveCriticalSection(&m_csAvInfo);
    }
    void AddPlayVideoId(DWORD dwNum)
    {
        EnterCriticalSection(&m_csAvInfo);
        dwPlayVideoId += dwNum;
        LeaveCriticalSection(&m_csAvInfo);
    }
    void SetPlayVideoId(DWORD dwNum)
    {
        EnterCriticalSection(&m_csAvInfo);
        dwPlayVideoId = dwNum;
        LeaveCriticalSection(&m_csAvInfo);
    }
    void SetPlayAudioId(DWORD dwNum)
    {
        EnterCriticalSection(&m_csAvInfo);
        dwPlayAudioId = dwNum;
        LeaveCriticalSection(&m_csAvInfo);
    }
};

typedef CMap<DWORD, DWORD&, CAvInfo*, CAvInfo*> CUserMap;

class CDlg_NAT;
class CSrv_NAT;

class CRecvAVMap
{
public: 
    CRecvAVMap();
    virtual ~CRecvAVMap();

private:
    CRITICAL_SECTION    m_csUsedAudio;
    CRITICAL_SECTION    m_csUsedVideo;
    HANDLE              m_hExit;
    HANDLE              m_hPlayThread;

    CSrv_NAT*           m_pSrvNat;
    CDlg_NAT*           m_pDlg;
    CBuffPool*          m_pBuffPool;

    HDC                 m_hdc;
    HDRAWDIB            m_hdib;
    BITMAPINFOHEADER*   m_pbmpheader;

    CUserMap            m_UserMap;
    CPoint              m_ptVideo[USER_COUNT];
    bool                m_bUsed[USER_COUNT];

    CAvInfo* FineUser(DWORD dwUserId);
    CAvInfo* AddUser(DWORD dwUserId);
    WORD GetCheckNum(PPACK pPack);
    WORD GetCheckNum(LPBYTE byData, INT nDataLen);

public:
    bool Init(CSrv_NAT* pSrvNat, CDlg_NAT* pDlg, CBuffPool* pBuffPool);
    void Uninit(void);
    bool AddVideoData(PPACK pPack);
    bool AddAudioData(PPACK pPack);
    BOOL DelUser(DWORD dwUserId);
    static unsigned __stdcall PlayThread(LPVOID pParam);
    bool PlayThreadRun(void);   
};
