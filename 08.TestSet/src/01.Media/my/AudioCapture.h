/*------------------------------------------------------------------------------*\
 [ģ������]
	CWaveIn

 [�ļ�����]
	WaveIn.h

 [����ļ�]
	WaveIn.cpp

 [Ŀ��] 
     ������Ƶ�ɼ�  
                                            
\*------------------------------------------------------------------------------*/

#ifndef _WAVEIN_H_
#define _WAVEIN_H_

typedef struct _tagRIFF_HEADER
{
    char  szRiffID[4];     // 'R','I','F','F'
    DWORD dwRiffSize;      // file size - 8
    char  szRiffFormat[4]; // 'W','A','V','E'
}RIFF_HEADER;

typedef struct _tagWAVE_FORMAT
{
    WORD  wFormatTag;
    WORD  wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD  wBlockAlign;
    WORD  wBitsPerSample;
}WAVE_FORMAT;

typedef struct _tagFMT_BLOCK
{
    char  szFmtID[4];		// 'f','m','t',' '
    DWORD dwFmtSize;		// ��ֵΪ16��18��18������ָ�����Ϣ
    WAVE_FORMAT wavFormat;
}FMT_BLOCK;

typedef struct _tagFACT_BLOCK// ��ѡ
{
	char  szFactID[4];		// 'f','a','c','t'
	DWORD dwFactSize;
	DWORD dwDataSize;
}FACT_BLOCK; 

typedef struct _tagDATA_BLOCK
{
    char szDataID[4];		// 'd','a','t','a'
    DWORD dwDataSize;
}DATA_BLOCK;

#define WISA_POOR	8000
#define WISA_LOW	11025
#define WISA_NORMAL	22050
#define WISA_HIGH	44100

#define NUM_BUF 10
//#define SIZE_AUDIO_FRAME 960
//#define SIZE_AUDIO_PACKED 60
#define SIZE_AUDIO_FRAME 15840
#define SIZE_AUDIO_PACKED 990
#include <mmsystem.h>

//#define G729_ENCODE
#ifdef G729_ENCODE
#include "G729.h"
#endif

//#define PLAY
#ifdef PLAY
#include "WaveOut.h"
#endif

class CWaveIn
{
public:
	//�̴߳���
	static DWORD WINAPI AudioInThreadProc(LPVOID lpParameter);	

	//��ȡ����
	virtual void ProcAudio(char *pszData, DWORD dwLen) = 0;

public:	
	//����¼��
	BOOL StartRec();
	//ֹͣ¼��
	BOOL StopRec();

	//��ʼ¼��
	BOOL OpenRecord();
	//����¼��
	BOOL CloseRecord();

	//��ȡʵ��
	inline DWORD GetInstance();
	//��ȡ����λ��
	inline WORD GetBit();
	//��ȡ��������
	inline DWORD GetSample();
	//��ȡƵ����
	inline WORD GetChannel();

	//���ò���λ��
	inline void SetBit(WORD wBit);
	//���ò�������
	inline void SetSample(DWORD dwSample);
	//���ò���Ƶ����
	inline void SetChannel(WORD wChannel);

	//��ȡ������Ϣ
	inline MMRESULT GetLastMMError();
	CString GetLastErrorString();

    //�����wav�ļ�
    void SaveFile(LPCSTR strFileName, bool bCreate, char *pszData, DWORD dwLen);

	//���캯��
	CWaveIn();
	//��������
	virtual ~CWaveIn();	

#ifdef G729_ENCODE
	//G729ѹ��
	CG729 m_G729;
#endif

#ifdef PLAY
	//��������
	CWaveOut m_WaveOut;
#endif

protected:
	//���豸
	BOOL OpenDev();
	//�ر��豸
	BOOL CloseDev();

	//ֹͣ�߳�
	BOOL StopThread();
	//�����߳�
	BOOL StartThread();

	//׼������
	BOOL PerPareBuffer();
	//�ͷŻ���
	BOOL FreeBuffer();

protected:
	//�û�ʵ������
	static DWORD s_dwInstance;	

protected:
	//Ƶ����
	WORD m_wChannel;
	//��������
	DWORD m_dwSample;
	//����λ��
	WORD m_wBit;

protected:
	//��Ƶ�����豸���
	HWAVEIN m_hIn;
	//�������÷�����Ϣ
	MMRESULT m_mmr;
	//�ص�����ָ��
	DWORD m_dwAudioInId;
	//�߳̾��
	HANDLE m_hAudioIn;
	//WAVEHDR�ṹ����ָ��
	WAVEHDR* m_pHdr;

	//�߳�������־
	BOOL m_bThreadStart;
	//�豸�򿪱��
	BOOL m_bDevOpen;
	//�ڴ������
	BOOL m_bAllocBuffer;
	//¼����ʼ���
	BOOL m_bRecord;
};

#endif