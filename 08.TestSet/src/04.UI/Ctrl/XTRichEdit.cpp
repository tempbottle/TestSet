// RichEdit.cpp 

#include "stdafx.h"
#include "XTRichEdit.h"
#include <afxole.h>


UINT WM_RICHEDIT_RETURN  = RegisterWindowMessage(_T("WM_RICHEDIT_RETURN"));

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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


IMPLEMENT_DYNAMIC(CXTRichEdit, CRichEditCtrl)
CXTRichEdit::CXTRichEdit()
	:parentWnd_(NULL)
{
}

CXTRichEdit::~CXTRichEdit()
{
	DeleteAll();
}

BEGIN_MESSAGE_MAP(CXTRichEdit, CRichEditCtrl)
	ON_NOTIFY_REFLECT(EN_MSGFILTER, OnEnMsgfilter)
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink) // ���ɴ������ ON_NOTIFY(EN_LINK, IDC_RICHEDIT_SHOW, OnEnLink)
	ON_WM_ERASEBKGND()	
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(EN_DROPFILES, OnEnDropfiles) // ����ק�ļ�
END_MESSAGE_MAP()

BOOL CXTRichEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CRichEditCtrl::PreCreateWindow(cs);
}

BOOL CXTRichEdit::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CXTRichEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((VK_RETURN == nChar) && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		::PostMessage(parentWnd_, WM_RICHEDIT_RETURN, NULL, NULL);
		return;
	}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTRichEdit::OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);

	// CRichEditCtrl::OnInitDialog() �����Է��� EM_SETEVENTMASK ��Ϣ
	// ������ ENM_KEYEVENTS �� ENM_MOUSEEVENTS ��־�Ŀؼ�
	// �������㵽 lParam �����С�

	*pResult = 0;

	//----------------------------------------------------

	if (pMsgFilter->msg == WM_KEYDOWN)
	{
		TRACE("%d\n", pMsgFilter->lParam);
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
			CXTRichEdit *pthis = (CXTRichEdit*)CXTRichEdit::FromHandle(pEnLink->nmhdr.hwndFrom);
			pthis->SetSel(pEnLink->chrg);

			char url[MAX_PATH] = "";
			pthis->GetSelText(url);

			TRACE("%s\n", url);

			ShellExecute(GetSafeHwnd(), "open", url, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
	default:break;
	}	
}

void CXTRichEdit::OnEnDropfiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENDROPFILES *pEnDropFiles = reinterpret_cast<ENDROPFILES *>(pNMHDR);
	// TODO:  �ؼ��������͸�֪ͨ����������д
	// CRichEditCtrl::OnInitDialog() �����Է��� EM_SETEVENTMASK ��Ϣ
	// ������ ENM_DROPFILES ��־�������㵽 lParam �����еĿؼ���

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	*pResult = 0;
}

/**
 *\fn         Init()
 *\brief      ��ʹ����
 */
void CXTRichEdit::Init()
{
	CWnd *wnd = GetParent();

	if (NULL != wnd)
	{
		parentWnd_ = wnd->GetSafeHwnd();
	}

	SetTargetDevice(NULL, 0);	// �Զ�����

	GetDefaultCharFormat(charfmt_);
	charfmt_.dwEffects &= ~CFE_AUTOCOLOR;
	charfmt_.dwEffects |= CFM_WEIGHT;
	
	LONG mask = GetEventMask();
	mask |= ENM_LINK;          // ������
    mask |= ENM_DROPFILES;     // ��ק�ļ�
	SetEventMask(mask);

	richEditOle_ = GetIRichEditOle();
}

/**
 *\fn         AddText(const char *text)
 *\brief      ����ı���
 *\param[in]  text  �ı����ݡ�
 */
void  CXTRichEdit::AddText(const char *text)
{
	ReplaceSel(text);
}

/**
 *\fn         AddText(const char *text, CHARFORMAT2 &format)
 *\brief      ��Ӹ�ʽ�ı���
 *\param[in]  text  �ı����ݡ�
 */
void  CXTRichEdit::AddText(const char *text, CHARFORMAT2 &format)
{
	SetSel(-1, -1);
	SetSelectionCharFormat(format);
	AddText(text);	
}

/**
 *\fn         AddText(const char *text, COLORREF color, int weight)
 *\brief      ����ı���
 *\param[in]  text   �ı����ݡ�
 *\param[in]  color  �ı���ɫ��
 *\param[in]  weight �ı��ֺš�
 */
void  CXTRichEdit::AddText(const char *text, COLORREF color, int weight)
{
	WORD lastWeight = charfmt_.wWeight;
	COLORREF lastColor = charfmt_.crTextColor;

	charfmt_.crTextColor = color;

	if (0 != weight)
	{
		charfmt_.wWeight = weight;
	}

	AddText(text, charfmt_);

	charfmt_.wWeight = lastWeight;
	charfmt_.crTextColor = lastColor;
}

/**
 *\fn         AddLink(const char *text, void *data)
 *\brief      ��ӳ������ı���
 *\param[in]  text   �ı����ݡ�
 *\param[in]  data   �������ݡ�
 */
KEY_AREA* CXTRichEdit::AddLink(const char *text, void *data)
{
	KEY_AREA *key = new KEY_AREA;
	if (NULL != key)
	{
		CHARFORMAT2 oldCharformat;
		GetSelectionCharFormat(oldCharformat);	// �õ��ɵ��ַ���ʽ

		CHARFORMAT2 charformat;
		charformat = oldCharformat;
		charformat.dwMask |= CFM_LINK;			// ������
		charformat.dwEffects |= CFE_LINK;
		AddText(text, charformat);

		SetSelectionCharFormat(oldCharformat);	// ���þɵ��ַ���ʽ

		CHARRANGE cr = {0};
		GetSel(cr);		
		cr.cpMin  = cr.cpMax - strlen(text);

		key->pos  = cr;
		key->data = data;
		key->type = KEY_TYPE_TEXT;
		key->id   = MakeID();

        keyMap_[key->id] = key;
	}
	return key;
}

/**
 *\fn         SetLink(int id, const char *text)
 *\brief      ��ӳ������ı���
 *\param[in]  id     ����ID��
 *\param[in]  text   �ı����ݡ�
 */
KEY_AREA* CXTRichEdit::SetLink(int id, const char *text)
{
	KEY_AREA_MAP::iterator iter = keyMap_.find(id);

	if (iter != keyMap_.end())
	{
		KEY_AREA *key = iter->second;

		CHARFORMAT2 oldCharformat;
		GetSelectionCharFormat(oldCharformat);	// �õ��ɵ��ַ���ʽ

		CHARFORMAT2 charformat;
		charformat = oldCharformat;
		charformat.dwMask |= CFM_LINK;			// ������
		charformat.dwEffects |= CFE_LINK;

		SetSel(key->pos);
		ReplaceSel(text);
		
		SetSelectionCharFormat(oldCharformat);	// ���þɵ��ַ���ʽ

		int offset = strlen(text) - (key->pos.cpMax - key->pos.cpMin);

		key->pos.cpMax += offset;				// ���¼���λ��		

		if (offset != 0)
		{
			KeyOffset(key, offset);			// ���¼��������Ŀؼ�λ��
		}

		return key;
	}

	return NULL;
}

/**
 *\fn         SetLink(KEY_AREA *key, const char *text)
 *\brief      ��ӳ������ı���
 *\param[in]  key    ��ָ�롣
 *\param[in]  text   �ı����ݡ�
 */
KEY_AREA* CXTRichEdit::SetLink(KEY_AREA *key, const char *text)
{
	return SetLink(key->id, text);
}

/**
 *\fn         AddImage(HBITMAP bitmap, void *data)
 *\brief      ���ͼƬ�ؼ��㡣
 *\param[in]  bitmap    BITMAP�����
 *\param[in]  data      �������ݡ�
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::AddImage(HBITMAP bitmap, void *data)
{
	KEY_AREA *key = new KEY_AREA;
	if (NULL != key)
    {
		key->id   = MakeID();
        key->type = KEY_TYPE_IMAGE;
        key->data = data;

        if (0 != InsertImage(bitmap, key->id))
        {
            delete key;
            return NULL;
        }

		SetSel(-1, -1);
		CHARRANGE cr = {0};
		GetSel(cr);
		cr.cpMin--;

		key->pos = cr;

        keyMap_[key->id] = key;
	}

	return key;
}

/**
 *\fn         AddImage(const char *filename, void *data)
 *\brief      ���ͼƬ�ؼ��㡣
 *\param[in]  filename  BITMAP�ļ�·������
 *\param[in]  data      �������ݡ�
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::AddImage(const char *filename, void *data)
{
	KEY_AREA *key = new KEY_AREA;
	if (NULL != key)
	{
		key->id   = MakeID();
		key->type = KEY_TYPE_IMAGE;
		key->data = data;

		if (0 != InsertImage(filename, key->id))
		{
			delete key;
			return NULL;
		}

		SetSel(-1, -1);
		CHARRANGE cr = {0};
		GetSel(cr);
		cr.cpMin--;

		key->pos = cr;

		keyMap_[key->id] = key;
	}

	return key;
}

/**
 *\fn         AddImage(const char *filename, CHARRANGE range, void *data)
 *\brief      ���ͼƬ�ؼ��㡣
 *\param[in]  filename  BITMAP�ļ�·������
 *\param[in]  range     λ�á�
 *\param[in]  data      �������ݡ�
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::AddImage(const char *filename, CHARRANGE range, void *data)
{
	KEY_AREA *key = new KEY_AREA;
	if (NULL != key)
	{
		key->id   = MakeID();
		key->type = KEY_TYPE_IMAGE;
		key->data = data;
		key->pos  = range;
		
		SetSel(range);

		if (0 != InsertImage(filename, key->id))
		{
			delete key;
			return NULL;
		}

		keyMap_[key->id] = key;
	}

	return key;
}

/**
 *\fn         SetImage(KEY_AREA *key, HBITMAP bitmap)
 *\brief      ����ͼƬ�ؼ��㡣
 *\param[in]  key		�ؼ��㡣
 *\param[in]  bitmap    λͼ��
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetImage(KEY_AREA *key, HBITMAP bitmap)
{
	if (NULL == key) return NULL;

	SetSel(key->pos);
	ReplaceSel("");

	if (0 != InsertImage(bitmap, key->id))
	{
		return NULL;
	}

	return key;
}

/**
 *\fn         SetImage(KEY_AREA *key, HBITMAP bitmap)
 *\brief      ����ͼƬ�ؼ��㡣
 *\param[in]  id		�ؼ���ID��
 *\param[in]  bitmap    λͼ��
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetImage(int id, HBITMAP bitmap)
{
	return SetImage(getKey(id), bitmap);
}

/**
 *\fn         SetImage(KEY_AREA *key, const char *filename)
 *\brief      ����ͼƬ�ؼ��㡣
 *\param[in]  key		�ؼ��㡣
 *\param[in]  filename  λͼ�ļ�����
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetImage(KEY_AREA *key, const char *filename)
{
	if (NULL == key) return NULL;

	SetSel(key->pos);
	ReplaceSel("");

	if (0 != InsertImage(filename, key->id))
	{
		return NULL;
	}

	return key;
}

/**
 *\fn         SetImage(int id, const char *filename)
 *\brief      ����ͼƬ�ؼ��㡣
 *\param[in]  id		�ؼ���ID��
 *\param[in]  filename  λͼ�ļ�����
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetImage(int id, const char *filename)
{
	return SetImage(getKey(id), filename);
}

/**
 *\fn         AddGif(const char *filename, void *data)
 *\brief      ���GIF�ļ���
 *\param[in]  filename	�ļ�����
 *\param[in]  data      �������ݡ�
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::AddGif(const char *filename, void *data)
{
	KEY_AREA *key = new KEY_AREA;
	if (NULL != key)
	{
		key->id   = MakeID();
		key->type = KEY_TYPE_IMAGE;
		key->data = data;

		if (0 != InsertGif(key, filename))
		{
			delete key;
			return NULL;
		}

		SetSel(-1, -1);
		CHARRANGE cr = {0};
		GetSel(cr);
		cr.cpMin--;

		key->pos = cr;

		keyMap_[key->id] = key;
	}

	return key;
}

/**
 *\fn         SetGif(KEY_AREA *key, const char *filename)
 *\brief      ����GIF�ļ���
 *\param[in]  key	    �ؼ��㡣
 *\param[in]  filename	�ļ�����
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetGif(KEY_AREA *key, const char *filename)
{
    if (NULL == key) return NULL;

    SetSel(key->pos);
    ReplaceSel("");

	if (0 != InsertGif(key, filename))
	{
		return NULL;
	}

    return key;
}

/**
 *\fn         SetGif(int id, const char *filename)
 *\brief      ����GIF�ļ���
 *\param[in]  id	    �ؼ���ID��
 *\param[in]  filename	�ļ�����
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::SetGif(int id, const char *filename)
{
    return SetGif(getKey(id), filename);
}

/**
 *\fn         AddTransFile(const char *filename,
 *\fn					  UINT64  fileLen,
 *\fn        			  const char *iconFilename,
 *\fn        			  const char *text,
 *\fn        			  void* data)
 *\brief      ��Ӵ����ļ��ؼ���
 *\param[in]  filename		�ļ�����
 *\param[in]  fileLen		�ļ�����
 *\param[in]  iconFilename	ͼ���ļ�����
 *\param[in]  text			�ı���Ϣ��
 *\param[in]  data			�������ݡ�
 *\return     ��ָ�롣
 */
KEY_AREA * CXTRichEdit::AddTransFile(const char *filename,
	UINT64  fileLen,
	const char *iconFilename,
	const char *text,
	void* data)
{
	KEY_AREA *key = new KEY_AREA;
	
	if (NULL != key)
	{
		key->id = MakeID();
		key->type = KEY_TYPE_TRANSFILE;
		key->data = data;
		
		if (0 != InsertTransFileCtrl(key, filename, fileLen, iconFilename, text))
		{
			delete key;
			return NULL;
		}
		
		SetSel(-1, -1);
		CHARRANGE cr = {0};
		GetSel(cr);

		cr.cpMin--;
		key->pos = cr;

        keyMap_[key->id] = key;
	}

	return key;
}

/**
 *\fn         SetTransFile(KEY_AREA *key, const char *text, int state, UINT64 param)
 *\brief      ���ô����ļ��ؼ���
 *\param[in]  key		�ؼ��㡣
 *\param[in]  text		����Ϣ��
 *\param[in]  state		״̬��(0-��ʾ�ļ������ļ���С,
 *\n						   1-��ʾ����,param/InsertTransFileCtrl��fileLen�Ľ���
 *\n						   2-��ʾ�ļ�������ɺ���ʱ)
 *\param[in]  param		������
 */
void CXTRichEdit::SetTransFile(KEY_AREA *key, const char *text, int state, UINT64 param)
{
	if (NULL == key || NULL == text) return;

	try
	{
		key->ctrl->SetFileTrans(_bstr_t(text), state, param);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
	}
}

/**
 *\fn         InsertImage(const char *fileame, int id)
 *\brief      ���ͼƬ��
 *\param[in]  fileame	�ļ�����
 *\param[in]  id		ID��
 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
 */
int CXTRichEdit::InsertImage(const char *filename, int id)
{
	try
	{
		// Use IPicture stuff to use JPG / GIF files
		HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hFile) return -1;

		// get file size
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if (-1 == dwFileSize) return -2;

		// alloc memory based on file size
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
		if (NULL == hGlobal) return -3;

		LPVOID pvData = GlobalLock(hGlobal);
		if (NULL == pvData) return -4;

		// read file and store in global memory
		DWORD dwBytesRead = 0;	
		BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
		if (FALSE == bRead) return -5;
		
		// close file
		CloseHandle(hFile);		

		GlobalUnlock(hGlobal);

		// create IStream* from global memory
		CComPtr<IStream> spStream;
		HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &spStream);
		if (FAILED(hr)) return -6;

		// create IPicture from image file
		CComPtr<IPicture> spPicture = NULL;
		hr = OleLoadPicture(spStream, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&spPicture);
		if (FAILED(hr)) return -7;

		// get image handle
		HANDLE hImage = 0;
		spPicture->get_Handle((OLE_HANDLE*)&hImage);

		GlobalFree(hGlobal);

		// Copy the image. Necessary, because upon p's release,
		// the handle is destroyed.
		HANDLE hImageCopy = CopyImage(hImage, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);

		return InsertImage(hImageCopy, id);
	}
	catch (_com_error e)
	{
		MessageBox(e.ErrorMessage());
		return -10;
	}
	catch(...)
	{
		LOG(INFO) << "InsertImage:" << filename << "Error:" << GetLastError();
		return -11;
	}
}

/**
 *\fn         InsertImage(HANDLE image, int id)
 *\brief      ���ͼƬ��
 *\param[in]  image		�ļ������
 *\param[in]  id		ID��
 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
 */
int CXTRichEdit::InsertImage(HANDLE image, int id)
{
	if (NULL == image) return -1;

	STGMEDIUM stgm = {0};
	stgm.pUnkForRelease = NULL;		// Use ReleaseStgMedium
	stgm.tymed = TYMED_GDI;			// Storage medium = HBITMAP handle
	stgm.hBitmap = (HBITMAP)image;

	FORMATETC fm = {0};
	fm.dwAspect = DVASPECT_CONTENT;	// Level of detail = Full content
	fm.cfFormat = CF_BITMAP ;		// Clipboard format = CF_BITMAP
	fm.tymed = TYMED_GDI;			// GDI
	fm.ptd = NULL;					// Target Device = Screen	
	fm.lindex = -1;					// Index = Not applicaple

	try
	{
		// �����ڴ�
		CComPtr<ILockBytes> spLockBytes = NULL;
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (sc != S_OK) AfxThrowOleException(sc);

		// ������������Դ
		CComPtr<IStorage> spStorage = NULL;
		sc = ::StgCreateDocfileOnILockBytes(spLockBytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &spStorage);
		if (sc != S_OK)
		{
			spLockBytes.Release();
			spLockBytes = NULL;
			AfxThrowOleException(sc);
		}

		// ���ݶ���
		COleDataSource dataSource;
		dataSource.CacheData(CF_BITMAP, &stgm);
		CComPtr<IDataObject> spDataObject = (LPDATAOBJECT)dataSource.GetInterface(&IID_IDataObject);

		// �ͻ���վ
		CComPtr<IOleClientSite> spClientSite;
		richEditOle_->GetClientSite(&spClientSite);

		// ����OLE����
		CComPtr<IOleObject> spOleObject;
		sc = OleCreateStaticFromData(spDataObject, IID_IOleObject, OLERENDER_FORMAT, &fm, spClientSite, spStorage, (void **)&spOleObject);
		if(sc!=S_OK) AfxThrowOleException(sc);

		// ����ClientSite
		CLSID clsid;
		sc = spOleObject->GetUserClassID(&clsid);
		if (sc != S_OK)	AfxThrowOleException(sc);

		REOBJECT reobject = {0};
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp       = REO_CP_SELECTION;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.dwUser   = id;
		reobject.clsid    = clsid;		
		reobject.poleobj  = spOleObject;
		reobject.polesite = spClientSite;
		reobject.pstg     = spStorage;

		HRESULT hr = richEditOle_->InsertObject(&reobject);

		if (FAILED(hr))	AfxThrowOleException(hr);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		return -20;
	}
	catch(...)
	{
		LOG(ERROR) << "InsertImage HANDLE:" << image << "Error:" << GetLastError();
		return -21;
	}

	return 0;
}

/**
 *\fn         InsertGif(KEY_AREA *key, const char *filename)
 *\brief      ���GIFͼƬ��
 *\param[in]  key		�ؼ��㡣
 *\param[in]  image		�ļ������
 *\return     �Ƿ�ɹ�,0�ɹ�����ʧ��
 */
int CXTRichEdit::InsertGif(KEY_AREA *key, const char *filename)
{
	if (NULL == key) return -1;

	try
	{
		CComPtr<IShowImage> spDispCtrl;

		LoadOleCtrl(key->id, &spDispCtrl);

		//  �ؼ�����
		HRESULT hr = spDispCtrl->LoadGif(_bstr_t(filename));
		if (FAILED(hr))	_com_issue_error(hr);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		return -10;
	}
	catch(...)
	{
		LOG(ERROR) << "InsertGif:" << filename << "Error %d" << GetLastError();
		return -11;
	}
	return 0;
}

/**
 *\fn         InsertTransFileCtrl(KEY_AREA *key, 
 *\fn					  const char *filename,
 *\fn					  UINT64  fileLen,
 *\fn					  const char *iconFilename,
 *\fn					  const char *text)
 *\brief      ��Ӵ����ļ��ؼ���
 *\param[in]  key			�ؼ��㡣
 *\param[in]  filename		�ļ�����
 *\param[in]  fileLen		�ļ�����
 *\param[in]  iconFilename	ͼ���ļ�����
 *\param[in]  text			�ı���Ϣ��
 *\return     �Ƿ�ɹ���
 */
int CXTRichEdit::InsertTransFileCtrl(KEY_AREA *key, 
	const char *filename,
	UINT64  fileLen,
	const char *iconFilename,
	const char *text)
{
	if (NULL == key) return -1;

	try
	{
		CComPtr<IShowImage> spDispCtrl;

		LoadOleCtrl(key->id, &spDispCtrl);

		// �ؼ�����
		HRESULT hr = spDispCtrl->LoadFileTrans(_bstr_t(filename), fileLen, _bstr_t(iconFilename));
		if (FAILED(hr))	_com_issue_error(hr);

		hr = spDispCtrl->SetFileTrans(_bstr_t(text), 0, 0);
		if (FAILED(hr))	_com_issue_error(hr);
		
		// ��ָ��
		key->ctrl = spDispCtrl;
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		return -10;
	}
	catch(...)
	{
		LOG(ERROR) << "InsertTransFileCtrl:" << filename << "Error:" << GetLastError();
		return -11;
	}
	return 0;
}

/**
 *\fn         LoadOleCtrl(int id)
 *\brief      ��ӿؼ���
 *\param[in]  id	�ؼ���ID��
 *\return     ָ�ڡ�
 */
bool CXTRichEdit::LoadOleCtrl(int id, IShowImage **showImage)
{
	try
	{
		// �����ڴ�
		CComPtr<ILockBytes> spLockBytes;
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &spLockBytes);
		if (sc != S_OK)	AfxThrowOleException(sc);

		// Ӧ���������ڴ洴���洢��
		CComPtr<IStorage> spStorage;
		sc = ::StgCreateDocfileOnILockBytes(spLockBytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &spStorage);
		if (sc != S_OK)
		{
			spLockBytes.Release();
			spLockBytes = NULL;
			AfxThrowOleException(sc);
		}

		// �ͻ���վ
		CComPtr<IOleClientSite> spClientSite;
		richEditOle_->GetClientSite(&spClientSite);

		// ����OLE����
		CLSID clsid;
		//CComPtr<IShowImage> spDispCtrl;
		HRESULT hr = CLSIDFromProgID(OLESTR("ShowImageOle.ShowImage"), &clsid);
		hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IShowImage), (LPVOID*)showImage);
		if (FAILED(hr))	_com_issue_error(hr);

		// �õ�IOleObject�ӿ�
		CComPtr<IOleObject> spObject;
		hr = (*showImage)->QueryInterface(IID_IOleObject, (void**)&spObject);
		if (FAILED(hr)) _com_issue_error(hr);

		// ���ÿɰ�������
		OleSetContainedObject(spObject, TRUE);

		// ����ClientSite
		spObject->SetClientSite(spClientSite);
		sc = spObject->GetUserClassID(&clsid);
		if (sc != S_OK)	AfxThrowOleException(sc);

		// ���뵽RichEdit
		REOBJECT reobject = {0};
		reobject.cbStruct = sizeof(REOBJECT);		
		reobject.cp       = REO_CP_SELECTION;	// can be selected
		reobject.dwFlags  = REO_BELOWBASELINE;	// goes in the same line of text line REO_RESIZABLE
		reobject.dvaspect = DVASPECT_CONTENT;	// content, but not static	
		reobject.dwUser   = id;					// ID
		reobject.clsid    = clsid;				// set clsid
		reobject.poleobj  = spObject;			// the very object
		reobject.polesite = spClientSite;		// client site contain the object
		reobject.pstg     = spStorage;			// the storage
		
		richEditOle_->InsertObject(&reobject);

		// ��ʾ
		spObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, spClientSite, 0, m_hWnd, NULL);
		spObject->DoVerb(OLEIVERB_SHOW,       NULL, spClientSite, 0, m_hWnd, NULL);
	
		return true;
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		return false;
	}
	catch(...)
	{
		LOG(ERROR) << "LoadOleCtrl Error:" << GetLastError();
		return false;
	}
}

/**
 *\fn         MakeID()
 *\brief      ����һ��Ψһ��ID��
 *\return     Ψһ��ID��
 */
DWORD CXTRichEdit::MakeID()
{
    int id = GetTickCount();

    while (true)
    {
        KEY_AREA_MAP::iterator iter = keyMap_.find(id);

		if (iter != keyMap_.end())
		{
			id++;
		}
		else
		{
			return id;
		}
	}
}

/**
 *\fn         getKey(int id)
 *\brief      �õ���ָ�롣
 *\param[in]  id    ��ID��
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::getKey(int id)
{
	KEY_AREA_MAP::iterator iter = keyMap_.find(id);

	if (iter != keyMap_.end())
	{
		return iter->second;
	}

    return NULL;
}

/**
 *\fn         getKey(CHARRANGE range)
 *\brief      �õ���ָ�롣
 *\param[in]  range    �ַ�λ�á�
 *\return     ��ָ�롣
 */
KEY_AREA* CXTRichEdit::getKey(CHARRANGE range)
{
	KEY_AREA_MAP::iterator iter = keyMap_.begin();

	for (; iter != keyMap_.end(); ++iter)
	{
		KEY_AREA *key = iter->second;

		if((key->pos.cpMin == range.cpMin) && (key->pos.cpMax == range.cpMax))
		{
			return key;
		}
	}

	return NULL;
}

/**
 *\fn         DeleteKey(KEY_AREA* key)
 *\brief      ɾ������
 *\param[in]  key    ��ָ�롣
 */
void CXTRichEdit::DeleteKey(KEY_AREA* key)
{
	KEY_AREA_MAP::iterator iter = keyMap_.find(key->id);

	if (iter != keyMap_.end())
	{
		keyMap_.erase(iter);
	}

	if(key->type == KEY_TYPE_TEXT)
	{
		SetLink(key, "");
	}
	else if(key->type == KEY_TYPE_LINK)
	{
		SetLink(key, "");
	}
}

/**
 *\fn         DeleteAll()
 *\brief      ɾ�����ڵ㡣
 */
void CXTRichEdit::DeleteAll()
{
	KEY_AREA_MAP::iterator iter = keyMap_.begin();

	for (; iter != keyMap_.end(); ++iter)
	{
		delete iter->second;
	}

	keyMap_.clear();
}

/**
 *\fn         KeyOffset(KEY_AREA *key, int offset)
 *\brief      ���ùؼ���λ�á�
 *\param[in]  key    ��ָ�롣
 *\param[in]  offset ƫ�ơ�
 */
void CXTRichEdit::KeyOffset(KEY_AREA *key, int offset)
{
	KEY_AREA_MAP::iterator iter = keyMap_.begin();

	for (; iter != keyMap_.end(); ++iter)
	{
		KEY_AREA *item = iter->second;

		if (item->pos.cpMin >= key->pos.cpMin)
		{
			item->pos.cpMin += offset;
			item->pos.cpMax += offset;
		}
	}
}

/**
 *\fn         GetCtrlInfo(LONG index, int &id, LONG &pos)
 *\brief      �õ��ؼ���Ϣ��
 *\param[in]  index		��š�
 *\param[out] id		�ؼ�ID��
 *\param[out] pos		�ؼ�λ�á�
 *\return     �Ƿ�ɹ���
 */
bool CXTRichEdit::GetCtrlInfo(LONG index, int &id, LONG &pos)
{
	REOBJECT object = {0};
	object.cbStruct = sizeof(REOBJECT);

	HRESULT hr = richEditOle_->GetObject(index, &object, REO_GETOBJ_POLEOBJ);
	if (hr != S_OK)	return false;

	id = object.dwUser;
	pos = object.cp;

	return true;
}