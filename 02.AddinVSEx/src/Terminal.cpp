
#include "stdafx.h"
#include "Terminal.h"

#define TERM_TEST_TEXT              "1234567890!@#$%^*()-\n1234567890!@#$%^*()-\n1234567890!@#$%^*()-"
#define TERM_RESERVER_CY            2000                            // ��������
#define TERM_EDIT_BORDER_WIDTH      21                              // �߿��
#define TERM_EDIT_BORDER_HEIGHT     4                               // �߿��

#define MIN(x, y)                   (((x)<(y)) ? (x) : (y))         // �Ƚ�С
#define MAX(x, y)                   (((x)>(y)) ? (x) : (y))         // �Ƚϴ�

#define SCROLL_LEN(y1,y2)           ((y1 >= y2) ? (y1 - y2) : (TERM_RESERVER_CY + y1 - y2)) // ����֮�ľ���
#define SCROLL_INC(x, s)            (++x %= s)                      // xѭ����1
#define SCROLL_DEC(x, s)            (--x %= s)                      // xѭ����1
#define SCROLL_ADD(x, y, s)         ((x += y) %= s)                 // xѭ����y
#define SCROLL_SUB(x, y, s)         ((x -= y) %= s)                 // xѭ����y
#define LIMIT_INC(x, l)             ((x < (l)) ? x++ : 0)           // x��ֹ��1
#define LIMIT_DEC(x, l)             ((x > (l)) ? x-- : 0)           // x��ֹ��1
#define LIMIT_ADD(x, y, l)          (((x + y) < (l)) ? x += y : 0)  // x��ֹ��y
#define LIMIT_SUB(x, y, l)          (((x - y) > (l)) ? x -= y : 0)  // x��ֹ��y

UINT CTerminal::pid_ = 0;
CTerminal* CTerminal::focusTerm_ = NULL;

CTerminal::CTerminal()
    :wndDC_(NULL),
    imageDC_(NULL),
    testDC_(NULL),
    font_(NULL),
    fgColor_(NULL),
    bgColor_(NULL),
    wnd_(NULL),
    bmi_(NULL),
    fontCx_(0),
    fontCy_(0),
    wndCx_(0),
    wndCy_(0),
    screenCx_(0),
    screenCy_(0),
    imageDcCx_(0),
    imageDcCy_(0),
    scrollPos_(0),
    scrollPageCx_(0),
    scrollPageCy_(0),
    imageTopPos_(0),
    cursorX_(0),
    cursorY_(0),
    saveCursorX_(0),
    saveCursorY_(0),
    lastCursorY_(0)
{
    // ��ɫ
    color_[0] = RGB(  0,   0,   0); // ��
    color_[1] = RGB(255,   0,   0); // ��
    color_[2] = RGB(  0, 255,   0); // ��
    color_[3] = RGB(255, 255,   0); // ��
    color_[4] = RGB(  0,   0, 255); // ��
    color_[5] = RGB(255,   0, 255); // �Ϻ�
    color_[6] = RGB(  0, 255, 255); // ����
    color_[7] = RGB(187, 187, 187); // ��

    fgColor_ = color_[7];
    bgColor_ = color_[0];

    InitializeCriticalSection(&wndDrawlock_);
}

CTerminal::~CTerminal()
{
    DeleteCriticalSection(&wndDrawlock_);
}

/*
 *\fn         int Init(HWND hwnd)
 *\brief      ��ʼ��
 *\param[in]  HWND hwnd ������
 *\return     int 0�ɹ�,����ʧ��
 */
int CTerminal::Init(HWND hwnd)
{
    wnd_ = hwnd;
    wndDC_ = xtDC_.Init(NULL, hwnd);

    //lfHeight=-MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    //lfWeight�ķ�ΧΪ0��1000����������µ���������Ϊ400������Ϊ700�����lfWeightΪ0����ʹ��Ĭ�ϵ�����������
    //lfItalicΪTRUEʱʹ��б��
    //lfUnderlineΪTRUEʱ����������»���
    //lfStrikeOutΪTRUEʱ���������ɾ����
    //lfCharSetָ���ַ���:ANSI_CHARSET
    //lfFaceName����

    LOGFONT lf = {0};
    //lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
    lf.lfHeight = -MulDiv(10, GetDeviceCaps(wndDC_, LOGPIXELSY), 72);
    strcpy_s(lf.lfFaceName, "Lucida Console");//Courier New
    font_ = ::CreateFontIndirect(&lf);
    HFONT oldFont = (HFONT)::SelectObject(wndDC_, font_);

    // �õ�������
    RECT rc = {0};
    ::DrawText(wndDC_, TERM_TEST_TEXT, strlen(TERM_TEST_TEXT), &rc, DT_CALCRECT);
    fontCx_ = rc.right / 20;
    fontCy_ = rc.bottom / 3;

    ::SelectObject(wndDC_, oldFont);

    // ������
    ::GetWindowRect(wnd_, &rc);
    wndCx_ = rc.right - rc.left;
    wndCy_ = rc.bottom - rc.top;

    // �õ���Ļ���
    screenCx_ = GetSystemMetrics(SM_CXSCREEN);
    screenCy_ = GetSystemMetrics(SM_CYSCREEN);

    // ����ͼ��DC���
    imageDcCx_ = screenCx_;
    imageDcCy_ = fontCy_ * TERM_RESERVER_CY;

    BITMAPINFOHEADER bmih = {0};
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biBitCount = 4;
    bmih.biCompression = BI_RGB;
    bmih.biPlanes = 1;
    bmih.biWidth = imageDcCx_;
    bmih.biHeight = imageDcCy_ + fontCy_;

    bmi_ = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 8 * sizeof(RGBQUAD));
    memset(bmi_, 0, sizeof(BITMAPINFO) + 8 * sizeof(RGBQUAD));
    bmi_->bmiHeader = bmih;

    for (int i = 0; i < 8; i++)
    {
        bmi_->bmiColors[i].rgbRed   = (BYTE)((color_[i] & 0x00FF0000) >> 16);
        bmi_->bmiColors[i].rgbGreen = (BYTE)((color_[i] & 0x0000FF00) >> 8);
        bmi_->bmiColors[i].rgbBlue  = (BYTE)((color_[i] & 0x000000FF));
    }

    // �����µ�ͼ��DC
    imageDC_ = xtDC_.AddNullDC(0, bmi_);

    // ����
    fgPen_ = ::CreatePen(PS_SOLID, 1, fgColor_);
    bgPen_ = ::CreatePen(PS_SOLID, 1, bgColor_);

    // ����ͼ��DC
    ::SetBkMode(imageDC_, OPAQUE);
    ::SetBkColor(imageDC_, bgColor_);
    ::SetTextColor(imageDC_, fgColor_);
    ::SelectObject(imageDC_, font_);

    // �������
    scrollPageCy_ = (wndCy_ - TERM_EDIT_BORDER_HEIGHT) / fontCy_;
    scrollPageCx_ = (wndCx_ - TERM_EDIT_BORDER_WIDTH) / fontCx_;
    scrollPos_ = TERM_RESERVER_CY - scrollPageCy_;
    cursorY_ = TERM_RESERVER_CY - 1;

    // ������
    SCROLLINFO info = {0};
    info.cbSize = sizeof (SCROLLINFO);
    info.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
    info.nMin = 0;
    info.nMax = TERM_RESERVER_CY - 1;
    info.nPage = scrollPageCy_;
    info.nPos = scrollPos_;
    SetScrollInfo(wnd_, SB_VERT, &info, FALSE);

    // test
    ::SetTextColor(imageDC_, RGB(255, 0, 0));
    imageTopPos_ = 0;
    char text[200] = "";
    for (int i = imageTopPos_; i < TERM_RESERVER_CY; i++)
    {
        rc.left = 0;
        rc.right = screenCx_;
        rc.top = (i - imageTopPos_) * fontCy_;
        rc.bottom = rc.top + fontCy_;

        sprintf_s(text, "%10d", i);

       ::DrawText(imageDC_, text, strlen(text), &rc, DT_LEFT);
    }

    for (UINT i = 0; i < imageTopPos_; i++)
    {
        rc.left = 0;
        rc.right = screenCx_;
        rc.top = (i + imageTopPos_) * fontCy_;
        rc.bottom = rc.top + fontCy_;

        sprintf_s(text, "%10d", i);

        ::DrawText(imageDC_, text, strlen(text), &rc, DT_LEFT);
    }

    ::SetTextColor(imageDC_, fgColor_);
    // test

    if (0 == pid_)
    {
        _beginthreadex(NULL, 0, showSursorThread, this, 0, &pid_);
    }

    // ˢ����Ļ
    ::InvalidateRect(wnd_, NULL, FALSE);

    return 0;
}

/**
 *\fn           UINT showSursorThread(void* param)
 *\brief        ��ʾ����߳�
 *\param[in]    void * param 
 *\return       void 0�ɹ�,����ʧ��
 */
UINT CTerminal::showSursorThread(void* /*param*/)
{
    UINT y = 0;
    UINT pageTop = 0;
    bool show = true;
    CTerminal* focusTerm = NULL;

    while(true)
    {
        Sleep(500);

        focusTerm = focusTerm_;

        if (NULL == focusTerm_) continue;

        pageTop = focusTerm->getCurPagePos();
        y = SCROLL_LEN(focusTerm->cursorY_, pageTop);

        if (y < focusTerm->scrollPageCy_)
        {
            focusTerm->drawWndCursor(focusTerm->cursorX_ * focusTerm->fontCx_, y * focusTerm->fontCy_, show);
        }

        show = !show;
    }

    return 0;
}

/**
 *\fn           void drawWndCursor(int x, int y, bool show)
 *\brief        ��ʾ���
 *\param[in]    int x ���x
 *\param[in]    int y ���y
 *\param[in]    bool show �Ƿ���ʾ���
 *\return       void ��
 */
void CTerminal::drawWndCursor(int x, int y, bool show)
{
    EnterCriticalSection(&wndDrawlock_);

    POINT pt = {0};
    HPEN oldPen = (HPEN)::SelectObject(wndDC_, (show) ? fgPen_ : bgPen_);

    ::MoveToEx(wndDC_, x, y, &pt);
    ::LineTo(wndDC_, x, y + fontCy_);

    ::SelectObject(wndDC_, oldPen);

    LeaveCriticalSection(&wndDrawlock_);
}

/**
 *\fn           void setFocus(bool focus)
 *\brief        ���ý���
 *\param[in]    bool focus �Ƿ��н���
 *\return       void ��
 */
void CTerminal::setFocus(bool focus)
{
    focusTerm_ = (focus) ? this : NULL;
}

/*
 *\fn         void drawWndDc()
 *\brief      ˢ��
 *\return     void ��
 */
void CTerminal::drawWndDc()
{
    EnterCriticalSection(&wndDrawlock_);

    PAINTSTRUCT ps = {0};
    ::BeginPaint(wnd_, &ps);

    int cx = wndCx_ - TERM_EDIT_BORDER_WIDTH;
    int cy = wndCy_ - TERM_EDIT_BORDER_HEIGHT;
    int left = cy - scrollPageCy_ * fontCy_;
    UINT curPagePos = getCurPagePos();

    if (curPagePos <= (TERM_RESERVER_CY - scrollPageCy_))
    {
        ::BitBlt(ps.hdc, 0, 0, cx, cy, imageDC_, 0, curPagePos * fontCy_, SRCCOPY);
    }
    else
    {
        UINT top = TERM_RESERVER_CY - curPagePos;

        ::BitBlt(ps.hdc, 0, 0, cx, top * fontCy_, imageDC_, 0, curPagePos * fontCy_, SRCCOPY);
        ::BitBlt(ps.hdc, 0, top * fontCy_, cx, (scrollPageCy_ - top) * fontCy_, imageDC_, 0, 0, SRCCOPY);
    }

    if (left > 0)
    {
        ::BitBlt(ps.hdc, 0, scrollPageCy_ * fontCy_, cx, left, imageDC_, 0, imageDcCy_, SRCCOPY);
    }

    // ��ʾ��Ϣ
    RECT rc = {0};
    char info[128];
    sprintf_s(info, "%03d:%03d imagePos:%03d scrollPos:%03d\ncurPage:%03d tailPage:%03d", 
        cursorY_, cursorX_, imageTopPos_, scrollPos_, getCurPagePos(), getTailPagePos());
    ::DrawText(ps.hdc, info, strlen(info), &rc, DT_CALCRECT);

    int len = rc.right - rc.left;
    rc.left += (wndCx_ - TERM_EDIT_BORDER_WIDTH - len);
    rc.right = rc.left + len;
    ::DrawText(ps.hdc, info, strlen(info), &rc, DT_WORDBREAK|DT_NOCLIP);

    ::EndPaint(wnd_, &ps);

    LeaveCriticalSection(&wndDrawlock_);
}

/*
 *\fn         void clearScreen(int type)
 *\brief      ����
 *\param[in]  int type �������:0-�ӹ�굽��Ļ��,1-��Ļ�������,2-ȫ��
 *\return     void ��
 */
void CTerminal::clearScreen(int type)
{
    UINT y = 0;
    UINT cy = 0;
    UINT pagePos = getTailPagePos();
    bool clear = true;

    switch (type)
    {
    case 0:
        {
            y = cursorY_;
            cy = scrollPageCy_ - SCROLL_LEN(cursorY_, pagePos);
            break;
        }
    case 1:
        {
            y = scrollPos_;
            cy = SCROLL_LEN(cursorY_, pagePos);
            break;
        }
    case 2:
        {
            y = cursorY_;
            cy = scrollPageCy_;
            break;
        }
    default:
        {
            clear = false;
            ATLTRACE2("Don't know clear screen type\n");
            break;
        }
    }

    if (clear)
    {
        EnterCriticalSection(&wndDrawlock_);

        for (UINT i = 0; i <= cy; i++)
        {
            UINT pos = (i + y) % TERM_RESERVER_CY;

            ::BitBlt(imageDC_, 0, pos * fontCy_, screenCx_, fontCy_, imageDC_, 0, imageDcCy_, SRCCOPY);
        }

        LeaveCriticalSection(&wndDrawlock_);
    }
}

/**
 *\fn           void clearLineText(int type, int count)
 *\brief        ����
 *\param[in]    int type:0-�ӹ�굽��β,1-���׵����,2-ȫ��
 *\param[in]    int count �ַ�����
 *\return       void ��
 */
void CTerminal::clearLineText(int type, int count)
{
    int x = 0;
    int cx = 0;
    bool clear = true;

    switch (type)
    {
    case 0:
        {
            x = cursorX_ * fontCx_;
            cx = (0 == count) ? screenCx_ : (count * fontCx_);
            break;
        }
    case 1:
        {
            x = 0;
            cx = cursorX_ * fontCx_;
            break;
        }
    case 2:
        {
            x = 0;
            cx = screenCx_;
            break;
        }
    default:
        {
            clear = false;
            ATLTRACE2("Don't know clear line text type\n");
            break;
        }
    }

    if (clear)
    {
        EnterCriticalSection(&wndDrawlock_);
        ::BitBlt(imageDC_, x, cursorY_ * fontCy_, cx, fontCy_, imageDC_, 0, imageDcCy_, SRCCOPY);
        LeaveCriticalSection(&wndDrawlock_);
    }
}

/*
 *\fn         void setWndRect(int cx, int cy)
 *\brief      ���ô�����
 *\param[in]  int cx ��
 *\param[in]  int cy ��
 *\return     void ��
 */
void CTerminal::setWndRect(int cx, int cy)
{
    wndCx_ = cx;
    wndCy_ = cy;

    scrollPageCx_ = (wndCx_ - TERM_EDIT_BORDER_WIDTH) / fontCx_;
    scrollPageCy_ = (wndCy_ - TERM_EDIT_BORDER_HEIGHT) / fontCy_;

    SCROLLINFO info = {0};
    info.cbSize = sizeof (SCROLLINFO);
    info.fMask = SIF_ALL;
    GetScrollInfo(wnd_, SB_VERT, &info);

    if (info.nPage != scrollPageCy_)
    {
        //SCROLL_DEC(imageTopPos_, TERM_RESERVER_CY); // ��Ϊ�������ᷢ��һ���µ�������ʾ�����Բż�һ��
        scrollPos_ = TERM_RESERVER_CY - scrollPageCy_;
        info.nPos = scrollPos_;
        info.nPage = scrollPageCy_;
        SetScrollInfo(wnd_, SB_VERT, &info, TRUE);
    }

    // ˢ����Ļ
    ::InvalidateRect(wnd_, NULL, FALSE);
}

/*
 *\fn         void scrollScreen(WPARAM wParam)
 *\brief      ��ֱ����
 *\param[in]  WPARAM wParam ����
 *\return     void ��
 */
/**
 *\fn           void scrollScreen(int type, int data)
 *\brief        ��ֱ����
 *\param[in]    int type ����
 *\param[in]    int data ����
 *\return       void 0�ɹ�,����ʧ��
 */
void CTerminal::scrollScreen(int type, int data)
{
    switch (/*LOWORD(wParam)*/type)
    {
    case SB_THUMBTRACK:
        {
            scrollPos_ = /*HIWORD(wParam)*/data;
            break;
        }
    case SB_PAGEDOWN:
        {
            scrollPos_ = ((scrollPos_ + scrollPageCy_) < (TERM_RESERVER_CY - scrollPageCy_)) ? (scrollPos_ + scrollPageCy_) : (TERM_RESERVER_CY - scrollPageCy_);
            break;
        }
    case SB_PAGEUP:
        {
            scrollPos_ = (scrollPos_ > scrollPageCy_) ? (scrollPos_ - scrollPageCy_) : 0;
            break;
        }
    case SB_LINEDOWN:
        {
            LIMIT_INC(scrollPos_, TERM_RESERVER_CY - scrollPageCy_);
            break;
        }
    case SB_LINEUP:
        {
            LIMIT_DEC(scrollPos_, 0);
            break;
        }
    case SB_TOP:
        {
            scrollPos_ = 0;
            break;
        }
    case SB_BOTTOM:
        {
            scrollPos_ = TERM_RESERVER_CY - scrollPageCy_;
            break;
        }
    }

    if (scrollPos_ != (UINT)::GetScrollPos(wnd_, SB_VERT))     // ����λ�øı�
    {
        ::SetScrollPos(wnd_, SB_VERT, scrollPos_, TRUE); // ����λ��
        ::InvalidateRect(wnd_, NULL, FALSE); // ˢ����Ļ
    }
}

/**
 *\fn           void drawLineText(const char *text, int textLen, bool newLen)
 *\brief        ˢ��һ��
 *\param[in]    const char * text ��������
 *\param[in]    int textLen ���ֳ�
 *\param[in]    bool newLen ����
 *\return       void 0�ɹ�,����ʧ��
 */
void CTerminal::drawLineText(const char *text, int textLen, bool newLen, bool unicode)
{
    EnterCriticalSection(&wndDrawlock_);

    if (textLen > 0)
    {
        RECT rc = {0};
        rc.left = cursorX_ * fontCx_;
        rc.top  = cursorY_ * fontCy_;
        rc.bottom = rc.top + fontCy_;

        if (!unicode)
        {
            cursorX_ += textLen;
            rc.right  = rc.left + textLen * fontCx_;
            ::DrawText(imageDC_, text, textLen, &rc, DT_LEFT);
        }
        else
        {
            cursorX_ += 2 * textLen;
            rc.right += rc.left + 2 * textLen * fontCx_;; // 1�����ֵ���2��Ӣ�ĵĿ��
            ::DrawTextW(imageDC_, (LPCWSTR)text, textLen, &rc, DT_LEFT);
        }
    }

    if (newLen)
    {
        cursorX_ = 0;
        SCROLL_INC(cursorY_, TERM_RESERVER_CY);

        if (cursorY_ == imageTopPos_)
        {
            SCROLL_INC(imageTopPos_, TERM_RESERVER_CY);
        }

        ::BitBlt(imageDC_, 0, cursorY_ * fontCy_, screenCx_, fontCy_, imageDC_, 0, imageDcCy_, SRCCOPY);
    }

    LeaveCriticalSection(&wndDrawlock_);
}

/*
 *\fn         int outputText(const char *text, int textLen)
 *\brief      �������
 *\param[in]  const char * text ��������
 *\param[in]  int textLen ���ֳ�
 *\return     int 0�ɹ�,����ʧ��
 */
int CTerminal::outputText(const char *text, int textLen)
{
    if (NULL == text || textLen <= 0) return -1;

    int buffTextLen = 0;
    const char *buff = text;

    for (int i = 0; i < textLen; i++)
    {
        if (isprint((unsigned char)text[i]))
        {
            if ((cursorX_ + buffTextLen) < scrollPageCx_)
            {
                buffTextLen++;
            }
            else // �µ�һ��
            {
                drawLineText(buff, buffTextLen, true, false);

                buff = &text[i];
                buffTextLen = 1;
            }
        }
        
        else
        {
            switch (text[i])
            {
            case 0x07: // ����
                {
                    drawLineText(buff, buffTextLen, false, false);
                    buff = &text[i+1];
                    buffTextLen = 0;

                    MessageBeep(MB_OK);
                    break;
                }
            case 0x08: // �˸�
                {
                    drawLineText(buff, buffTextLen, false, false);

                    if (0 == cursorX_)
                    {
                        cursorX_ = scrollPageCx_;
                        SCROLL_DEC(cursorY_, TERM_RESERVER_CY);
                        SCROLL_DEC(imageTopPos_, TERM_RESERVER_CY);
                    }
                    else
                    {
                        cursorX_--;
                    }

                    buff = &text[i+1];
                    buffTextLen = 0;
                    break;
                }
            case 0x0d: // �س�
                {
                    drawLineText(buff, buffTextLen, true, false);

                    if (0x0a == text[i+1])
                    {
                        i++;
                    }

                    buff = &text[i+1];
                    buffTextLen = 0;
                    break;
                }
            case 0x1b: // ANSI���������Esc[��Ϊ������Ŀ�ʼ��־
                {
                    drawLineText(buff, buffTextLen, false, false);

                    int len = 1 + processCmd(&text[i+2]);
                    i += len;

                    buff = &text[i+1];
                    buffTextLen = 0;
                    break;
                }
            default:
                {
                    UINT32 c = (UINT8)text[i];
                    UINT32 unicode = 0;

                    // UTF8����
                    if ((c & 0xf0) == 0xe0)
                    {
                        UINT32 c0 = (c & 0x0f) << 12;
                        UINT32 c1 = ((UINT32)(UINT8)text[i+1] & 0x3f) << 6;
                        UINT32 c2 = ((UINT32)(UINT8)text[i+2] & 0x3f);
                        unicode = (c0 | c1 | c2);
                        i += 2;
                    }
                    else if ((c & 0xf8) == 0xf0)
                    {
                        UINT32 c0 = (c & 0x0f) << 18;
                        UINT32 c1 = ((UINT32)(UINT8)text[i+1] & 0x3f) << 12;
                        UINT32 c2 = ((UINT32)(UINT8)text[i+2] & 0x3f) << 6;
                        UINT32 c3 = ((UINT32)(UINT8)text[i+3] & 0x3f);
                        unicode = (c0 | c1 | c2 | c3);
                        i += 3;
                    }

                    if (unicode > 0)
                    {
                        drawLineText(buff, buffTextLen, false, false);
                        drawLineText((const char*)&unicode, 1, false, true);

                        buff = &text[i+1];
                        buffTextLen = 0;
                    }

                    char info[16];
                    sprintf_s(info, "----%x\n", c);
                    ATLTRACE2(info);
                    break;
                }
            }
        }
    }

    if (buffTextLen > 0)
    {
        drawLineText(buff, buffTextLen, false, false);
    }

    // ˢ����Ļ
    ::InvalidateRect(wnd_, NULL, FALSE);

    return 0;
}

/**
 *\fn           int processCmd(const char *text)
 *\brief        ��������
 *\param[in]    const char * text �����ַ�
 *\return       int �����
 */
int CTerminal::processCmd(const char *text)
{
    int i = 0;
    int len = strlen(text);
    UINT digit[5] = {0};
    UINT digitCount = 0;
    bool exit = false;

    for (i = 0; !exit && i < len; i++)
    {
        switch (text[i])
        {
        case ';': // �ָ�
            {
                digitCount++;
                break;
            }
        case 'A': // [nA ������� n �С�<<������Ļ��������Ч>>
            {
                SCROLL_SUB(cursorY_, ((0 == digit[1])? 1: digit[1]), TERM_RESERVER_CY);
                exit = true;
                break;
            }
        case 'B': // [nB ������� n �С�<<������Ļ�׶�����Ч>>
            {
                SCROLL_ADD(cursorY_, ((0 == digit[1])? 1: digit[1]), TERM_RESERVER_CY);
                exit = true;
                break;
            }
        case 'C': // [nC ������� n �С�<<��������Ļ���Ҷ�����Ч>>
            {
                LIMIT_ADD(cursorX_, ((0 == digit[1])? 1: digit[1]), scrollPageCx_-1);
                exit = true;
                break;
            }
        case 'D': // [nD ������� n �С�<<��������Ļ���������Ч>>
            {
                LIMIT_SUB(cursorX_, ((0 == digit[1])? 1: digit[1]), 0);
                exit = true;
                break;
            }
        case 'H': // [X;YH ����������� X �У��� Y �е�λ�á�
            {
                moveCursor(digit[2], digit[1]);
                exit = true;
                break;
            }
        case 'n': // [6n ������λ�á�
            {
                exit = true;
                break;
            }
        case 's': // [s ����Ŀǰ�Ĺ��λ�á�
            {
                saveCursorX_ = cursorX_;
                saveCursorY_ = cursorY_;
                exit = true;
                break;
            }
        case 'u': // [u ȡ������Ĺ��λ����ʹ�á�
            {
                cursorX_ = saveCursorX_;
                cursorY_ = saveCursorY_;
                exit = true;
                break;
            }
        case 'J': // [2J �����Ļ��[0J��굽��Ļ��,[1J��Ļ�������
            {
                clearScreen(digit[1]);
                exit = true;
                break;
            }
        case 'K': // [K �����λ�ÿ�ʼ�����н����������ַ������[1K��ʼ�����, [2K����
            {
                clearLineText(digit[1], 0);
                exit = true;
                break;
            }
        case 'P': // [nP ɾ��n���ַ�
            {
                clearLineText(0, 0);
                exit = true;
                break;
            }
        case 'm': // [n1;n2;......m �趨��ʾ�ַ�������״̬
            {
                setColor(digitCount, &digit[1]);
                exit = true;
                break;
            }
        case 'I': // [0I �ָ���������,[1I ����,[2I ����,[3I ����
            {
                exit = true;
                break;
            }
        case 'G': // [G ����
            {
                MessageBeep(MB_OK);
                exit = true;
                break;
            }
        case 'U': // [nU URLȷ��(n���ֽ�)
            {
                exit = true;
                break;
            }
        case 'M': // [nM ��ʱ(nΪ΢����) <<Cterm��Sterm 1.266+�汾��֧�ִ˹���>>
            {
                exit = true;
                break;
            }
        default:
            {
                if (isdigit(text[i])) // ����
                {
                    if (0 == digitCount) digitCount = 1;

                    digit[digitCount] *= 10;
                    digit[digitCount] += (text[i] - '0');
                }
                else
                {
                    char info[128];
                    sprintf_s(info, "don't know cmd----%d %c\n", text[i], text[i]);
                    ATLTRACE2(info);
                }
            }
        }
    }

    return i;
}

/**
 *\fn           void moveCursor(UINT x, UINT y)
 *\brief        �ƶ����
 *\param[in]    UINT x ���X��λ��
 *\param[in]    UINT y ���Y��λ��
 *\return       void ��
 */
void CTerminal::moveCursor(UINT x, UINT y)
{
    if (0 == x && 0 == y)
    {
        if (0 == lastCursorY_)
        {
            SCROLL_ADD(imageTopPos_, scrollPageCy_, TERM_RESERVER_CY);
            cursorY_ = getCurPagePos();
            lastCursorY_ = cursorY_;
        }
        else
        {
            cursorY_ = lastCursorY_;
        }

        cursorX_ = 0;
    }
    else
    {
        cursorX_ = x - 1;
        cursorY_ = (lastCursorY_ + y - 1) % TERM_RESERVER_CY;

        if (y >= scrollPageCy_)
        {
            int cy = y / scrollPageCy_;
            SCROLL_ADD(imageTopPos_, cy, TERM_RESERVER_CY);
        }
    }
}

/**
 *\fn           void setColor(UINT count, UINT *data)
 *\brief        ������ɫ
 *\param[in]    UINT count ���ݸ���
 *\param[in]    UINT * data ����
 *\return       void ��
 */
void CTerminal::setColor(UINT count, UINT *data)
{
    // 0 �������� OFF��������������ʾģʽ (Normal)
    // 1 ��������ʾ (Bright)
    // 4 ���»��� (Underline)
    // 5 ��˸��ʾ (Flash)
    // 7 ������ʾ (Inverse)
    // 8 ���ɼ� (Invisable)
    // ǰ�� ���� ��ɫ
    // 30   40   ��ɫ
    // 31   41   ��ɫ
    // 32   42   ��ɫ
    // 33   43   ��ɫ
    // 34   44   ��ɫ
    // 35   45   �Ϻ�ɫ
    // 36   46   ����ɫ
    // 37   47   ��ɫ

    bool updateColor = false;

    EnterCriticalSection(&wndDrawlock_);

    for (UINT i = 0; i < count; i++)
    {
        if (data[i] == 0)
        {
            fgColor_ = color_[7];
            bgColor_ = color_[0];
            updateColor = true;
        }
        else if (data[i] == 7)
        {
            COLORREF temp = fgColor_;
            fgColor_ = bgColor_;
            bgColor_ = temp;
            updateColor = true;
        }
        else if (data[i] == 8)
        {
            fgColor_ = color_[0];
            bgColor_ = color_[0];
            updateColor = true;
        }
        else if (data[i] >= 30 && data[i] <= 37)
        {
            fgColor_ = color_[data[i] - 30];
            ::SetTextColor(imageDC_, fgColor_);
        }
        else if (data[i] >= 40 && data[i] <= 47)
        {
            bgColor_ = color_[data[i] - 40];
            ::SetBkColor(imageDC_, bgColor_);
        }

        if (updateColor)
        {
            updateColor = false;
            ::SetBkColor(imageDC_, bgColor_);
            ::SetTextColor(imageDC_, fgColor_);
        }
    }

    LeaveCriticalSection(&wndDrawlock_);
}

/**
 *\fn           UINT getCurPagePos()
 *\brief        �õ���ǰҳ��λ��
 *\return       UINT ��ǰҳҳ��λ��
 */
UINT CTerminal::getCurPagePos()
{
    return ((imageTopPos_ + scrollPos_) % TERM_RESERVER_CY);
}

/**
 *\fn           UINT getTailPagePos()
 *\brief        �õ�βҳҳ��λ��
 *\return       UINT βҳҳ��λ��
 */
UINT CTerminal::getTailPagePos()
{
    return ((imageTopPos_ + TERM_RESERVER_CY - scrollPageCy_) % TERM_RESERVER_CY);
}
