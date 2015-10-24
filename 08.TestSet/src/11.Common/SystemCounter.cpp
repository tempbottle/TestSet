// SystemCounter.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SystemCounter.h"
#include <PdhMsg.h>
#pragma comment(lib , "Pdh.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CSystemCounter::CSystemCounter()
	:m_hQuery(NULL),
	m_pdhStatus(0),
	m_lpsMachineName(NULL),
	m_lpsObjectListBuffer(NULL),
	m_dwObjectListSize(0),
	m_lpsCounterListBuffer(NULL),
	m_dwCounterListSize(0),
	m_lpsInstanceListBuffer(NULL),
	m_dwInstanceListSize(0)
{
}

CSystemCounter::~CSystemCounter()
{
	ClosePDH();
}

bool CSystemCounter::OpenPDH(void)
{
	m_pdhStatus = PdhOpenQuery(0, 0, &m_hQuery);

	if (ERROR_SUCCESS != m_pdhStatus) return false;

	return true;
}

void CSystemCounter::ClosePDH(void)
{
	// �رռ�����
	if (NULL != m_hQuery)
	{
		PdhCloseQuery(m_hQuery);
		m_hQuery = NULL;
	}

	if (NULL != m_lpsObjectListBuffer)
	{
		delete[] m_lpsObjectListBuffer;
		m_lpsObjectListBuffer = NULL;
	}

	if (NULL != m_lpsCounterListBuffer)
	{
		delete[] m_lpsCounterListBuffer;
		m_lpsCounterListBuffer= NULL;
	}

	if (NULL != m_lpsInstanceListBuffer)
	{
		delete[] m_lpsInstanceListBuffer;
		m_lpsInstanceListBuffer= NULL;
	}
}

bool CSystemCounter::GetObjects(CListCtrl *pListObject)
{
	if (NULL == m_hQuery)
		OpenPDH();

	if (NULL == m_hQuery) return false;

	if (NULL != m_lpsObjectListBuffer) delete[] m_lpsObjectListBuffer;

	// ö�ټ�����Ŀ
	// ��һ�εõ���������С���ڶ��εõ���
	m_pdhStatus = PdhEnumObjects(
		NULL,
		m_lpsMachineName,
		m_lpsObjectListBuffer,
		&m_dwObjectListSize,
		PERF_DETAIL_WIZARD,
		TRUE);

	m_lpsObjectListBuffer = new TCHAR[m_dwObjectListSize + 1];
	if (m_lpsObjectListBuffer == NULL) return false;

	m_pdhStatus = PdhEnumObjects(
		NULL,
		m_lpsMachineName,
		m_lpsObjectListBuffer,
		&m_dwObjectListSize,
		PERF_DETAIL_WIZARD,
		TRUE);

	if (ERROR_SUCCESS != m_pdhStatus) return false;

	InsertListItem(pListObject, m_lpsObjectListBuffer, m_dwObjectListSize);

	return true;	
}
bool CSystemCounter::GetCounters(LPCTSTR lpcsObject, CListCtrl *pListCounter, CListCtrl *pListInstances)
{
	if (NULL == m_hQuery)
		OpenPDH();

	if (NULL == m_hQuery) return false;

	if (NULL != m_lpsCounterListBuffer) delete[] m_lpsCounterListBuffer;
	if (NULL != m_lpsInstanceListBuffer) delete[] m_lpsInstanceListBuffer;

	m_pdhStatus = PdhEnumObjectItems(
		NULL,                   // [IN]����Դ��NT4.0����ΪNULL
		m_lpsMachineName,       // [IN]�����������ػ���ΪNULL
		lpcsObject,				// [IN]������������
		m_lpsCounterListBuffer, // [OUT]���ռ������б�Ļ�����������������б���Ϊ0�������Ϊ��
		&m_dwCounterListSize,	// [IN/OUT]���û���ռ������б���
		m_lpsInstanceListBuffer,// [OUT]����ʵ���б�Ļ�����������������б���Ϊ0�������Ϊ��
		&m_dwInstanceListSize,	// [IN/OUT]���û����ʵ���б���
		PERF_DETAIL_WIZARD,		// ��ȡ��Ϣ�ļ���
								// PERF_DETAIL_NOVICE �������� 
								// PERF_DETAIL_ADVANCE �߼����𣨰���������
								// PERF_DETAIL_EXPERT ר�Ҽ��𣨰��������͸߼���
								// PERF_DETAIL_WIZARD ϵͳ���𣨰������м���
		0);						// ���һ������ϵͳ����Ϊ0

	m_lpsCounterListBuffer = new TCHAR[m_dwCounterListSize + 1];
	if (NULL == m_lpsCounterListBuffer) return false;

	m_lpsInstanceListBuffer = new TCHAR[m_dwInstanceListSize + 1];
	if (NULL == m_lpsInstanceListBuffer) return false;

	// ��ȡָ����������������м�������ʵ��
	m_pdhStatus = PdhEnumObjectItems(
		NULL,
		m_lpsMachineName,
		lpcsObject,
		m_lpsCounterListBuffer,
		&m_dwCounterListSize,
		m_lpsInstanceListBuffer,
		&m_dwInstanceListSize,
		PERF_DETAIL_WIZARD,
		0);

	if (m_pdhStatus != ERROR_SUCCESS) return false;

	InsertListItem(pListCounter, m_lpsCounterListBuffer, m_dwCounterListSize);
	InsertListItem(pListInstances, m_lpsInstanceListBuffer, m_dwInstanceListSize);

	return true;
}

bool CSystemCounter::GetValue(LONG &lValue, LPTSTR lpcsObject, LPTSTR lpcsCounter, LPTSTR lpcsInstance/* = NULL*/)
{
	if (NULL == m_hQuery)
		OpenPDH();

	if (NULL == m_hQuery) return false;

	TCHAR szBuffer[256] = TEXT("");
	DWORD dwBufferSize = sizeof(szBuffer);

	HCOUNTER hCouterSet = {0};	
	PDH_FMT_COUNTERVALUE pdhfmtValue = {0};
	PDH_COUNTER_PATH_ELEMENTS pdhCpe = {0}; 

	pdhCpe.szMachineName = m_lpsMachineName;
	pdhCpe.szObjectName = lpcsObject; 
	pdhCpe.szCounterName = lpcsCounter;
	pdhCpe.szInstanceName = lpcsInstance;
	pdhCpe.szParentInstance = NULL; 
	pdhCpe.dwInstanceIndex = -1;

	// get path
	m_pdhStatus = PdhMakeCounterPath(&pdhCpe, szBuffer, &dwBufferSize, 0);
	if (ERROR_SUCCESS != m_pdhStatus) return false;

	// add counter
	m_pdhStatus = PdhAddCounter(m_hQuery, szBuffer, 0, &hCouterSet); 
	if (ERROR_SUCCESS != m_pdhStatus) return false;

	// get the data. 
	m_pdhStatus = PdhCollectQueryData(m_hQuery);
	if (ERROR_SUCCESS != m_pdhStatus) return false;

	// format counter values. 
	m_pdhStatus = PdhGetFormattedCounterValue(hCouterSet, PDH_FMT_LONG | PDH_FMT_NOSCALE, NULL, &pdhfmtValue);
	if (ERROR_SUCCESS != m_pdhStatus) return false;

	lValue = pdhfmtValue.longValue;

	return true;
}


void CSystemCounter::InsertListItem(CListCtrl *pList, LPTSTR lpsInfo, int nInfoLen)
{
	if (NULL == pList) return;

	int nSubLen = 0;

	LPTSTR lpsHead = lpsInfo;

	while (NULL != lpsHead && (nSubLen = (int)_tcslen(lpsHead)) > 0 && nInfoLen > 0)
	{
		pList->InsertItem(pList->GetItemCount(), lpsHead);

		lpsHead += nSubLen + 1;
		nInfoLen -= nSubLen + 1;
	}
}