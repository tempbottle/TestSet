#include "stdafx.h"
#include "ComPrint.h"


CComPrint::CComPrint()
	:m_hCom(NULL),
	m_bPortReady(FALSE),
	m_bWriteRC(FALSE),
	m_bReadRC(FALSE),
	m_dwBytesWritten(0),
	m_dwBytesRead(0)
{
	memset(&m_dcb, 0, sizeof(m_dcb));
	memset(&m_CommTimeouts, 0, sizeof(m_CommTimeouts));
}

CComPrint::~CComPrint()
{
	CloseHandle(m_hCom);
}

int CComPrint::OpenCOM(int nCmdNum)
{
	m_sComPort.Format(_T("COM%d"), nCmdNum);

	m_hCom = CreateFile(m_sComPort, 
		GENERIC_READ | GENERIC_WRITE,
		0,				// exclusive access
		NULL,			// no security
		OPEN_EXISTING,
		0,				// no overlapped I/O
		NULL);			// null template

	if (NULL != m_hCom)
	{
		m_bPortReady		= SetupComm(m_hCom, 128, 128);
		m_bPortReady		= GetCommState(m_hCom, &m_dcb);

		m_dcb.BaudRate		= 9600;
		m_dcb.ByteSize		= 8;
		m_dcb.Parity		= NOPARITY;
		m_dcb.StopBits		= ONESTOPBIT;
		m_dcb.fAbortOnError = TRUE;
		m_dcb.fBinary		= TRUE;

		m_bPortReady		= SetCommState(m_hCom, &m_dcb);
		m_bPortReady		= GetCommTimeouts (m_hCom, &m_CommTimeouts);

		m_CommTimeouts.ReadIntervalTimeout			= 50;
		m_CommTimeouts.ReadTotalTimeoutConstant		= 50;
		m_CommTimeouts.ReadTotalTimeoutMultiplier	= 10;
		m_CommTimeouts.WriteTotalTimeoutConstant	= 50;
		m_CommTimeouts.WriteTotalTimeoutMultiplier	= 10;

		m_bPortReady		= SetCommTimeouts (m_hCom, &m_CommTimeouts);

		return 0;
	}
	return -1;
}

void CComPrint::CloseCOM()
{
	CloseHandle(m_hCom);
	m_hCom = NULL;
}



// ѡ���ַ�������
// 27,54,		ʹ���ַ���1���ַ����д�ӡ���ַ���1�����ַ�224��,���뷶Χ20H-FFH��32-255������ASCII�ַ���������ͼ�η��ŵ�
// 27,55,		ʹ���ַ���2���ַ����д�ӡ���ַ���2�����ַ�224��,���뷶Χ20H-FFH��32-255�������£�����������Ƭ������
// 28,38,		���뺺�ַ�ʽ,���뷶Χ20H-7FH��32-127��

// ��ֽ����
// 10,			��ӡ����������,��ӡ����ǰ��ֽһ���ַ���
// 27,74,n		��ӡ����ǰn��, n �� 1 - 255 ֮��

// ��ʽ��������
// 27,49,n		Ϊ����Ļ�����������n���м��(��Ϊ�Զ�ֵ�����ܷŴ����С����Ӱ��), n �� 1 - 255 ֮��
// 27,112,n		�����ַ�֮��ļ��Ϊn��, n �� 0 - 255 ֮��
// 27,66, n1,n2...0,���봹ֱ���������ֵn1,n2,��������8�����ֵ
// 11			��ӡ��һ�������һ��ֱ���λ
// 27,68, n1,n2...0,����ˮƽ���������ֵn1,n2,��������8�����ֵ
// 9			��ӡ��һ�������һˮƽ���λ
// 27,102,m,n	���m=0,����ӡn���ո�,���m=1,,����ӡn������, n �� 1 - 255 ֮��
// 27,81,		��������,�Ҳ಻�಻��ӡ���ַ���
// 27,108,		��������,��಻�಻��ӡ���ַ���
// 27,109,n		�Ҷȴ�ӡ,nȡ1~6,1��ɫ��ǳ,6����,Ĭ��ֵΪ1

// �ַ���������
// 27,85,n		����Ŵ�,nȡ1~8,16�����ӡ��Ϊn=2,24����n=1
// 27,86,n		����Ŵ�,nȡ1~8,16�����ӡ��Ϊn=2,24����n=1
// 27,87,n		������Ŵ�,nȡ1~8,16�����ӡ��Ϊn=2,24����n=1
// 27,45,n		n=1�����»��ߴ�ӡ,n=0�������»��ߴ�ӡ
// 27,43,n		n=1�����ϻ��ߴ�ӡ,n=0�������ϻ��ߴ�ӡ
// 27,105,n		n=1�����״�ӡ,n=0��������ӡ
// 27,99,n		n=1�����ӡ,n=0�����ӡ
// 28,73,n		��ת6*8�ַ�,n=0Ϊ0��,,n=1Ϊ90��,n=2Ϊ180��,n=3Ϊ270��

// �û������ַ���������
// 27,38,m,n1,n2,...,ni, �û��Զ����ַ�,mΪ���û��Զ����ַ���ȡ(32~61)
// 27,37,m,n1,n2,...,ni, �滻�û��Զ����ַ�,mΪ���û��Զ����ַ���ȡ(32~61)
// 27,58,m,		�ָ��ַ����е��ַ�

// ͼ�δ�ӡ����
// 27,75,ml,mh,n1,n2,...,ni ��mh*256+ml

// ��ʹ������
// 27,64		��ʼ����ӡ��
// 13			�Ի������ڵ�������ַ����д���
// 27,100,n n=0��ӡ��������,n=1��ӡ������
// 28,118		���������ʹ�ӡ���Ƿ���ȱֽ״̬,00Ϊ����,04ȱֽ

// �������ӡ
// 29,87,n1,n2    n1Ϊ����խ���ߴ�,Ĭ��n1=3,n2Ϊ��������ߴ�
// 29,107,n,[d],0 nѡ���ӡ����ϵͳ,n=2ΪEAN-13,n=3ΪEAN-8,[d]Ϊ��ӡ�������ַ�
// 29,72,n        n=0����ӡHRI�ַ�,Ĭ��n=0,n=1Ϊ��ӡHRI
// 29,104,n       ���������,nȡ0~255,n=0Ϊ256��
// 29,119,n       ���������,nȡ0~4,Ĭ��n=3, n=1,խ����1,������3, n=2,խ����2,������3, n=3,խ����3,������7, n=4,խ����4,������9


// �����ӡ
void CComPrint::Print(CString csText)
{
	char szCmd[3];
	memset(szCmd,'\0',3);
	szCmd[0] = 27;
	szCmd[1] = 99;
	szCmd[2] = 1; //0,�����ӡ  1,�����ӡ

	PurgeComm(m_hCom, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);

	WriteFile(m_hCom, szCmd, 3, &m_dwBytesWritten, NULL);

	WriteFile(m_hCom, csText.GetString(), (DWORD)csText.GetLength(), &m_dwBytesWritten, NULL);
}


void CComPrint::WinPrint(CString csText)
{
	char szPrinter[200] = "";
	char *pszDevice = NULL;
	char *pszDriver = NULL;
	char *pszOutput = NULL;

	HDC hdcPrint = NULL; // ����һ���豸�������

	static DOCINFO di={sizeof(DOCINFO),"printer",NULL};

	// �õ��豸�ַ�����������szPrinter��
	GetProfileString("windows","device",",,,",szPrinter,80); 

	// ���豸�ַ����ֽ�
	if( (NULL != (pszDevice = strtok(szPrinter,","))) && 
		(NULL != (pszDriver = strtok(NULL,","))) &&
		(NULL != (pszOutput = strtok(NULL,","))))
	{
		// ����һ����ӡ���豸���
		if((hdcPrint = CreateDC(pszDriver,pszDevice, pszOutput, NULL)) != 0)
		{ 
			if (StartDoc(hdcPrint, &di) > 0)	//��ʼִ��һ����ӡ��ҵ 
			{ 
				StartPage(hdcPrint);			//��ӡ����ֽ,��ʼ��ӡ 
				SaveDC(hdcPrint);				//�����ӡ���豸���

				// �������
				CRect rcText(0,0,0,0);
				::DrawText(hdcPrint, csText, csText.GetLength(), &rcText, DT_CALCRECT);	// �����С
				::DrawText(hdcPrint, csText, csText.GetLength(), &rcText, DT_WORDBREAK);

				RestoreDC(hdcPrint,-1);			//�ָ���ӡ���豸��� 
				EndPage(hdcPrint);				//��ӡ��ֽͣ,ֹͣ��ӡ 
				EndDoc(hdcPrint);				//����һ����ӡ��ҵ 
			} 

			// ��API����DeleteDC����һ����ӡ���豸��� 
			DeleteDC(hdcPrint);
		}
	}
}

void CComPrint::PrintBmp(UINT nBmpId)
{
	char szPrinter[200] = "";
	char *pszDevice = NULL;
	char *pszDriver = NULL;
	char *pszOutput = NULL;

	HDC hdcPrint = NULL; // ����һ���豸�������

	static DOCINFO di={sizeof(DOCINFO),"printer",NULL};

	// �õ��豸�ַ�����������szPrinter��
	GetProfileString("windows","device",",,,",szPrinter,80); 

	// ���豸�ַ����ֽ�
	if( (NULL != (pszDevice = strtok(szPrinter,","))) && 
		(NULL != (pszDriver = strtok(NULL,","))) &&
		(NULL != (pszOutput = strtok(NULL,","))))
	{
		// ����һ����ӡ���豸���
		if((hdcPrint = CreateDC(pszDriver,pszDevice, pszOutput, NULL)) != 0)
		{ 
			if (StartDoc(hdcPrint, &di) > 0)	//��ʼִ��һ����ӡ��ҵ 
			{ 
				StartPage(hdcPrint);			//��ӡ����ֽ,��ʼ��ӡ 
				SaveDC(hdcPrint);				//�����ӡ���豸���

				#pragma warning(disable:4312)

				// ���ͼƬ
				HBITMAP hBitmap=::LoadBitmap(AfxGetInstanceHandle(),(LPCTSTR)nBmpId);
				BITMAP bitmap;
				::GetObject(hBitmap,sizeof(BITMAP),&bitmap);

				#pragma warning(default:4312)

				HDC dcMem;
				dcMem=::CreateCompatibleDC(hdcPrint);
				HBITMAP hOldBmp=(HBITMAP)::SelectObject(dcMem,hBitmap);

				int nVertCenterPos = ::GetDeviceCaps(hdcPrint, VERTRES) / 2;
				::StretchBlt(hdcPrint,1,50,400,bitmap.bmHeight,dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);

				::SelectObject(dcMem,hOldBmp);
				::DeleteDC(dcMem);
				::DeleteObject(hBitmap);

				RestoreDC(hdcPrint,-1); //�ָ���ӡ���豸��� 
				EndPage(hdcPrint); //��ӡ��ֽͣ,ֹͣ��ӡ 
				EndDoc(hdcPrint); //����һ����ӡ��ҵ 
			} 

			// ��API����DeleteDC����һ����ӡ���豸��� 
			DeleteDC(hdcPrint);
		}
	}
}

