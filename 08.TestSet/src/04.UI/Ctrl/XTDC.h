
/**
˫����DC����Ҫ������DC��һ������DC��һ������DC������һ������λͼ�������б�ӰͼƬ��DC
*/

#pragma once
#include "../PNG.h"
#include <map>

typedef struct _tagXTDC
{
    HDC     dc;
    HGDIOBJ image;
    HGDIOBJ oldImage;
    void    *data;

    _tagXTDC::_tagXTDC()
    {
        dc = NULL;
        image = NULL;
        oldImage = NULL;
        data = NULL;
    }

}XTDC, *PXTDC;

typedef std::map<int, XTDC> XTDCMAP;

enum { WNDDC, COMPDC, IMAGEDC };

/**
*\class  CXTDC
*\brief  ����˫����DC����Ҫ������DC��һ������DC��һ������DC������һ������λͼ�������б�ӰͼƬ��DC
*\date   2012-06-25
*\author �ź���
*\par    Revision History:
*/
class CXTDC
{
public:
    CXTDC();
    ~CXTDC();

private:
    HMODULE module_;        // DLLģ����������ͼƬ��
    HWND    wnd_;           // ����
    XTDCMAP compDcMap_;     // ����DC
    XTDCMAP imageDcMap_;    // ͼ��DC

public:
    HDC     dc_;            // ��ʾDC
    HDC     curCompDC_;     // ��ǰ����DC
    HDC     curImageDC_;    // ��ǰͼ��DC

public:
    /**
     *\fn           HDC Init(HMODULE module, HWND wnd, bool windowDC)
     *\brief        ��ʼ��DC
     *\param[in]    HMODULE module ģ����,���Ϊ��,����ر��������Դ,
     *\             Ҳ��module = LoadLibrary("****.dll");��DLL�м�����Դ
     *\param[in]    HWND wnd ������
     *\param[in]    bool windowDC �Ƿ�ʹ��winddowDC
     *\return       HDC 0�ɹ�,����ʧ��
     */
    HDC Init(HMODULE module, HWND hwnd, bool bWindowDC = false);

    /**
     *\fn           HDC AddCompDC(int id)
     *\brief        ��Ӽ���DC
     *\param[in]    int id ����DCID
     *\return       ����DC���
     */
    HDC AddCompDC(int id = 0);

    /**
     *\fn           HDC AddBmpDC(int id, int bmpId)
     *\brief        ���BMPͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    int bmpId ��ԴID
     *\return       BMPͼ��DC���
     */
    HDC AddBmpDC(int id, int bmpId);

    /**
     *\fn           HDC AddBmpDC(int id, const char *filename)
     *\brief        ���ļ����BMPͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    const char * filename BMPͼƬ·��
     *\return       BMPͼ��DC���
     */
    HDC AddBmpDC(int id, const char *filename);

    /**
     *\fn           HDC AddPngDC(int id, int pngId)
     *\brief        ���PNGͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    int pngId ��ԴID
     *\return       PNGͼ��DC���
     */
    HDC AddPngDC(int id, int pngId);

    /**
     *\fn           HDC AddPngDC(int id, const char *filename)
     *\brief        ���PNGͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    const char * filename PNGͼƬ·��
     *\return       PNGͼ��DC���
     */
    HDC AddPngDC(int id, const char *filename);

    /**
     *\fn           HDC AddGifDC(int id, int gifId)
     *\brief        ���GIFͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    int gifId ��ԴID
     *\return       GIFͼ��DC���
     */
    HDC AddGifDC(int id, int gifId);

    /**
     *\fn           HDC AddGifDC(int id, const char *filename)
     *\brief        ���GIFͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    const char * filename GIFͼƬ·��
     *\return       GIFͼ��DC���
     */
    HDC AddGifDC(int id, const char *filename);

    /**
     *\fn           HDC AddImageDC(int id, int resId)
     *\brief        ���JPGͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    int resId ��ԴID
     *\return       JPGͼ��DC���
     */
    HDC AddImageDC(int id, int resId);

    /**
     *\fn           HDC AddImageDC(int id, const char *filename)
     *\brief        ���JPGͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    const char * filename JPGͼƬ·��
     *\return       JPGͼ��DC���
     */
    HDC AddImageDC(int id, const char *filename);

    /**
     *\fn           HDC AddNullDC(int id, int cx, int cy)
     *\brief        ��ӿյ�ͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    int cx ͼ���
     *\param[in]    int cy ͼ���
     *\return       ͼ��DC���
     */
    HDC AddNullDC(int id, int cx, int cy);

    /**
     *\fn           HDC AddNullDC(int id, BITMAPINFO *bmi)
     *\brief        ��ӿյ�ͼ��DC
     *\param[in]    int id ͼ��DC���
     *\param[in]    BITMAPINFO * bmi ͼ����Ϣ
     *\return       ͼ��DC���
     */
    HDC AddNullDC(int id, BITMAPINFO *bmi);

    /**
     *\fn           HDC GetHDC(int type, int id)
     *\brief        �õ�DC
     *\param[in]    int type DC����:WNDDC, COMPDC, IMAGEDC
     *\param[in]    int id DC���
     *\return       HDC 0�ɹ�,����ʧ��
     */
    HDC GetHDC(int type, int id);

    /**
     *\fn           HDC GetCurImageDC()
     *\brief        �õ���ǰ��ͼ��DC
     *\return       ��ǰ��ͼ��DC
     */
    HDC GetCurImageDC() { return curImageDC_; }

    /**
     *\fn           HDC SelectImageDC(int id)
     *\brief        ѡȡͼ��ID
     *\param[in]    int id ͼ��DC���
     *\return       ͼ��DC���
     */
    HDC SelectImageDC(int id);

    /**
     *\fn           bool GetBmpRect(int id, int &cx, int &cy)
     *\brief        �õ�BMPͼ��DC�Ŀ��
     *\param[in]    int id ͼ��DC�����
     *\param[out]   int & cx ��
     *\param[out]   int & cy ��
     *\return       bool �Ƿ�ɹ�
     */
    bool GetBmpRect(int id, int &cx, int &cy);

    /**
     *\fn           void DeleteDC(int type, int id)
     *\brief        
     *\param[in]    int type DC����:WNDDC, COMPDC, IMAGEDC
     *\param[in]    int id DC���
     *\return       void ��
     */
    void DeleteDC(int type, int id);

    /**
     *\fn           void Release()
     *\brief        �ͷ�ȫ��DC
     *\return       void ��
     */
    void Release();

    /**
     *\fn           void* LoadRes(int resId, const char *type)
     *\brief        ������Դ
     *\param[in]    int resId ��ԴID
     *\param[in]    const char * type ��Դ����
     *\return       void* ��Դ����
     */
    void* LoadRes(int resId, const char *type);
};
