
/**
	�ɵõ����������̺�ϵͳ�ĸ��ּ�������ֵ��������������ȫ����ֵ���Ϳ������/������/���ܾ���ͬ���Ĺ���
*/
#pragma once
#include <Pdh.h>

class CSystemCounter
{
public:
	CSystemCounter();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CSystemCounter();

	bool OpenPDH(void);
	void ClosePDH(void);

	bool GetObjects(CListCtrl *pListObject);
	bool GetCounters(LPCTSTR lpcsObject, CListCtrl *pListCounter, CListCtrl *pListInstances);
	bool GetValue(LONG &lValue, LPTSTR lpcsObject, LPTSTR lpcsCounter, LPTSTR lpcsInstance = NULL);

protected:
	void InsertListItem(CListCtrl *pList, LPTSTR lpsInfo, int nInfoLen);	

private:
	HQUERY		m_hQuery;
	PDH_STATUS	m_pdhStatus;

	LPTSTR		m_lpsMachineName;
	LPTSTR		m_lpsObjectListBuffer;
	DWORD		m_dwObjectListSize;
	LPTSTR		m_lpsCounterListBuffer;	
	DWORD		m_dwCounterListSize;
	LPTSTR		m_lpsInstanceListBuffer;	
	DWORD		m_dwInstanceListSize;
};
