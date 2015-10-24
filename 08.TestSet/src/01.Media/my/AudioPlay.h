/*------------------------------------------------------------------------------*\
 [ģ������]
	CWaveOut

 [�ļ�����]
	CWaveOut.h

 [����ļ�]
	CWaveOut.cpp

 [Ŀ��] 
     ������Ƶ����
     	  
 [����]		
	��װ waveout api
	ֻʹ���� pcm ��ʽ
                                               
\*------------------------------------------------------------------------------*/

#ifndef _WAVEOUT_H_
#define _WAVEOUT_H_

#define WOSA_POOR	8000
#define WOSA_LOW	11025
#define WOSA_NORMAL	22050
#define WOSA_HIGH	44100

#define PLAY_DELAY 10
#define SIZE_AUDIO_FRAME 15840

#include <mmsystem.h>
#include <afxmt.h>

//#define G729_DECODE
#ifdef G729_DECODE
#include "G729.h"
#endif

class CWaveOut  
{
public:
	//�̴߳���
	static DWORD WINAPI AudioOutThreadProc(LPVOID lpParameter);

public:
	//�����ļ����ø�ʽ
	BOOL SetFormatByFile(CString file);
	//����
	BOOL Play(char *pszBuf, DWORD dwLen);

	//��ʼ����
	BOOL StartPlay();
	//ֹͣ����
	BOOL StopPlay();	

	//��ȡ������Ŀ
	inline int GetBufferNum();
	//���ٻ�����Ŀ
	inline void BufferSub();
	//���ӻ�����Ŀ
	inline void BufferAdd();

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
	//����Ƶ����
	inline void SetChannel(WORD wChannel);

	//��ȡ������Ϣ
	inline MMRESULT GetLastMMError();
	CString GetLastErrorString();

	//���캯��
	CWaveOut();
	//��������
	virtual ~CWaveOut();

#ifdef G729_DECODE
	//G729ѹ��
	CG729 m_G729;
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
	//�������÷�����Ϣ
	MMRESULT m_mmr;
	//�����豸���
	HWAVEOUT m_hOut;
	//�߳̾��
	HANDLE m_hAudioOut;
	//�ص�����ָ��
	DWORD m_dwAudioOutId;

	//������Ŀ
	int m_iBufferNum;
	//��Ҫ����(critical section)
	CCriticalSection m_csLock;

	//�߳�������־
	BOOL m_bThreadStart;
	//�����豸�򿪱�־
	BOOL m_bDevOpen;

	//�̴߳���
	friend DWORD WINAPI AudioOutThreadProc(LPVOID lpParameter);
};

#endif
