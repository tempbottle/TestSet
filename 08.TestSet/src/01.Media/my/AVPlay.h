
#pragma once
#include <afxtempl.h>
#include <vfw.h>
#include "AV/G729.h"
#include "AV/H264.h"
#include "AV/WaveOut.h"
#include "AV/PackHead.h"
#include "Common/KeyList.h"


typedef struct tagPACKHEAD* PPACK;
class CPackQueue;
class CBuffPool;

typedef CArray<PPACK, PPACK>    PPACK_ARRAY;

enum { PLAY, PAUSE, STOP, FORWARD, BACKWARD };

typedef struct _tagPLAYINFO
{
    HANDLE              hPlayThread;    // �����߳�
    DWORD               dwChannId;      // ��·����Ƶ��ID
    DWORD               dwLastTime;     // ��һ�β��ŵ�ʱ��
    BOOL                bPlay;          // �Ƿ��ڲ���
    BOOL                bSaveAV;        // ����AV�ļ�
    BOOL                bSaveImage;     // ����һ֡��Ƶ
    BOOL                bShowBigVideo;  // ��ʾ�����Ƶ

    INT                 x;              // ��Ƶ��ʾ��λ��
    INT                 y;
    INT                 x1;             // ����Ƶ��ʾλ��
    INT                 y1;
    INT                 cx;             // ����Ƶ���
    INT                 cy;
    HDC                 hDC;            // ��ʾ��DC
    HDRAWDIB            hDib;
    BITMAPINFOHEADER    *pBmpHeader;    // λͼ����Ϣ
    CH264               H264;           // H264����

    CKeyList            AVList;         // ����Ƶ�������ݰ�,����Ϊʱ��

    PPACK_ARRAY         AVArray;        // ���ڲ����ļ�
    BYTE                ePlayType;      // ������ͣ/ֹͣ/���/����

    DWORD               dwCreateTime;   // �����ļ�ʱ��ʱ��
    CFile               File;           // ������ļ�
    CString             strFile;        // �ļ���
    HWND                hWnd;           // ��Ƶ��ʾ����,���ڷ��Ͳ�������Ϣ

}PLAYINFO, *PPLAYINFO;

typedef CMap<DWORD, DWORD, PPLAYINFO, PPLAYINFO>    PLAYINFO_MAP;
typedef CList<PPLAYINFO, PPLAYINFO>                 PLAYINFO_LIST;


class CAVPlay;

typedef struct _tagTHREADPARM
{
    CAVPlay *pThis;
    DWORD    dwId;
    HWND     hWnd;
}THREADPARM, *PTHREADPARM;


/**
    ����AV����,��Ƶ����Ƶ����һ���,��ʱ��IDΪ����ʱ��
**/
class CAVPlay
{
public:
    CAVPlay();
    ~CAVPlay();

protected:
    CRITICAL_SECTION    m_csPlay;           // �����ٽ���
    HANDLE		        m_hExit;            // �˳��¼�

    CBuffPool           *m_pBigBuffPool;    // ��Ļ�����
    CBuffPool           *m_pSubBuffPool;    // С�Ļ�����
    
    PLAYINFO_MAP        m_mapPlayAV;        // �Ѿ����յ�������
    PLAYINFO_LIST       m_lstFree;          // ���е�

    CG729               m_G729;             // ��ƵG729�����,��ͬͨ������Ƶ����ͬһ��
    CWaveOut			m_WaveOut;          // ��Ƶ�Ĳ���
    char                m_szAudio[16000];   // ��Ƶ���ݵĻ�����

    enum {AUDIO = 1, VIDEO, VIDEO_BIG};

protected:
    void NewPlayer(PPLAYINFO *ppPlayInfo, DWORD dwChanId, LPCSTR strFile, HDC hDC, HWND hWnd, int x, int y,
        BOOL bShowBigVideo, int x1, int y1, int cx, int cy);
    void DelPlayer(PPLAYINFO pPlayInfo, BOOL bDel);
    void SaveAV(BYTE byType, CFile *pFile, WORD wSleep, PBYTE pbyData, WORD wLen);
    void SaveBmp(PPLAYINFO pPlayInfo, PBYTE pbyData);
    void Destory(void);    
    void ProcessPlayAV(DWORD dwAVId);
    void ProcessPreFile(DWORD dwAVId);
    void ProcessPlayFile(DWORD dwAVId);
    
    static unsigned __stdcall PlayAVThread(void* pParam);
    static unsigned __stdcall PlayFileThread(void* pParam);

public:
    void  Init(CBuffPool *pBigBuffPool, CBuffPool *pSubBuffPool);
    void  AddPlayer(DWORD dwChanId, HDC hDC, int x, int y, BOOL bShowBigVideo, int x1, int y1, int cx, int cy);
    void  AddPlayer(DWORD dwChanId, LPCSTR strFile, HWND hWnd, int x, int y, BOOL bShowBigVideo, int x1, int y1, int cx, int cy);
    void  DelPlayer(DWORD dwChanId);
    void  SetPlayFile(DWORD dwChanId, BYTE ePlayType);
    void  StartSaveFile(DWORD dwChanId, LPCSTR strFile);
    DWORD StopSaveFile(DWORD dwChanId);
    void  SaveImage(DWORD dwChanId, LPCSTR strFile);
    void  StartShowBigVideo(DWORD dwChanId);
    void  StopShowBigVideo(DWORD dwChanId);
    BOOL  AddAV(DWORD dwChanId, PPACK pFrame);
};