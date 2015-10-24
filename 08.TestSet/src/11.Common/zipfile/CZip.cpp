
#include "stdafx.h"
#include "CZip.h"
#include "zip.h"
#include "unzip.h"


CZip::CZip(void)
{
}

CZip::~CZip(void)
{
}

// pszIn       [IN]  Ҫѹ��������
// nInDataLen  [IN]  Ҫѹ�������ݳ�
// ppszOut     [OUT] ѹ��������
// nOutDataLen [OUT] ѹ�������ݳ�
bool CZip::Compress(char *pszIn, UINT nInDataLen, char **ppszOut, UINT &nOutDataLen)
{
    HZIP hz = CreateZip(m_szBuf, BUFF_SIZE, 0);                                 // ѹ���ļ�,CreateZip(L"simple1.zip",0);

    if (ZR_OK == ZipAdd(hz, _T("pack"), pszIn, nInDataLen))						// ѹ��
    {
        if (ZR_OK == ZipGetMemory(hz, (void**)ppszOut, (ULONG*)&nOutDataLen))	// �õ�ѹ���������
        {
            CloseZip(hz);
            return true;
        }
    }

    CloseZip(hz);
    return false;
}

// pszIn       [IN]     ѹ��������
// nInDataLen  [IN]     ѹ�������ݳ�
// ppszOut     [IN/OUT] ��ѹ���������(����������Ϊ��)
// nOutDataLen [IN/OUT] ��ѹ��������ݳ�(��������)
bool CZip::UnCompress(char *pszIn, UINT nInDataLen, char *pszOut, UINT &nOutDataLen)
{
	if (NULL == pszOut) return false;

    int i = 0;
    ZIPENTRY ze = {0};
    HZIP hz = OpenZip(pszIn, nInDataLen, 0);

    if (ZR_OK == FindZipItem(hz, _T("pack"), true, &i, &ze))
    {
        if (ZR_OK == UnzipItem(hz, i, pszOut, nOutDataLen)) // ��ѹ
        {
            nOutDataLen = ze.unc_size;
            CloseZip(hz);
            return true;
        }
    }

    CloseZip(hz);
    return false;
}
