#pragma once


class CXTDC;

class CXTProgress
{
public:
    CXTProgress();
    virtual ~CXTProgress();

private:
    HDC      m_hDC;     // ������ʾDC������DC
    HDC      m_hCompDC; // �м���λͼ��DC
    HDC      m_hBmpDC;  // ����λͼDC
    HFONT    m_hFont;   // text font handler
    COLORREF m_clFont;  // text font color

    SIZE  m_Sz;
    POINT m_Pt;
    POINT m_SrcPt;

    UINT  m_nMax; // progress max number

public:
    bool Initial(LPCTSTR strIniFile);
    bool Initial(int x, int y, int cx, int cy, int sx, int sy, CXTDC *pDBuffDC);
    bool SetParam(UINT nMax);
    void DrawImage(UINT nProgress, LPCTSTR strMessage = NULL);
};


