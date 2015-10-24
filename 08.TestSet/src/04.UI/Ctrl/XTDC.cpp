
#include "stdafx.h"
#include "XTDC.h"


CXTDC::CXTDC()
    :module_(NULL),
    wnd_(NULL),
    dc_(NULL),
    curCompDC_(NULL),
    curImageDC_(NULL)
{
}

CXTDC::~CXTDC()
{
    Release();
}

void CXTDC::Release()
{
    if (NULL != dc_)
    {
        ::ReleaseDC(wnd_, dc_);
        dc_ = NULL;
    }

    XTDCMAP::iterator iter = compDcMap_.begin();
    for (; iter != compDcMap_.end(); ++iter)
    {
        XTDC xtDC = (*iter).second;

        if (xtDC.dc && xtDC.oldImage)
        {
            SelectObject(xtDC.dc, xtDC.oldImage);
        }

        if (xtDC.dc)
        {
            ::DeleteDC(xtDC.dc);
        }

        if (xtDC.image)
        {
            DeleteObject(xtDC.image);
        }

        if (xtDC.data)
        {
            delete xtDC.data;
        }
    }

    compDcMap_.clear();

    iter = imageDcMap_.begin();
    for (; iter != imageDcMap_.end(); ++iter)
    {
        XTDC xtDC = (*iter).second;

        if (xtDC.dc && xtDC.oldImage)
        {
            SelectObject(xtDC.dc, xtDC.oldImage);
        }

        if (xtDC.dc)
        {
            ::DeleteDC(xtDC.dc);
        }

        if (xtDC.image)
        {
            DeleteObject(xtDC.image);
        }
    }
    imageDcMap_.clear();
}

#define IN_ATL

/**
 *\fn           HDC Init(HMODULE module, HWND wnd, bool windowDC)
 *\brief        ��ʼ��DC
 *\param[in]    HMODULE module ģ����,���Ϊ��,����ر��������Դ,
 *\             Ҳ��module = LoadLibrary("****.dll");��DLL�м�����Դ
 *\param[in]    HWND wnd ������
 *\param[in]    bool windowDC �Ƿ�ʹ��winddowDC
 *\return       HDC 0�ɹ�,����ʧ��
 */
HDC CXTDC::Init(HMODULE module, HWND wnd, bool windowDC/* = false*/)
{
    if (NULL == module)
    {
#ifdef IN_ATL
        module_ = (HMODULE)ATL::_pModule->GetModuleInstance();
#else
        module_ = AfxGetResourceHandle();
#endif
    }
    else
    {
        module_ = module;
    }

    wnd_ = wnd;

    dc_ = windowDC ? GetWindowDC(wnd) : GetDC(wnd);

    return dc_;
}

/**
 *\fn           HDC AddCompDC(int id)
 *\brief        ��Ӽ���DC
 *\param[in]    int id ����DCID
 *\return       ����DC���
 */
HDC CXTDC::AddCompDC(int id)
{
    DeleteDC(COMPDC, id);

    int width = GetDeviceCaps(dc_, HORZRES);
    int height = GetDeviceCaps(dc_, VERTRES);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(dc_);
    xtDC.image = CreateCompatibleBitmap(dc_, width, height);
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);
    compDcMap_[id] = xtDC;

    SetBkMode(xtDC.dc, TRANSPARENT);

    return xtDC.dc;
}

/**
 *\fn           HDC AddBmpDC(int id, int bmpId)
 *\brief        ���BMPͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    int bmpId ��ԴID
 *\return       BMPͼ��DC���
 */
HDC CXTDC::AddBmpDC(int id, int bmpId)
{
    DeleteDC(IMAGEDC, id);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = (HGDIOBJ)LoadImage(module_, MAKEINTRESOURCE(bmpId), IMAGE_BITMAP, 0, 0, 0);

    if (NULL == xtDC.image)
    {
        return NULL;
    }

    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    return xtDC.dc;
}

/**
 *\fn           HDC AddBmpDC(int id, const char *filename)
 *\brief        ���ļ����BMPͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    const char * filename BMPͼƬ·��
 *\return       BMPͼ��DC���
 */
HDC CXTDC::AddBmpDC(int id, const char *filename)
{
    DeleteDC(IMAGEDC, id);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = LoadImage(module_, filename, IMAGE_BITMAP, 0, 0, 0);

    if (NULL == xtDC.image)
    {
        return NULL;
    }

    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    return xtDC.dc;
}

/**
 *\fn           HDC AddPngDC(int id, int pngId)
 *\brief        ���PNGͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    int pngId ��ԴID
 *\return       PNGͼ��DC���
 */
HDC CXTDC::AddPngDC(int id, int pngId)
{
    DeleteDC(IMAGEDC, id);

    CPNG *png = new CPNG; // PNG�ļ�

    if (!png->Load(module_, pngId)) return false;

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = CreateCompatibleBitmap(dc_, png->getWidth(), png->getHeight());
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);
    xtDC.data = png;

    png->Draw(xtDC.dc, 0, 0);

    imageDcMap_[id] = xtDC;

    return xtDC.dc;
}

/**
 *\fn           HDC AddPngDC(int id, const char *filename)
 *\brief        ���PNGͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    const char * filename PNGͼƬ·��
 *\return       PNGͼ��DC���
 */
HDC CXTDC::AddPngDC(int /*id*/, const char * /*filename*/)
{
    return NULL;
}

/**
 *\fn           HDC AddGifDC(int id, int gifId)
 *\brief        ���GIFͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    int gifId ��ԴID
 *\return       GIFͼ��DC���
 */
HDC CXTDC::AddGifDC(int /*id*/, int /*gifId*/)
{
    return NULL;
}

/**
 *\fn           HDC AddGifDC(int id, const char *filename)
 *\brief        ���GIFͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    const char * filename GIFͼƬ·��
 *\return       GIFͼ��DC���
 */
HDC CXTDC::AddGifDC(int /*id*/, const char * /*filename*/)
{
    return NULL;
}

/**
 *\fn           HDC AddImageDC(int id, int resId)
 *\brief        ���JPGͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    int resId ��ԴID
 *\return       JPGͼ��DC���
 */
HDC CXTDC::AddImageDC(int id, int resId)
{
    DeleteDC(IMAGEDC, id);

    HRSRC   res     = FindResource(module_, MAKEINTRESOURCE(resId), _T("jpg"));
    HGLOBAL resData = LoadResource(module_, (HRSRC)res);
    DWORD   resSize = SizeofResource(module_, (HRSRC)res);

    if (NULL == resData)
    {
        return NULL;
    }

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, resSize);
    if (NULL == hGlobal)
    {
        return NULL;
    }

    LPVOID pvData = GlobalLock(hGlobal);// �����ڴ�
    if (NULL == pvData)
    {
        FreeResource(resData);
        GlobalFree(hGlobal);
        return NULL;
    }

    LockResource(resData);              // ������Դ
    memcpy(pvData, resData, resSize);   // �����ڴ�
    GlobalUnlock(hGlobal);              // �����ڴ�
    FreeResource(resData);              // �ͷ���Դ

    CComPtr<IStream> spStream = NULL;
    HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &spStream); // ����IStream
    if (!SUCCEEDED(hr))
    {
        GlobalFree(hGlobal);
        return NULL;
    }

    CComPtr<IPicture> spPicture;
    hr = OleLoadPicture(spStream, resSize, FALSE, IID_IPicture, (LPVOID*)&spPicture);   // ����IPicture
    if (!SUCCEEDED(hr))
    {
        GlobalFree(hGlobal);
        return NULL;
    }

    GlobalFree(hGlobal);

    OLE_HANDLE picHandle = NULL;
    spPicture->get_Handle(&picHandle);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = (HGDIOBJ)picHandle;
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    return xtDC.dc;
}

/**
 *\fn           HDC AddImageDC(int id, const char *filename)
 *\brief        ���JPGͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    const char * filename JPGͼƬ·��
 *\return       JPGͼ��DC���
 */
HDC CXTDC::AddImageDC(int id, const char *filename)
{
    DeleteDC(IMAGEDC, id);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)	return NULL;

    DWORD dwFileSize = GetFileSize(hFile, NULL);

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
    if (NULL == hGlobal) return false;

    LPVOID pvData = GlobalLock(hGlobal);// �����ڴ�
    if (NULL == pvData)
    {
        GlobalFree(hGlobal);
        return NULL;
    }

    DWORD dwReadLen = 0;
    if (!ReadFile(hFile, pvData, dwFileSize, &dwReadLen, NULL))
    {
        GlobalFree(hGlobal);
        CloseHandle(hFile);
        return false;
    }

    if (dwReadLen != dwFileSize)
    {
        GlobalFree(hGlobal);
        CloseHandle(hFile);
        return false;
    }

    GlobalUnlock(hGlobal);
    CloseHandle(hFile);

    CComPtr<IStream> spStream = NULL;
    HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &spStream);// ����IStream
    if (!SUCCEEDED(hr))
    {
        GlobalFree(hGlobal);
        return NULL;
    }

    CComPtr<IPicture> spPicture;
    hr = OleLoadPicture(spStream, dwFileSize, FALSE, IID_IPicture, (LPVOID*)&spPicture);// ����IPicture
    if (!SUCCEEDED(hr))
    {
        GlobalFree(hGlobal);
        return NULL;
    }

    GlobalFree(hGlobal);

    OLE_HANDLE picHandle = NULL;
    spPicture->get_Handle(&picHandle);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = (HGDIOBJ)picHandle;
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    return xtDC.dc;
}

/**
 *\fn           HDC AddNullDC(int id, int cx, int cy)
 *\brief        ��ӿյ�ͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       ͼ��DC���
 */
HDC CXTDC::AddNullDC(int id, int cx, int cy)
{
    DeleteDC(IMAGEDC, id);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = CreateCompatibleBitmap(dc_, cx, cy);
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    FloodFill(xtDC.dc, 0, 0, RGB(0, 0, 0));

    return xtDC.dc;
}

/**
 *\fn           HDC AddNullDC(int id, BITMAPINFO *bmi)
 *\brief        ��ӿյ�ͼ��DC
 *\param[in]    int id ͼ��DC���
 *\param[in]    BITMAPINFO * bmi ͼ����Ϣ
 *\return       ͼ��DC���
 */
HDC CXTDC::AddNullDC(int id, BITMAPINFO *bmi)
{
    if (NULL == bmi) return NULL;

    DeleteDC(IMAGEDC, id);

    XTDC xtDC;
    xtDC.dc = CreateCompatibleDC(NULL);
    xtDC.image = ::CreateDIBSection(xtDC.dc, bmi, DIB_RGB_COLORS, &xtDC.data, NULL, 0); // ϵͳ���Լ��ͷ�xtDC.data��
    xtDC.oldImage = SelectObject(xtDC.dc, xtDC.image);

    imageDcMap_[id] = xtDC;

    return (NULL != xtDC.image) ? xtDC.dc : NULL;
}

/**
 *\fn           HDC SelectImageDC(int id)
 *\brief        ѡȡͼ��ID
 *\param[in]    int id ͼ��DC���
 *\return       ͼ��DC���
 */
HDC CXTDC::SelectImageDC(int id)
{
    int cx = 0;
    int cy = 0;

    if (GetBmpRect(id, cx, cy))
    {
        curImageDC_ = GetHDC(IMAGEDC, id);
        ::BitBlt(curCompDC_, 0, 0, cx, cy, curImageDC_, 0, 0, SRCCOPY);		
        return curImageDC_;
    }

    return NULL;
}

/**
 *\fn           void DeleteDC(int type, int id)
 *\brief        
 *\param[in]    int type DC����:WNDDC, COMPDC, IMAGEDC
 *\param[in]    int id DC���
 *\return       void ��
 */
void CXTDC::DeleteDC(int type, int id)
{
    if (COMPDC == type)
    {
        XTDCMAP::iterator iter = compDcMap_.find(id);
        if (compDcMap_.end() != iter)
        {
            XTDC xtDC = (*iter).second;
            if (xtDC.dc && xtDC.oldImage)
            {
                SelectObject(xtDC.dc, xtDC.oldImage);
            }

            if (xtDC.dc)
            {
                ::DeleteDC(xtDC.dc);
            }

            xtDC.dc = NULL;
            xtDC.image = NULL;
            xtDC.oldImage = NULL;

            compDcMap_.erase(id);
        }
    }
    else
    {
        XTDCMAP::iterator iter = imageDcMap_.find(id);
        if (imageDcMap_.end() != iter)
        {
            XTDC xtDC = (*iter).second;
            if (xtDC.dc && xtDC.oldImage)
            {
                SelectObject(xtDC.dc, xtDC.oldImage);
            }

            if (xtDC.dc)
            {
                ::DeleteDC(xtDC.dc);
            }

            xtDC.dc = NULL;
            xtDC.image = NULL;
            xtDC.oldImage = NULL;

            imageDcMap_.erase(id);
        }
    }
}

/**
 *\fn           HDC GetHDC(int type, int id)
 *\brief        �õ�DC
 *\param[in]    int type DC����:WNDDC, COMPDC, IMAGEDC
 *\param[in]    int id DC���
 *\return       HDC 0�ɹ�,����ʧ��
 */
HDC CXTDC::GetHDC(int type, int id /*= 0*/)
{
    switch (type)
    {
    case WNDDC:
        {
            return dc_;
        }
    case COMPDC:
        {
            XTDCMAP::iterator iter = compDcMap_.find(id);
            if (compDcMap_.end() != iter)
            {
                return (*iter).second.dc;
            }
            break;
        }
    default:
        {
            XTDCMAP::iterator iter = imageDcMap_.find(id);
            if (imageDcMap_.end() != iter)
            {
                return (*iter).second.dc;
            }
            break;
        }
    }
    return NULL;
}

/**
 *\fn           bool GetBmpRect(int id, int &cx, int &cy)
 *\brief        �õ�BMPͼ��DC�Ŀ��
 *\param[in]    int id ͼ��DC�����
 *\param[out]   int & cx ��
 *\param[out]   int & cy ��
 *\return       bool �Ƿ�ɹ�
 */
bool CXTDC::GetBmpRect(int id, int &cx, int &cy)
{
    XTDCMAP::iterator iter = imageDcMap_.find(id);
    if (imageDcMap_.end() != iter)
    {
        BITMAP bmpInfo = {0};
        GetObject((*iter).second.image, sizeof(bmpInfo), &bmpInfo);	// �õ�λͼ��Ϣ
        cx = bmpInfo.bmWidth;
        cy = bmpInfo.bmHeight;
        return true;
    }
    return false;
}

/**
 *\fn           void* LoadRes(int resId, const char *type)
 *\brief        ������Դ
 *\param[in]    int resId ��ԴID
 *\param[in]    const char * type ��Դ����
 *\return       void* ��Դ����
 */
void* CXTDC::LoadRes(int resId, const char *type)
{
    HRSRC   res     = FindResource(module_, MAKEINTRESOURCE(resId), type);
    HGLOBAL resData = LoadResource(module_, (HRSRC)res);
    DWORD   resSize = SizeofResource(module_, (HRSRC)res);

    if (NULL == resData)
    {
        return NULL;
    }

    void *data = new char[resSize];

    ::LockResource(resData);        // ������Դ
    memcpy(data, resData, resSize); // �����ڴ�
    ::FreeResource(resData);        // �ͷ���Դ

    return data;
}