#pragma once
#include "Common\KeyList.h"
#include "Network\BuffPool.h"

class CAVList : public CKeyList
{
public:
	CAVList(void);
	~CAVList(void);

private:
	DWORD		m_dwAudioId;	// ��ƵID
	DWORD		m_dwVideoId;	// ��ƵID
	WORD		m_wLimitNum;	// �ڵ�����
	CBuffPool	*m_pBuffPool;

public:
	bool Init(CBuffPool* pBuffPool, WORD wLimitNum);
	bool AddLimitNode(DWORD dwKey, LPBYTE pbyData);
	virtual void DelAll(void);

	int EnumAll(void);
};
