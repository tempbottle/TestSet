
#include "stdafx.h"
#include "WaveOut.h"
#pragma comment(lib,"Winmm")

/*
�߳�    ״̬    �˳���

����    ����    600
����    ǿ��    601
����    �쳣    602
*/


DWORD CWaveOut::s_dwInstance = 0;

//�̴߳���
DWORD WINAPI CWaveOut::AudioOutThreadProc(LPVOID lpParameter)
{
	CWaveOut *pWaveIn;
	pWaveIn = (CWaveOut*)lpParameter;

	MSG msg;
	//��Ϣ������ѭ��
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		//�򿪲����豸
		case WOM_OPEN:
			break;

		//�رղ����豸
		case WOM_CLOSE:
			break;

		//�������
		case WOM_DONE:
			//������ʽ
			WAVEHDR* pwh = (WAVEHDR*)msg.lParam;
			//�ͷŲ�������
			waveOutUnprepareHeader((HWAVEOUT)msg.wParam, pwh, sizeof(WAVEHDR));
			//���ٲ���������Ŀ
			pWaveIn->BufferSub();
			//ɾ��Play����ʱ������ڴ�
			delete[] pwh->lpData;
			//ɾ��������ʽ
			delete pwh;
			break;
		}
	}

	//����
	return (DWORD)(msg.wParam);
}
//���캯�������ڳ�ʼ��
CWaveOut::CWaveOut():
	 m_wChannel(1),
	 m_dwSample(WOSA_POOR),//WISA_MIN
	 m_wBit(16),
     m_mmr(0),
     m_hOut(NULL),
     m_hAudioOut(NULL),
     m_dwAudioOutId(0),
     m_bThreadStart(FALSE),
     m_bDevOpen(FALSE)
{
    s_dwInstance++;
}
//��������
CWaveOut::~CWaveOut()
{
	StopPlay();
}
//�����߳�
BOOL CWaveOut::StartThread()
{
	//�߳��Ѿ�����
	if (m_bThreadStart)
	{
		//����
		return FALSE;
	}

	//�����߳�
	m_hAudioOut = CreateThread(0, 0, AudioOutThreadProc, this, 0, &m_dwAudioOutId);
	//����
	if (!m_hAudioOut)
	{
		//����
		return FALSE;
	}

	//�����߳��������
	m_bThreadStart = TRUE;
	//����
	return TRUE;
}
//ֹͣ�߳�
BOOL CWaveOut::StopThread()
{
	//�߳��Ѿ�ֹͣ
	if (!m_bThreadStart)
	{
		//����
		return FALSE;
	}

	//�߳̾����Ч
	if (m_hAudioOut)
	{
		//ѭ������
		int t=50;
		//�߳̽���״̬
		DWORD ExitCode;
		//�߳̽������
		BOOL bEnd = FALSE;

		//����WM_QUIT��Ϣ
		PostThreadMessage(m_dwAudioOutId, WM_QUIT, 0, 0);
		while(t)
		{
			//��ȡ�߳̽���״̬��Ϣ
			GetExitCodeThread(m_hAudioOut, &ExitCode);
			//�߳̽���
			if (ExitCode!= STILL_ACTIVE)
			{
				//�����߳���δ�������
				bEnd = TRUE;
				break;
			}

			//����10����
			else
			{
				Sleep(10);
			}
			//�޸�ѭ������
			t--;
		}

		//�߳���δ����
		if(!bEnd)
		{
			//�����߳�
			TerminateThread(m_hAudioOut, 601);
		}
		//������Ƶ�豸�����Ч
		m_hAudioOut = 0;
	}

	//�����߳�ֹͣ���
	m_bThreadStart = FALSE;
	//����
	return TRUE;
}
//�򿪲����豸
BOOL CWaveOut::OpenDev()
{
	//�豸�Ѿ���
	if (m_bDevOpen)
	{
		//����
		return FALSE;
	}

	//���ò��������ʽ
	WAVEFORMATEX wfx = {0};
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = m_wChannel;
	wfx.nSamplesPerSec = m_dwSample;
	wfx.nAvgBytesPerSec = m_wChannel * m_dwSample * m_wBit / 8;
	wfx.nBlockAlign = m_wBit * m_wChannel / 8;
	wfx.wBitsPerSample = m_wBit;
	wfx.cbSize = 0;

	//��鲥���豸�Ƿ�֧��������ʽ
	m_mmr = waveOutOpen (0,WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_QUERY);
	//����
	if (m_mmr)
	{
		//����
		return FALSE;
	}

	//�򿪲����豸
	m_mmr = waveOutOpen(&m_hOut, WAVE_MAPPER, &wfx, m_dwAudioOutId, s_dwInstance, CALLBACK_THREAD);
	//����
	if(m_mmr)
	{
		//����
		return FALSE;
	}

	//�����豸�򿪱��
	m_bDevOpen = TRUE;
	//��ʼ������������Ŀ
	m_iBufferNum = 0;
	//����
	return TRUE;
}
//�رղ����豸
BOOL CWaveOut::CloseDev()
{
	//�����豸�Ѿ��ر�
	if (!m_bDevOpen)
	{
		//����
		return FALSE;
	}

	//�����豸�����Ч
	if (!m_hOut)
	{
		//����
		return FALSE;
	}

	//�رղ����豸
	m_mmr = waveOutClose(m_hOut);
	//����
	if (m_mmr)
	{
		//����
		return FALSE;
	}

	//���ò����豸�����Ч
	m_hOut = 0;

	//���ò����豸�򿪱��
	m_bDevOpen = FALSE;
	//����
	return TRUE;
}
//��ʼ����
BOOL CWaveOut::StartPlay()
{
	//�������ر�־
	BOOL bRet=FALSE;

	//�����߳�ʧ��
	if (!StartThread())           
	{
		goto Exit;
	}

	//�򿪲����豸ʧ��
	if (!OpenDev())
	{
		goto Exit1;
	}

	//���ú������ر�־
	bRet = TRUE;
	goto Exit;
Exit1:
	//ֹͣ�߳�
	StopThread();
Exit:
	//����
	return bRet;
}
//ֹͣ����
BOOL CWaveOut::StopPlay()
{
	//�رղ����豸
	CloseDev();
	//�ر��߳�
	StopThread();
	//����
	return TRUE;
}

MMRESULT CWaveOut::GetLastMMError()
{
	return m_mmr;
}

CString CWaveOut::GetLastErrorString()
{
	TCHAR buffer[256];
	memset(buffer,0,sizeof(buffer));
	waveOutGetErrorText(m_mmr,buffer,256);
	return CString(buffer);
}

void CWaveOut::SetChannel(WORD wChannel)
{
	m_wChannel = wChannel;//(m_wChannel == wChannel) ? 2:1;
}

void CWaveOut::SetSample(DWORD dwSample)
{
	m_dwSample = dwSample;
}

void CWaveOut::SetBit(WORD wBit)
{
	m_wBit = (wBit == 8) ? 8:16;
}

DWORD CWaveOut::GetInstance()
{
	return s_dwInstance;
}

WORD CWaveOut::GetBit()
{
	return m_wBit;
}

DWORD CWaveOut::GetSample()
{
	return m_dwSample;
}

WORD CWaveOut::GetChannel()
{
	return m_wChannel;
}
//����
BOOL CWaveOut::Play(char *pszBuf, DWORD dwLen)
{
	try
	{
		//�����豸δ��
		if (!m_bDevOpen)
		{
			//����
			return FALSE;
		}

		//��ʱ̫��
		if (GetBufferNum () > PLAY_DELAY)
		{
			//����
			return TRUE;
		}

		//�½�WAVEHDR�ṹ�����ڽ��ղ�����ʽ
		LPWAVEHDR pwh = new WAVEHDR;
		//����
		if (!pwh)
		{
			//����
			return FALSE;
		}

        memset(pwh, 0, sizeof(WAVEHDR));
		pwh->lpData = new char[SIZE_AUDIO_FRAME];
		memset(pwh->lpData, 0, SIZE_AUDIO_FRAME);

#ifdef G729_DECODE
        m_G729.Decode(pszBuf, dwLen, pwh->lpData, (int*)&(pwh->dwBufferLength));
#else
        memcpy(pwh->lpData, pszBuf, dwLen);
        pwh->dwBufferLength = dwLen;
#endif		

		//Ϊ�ط���������׼��
		m_mmr = waveOutPrepareHeader(m_hOut, pwh, sizeof(WAVEHDR));
		//����
		if (m_mmr)
		{
			//����
			return FALSE;
		}

		//�����ݷ��������豸
		m_mmr = waveOutWrite(m_hOut, pwh, sizeof(WAVEHDR));
		//����
		if (m_mmr)
		{
			//����
			return FALSE;
		}

		//������Ŀ��1
		m_iBufferNum++;
		//����
	}
	catch(...)
	{
		TRACE("CWaveOut ERROR\n");
	}
	return TRUE;
}
//���ӻ�����Ŀ
void CWaveOut::BufferAdd()
{
	//��ס�ڴ�
	m_csLock.Lock();
	//������Ŀ��1
	m_iBufferNum++;
	//�������
	m_csLock.Unlock();
}
//���ٻ�����Ŀ
void CWaveOut::BufferSub()
{
	//��ס�ڴ�
	m_csLock.Lock();
	//������Ŀ��1
	m_iBufferNum--;
	//�������
	m_csLock.Unlock();
}
//��ȡ������Ŀ
int CWaveOut::GetBufferNum()
{
	//�м����
	int iTemp;
	//��ס�ڴ�
	m_csLock.Lock();
	//��û�����Ŀ
	iTemp = m_iBufferNum;
	//�������
	m_csLock.Unlock();
	//����
	return iTemp;
}

BOOL CWaveOut::SetFormatByFile(CString file)
{
	#pragma pack(push, 1)
	struct FileHeader
	{
		char cFlagFiff[4];
		unsigned __int32 iFileLen;
		char cFlagWave[4];
		char cFlagFmt[4];
		char cResove[4];
		unsigned __int16 cFormat;
		unsigned __int16 cChannel;
		unsigned __int32 cSample;
		unsigned __int32 cBytePerSec;
		unsigned __int16 cByteprocess;
		unsigned __int16 cBit;
		char cFlagDat[4];
		unsigned __int32 iLenData;
	};
	#pragma pack(pop)

	CFile fi;
	if (!fi.Open(file, CFile::modeRead,NULL))
	{
		return FALSE;
	};
	struct FileHeader head;
	fi.Read((void *)&head,sizeof (head));
	fi.Close();

	this->SetChannel(head.cChannel);
	this->SetSample(head.cSample);
	this->SetBit(head.cBit);
	return TRUE;
}
