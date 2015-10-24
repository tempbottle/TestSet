#include "StdAfx.h"
#include ".\sink.h"

CSink::CSink(void)
: m_pEdit(NULL)
{
}

CSink::~CSink(void)
{
}

void CSink::SetResultWnd(CEdit * pEdit)
{
	m_pEdit = pEdit;
}

// STDMETHODIMP �Ǻ꣬�ȼ��� long __stdcall
STDMETHODIMP CSink::QueryInterface(const struct _GUID &iid,void ** ppv)
{
	*ppv=this;
	return S_OK;
}

ULONG __stdcall CSink::AddRef(void)
{	return 1;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

ULONG __stdcall CSink::Release(void)
{	return 0;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

STDMETHODIMP CSink::GetTypeInfoCount(unsigned int *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetIDsOfNames(const IID &,LPOLESTR *,UINT,LCID,DISPID *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::Invoke(
				long dispID,
				const struct _GUID &,
				unsigned long,
				unsigned short,
				struct tagDISPPARAMS * pParams,
				struct tagVARIANT *,
				struct tagEXCEPINFO *,
				unsigned int *)
{							// ֻ��Ҫʵ��������㹻��
	switch(dispID)
	{
	case 1:	//���ݲ�ͬ��dispID,��ɲ�ͬ�Ļص�����
		if( !m_pEdit )
		{
			AfxMessageBox( _T("û�е��� SetResultWnd() ������ʾ����Ĵ���") );
		}
		else
		{
			CString str;
			str.Format( _T("%d"), pParams->rgvarg[0].lVal );
			m_pEdit->SetWindowText( str );
		}
		break;
	default:
		AfxMessageBox( _T("��ô���ܣ�������û���������ĺ���ѽ") );
		break;
	}
	return S_OK;
}
