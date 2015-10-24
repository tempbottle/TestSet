
#pragma once
#include "Sink.h"


class CWebInvoke
{
public:
    CWebInvoke();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
    virtual ~CWebInvoke();

protected:	
    CSink	m_sink;			// ����������
    CEvent	m_evComplete;
    LONG	m_lErrorCode;

    void SetProxy(LPCSTR pszProxy);
    void UnenableProxy(void);

public:	
    DWORD OpenUrl(LPCSTR strUrl, LPSTR strProxy, DWORD dwTime);
};
