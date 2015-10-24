// ExtRichEdit.cpp : implementation file
//

#include "stdafx.h"
#include "XTRichEdit.h"
#include ".\xtrichedit.h"



_AFX_RICHEDITEX_STATE::_AFX_RICHEDITEX_STATE()
{
    m_hInstRichEdit20 = NULL ;
}

_AFX_RICHEDITEX_STATE::~_AFX_RICHEDITEX_STATE()
{
    if (m_hInstRichEdit20 != NULL)
    {
        ::FreeLibrary(m_hInstRichEdit20) ;
    }
}

_AFX_RICHEDITEX_STATE _afxRichEditStateEx;


BOOL PASCAL AfxInitRichEditEx()
{
    if (!::AfxInitRichEdit())
    {
        return FALSE ;
    }
    _AFX_RICHEDITEX_STATE* l_pState = &_afxRichEditStateEx ;
    if (l_pState->m_hInstRichEdit20 == NULL)
    {
        l_pState->m_hInstRichEdit20 = LoadLibraryA("RICHED20.DLL") ;
    }
    return l_pState->m_hInstRichEdit20 != NULL ;
} 

CXTRichEdit::CXTRichEdit()
{
}

CXTRichEdit::~CXTRichEdit()
{
}

BOOL CXTRichEdit::Create(DWORD in_dwStyle,  const RECT& in_rcRect,
                             CWnd* in_pParentWnd, UINT in_nID)
{
    if (!::AfxInitRichEditEx())
    {
        return FALSE ;
    }
    CWnd* l_pWnd = this ;
    return l_pWnd->Create(_T("RichEdit20A"), NULL, in_dwStyle, 
                           in_rcRect, in_pParentWnd, in_nID);
}

BOOL CXTRichEdit::CreateEx1(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		LPVOID lpParam /* = NULL */)
{
/*	return CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
		rect,(pParentWnd != NULL) ? pParentWnd->GetSafeHwnd() : NULL,
		(HMENU) nID, lpParam);
*/
	if (!::AfxInitRichEditEx())
    {
        return FALSE ;
    }

	m_Rect = rect;

	CWnd* l_pWnd = this ;
    return l_pWnd->CreateEx(dwExStyle, _T("RichEdit20A"), NULL, dwStyle, 
                           rect, pParentWnd, nID, NULL);
}

BEGIN_MESSAGE_MAP(CXTRichEdit, CRichEditCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(EN_MSGFILTER, OnEnMsgfilter)
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CXTRichEdit::InitSettings()
{	
	unsigned mask = (unsigned)::SendMessage(m_hWnd, EM_GETEVENTMASK, 0, 0);
    ::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);
    ::SendMessage(m_hWnd, EM_AUTOURLDETECT, true, 0);
}

BOOL CXTRichEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CRichEditCtrl::PreCreateWindow(cs);
}

BOOL CXTRichEdit::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CRichEditCtrl::OnEraseBkgnd(pDC);	
}
/*
void CXTRichEdit::InsertFace(CString strPicPath)
{
	LPLOCKBYTES		lpLockBytes    = NULL;
	LPSTORAGE		lpStorage      = NULL;
	LPRICHEDITOLE	lpRichEditOle  = NULL;
	LPOLECLIENTSITE lpClientSite   = NULL;

	IGifAnimatorPtr	lpAnimator     = NULL;
	LPOLEOBJECT		lpObject       = NULL;

	HRESULT         hr             = S_OK;
	CLSID           clsid;

	try
	{
		// ����
		hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
		if (hr != S_OK)	AfxThrowOleException(hr);
		ASSERT(lpLockBytes != NULL);

		// ���������ĵ�
		hr = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &lpStorage);
		if (hr != S_OK)
		{
			VERIFY(lpLockBytes->Release() == 0);
			lpLockBytes = NULL;
			AfxThrowOleException(hr);
		}
		ASSERT(lpStorage != NULL);

		// �õ�OLEָ��
		lpRichEditOle = GetIRichEditOle();
		ASSERT(lpRichEditOle != NULL);

		// �õ��ͻ�վ
		lpRichEditOle->GetClientSite(&lpClientSite);
		ASSERT(lpClientSite != NULL);
	
		// ��ʹ��COM�ӿ�
		hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);		
		//hr = CoInitialize(NULL);
		if (FAILED(hr)) _com_issue_error(hr);
		
		// �õ�IGifAnimatorPtr����
		//here, I used a smart point, so I do not need to free it
		hr = lpAnimator.CreateInstance(CLSID_GifAnimator);	
		if (FAILED(hr)) _com_issue_error(hr);

		// ת���ַ���ʽ
		BSTR path = strPicPath.AllocSysString();

		// ����GIF
		hr = lpAnimator->LoadFromFile(path);
		if (FAILED(hr)) _com_issue_error(hr);
		
		// Ҫ�����OLE����ӿ�
		hr = lpAnimator.QueryInterface(IID_IOleObject, (void**)&lpObject);
		if (FAILED(hr)) _com_issue_error(hr);
		
		// ��������
		OleSetContainedObject(lpObject, TRUE);

		// �õ�ID
		hr = lpObject->GetUserClassID(&clsid);
		if (hr != S_OK) AfxThrowOleException(hr);

		SIZEL sizel;
		sizel.cx = sizel.cy = 0;

		// ����ṹ
		REOBJECT reobject = {0};
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.clsid    = clsid;
		reobject.cp       = REO_CP_SELECTION;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.dwFlags  = REO_BELOWBASELINE;
		reobject.dwUser   = 0;
		reobject.poleobj  = lpObject;
		reobject.polesite = lpClientSite;
		reobject.pstg     = lpStorage;
		reobject.sizel    = sizel;

		// ����OLE����
		hr = lpRichEditOle->InsertObject(&reobject);
		if (FAILED(hr)) _com_issue_error(hr);

		HWND hWndRT = this->m_hWnd;
		::SendMessage(hWndRT, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);

		// ��ʼ����
		VARIANT_BOOL ret = lpAnimator->TriggerFrameChange();
		
		// ��ʾ
		lpObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, lpClientSite, 0, m_hWnd, NULL);
		lpObject->DoVerb(OLEIVERB_SHOW, NULL, lpClientSite, 0, m_hWnd, NULL);
		
		// �ػ洰��
		RedrawWindow();

		if (lpObject)
		{
			lpObject->Release();
			lpObject = NULL;
		}
		if (lpClientSite)
		{
			lpClientSite->Release();
			lpClientSite = NULL;
		}
		if (lpRichEditOle)
		{
			lpRichEditOle->Release();
			lpRichEditOle = NULL;
		}
		if (lpStorage)
		{
			lpStorage->Release();
			lpStorage = NULL;
		}
		if (lpLockBytes)
		{
			lpLockBytes->Release();
			lpLockBytes = NULL;
		}		

		SysFreeString(path);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		::CoUninitialize();	
	}
}

// ����ole������
int CXTRichEdit::GetGifCount(void)
{
	LPRICHEDITOLE	lpRichEditOle = NULL;
	lpRichEditOle = GetIRichEditOle();
	int nCount = lpRichEditOle->GetObjectCount();
	if (lpRichEditOle)
	{
		lpRichEditOle->Release();
		lpRichEditOle = NULL;
	}
	return nCount;
}
bool CXTRichEdit::GetGifFileName(int nIndex, int &nPos, CString &strGifName)
{
	IGifAnimatorPtr	lpAnimator = NULL;
	BSTR bstrFileName;

	REOBJECT object;
	memset(&object, 0, sizeof(REOBJECT));
	object.cbStruct = sizeof(REOBJECT);

	LPRICHEDITOLE	lpRichEditOle = NULL;
	lpRichEditOle = GetIRichEditOle();
	if (NULL == lpRichEditOle)
		return false;
	
	HRESULT hr = lpRichEditOle->GetObject(nIndex, &object, REO_GETOBJ_POLEOBJ);
	if (hr != S_OK)
	{
		lpRichEditOle->Release();
		lpRichEditOle = NULL;
		return false;
	}

	nPos = (int)object.cp;
	
	hr = object.poleobj->QueryInterface(IID_IGifAnimator, (void**)&lpAnimator);
	if (hr == S_OK)
	{
		 bstrFileName = lpAnimator->GetFilePath();
		 CString strName(bstrFileName);
		 strName.Delete(0, strName.GetLength() - 8);
		 strGifName = strName;
	} 

	if (NULL != lpAnimator)
	{
		lpAnimator->Release();
		lpAnimator = NULL;
	}

	if (NULL != lpRichEditOle)
	{
		lpRichEditOle->Release();
		lpRichEditOle = NULL;
	}

//	SysFreeString(bstrFileName);

	return true;
}
*/
void CXTRichEdit::OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	// TODO:  �ؼ��������͸�֪ͨ����������д
	// CRichEditCtrl::OnInitDialog() �����Է��� EM_SETEVENTMASK ��Ϣ
	// ������ ENM_KEYEVENTS �� ENM_MOUSEEVENTS ��־�Ŀؼ�
	// �������㵽 lParam �����С�
	
	*pResult = 0;

	//----------------------------------------------------

	if (pMsgFilter->msg == WM_KEYDOWN || (pMsgFilter->msg == WM_KEYUP && pMsgFilter->wParam == 46))//����̧��DEL��ʱ��ˢ��һ��
	{
		this->GetParent()->InvalidateRect(&m_Rect);
	}

	if (pMsgFilter->msg == WM_VSCROLL || pMsgFilter->msg == WM_MOUSEWHEEL)
	{
		this->GetParent()->InvalidateRect(&m_Rect);
	}	
}

void CXTRichEdit::OnEnLink(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	*pResult = 0;

	//----------------------------------------------------

	switch (pEnLink->msg)
	{
	case WM_LBUTTONDOWN:
		{
			CString l_URL ;
			CHARRANGE l_CharRange ;
			CXTRichEdit *m_TempEdit;
			m_TempEdit = (CXTRichEdit*)CXTRichEdit::FromHandle(pEnLink->nmhdr.hwndFrom);
			m_TempEdit->GetSel(l_CharRange) ;
			m_TempEdit->SetSel(pEnLink->chrg) ;
			l_URL = m_TempEdit->GetSelText() ;
			m_TempEdit->SetSel(l_CharRange) ;

			CWaitCursor l_WaitCursor;
			ShellExecute(this->GetSafeHwnd(), _T("open"), l_URL, NULL, NULL, SW_SHOWNORMAL);

			break;
		}
	default:
		{
			break;
		}
	}	
}

void CXTRichEdit::OnMove(int x, int y)
{
	CRichEditCtrl::OnMove(x, y);

	m_Rect.left = x;
	m_Rect.top = y;
}

void CXTRichEdit::OnSize(UINT nType, int cx, int cy)
{
	CRichEditCtrl::OnSize(nType, cx, cy);

	m_Rect.right = m_Rect.left + cx;
	m_Rect.bottom = m_Rect.top + cy;
}
