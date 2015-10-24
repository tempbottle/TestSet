#include "StdAfx.h"
#include ".\sink.h"


/*
��COM����� ���ӵ�����Ӻ���Fire_OpenFile()��COM�е�����������ֱ�ӵ���Fire_OpenFile()
�ͻ�����ҪCSink��

1,���Ҫ�õ����ӵ�(connect point), �½�����ʱ���Ͳ�Ҫѡ�����Ի���Ҫѡ�д���ʹ��
2,���ATL�򵥶��󣬻�ATL�ؼ�ʱ��Ҫѡ�����ӵ�

ATL��com����������ҳ����ʱ������������¼��ģ��������RichEdit2���ؾͲ�����������¼�


template <class T>
class CProxy_IFileTransEvents : public IConnectionPointImpl<T, &__uuidof(_IFileTransEvents), CComDynamicUnkArray>
{
	//������ཫ�����������ɡ�
public:

	HRESULT Fire_OpenFile(BSTR bstrFileName)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int nConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < nConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				CComVariant avarParams[1];
				avarParams[0].vt = VT_BSTR;
				avarParams[0].bstrVal = bstrFileName;
				DISPPARAMS params = { avarParams, NULL, 1, 0 };

				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return E_NOTIMPL;
	}
};
*/


CSink::CSink(void)
    :m_dwRef(0),
    m_pevComplete(NULL),
    m_plErrorCode(NULL)
{
}

CSink::~CSink(void)
{
    m_dwRef--;
}

void CSink::SetParam(CEvent *pevComplete, LONG *plErrorCode)
{
    m_pevComplete = pevComplete;
    m_plErrorCode = plErrorCode;
}

// STDMETHODIMP �Ǻ꣬�ȼ��� long __stdcall
STDMETHODIMP CSink::QueryInterface(const struct _GUID &iid,void ** ppv)
{
    if (!ppv) return E_POINTER;

    *ppv = NULL;
    if (IsEqualIID(iid, __uuidof(IDispatch)) || IsEqualIID(iid, __uuidof(IUnknown)))
    {
        *ppv = (IDispatch *)this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG __stdcall CSink::AddRef(void)
{
    return ++m_dwRef;
}

ULONG __stdcall CSink::Release(void)
{
    if (--m_dwRef) return m_dwRef;
    //	delete this;
    return 0;
}

STDMETHODIMP CSink::GetTypeInfoCount(unsigned int *)
{ return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo **)
{ return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetIDsOfNames(const IID &,LPOLESTR *,UINT,LCID,DISPID *)
{ return E_NOTIMPL; }	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::Invoke(long dispID,
                           const struct _GUID &,
                           unsigned long,
                           unsigned short,
                           DISPPARAMS * pParams,
                           VARIANT *,
                           EXCEPINFO *,
                           unsigned int *)
{
    if (NULL == this)
        return S_OK;

    try
    {
        TRACE(_T("%d\n"), dispID);

        if (dispID == 271)	// DISPID_NAVIGATEERROR
        {
            VARIANT StatusCode = *((pParams->rgvarg[1]).pvarVal);

            if (StatusCode.lVal == -2146697211)
                *m_plErrorCode = 3;
            else
                *m_plErrorCode = StatusCode.lVal;

            m_pevComplete->SetEvent();

            char buf[200];
            sprintf(buf, "Navigate Error, code is: %d\n", StatusCode.lVal);//static_cast<int>(StatusCode.lVal));
            TRACE(buf);		
        }
        else if(dispID == 259)	// DISPID_DOCUMENTCOMPLETE
        {
            m_plErrorCode = 0;
            m_pevComplete->SetEvent();
        }
    }
    catch (...)
    {
        TRACE(_T("sink error %d\n"), ::GetLastError());
    }

    return S_OK;
}
