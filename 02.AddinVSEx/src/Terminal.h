
#pragma once
#include "04.UI/Ctrl/XTDC.h"

// �õ�git�ύ�Ĵ���
// git rev-list --all | find /v "" /c

class CTerminal
{
public:
    CTerminal();
    ~CTerminal();

private:
    CXTDC               xtDC_;          // XTDC
    HDC                 wndDC_;         // ����DC
    HDC                 imageDC_;       // ͼ��DC
    HDC                 testDC_;        // ͼ��DC

    HFONT               font_;          // ����
    COLORREF            color_[8];      // ��ɫ
    COLORREF            fgColor_;       // ��ǰ������ɫ
    COLORREF            bgColor_;       // ��ǰ������ɫ
    HPEN                fgPen_;         // ����
    HPEN                bgPen_;         // ����
    HWND                wnd_;           // ������

    BITMAPINFO          *bmi_;          // BMP��Ϣ

    UINT                fontCx_;        // �����(����)
    UINT                fontCy_;        // �����
    UINT                wndCx_;         // �����
    UINT                wndCy_;         // �����
    UINT                screenCx_;      // ��Ļ��
    UINT                screenCy_;      // ��Ļ��
    UINT                imageDcCx_;     // ͼ��DC��
    UINT                imageDcCy_;     // ͼ��DC��Ļ��

    UINT                scrollPos_;     // ������λ��(�ַ���)
    UINT                scrollPageCx_;  // ������ҳ��
    UINT                scrollPageCy_;  // ������ҳ��

    UINT                imageTopPos_;   // ͼ��DC��ʼλ��,ѭ�������ƶ�

    UINT                cursorX_;       // ��ǰ���λ��X
    UINT                cursorY_;       // ��ǰ���λ��Y
    UINT                saveCursorX_;   // ���浱ǰ���λ��X
    UINT                saveCursorY_;   // ���浱ǰ���λ��Y
    UINT                lastCursorY_;   // ���浱ǰ���λ��Y

    static UINT         pid_;           // ��ʾ�����߳�
    static CTerminal    *focusTerm_;    // ��ǰ��ý�����ն�

    CRITICAL_SECTION    wndDrawlock_;   // ˢ�´�����

public:
    /**
     *\fn           void showSursorThread(void* param)
     *\brief        ��ʾ����߳�
     *\param[in]    void * param 
     *\return       void 0�ɹ�,����ʧ��
     */
    static UINT __stdcall showSursorThread(void* param);

    /*
     *\fn         int Init(HWND hwnd)
     *\brief      ��ʼ��
     *\param[in]  HWND hwnd ������
     *\return     int 0�ɹ�,����ʧ��
     */
    int Init(HWND hwnd);

    /**
     *\fn           void setFocus(bool focus)
     *\brief        ���ý���
     *\param[in]    bool focus �Ƿ��н���
     *\return       void ��
     */
    void setFocus(bool focus);

    /**
     *\fn           void getTermRect(int &cx, int &cy)
     *\brief        �õ��ն˵ĺ����ַ�����
     *\param[in]    int & cx �����ַ�����
     *\param[in]    int & cy �����ַ�����
     *\return       void ��
     */
    void getTermRect(int &cx, int &cy) { cx = scrollPageCx_; cy = scrollPageCy_;}

    /*
     *\fn         void setWndRect(int cx, int cy)
     *\brief      ���ô�����
     *\param[in]  int cx ��
     *\param[in]  int cy ��
     *\return     void ��
     */
    void setWndRect(int cx, int cy);

    /**
     *\fn           void scrollScreen(int type, int data)
     *\brief        ��ֱ����
     *\param[in]    int type ����
     *\param[in]    int data ����
     *\return       void 0�ɹ�,����ʧ��
     */
    void scrollScreen(int type, int data);

    /*
     *\fn         void drawWndDc()
     *\brief      ˢ��
     *\return     void ��
     */
    void drawWndDc();

    /**
     *\fn           void drawWndCursor(int x, int y, bool show)
     *\brief        ��ʾ���
     *\param[in]    int x ���x
     *\param[in]    int y ���y
     *\param[in]    bool show �Ƿ���ʾ���
     *\return       void ��
     */
    void drawWndCursor(int x, int y, bool show);

    /*
     *\fn         void clearScreen(int type)
     *\brief      ����
     *\param[in]  int type �������:0-�ӹ�굽��Ļ��,1-��Ļ�������,2-ȫ��
     *\return     void ��
     */
    void clearScreen(int type);

    /**
     *\fn           void clearLineText(int type, int count)
     *\brief        ����
     *\param[in]    int type:0-�ӹ�굽��β,1-���׵����,2-ȫ��
     *\param[in]    int count �ַ�����
     *\return       void ��
     */
    void clearLineText(int type, int count);

    /**
     *\fn           void drawLineText(const char *text, int textLen, bool newLen)
     *\brief        ˢ��һ��
     *\param[in]    const char * text ��������
     *\param[in]    int textLen ���ֳ�
     *\param[in]    bool newLen ����
     *\return       void 0�ɹ�,����ʧ��
     */
    void drawLineText(const char *text, int textLen, bool newLen, bool unicode);

    /*
     *\fn         int outputText((const char *text, int textLen)
     *\brief      �������
     *\param[in]  const char * text ��������
     *\param[in]  int textLen ���ֳ�
     *\return     int 0�ɹ�,����ʧ��
     */
    int outputText(const char *text, int textLen);

    /**
     *\fn           int processCmd(const char *text)
     *\brief        ��������
     *\param[in]    const char * text �����ַ�
     *\return       int 0�ɹ�,����ʧ��
     */
    int processCmd(const char *text);

    /**
     *\fn           void moveCursor(UINT x, UINT y)
     *\brief        �ƶ����
     *\param[in]    UINT x ���X��λ��
     *\param[in]    UINT y ���Y��λ��
     *\return       void ��
     */
    void moveCursor(UINT x, UINT y);

    /**
     *\fn           void setColor(UINT count, UINT *data)
     *\brief        ������ɫ
     *\param[in]    UINT count ���ݸ���
     *\param[in]    UINT * data ����
     *\return       void ��
     */
    void setColor(UINT count, UINT *data);

    /**
     *\fn           UINT getCurPagePos()
     *\brief        �õ���ǰҳ��λ��
     *\return       UINT ��ǰҳҳ��λ��
     */
    UINT getCurPagePos();

    /**
     *\fn           UINT getTailPagePos()
     *\brief        �õ�βҳҳ��λ��
     *\return       UINT βҳҳ��λ��
     */
    UINT getTailPagePos();

    /**
     *\fn           void setNewCmd()
     *\brief        ���������
     *\return       void ��
     */
    void setNewCmd() { lastCursorY_ = 0; }
};


