#include "stdafx.h"
#include "WaveIn.h"
#include <math.h>
#pragma comment(lib,"Winmm")

/*
�߳�    ״̬    �˳���

¼��    ����    500
¼��    ǿ��    501
¼��    �쳣    502
*/


//�û�ʵ������
DWORD CWaveIn::s_dwInstance = 0;

//�̴߳���
DWORD WINAPI CWaveIn::AudioInThreadProc(LPVOID lpParameter)
{
    CWaveIn *pWaveIn;
    pWaveIn = (CWaveIn *)lpParameter;

#ifdef G729_ENCODE
    CG729* pG729 = &(pWaveIn->m_G729);
    
    if (NULL == pG729)
        AfxMessageBox(_T("CWaveIn ��������!"), MB_OK);
#endif  

    //����
    char szBuf[16000] = {""};
    DWORD dwLen = 0;

    //��Ϣ������ѭ��
    MSG msg;

    try
    {
        while (GetMessage(&msg, 0, 0, 0))
        {
            switch (msg.message )
            {
                //��¼���豸
            case MM_WIM_OPEN:
                break;

                //�ر�¼���豸
            case MM_WIM_CLOSE:
                break;

                //¼���豸��������
            case MM_WIM_DATA:
                //¼����ʽ
                WAVEHDR* pWH = (WAVEHDR*)msg.lParam;

                //�ͷŻ���
                waveInUnprepareHeader((HWAVEIN)msg.wParam, pWH, sizeof(WAVEHDR));

                //�Ƿ�����
                if (pWH->dwBytesRecorded != SIZE_AUDIO_FRAME)
                    break;
#ifdef G729_ENCODE
                //G729����
                pG729->Encode(pWH->lpData, pWH->dwBytesRecorded, szBuf, (int*)&dwLen);
#else
                dwLen = pWH->dwBytesRecorded;
                memcpy(szBuf, pWH->lpData, dwLen);
#endif

#ifdef PLAY
                //ֱ�Ӳ���
                pWaveIn->m_WaveOut.Play(szBuf, dwLen);
#else
                //��������
                pWaveIn->ProcAudio(szBuf, dwLen);
#endif

                //Ϊ��Ƶ�豸����һ������,׼������¼��
                waveInPrepareHeader((HWAVEIN)msg.wParam, pWH, sizeof(WAVEHDR));
                waveInAddBuffer((HWAVEIN)msg.wParam, pWH, sizeof(WAVEHDR));
                break;
            }
        }// end while
    }
    catch(...)
    {
        TRACE(_T("CWaveIn.ProcAudio Error %d\n"), ::GetLastError());
        return 502;
    }   

    //����
    return (DWORD)msg.wParam;
}
//���캯��
CWaveIn::CWaveIn()
    :m_wChannel(1),
    m_dwSample (WISA_POOR),//WISA_MIN
    m_wBit(16),
    m_hIn(NULL),
    m_mmr(0),
    m_dwAudioInId(0),
    m_hAudioIn(NULL),
    m_pHdr(NULL),
    m_bThreadStart(FALSE),
    m_bDevOpen(FALSE),
    m_bAllocBuffer(FALSE),
    m_bRecord(FALSE)
{
    s_dwInstance++;
}
//��������
CWaveIn::~CWaveIn()
{
    StopRec();
}

//�ر�¼���豸
BOOL CWaveIn::CloseDev()
{
    //�豸�Ѿ��ر�
    if (!m_bDevOpen)
    {
        //����
        return FALSE;
    }

    //�豸�����Ч
    if (!m_hIn)
    {
        //����
        return FALSE;
    }

    //�ر�¼���豸
    m_mmr = waveInClose(m_hIn);
    //����
    if (m_mmr)
    {
        //�����豸�����Ч
        m_hIn = 0;
        //�����豸�������
        m_bDevOpen = FALSE;
        //����
        return FALSE;
    }

    //�����豸�����Ч
    m_hIn = 0;
    //�����豸�������
    m_bDevOpen = FALSE;

    //����
    return TRUE;
}
//��¼���豸
BOOL CWaveIn::OpenDev()
{
    //�Ѿ���¼���豸
    if (m_bDevOpen)
    {
        //����
        return FALSE;
    }

    //����¼���豸�����ʽ
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = m_wChannel;
    wfx.nSamplesPerSec = m_dwSample;
    wfx.nAvgBytesPerSec = m_wChannel * m_dwSample * m_wBit / 8;
    wfx.nBlockAlign = m_wBit * m_wChannel / 8;
    wfx.wBitsPerSample = m_wBit;
    wfx.cbSize = 0;

    //���¼���豸�Ƿ�֧���趨�������ʽ
    m_mmr = waveInOpen(0, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_QUERY);
    //����
    if (m_mmr)
    {
        //����
        return FALSE;
    }

    //��¼���豸
    m_mmr = waveInOpen(&m_hIn, WAVE_MAPPER, &wfx, m_dwAudioInId, s_dwInstance, CALLBACK_THREAD);
    //����
    if (m_mmr)
    {
        //����
        return FALSE;
    }

    //�����豸�򿪱��
    m_bDevOpen = TRUE;

    //����
    return TRUE;
}
//�����߳�
BOOL CWaveIn::StartThread()
{
    //�߳��Ѿ�����
    if (m_bThreadStart)
    {
        //����
        return FALSE;
    }

    //�����߳�
    m_hAudioIn = CreateThread(0, 0, AudioInThreadProc, this, 0, &m_dwAudioInId);
    //����
    if (!m_hAudioIn)
    {
        //����
        return FALSE;
    }

    //�����߳��������
    m_bThreadStart = TRUE;

    //����
    return TRUE;
}
//�ر��߳�
BOOL CWaveIn::StopThread()
{
    //�߳��Ѿ�ֹͣ
    if (!m_bThreadStart)
    {
        //����
        return FALSE;
    }

    //�߳̾����Ч
    if (m_hAudioIn)
    {
        //ѭ������
        int t = 50;
        //�߳̽���״̬��Ϣ
        DWORD ExitCode;
        //�߳̽������
        BOOL bEnd=FALSE;

        //����WM_QUIT��Ϣ
        PostThreadMessage(m_dwAudioInId,WM_QUIT,0,0);

        //��ѭ��
        while (t)
        {
            //��ȡ�߳̽���״̬��Ϣ
            GetExitCodeThread(m_hAudioIn,&ExitCode);
            //��δ����
            if (ExitCode!= STILL_ACTIVE)
            {
                //�����߳̽���
                bEnd=TRUE;
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
        if (!bEnd)
        {
            //��ֹ�߳�
            TerminateThread(m_hAudioIn,501);
        }

        //¼���豸�����Ч
        m_hAudioIn=0;
    }

    //�����߳̽������
    m_bThreadStart = FALSE;
    //����
    return TRUE;
}

//����¼��
BOOL CWaveIn::StartRec()
{
#ifdef PLAY
    //��ʼ����
    m_WaveOut.StartPlay();
#endif

    //����ֵ
    BOOL bRet = FALSE;	

    //�����߳�ʧ��
    if (!StartThread())           
    {
        goto Exit;
    }

    //��¼���豸ʧ��
    if (!OpenDev())
    {
        goto Exit1;
    }

    //׼������ʧ��
    if (!PerPareBuffer())
    {
        goto Exit2;
    }
/*
    //��ʼ¼��
    if (!OpenRecord())
    {
        goto Exit3;
    }
*/
    //���÷���ֵ
    bRet = TRUE;
    goto Exit;
/*
    Exit3:
    //�ͷŻ���
    FreeBuffer();
*/
Exit2:
    //�ر�¼���豸
    CloseDev();

Exit1:
    //ֹͣ�߳�
    StopThread();

Exit:
    //����
    return bRet;
}

//��ֹ¼��
BOOL CWaveIn::StopRec()
{
    //ֹͣ¼��
    CloseRecord();
    //����1500����
    Sleep(800);
    //�ͷŻ���
    FreeBuffer();
    //�ر�¼���豸
    if (CloseDev())
    {
        //ֹͣ�߳�
        StopThread();
    }
    //����
    return TRUE;
}
//�����ڴ�
BOOL CWaveIn::PerPareBuffer()
{
    //�Ѿ������ڴ�
    if (m_bAllocBuffer)
    {
        //����
        return FALSE;
    }

    //����¼���豸
    m_mmr = waveInReset(m_hIn);
    //����
    if (m_mmr)
    {
        //����
        return FALSE;
    }

    //ѭ������
    UINT i;
    //�½�WAVEHDR�ṹ
    m_pHdr = new WAVEHDR[NUM_BUF];

    for (i = 0; i < NUM_BUF; i++)
    {
        //��ʼ��
        ZeroMemory(&m_pHdr[i], sizeof(WAVEHDR));
        //���λ���
        m_pHdr[i].lpData = new char[SIZE_AUDIO_FRAME];
        //�����С
        m_pHdr[i].dwBufferLength = SIZE_AUDIO_FRAME;
        m_pHdr[i].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        m_pHdr[i].dwLoops = 1;

        //׼������
        m_mmr = waveInPrepareHeader(m_hIn, &m_pHdr[i], sizeof(WAVEHDR));
        //����
        if (m_mmr)
        {
            //����
            return FALSE;
        }

        //��ӻ��浽¼���豸
        m_mmr = waveInAddBuffer(m_hIn, &m_pHdr[i], sizeof(WAVEHDR));
        //����
        if (m_mmr)
        {
            //����
            return FALSE;
        }
    }

    //�����ڴ������
    m_bAllocBuffer = TRUE;

    //����
    return TRUE;
}

//�ͷŻ���
BOOL CWaveIn::FreeBuffer()
{
    //�ڴ������Ϊ��
    if (!m_bAllocBuffer)
    {
        //����
        return FALSE;
    }

    //ѭ�����
    UINT i;
    //�ڴ��Ѿ��ͷ�
    if (!m_pHdr)
    {
        //����
        return FALSE;
    }

    for (i = 0; i < NUM_BUF; i++)
    {
        //ע��¼���豸
        m_mmr = waveInUnprepareHeader(m_hIn, &m_pHdr[i], sizeof(WAVEHDR));

        //����
/*		if (m_mmr)
        {
        //����100����
        Sleep(100);
        //����
        continue;
        }
*/		
        //�ͷ������ڴ浥Ԫ
        if (m_pHdr[i].lpData)
        {
            delete[] m_pHdr[i].lpData;
            m_pHdr[i].lpData = NULL;
        }
    }

    //�ͷ�WAVEHDR�ṹ
    delete []m_pHdr;
    m_pHdr = NULL;

    //�����ڴ������
    m_bAllocBuffer = FALSE;

    //����
    return TRUE;
}
//��ʼ¼��
BOOL CWaveIn::OpenRecord()
{
    //�Ѿ���ʼ¼��
    if (m_bRecord)
    {
        //����
        return FALSE;
    }

    //¼���豸�����Ч
    if(!m_hIn)
    {
        //����
        return FALSE;
    }

    //��ʼ¼��
    m_mmr = waveInStart(m_hIn);
    //����
    if (m_mmr)
    {
        //����
        return FALSE;
    }

    //����¼����ʼ���
    m_bRecord = TRUE;

    //����
    return TRUE;
}

//ֹͣ¼��
BOOL CWaveIn::CloseRecord()
{
    //�Ѿ�ֹͣ¼��
    if (!m_bRecord)
    {
        //����
        return FALSE;
    }

    //¼���豸�����Ч
    if(!m_hIn)
    {
        //����
        return FALSE;
    }

    //����¼���豸��ֹͣ¼��
    m_mmr = waveInReset(m_hIn);

    //����
    if (m_mmr)
    {
        //����
        return FALSE;
    }

    //����¼��ֹͣ���
    m_bRecord = FALSE;

    //����
    return TRUE;
}
/*
void CWaveIn::GetData(char *pBuffer,int iLen)
{
static int i = 0;
static SYSTEMTIME time,time1;
static long l = 0;
if (i == 0)
{
::GetLocalTime (&time);
}
::GetLocalTime (&time1);

int j = time1.wSecond - time.wSecond;
if (abs(j) >= 1 )
{
memcpy(&time,&time1,sizeof(time));
TRACE("Len / second %d \n",l);
l = 0;
}
l = l + iLen;
i++;
}
*/

MMRESULT CWaveIn::GetLastMMError()
{
    return m_mmr;
}

CString CWaveIn::GetLastErrorString()
{
    TCHAR buffer[256];
    memset(buffer,0,sizeof(buffer));
    waveInGetErrorText(m_mmr,buffer,256);
    return CString(buffer);
}

void CWaveIn::SetChannel(WORD wChannel)
{
    m_wChannel = (m_wChannel == wChannel) ? 2:1;
}

void CWaveIn::SetSample(DWORD dwSample)
{
    m_dwSample = dwSample;
}

void CWaveIn::SetBit(WORD wBit)
{
    m_wBit = (wBit == 8) ? 8:16;
}

DWORD CWaveIn::GetInstance()
{
    return s_dwInstance;
}

WORD CWaveIn::GetBit()
{
    return m_wBit;
}

DWORD CWaveIn::GetSample()
{
    return m_dwSample;
}

WORD CWaveIn::GetChannel()
{
    return m_wChannel;
}


void CWaveIn::SaveFile(LPCSTR strFileName, bool bCreate, char *pszData, DWORD dwLen)
{
    if (bCreate)
    {
        CFile file;
        if (file.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
        {
            RIFF_HEADER riff_header = {0};
            FMT_BLOCK   fmt_block   = {0};
            DATA_BLOCK  data_block  = {0};

            memcpy(riff_header.szRiffID, "RIFF", 4);
            memcpy(riff_header.szRiffFormat, "WAVE", 4);
            riff_header.dwRiffSize = sizeof(RIFF_HEADER) + sizeof(FMT_BLOCK) + sizeof(DATA_BLOCK) + dwLen - 8;

            memcpy(fmt_block.szFmtID, "fmt ", 4);
            fmt_block.dwFmtSize = sizeof(WAVE_FORMAT);
            fmt_block.wavFormat.wFormatTag = WAVE_FORMAT_PCM;
            fmt_block.wavFormat.wChannels = m_wChannel;
            fmt_block.wavFormat.dwSamplesPerSec = m_dwSample;
            fmt_block.wavFormat.dwAvgBytesPerSec = m_wChannel * m_dwSample * m_wBit / 8;
            fmt_block.wavFormat.wBlockAlign = m_wBit * m_wChannel / 8;
            fmt_block.wavFormat.wBitsPerSample = m_wBit;

            memcpy(data_block.szDataID, "data", 4);
            data_block.dwDataSize = dwLen;

            file.Write(&riff_header, sizeof(RIFF_HEADER));
            file.Write(&fmt_block, sizeof(FMT_BLOCK));
            file.Write(&data_block, sizeof(DATA_BLOCK));
            file.Write(pszData, dwLen);
            file.Close();
        }
    }
    else
    {
        CFile file;
        if (file.Open(strFileName, CFile::modeReadWrite))
        {
            RIFF_HEADER riff_header = {0};
            FMT_BLOCK   fmt_block   = {0};
            DATA_BLOCK  data_block  = {0};
			FACT_BLOCK  fat_block   = {0};
			WORD		wData		= 0;
			BOOL		bHaveFact	= FALSE;

            file.Read(&riff_header, sizeof(RIFF_HEADER));
            file.Read(&fmt_block, sizeof(FMT_BLOCK));

			if (18 == fmt_block.dwFmtSize)	// 18 ��2���ֽڵ���չ����
				file.Read(&wData, 2);

			file.Read(&fat_block, sizeof(FACT_BLOCK));

			if (0 != strncmp(fat_block.szFactID, "fact", 4))	// FACT_BLOCK�ǿ�ѡ��
				file.Seek(-12, CFile::current);
			else
				bHaveFact = TRUE;

			file.Read(&data_block, sizeof(DATA_BLOCK));

            riff_header.dwRiffSize += dwLen;
            fat_block.dwDataSize += dwLen;
			data_block.dwDataSize += dwLen;

            file.SeekToBegin();
            file.Write(&riff_header, sizeof(RIFF_HEADER));
            file.Write(&fmt_block, sizeof(FMT_BLOCK));
			if (18 == fmt_block.dwFmtSize) file.Seek(2, CFile::current);
			if (bHaveFact) file.Write(&fmt_block, sizeof(FMT_BLOCK));
            file.Write(&data_block, sizeof(DATA_BLOCK));
            file.SeekToEnd();
            file.Write(pszData, dwLen);
            file.Close();
        }
    }
}