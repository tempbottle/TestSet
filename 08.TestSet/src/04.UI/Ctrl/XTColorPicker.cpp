
#include "stdafx.h"
#include "XTColorPicker.h"
#include <math.h>
#include "windowsx.h"		// for GET_X_LPARAM

#define DIVIDER_LINE_ROW	9
#define CUSTOM_BUTTON_ROW	10
#define FIRST_CUSTOM_ROW	11

ColorTableEntry CXTColorPopup::m_crColors[] =
{
    // row 0
    RGB(128,   0,  64), _T("Light Maroon"),        -1,
        RGB(139,   0,   0), _T("Dark Red"),            -1,
        RGB(255,   0,   0), _T("Red"),                 -1,
        RGB(255,   0, 128), _T("Deep Pink"),           -1,
        RGB(255,   0, 255), _T("Magenta"),             -1,
        RGB(255, 128, 192), _T("Pink"),                -1,
        RGB(255, 128, 255), _T("Light Pink"),          -1,
        RGB(255, 208, 232), _T("Very Light Pink"),     -1,

        // row 1
        RGB(255, 140,   0), _T("Dark Orange"),         -1,
        RGB(255, 165,   0), _T("Orange"),              -1,
        RGB(255, 128, 128), _T("Light Salmon"),        -1,
        RGB(255, 224, 160), _T("Light Orange"),        -1,
        RGB(255, 215,   0), _T("Gold"),                -1,
        RGB(255, 255,   0), _T("Yellow"),              -1,
        RGB(255, 255, 128), _T("Desert Yellow"),       -1,
        RGB(255, 255, 184), _T("Pale Yellow"),         -1,

        // row 2
        RGB(  0,  64,   0), _T("Dark Green"),          -1,
        RGB(  0,  64,  64), _T("Medium Green"),        -1,
        RGB(  0, 128,   0), _T("Green"),               -1,
        RGB(  0, 128,  64), _T("Dark Teal"),           -1,
        RGB(128, 128,   0), _T("Olive"),               -1,
        RGB(  0, 128, 128), _T("Teal"),                -1,
        RGB( 64, 128, 128), _T("Turquoise"),           -1,
        RGB(  0, 255,  64), _T("Medium Spring Green"), -1,

        // row 3
        RGB(128, 255,   0), _T("Mint Green"),          -1,
        RGB(  0, 255,   0), _T("Lime"),                -1,
        RGB(  0, 255, 128), _T("Spring Green"),        -1,
        RGB(152, 251, 152), _T("Pale Green"),          -1,
        RGB(127, 255, 212), _T("Aquamarine"),          -1,
        RGB(192, 220, 192), _T("Very Pale Green"),     -1,
        RGB( 64,   0,  64), _T("Deep Purple"),         -1,
        RGB(128,   0, 128), _T("Purple"),              -1,

        // row 4
        RGB( 64,   0, 128), _T("Deep Violet"),         -1,
        RGB(  0,   0,  64), _T("Midnight Blue"),       -1,
        RGB(  0,   0, 160), _T("Dark Blue"),           -1,
        RGB(  0,   0, 128), _T("Navy"),                -1,
        RGB(128,   0, 255), _T("Violet"),              -1,
        RGB(  0,   0, 255), _T("Blue"),                -1,
        RGB(  0,  64, 128), _T("Medium Blue"),         -1,
        RGB(  0, 128, 255), _T("Light Sky Blue"),      -1,

        // row 5
        RGB(100, 149, 237), _T("Cornflower Blue"),     -1,
        RGB(128, 128, 192), _T("Sky Blue"),            -1,
        RGB(  0, 128, 192), _T("Dark Cyan"),           -1,
        RGB(128, 128, 255), _T("Medium Slate Blue"),   -1,
        RGB(128, 255, 255), _T("Light Blue"),          -1,
        RGB(  0, 255, 255), _T("Cyan"),                -1,
        RGB(176, 224, 230), _T("Powder Blue"),         -1,
        RGB(224, 255, 255), _T("Light Cyan"),          -1,

        // row 6
        RGB(  0,   0,   0), _T("Black"),               -1,
        RGB(128, 128, 128), _T("Gray"),                -1,
        RGB(152, 152, 152), _T("Gray 40%"),            -1,
        RGB(192, 192, 192), _T("Silver"),              -1,
        RGB(211, 211, 211), _T("Light Gray"),          -1,
        RGB(245, 245, 220), _T("Beige"),               -1,
        RGB(255, 251, 240), _T("Floral White"),        -1,
        RGB(255, 255, 255), _T("White"),               -1,

        // row 7 - window system colors
        RGB(  0,   0,   0), _T("Window"),              COLOR_WINDOW,
        RGB(  0,   0,   0), _T("WindowText"),          COLOR_WINDOWTEXT,
        RGB(  0,   0,   0), _T("InfoBk"),              COLOR_INFOBK,
        RGB(  0,   0,   0), _T("InfoText"),            COLOR_INFOTEXT,
        RGB(  0,   0,   0), _T("ActiveCaption"),       COLOR_ACTIVECAPTION,
        RGB(  0,   0,   0), _T("CaptionText"),         COLOR_CAPTIONTEXT,
        RGB(  0,   0,   0), _T("InactiveCaption"),     COLOR_INACTIVECAPTION,
        RGB(  0,   0,   0), _T("InactiveCaptionText"), COLOR_INACTIVECAPTIONTEXT,

        // row 8 - window system colors, row #2
        RGB(  0,   0,   0), _T("BtnFace"),             COLOR_BTNFACE,
        RGB(  0,   0,   0), _T("BtnText"),             COLOR_BTNTEXT,
        RGB(  0,   0,   0), _T("BtnShadow"),           COLOR_BTNSHADOW,
        RGB(  0,   0,   0), _T("BtnHighlight"),        COLOR_BTNHIGHLIGHT,
        RGB(  0,   0,   0), _T("Highlight"),           COLOR_HIGHLIGHT,
        RGB(  0,   0,   0), _T("HighlightText"),       COLOR_HIGHLIGHTTEXT,
        RGB(  0,   0,   0), _T("3DLight"),             COLOR_3DLIGHT,
        RGB(  0,   0,   0), _T("Desktop"),             COLOR_DESKTOP,

        // row 9 - divider line
        RGB(192, 192, 192), _T("1"),                   -1,
        RGB(192, 192, 192), _T("2"),                   -1,
        RGB(192, 192, 192), _T("3"),                   -1,
        RGB(192, 192, 192), _T("4"),                   -1,
        RGB(192, 192, 192), _T("5"),                   -1,
        RGB(192, 192, 192), _T("6"),                   -1,
        RGB(192, 192, 192), _T("7"),                   -1,
        RGB(192, 192, 192), _T("8"),                   -1,

        // row 10 - custom button
        RGB(192, 192, 192), _T("9"),                   -1,
        RGB(192, 192, 192), _T("10"),                  -1,
        RGB(192, 192, 192), _T("11"),                  -1,
        RGB(192, 192, 192), _T("12"),                  -1,
        RGB(192, 192, 192), _T("13"),                  -1,
        RGB(192, 192, 192), _T("14"),                  -1,
        RGB(192, 192, 192), _T("15"),                  -1,
        RGB(192, 192, 192), _T("16"),                  -1,

        // custom colors
        // row 11
        (unsigned long)-1, _T("Custom color 1"),       -1,
        (unsigned long)-1, _T("Custom color 2"),       -1,
        (unsigned long)-1, _T("Custom color 3"),       -1,
        (unsigned long)-1, _T("Custom color 4"),       -1,
        (unsigned long)-1, _T("Custom color 5"),       -1,
        (unsigned long)-1, _T("Custom color 6"),       -1,
        (unsigned long)-1, _T("Custom color 7"),       -1,
        (unsigned long)-1, _T("Custom color 8"),       -1,

        // row 12
        (unsigned long)-1, _T("Custom color 9"),       -1,
        (unsigned long)-1, _T("Custom color 10"),      -1,
        (unsigned long)-1, _T("Custom color 11"),      -1,
        (unsigned long)-1, _T("Custom color 12"),      -1,
        (unsigned long)-1, _T("Custom color 13"),      -1,
        (unsigned long)-1, _T("Custom color 14"),      -1,
        (unsigned long)-1, _T("Custom color 15"),      -1,
        (unsigned long)-1, _T("Custom color 16"),      -1
};


BEGIN_MESSAGE_MAP(CXTColorPopup, CWnd)
    ON_WM_NCDESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CXTColorPopup::CXTColorPopup()
{
    Init();
}

CXTColorPopup::CXTColorPopup(CWnd * pParentWnd,
                             CPoint p,
                             COLORREF crColor,
                             COLORREF * pCustomColors,
                             int nSelectedRow,
                             int nSelectedCol,
                             LPCTSTR szCustomText /* = NULL */)
{
    Init();

    m_nSelectedRow  = nSelectedRow;
    m_nSelectedCol  = nSelectedCol;
    m_crColor       = m_crInitialColor = crColor;
    m_pParent       = pParentWnd;

    int i;
    if (nSelectedRow == -1 || nSelectedCol == -1)
    {
        // no selection specified - look thru the color table for a match
        for (i = 0; i < m_nNumColors; i++)
        {
            if (m_crColors[i].crColor == crColor)
            {
                m_nSelectedRow = i / 8;
                m_nSelectedCol = i % 8;
                break;
            }
        }
    }

    if (pCustomColors)
    {
        m_pCustomColors = pCustomColors;

        for (i = 0; i < 16; i++)
        {
            m_crColors[FIRST_CUSTOM_ROW * 8 + i].crColor = m_pCustomColors[i];
        }
    }

    Create(p, crColor, pParentWnd, szCustomText);
}

void CXTColorPopup::Init()
{
    m_nNumColors    = sizeof(m_crColors) / sizeof(ColorTableEntry);
    ASSERT(m_nNumColors == 104);
    m_nNumColumns   = 0;
    m_nNumRows      = 0;
    m_nBoxSize      = 18;
    m_nMargin       = ::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentRow   = -1;
    m_nCurrentCol   = -1;
    m_nSelectedRow  = -1;
    m_nSelectedCol  = -1;
    m_crColor       = m_crInitialColor = RGB(255, 255, 255);
    m_pCustomColors = NULL;
    m_bShowCustom   = TRUE;
    m_strCustomText = _T("Custom...");
    m_pParent       = NULL;

    // Make sure the color square is at least 5 x 5;
    if (m_nBoxSize - 2 * m_nMargin - 2 < 5)
        m_nBoxSize = 5 + 2 * m_nMargin + 2;

    // set system colors
    for (int i = 0; i < m_nNumColors; i++)
        if (m_crColors[i].nSystemColor != -1)
            m_crColors[i].crColor = GetSysColor(m_crColors[i].nSystemColor);
}

CXTColorPopup::~CXTColorPopup()
{
    if (m_Font.GetSafeHandle())
        m_Font.DeleteObject();
}

BOOL CXTColorPopup::Create(CPoint p,
                           COLORREF crColor,
                           CWnd * pParentWnd,
                           LPCTSTR szCustomText /* = NULL */)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));

    m_pParent  = pParentWnd;
    m_crColor  = m_crInitialColor = crColor;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(
        CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW,
        0, (HBRUSH)GetStockObject(LTGRAY_BRUSH), 0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE | WS_POPUP,
        p.x, p.y, 100, 100, // size updated soon
        pParentWnd->GetSafeHwnd(), 0, NULL))
    {
        return FALSE;
    }

    // Store the Custom text
    if (szCustomText != NULL)
        m_strCustomText = szCustomText;

    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

// PreTranslateMessage - for tooltips
BOOL CXTColorPopup::PreTranslateMessage(MSG * pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

// OnKeyDown - if an arrow key is pressed, then move the selection
void CXTColorPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int row = m_nCurrentRow;
    int col = m_nCurrentCol;

    if (row == -1 || col == -1)
    {
        row = m_nSelectedRow;
        col = m_nSelectedCol;
    }

    if (nChar == VK_DOWN)
    {
        row++;
        if (row < 0)
        {
            row = 0;
            col = 0;
        }
        else if (row == DIVIDER_LINE_ROW)
        {
            row++;
        }
        else if (row >= m_nNumRows)
        {
            row = 0;
        }
        ChangeSelection(row, col);
    }

    if (nChar == VK_UP)
    {
        row--;
        if (row < 0)
        {
            row = m_nNumRows - 1;
        }
        else if (row == DIVIDER_LINE_ROW)
        {
            row--;
        }
        ChangeSelection(row, col);
    }

    if (nChar == VK_RIGHT)
    {
        if (col < 0)
        {
            row = 0;
            col = 0;
        }
        else if (col < m_nNumColumns - 1)
        {
            col++;
        }
        else
        {
            col = 0;
        }
        ChangeSelection(row, col);
    }

    if (nChar == VK_LEFT)
    {
        if (col < 0)
        {
            row = m_nNumRows - 1;
            col = m_nNumColumns - 1;
        }
        else if (col > 0)
        {
            col--;
        }
        else
        {
            col = m_nNumColumns - 1;
        }
        ChangeSelection(row, col);
    }

    if (nChar == VK_ESCAPE)
    {
        m_crColor = m_crInitialColor;
        EndSelection(CPN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN)
    {
        EndSelection(CPN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// OnNcDestroy - auto-deletion
void CXTColorPopup::OnNcDestroy()
{
    CWnd::OnNcDestroy();
    delete this;
}

// OnPaint
void CXTColorPopup::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // Draw color cells
    for (int row = 0; row < m_nNumRows; row++)
        for (int col = 0; col < m_nNumColumns; col++)
            DrawCell(&dc, row, col);

    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

// OnMouseMove
void CXTColorPopup::OnMouseMove(UINT nFlags, CPoint point)
{
    int row, col;

    if (point.x < 0 || point.y < 0)
    {
        ChangeSelection(-1, -1);
        CWnd::OnMouseMove(nFlags, point);
        return;
    }

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // First check we aren't in text box
    if (m_bShowCustom && m_TextRect.PtInRect(point))
    {
        row = CUSTOM_BUTTON_ROW;        // Special value meaning Custom button (hack!)
        col = 0;
    }
    else if (m_bShowCustom && m_DividerRect.PtInRect(point))
    {
        row = DIVIDER_LINE_ROW;        // Special value meaning divider line
        col = 0;
    }
    else
    {
        // Get the row and column
        row = point.y / m_nBoxSize,
            col = point.x / m_nBoxSize;

        // In range? If not, default and exit
        if (row < 0 || row >= m_nNumRows || row == DIVIDER_LINE_ROW ||
            col < 0 || col >= m_nNumColumns)
        {
            ChangeSelection(-1, -1);
            CWnd::OnMouseMove(nFlags, point);
            return;
        }
    }

    // OK - we have the row and column of the current selection (may be TEXT_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.
    if (row != m_nCurrentRow || col != m_nCurrentCol)
    {
        ChangeSelection(row, col);
    }

    CWnd::OnMouseMove(nFlags, point);
}

// OnLButtonUp - end selection on LButtonUp
void CXTColorPopup::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(GET_X_LPARAM(pos), GET_Y_LPARAM(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(CPN_SELENDOK);
    else
        EndSelection(CPN_SELENDCANCEL);
}

// GetCellRect - gets the dimensions of the color cell given by (row,col)
BOOL CXTColorPopup::GetCellRect(int row, int col, const LPRECT & rect)
{
    if (row < 0 || row >= m_nNumRows || col < 0 || col >= m_nNumColumns) return FALSE;

    rect->left = col * m_nBoxSize + m_nMargin;
    rect->top  = row * m_nBoxSize + m_nMargin;

    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;

    if (row == DIVIDER_LINE_ROW)
        return FALSE;
    else
        return TRUE;
}

// SetWindowSize - works out an appropriate size and position of this window
void CXTColorPopup::SetWindowSize()
{
    CSize TextSize;

    // If we are showing a custom text area, get the font and text size.
    if (m_bShowCustom)
    {
        // Create the font
        if (m_Font.GetSafeHandle() == NULL)
        {
// 			NONCLIENTMETRICS ncm;
// 			ncm.cbSize = sizeof(NONCLIENTMETRICS);
// 			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
// 			m_Font.CreateFontIndirect(&(ncm.lfMessageFont));

			LOGFONT lf  = {0};
			lf.lfHeight = 15;
			m_Font.CreateFontIndirect(&lf);
        }

        // Get the size of the custom text
        CClientDC dc(this);
        CFont * pOldFont = (CFont *) dc.SelectObject(&m_Font);
        TextSize = dc.GetTextExtent(m_strCustomText) + CSize(2 * m_nMargin, 2 * m_nMargin);
        dc.SelectObject(pOldFont);
    }

    // Get the number of columns and rows
    m_nNumColumns = 8;
    m_nNumRows = (m_nNumColors / m_nNumColumns);
    if (m_nNumColors % m_nNumColumns)
        m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top,
        rect.left + m_nNumColumns * m_nBoxSize + 2 * m_nMargin,
        rect.top + m_nNumRows * m_nBoxSize + 2 * m_nMargin);

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_bShowCustom)
    {
        //m_WindowRect.bottom += (m_nMargin + TextSize.cy);
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width() - 2 * m_nMargin;

        // Work out the text area
        m_TextRect.SetRect(m_nMargin,
            m_nMargin + CUSTOM_BUTTON_ROW*m_nBoxSize,
            m_nMargin + TextSize.cx,
            m_nMargin + CUSTOM_BUTTON_ROW*m_nBoxSize + TextSize.cy);
        m_DividerRect.SetRect(m_nMargin,
            m_nMargin + DIVIDER_LINE_ROW*m_nBoxSize,
            m_nMargin + TextSize.cx,
            m_nMargin + DIVIDER_LINE_ROW*m_nBoxSize + m_nBoxSize);
    }

    // Set the window size and position
    MoveWindow(m_WindowRect, TRUE);
}

// CreateToolTips
void CXTColorPopup::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(this))
        return;

    // Add a tool for each cell
    for (int row = 0; row < m_nNumRows; row++)
    {
        if (row == DIVIDER_LINE_ROW)
            continue;

        if (row == CUSTOM_BUTTON_ROW)
        {
            m_ToolTip.AddTool(this, _T("Selects a custom color"), m_TextRect, 1);
            continue;
        }

        for (int col = 0; col < m_nNumColumns; col++)
        {
            CRect rect;
            if (!GetCellRect(row, col, rect))
            {
                continue;
            }

            CString strColorName(GetColorName(row, col));
            strColorName.AppendFormat(_T(",RGB=%06X"), GetColor(row, col));

            VERIFY(m_ToolTip.AddTool(this, strColorName/*GetColorName(row, col)*/, rect, 1));
        }
    }
}

// ChangeSelection
void CXTColorPopup::ChangeSelection(int row, int col)
{
    CClientDC dc(this);        // device context for drawing

    //int row = (row1 < 5) ? row1 : row1 + 1;

    if ((m_nCurrentRow >= 0 && m_nCurrentRow < m_nNumRows &&
        m_nCurrentCol >= 0 && m_nCurrentCol < m_nNumColumns) ||
        (m_nCurrentRow == CUSTOM_BUTTON_ROW) ||
        (m_nCurrentRow == DIVIDER_LINE_ROW))
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldRow = m_nCurrentRow;
        int OldCol = m_nCurrentCol;
        m_nCurrentRow = m_nCurrentCol = -1;
        DrawCell(&dc, OldRow, OldCol);
    }

    if (row == -1 || col == -1) return;

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentRow = row;
    m_nCurrentCol = col;

    if (row == DIVIDER_LINE_ROW)
        return;

    DrawCell(&dc, row, col);

    // Store the current color
    if (m_nCurrentRow == CUSTOM_BUTTON_ROW)// && m_nCurrentCol == TEXT_BOX_VALUE)
    {
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crInitialColor, 0);
    }
    else
    {
        m_crColor = GetColor(m_nCurrentRow, m_nCurrentCol);
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crColor, 0);
    }
}

// EndSelection
void CXTColorPopup::EndSelection(int nMessage)
{
    ReleaseCapture();

    if (nMessage != CPN_SELENDCANCEL && m_nCurrentRow == DIVIDER_LINE_ROW)
    {
        // ignore click on divider line
        SetCapture();
        return;
    }
    else if (nMessage != CPN_SELENDCANCEL && m_nCurrentRow == CUSTOM_BUTTON_ROW)
    {
        // if custom text selected, perform a custom color selection
        COLORREF rgb[16];
        int i, row, col;
        for (i = 0, row = FIRST_CUSTOM_ROW; row < FIRST_CUSTOM_ROW+2; row++)
            for (col = 0; col < 8; col++)
                rgb[i++] = GetColor(row, col);

        CColorDialog dlg(m_crInitialColor, CC_FULLOPEN | CC_ANYCOLOR, this);
        dlg.m_cc.lStructSize  = sizeof(CHOOSECOLOR);
        dlg.m_cc.hwndOwner    = m_hWnd;
        dlg.m_cc.lpCustColors = rgb;
        dlg.m_cc.Flags        |= CC_FULLOPEN | CC_ANYCOLOR;

        if (dlg.DoModal() == IDOK)
        {
            m_crColor = dlg.GetColor();
            for (i = 0, row = FIRST_CUSTOM_ROW; row < FIRST_CUSTOM_ROW+2; row++)
                for (col = 0; col < 8; col++)
                    SetColor(row, col, rgb[i++]);
            Invalidate();
        }
        SetCapture();
        return;
    }

    if (nMessage == CPN_SELENDCANCEL)  m_crColor = m_crInitialColor;

    m_nSelectedRow = m_nCurrentRow;
    m_nSelectedCol = m_nCurrentCol;

    if (m_pCustomColors)
    {
        for (int i = 0; i < 16; i++)
        {
            m_pCustomColors[i] = m_crColors[FIRST_CUSTOM_ROW * 8 + i].crColor;
        }
    }

    m_pParent->SendMessage(nMessage, (WPARAM) m_crColor, MAKELONG((WORD)m_nSelectedRow, (WORD)m_nSelectedCol));

    DestroyWindow();
}

// DrawCell
void CXTColorPopup::DrawCell(CDC * pDC, int row, int col)
{
    // This is a special hack for the text box
    if (m_bShowCustom && row == CUSTOM_BUTTON_ROW)// && row == TEXT_BOX_VALUE)
    {
        pDC->FillSolidRect(m_TextRect, ::GetSysColor(COLOR_3DFACE));
        if (m_nCurrentRow == row && m_nCurrentCol == col)
            pDC->DrawEdge(m_TextRect, EDGE_RAISED, BF_RECT);

        // Draw custom text
        CFont * pOldFont = (CFont *) pDC->SelectObject(&m_Font);
        pDC->DrawText(m_strCustomText, m_TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);
        return;
    }

    // row/col in range?
    ASSERT(row >= 0 && row < m_nNumRows);
    ASSERT(col >= 0 && col < m_nNumColumns);

    CRect rect;
    if (!GetCellRect(row, col, rect))
    {
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));
        if (row != DIVIDER_LINE_ROW) return;

        // we're trying to draw divider line
        CRect divider(rect);
        int h = divider.Height();
        GetCellRect(row, 7, rect);
        divider.right = rect.right;
        divider.top += (h/2) - 2;
        divider.bottom -= (h/2) - 2;
        UINT nFlags = BF_BOTTOM | BF_TOP;
        if (col == 0) nFlags |= BF_LEFT;
        if (col == 7) nFlags |= BF_RIGHT;
        pDC->DrawEdge(divider, EDGE_SUNKEN, nFlags);
        return;
    }

    // fill background
    pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw raised edge if selected
    if (m_nCurrentRow == row && m_nCurrentCol == col)
        pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
    else if (m_nSelectedRow == row && m_nSelectedCol == col)
        pDC->DrawEdge(rect, EDGE_SUNKEN, BF_RECT);

    COLORREF rgbCell = GetColor(row, col);
    if (rgbCell == -1)
        rgbCell = RGB(255, 255, 255);
    CBrush brush(rgbCell);
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

    CBrush * pOldBrush = (CBrush *) pDC->SelectObject(&brush);
    CPen * pOldPen = (CPen *) pDC->SelectObject(&pen);

    // Draw the cell color
    rect.DeflateRect(m_nMargin + 1, m_nMargin + 1);
    pDC->Rectangle(rect);

    // restore DC and cleanup
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    brush.DeleteObject();
    pen.DeleteObject();
}

//---------------------------------------------------------------------------------------------------------

// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

void AFXAPI DDX_ColorPicker(CDataExchange * pDX, int nIDC, COLORREF & crColor)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT(hWndCtrl != NULL);

    CXTColorPicker * pColorPicker = (CXTColorPicker *) CWnd::FromHandle(hWndCtrl);

    if (pDX->m_bSaveAndValidate)
    {
        crColor = pColorPicker->GetColor();
    }
    else // initializing
    {
        pColorPicker->SetColor(crColor);
    }
}

void AFXAPI DDX_ColorPickerCustom(CDataExchange * pDX, int nIDC, CString & strCustomColors)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT(hWndCtrl != NULL);

    CXTColorPicker * pColorPicker = (CXTColorPicker *) CWnd::FromHandle(hWndCtrl);

    if (pDX->m_bSaveAndValidate)
    {
        strCustomColors = pColorPicker->GetCustomColors();
    }
    else // initializing
    {
        pColorPicker->SetCustomColors(strCustomColors);
    }
}


IMPLEMENT_DYNCREATE(CXTColorPicker, CButton)
BEGIN_MESSAGE_MAP(CXTColorPicker, CButton)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_ENABLE()
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE, OnSelChange)
END_MESSAGE_MAP()

CXTColorPicker::CXTColorPicker()
{
    m_crEnabledColor = m_crColor = ::GetSysColor(COLOR_3DFACE);    // initial color
    m_bTrackSelection = FALSE;
    m_nSelectedRow = -1;
    m_nSelectedCol = -1;

    m_crColor = RGB(255,255,255);
    m_DefaultColor = ::GetSysColor(COLOR_APPWORKSPACE);
    m_bPopupActive = FALSE;

    for (int i = 0; i < 16; i++)
        m_CustomColors[i] = RGB(255,255,255);
}

CXTColorPicker::~CXTColorPicker()
{
}

LONG CXTColorPicker::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    m_nSelectedRow = LOWORD(lParam);
    m_nSelectedCol = HIWORD(lParam);

    CWnd * pParent = GetParent();
    if (pParent)
    {
        pParent->SendMessage(CPN_CLOSEUP, wParam, (LPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, wParam, (LPARAM) GetDlgCtrlID());
    }

    COLORREF crNewColor = (COLORREF) wParam;
    if (crNewColor != m_crColor)
    {
        SetColor(crNewColor);
        if (pParent)
            pParent->SendMessage(CPN_SELCHANGE, wParam, (LPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CXTColorPicker::OnSelEndCancel(WPARAM wParam, LPARAM)
{
    SetColor((COLORREF) wParam);

    CWnd * pParent = GetParent();
    if (pParent)
    {
        pParent->SendMessage(CPN_CLOSEUP, wParam, (LPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, wParam, (LPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CXTColorPicker::OnSelChange(WPARAM wParam, LPARAM)
{
    if (m_bTrackSelection)
        SetColor((COLORREF) wParam);

    return TRUE;
}

int CXTColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return - 1;

    return 0;
}

// On mouse click, create and show a CXColorPopup window for color selection
BOOL CXTColorPicker::OnClicked()
{
    CRect rect;
    GetWindowRect(rect);

    // note that m_CustomColors will be updated by CXColorPopup handler
    // when user selects a color
    new CXTColorPopup(this, CPoint(rect.left, rect.bottom), m_crColor, m_CustomColors);

    CWnd * pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)m_crColor, (WPARAM) GetDlgCtrlID());

    // docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.
    return TRUE;
}

// CXColourPicker overrides
void CXTColorPicker::PreSubclassWindow()
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
}

// CXColourPicker implementation
CSize CXTColorPicker::GetArrowSize()
{
    // Get size dimensions of edges
    CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Get size of dropdown arrow
    int nArrowWidth  = ::GetSystemMetrics(SM_CXHTHUMB) / 3 + 2;
    int nArrowHeight = ::GetSystemMetrics(SM_CYVTHUMB) / 3 + 2;
    CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

    // Make sure arrow size is an odd number (just looks neater)
    if ((ArrowSize.cx & 1) == 0)
    {
        ArrowSize.cx++;
        ArrowSize.cy++;
    }

    return ArrowSize;
}

void CXTColorPicker::SetColor(COLORREF crColor)
{
    m_crEnabledColor = m_crColor = crColor;
    RedrawWindow();
}

COLORREF CXTColorPicker::GetColor()
{
    return m_crColor;
}

void CXTColorPicker::SetCustomColors(COLORREF * pCustomColors)
{
    ASSERT(pCustomColors);
    for (int i = 0; i < 16; i++)
        m_CustomColors[i] = pCustomColors[i];
}

void CXTColorPicker::GetCustomColors(COLORREF * pCustomColors)
{
    ASSERT(pCustomColors);
    for (int i = 0; i < 16; i++)
        pCustomColors[i] = m_CustomColors[i];
}

void CXTColorPicker::SetCustomColors(LPCTSTR lpszCustomColors)
{
    int i = 0;

    // initialize to white
    for (i = 0; i < 16; i++)
        m_CustomColors[i] = RGB(255,255,255);

    ASSERT(lpszCustomColors);
    if (lpszCustomColors == NULL)
        return;

    TCHAR szCustomColors[256];
    memset(szCustomColors, 0, sizeof(szCustomColors));
    _tcsncpy(szCustomColors, lpszCustomColors, _countof(szCustomColors)-2);

    TCHAR *cp = NULL;
    cp = _tcstok(szCustomColors, _T(" "));

    for (i = 0; i < 16; i++)
    {
        if (!cp) break;
        m_CustomColors[i] = _tcstoul(cp, NULL, 16);
        cp = _tcstok(NULL, _T(" "));
    }
}

CString CXTColorPicker::GetCustomColors()
{
    CString strCustomColors = _T("");
    CString str = _T("");
    for (int i = 0; i < 16; i++)
    {
        str.Format(_T("%06X"), m_CustomColors[i]);
        if (!strCustomColors.IsEmpty())
            strCustomColors += _T(" ");
        strCustomColors += str;
    }
    return strCustomColors;
}

BOOL CXTColorPicker::OnEraseBkgnd(CDC* pDC)
{
    CRect rectClient;        // Rectangle for entire client area
    GetClientRect(&rectClient);

    pDC->FillSolidRect(&rectClient, m_crColor);

    return TRUE;
}

void CXTColorPicker::OnEnable(BOOL bEnable)
{
    if (bEnable)
    {
        m_crColor = m_crEnabledColor;
    }
    else
    {
        m_crEnabledColor = m_crColor;
        m_crColor = RGB(192,192,192);
    }
    RedrawWindow();

    CButton::OnEnable(bEnable);
}

const int g_ciArrowSizeX = 7;
const int g_ciArrowSizeY = 4;

void CXTColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    ASSERT(lpDrawItemStruct);

    CDC*  pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
    UINT  state = lpDrawItemStruct->itemState;
    CRect rDraw = lpDrawItemStruct->rcItem;
    CRect rArrow;

    if (m_bPopupActive) state |= ODS_SELECTED|ODS_FOCUS;

    // Draw Outer Edge
    UINT uFrameState = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;

    if (state & ODS_SELECTED) uFrameState |= DFCS_PUSHED;

    if (state & ODS_DISABLED) uFrameState |= DFCS_INACTIVE;

    pDC->DrawFrameControl(&rDraw, DFC_BUTTON, uFrameState);

    if (state & ODS_SELECTED) rDraw.OffsetRect(1,1);

    // Draw Focus
    if (state & ODS_FOCUS)
    {
        RECT rFocus = {rDraw.left, rDraw.top, rDraw.right - 1, rDraw.bottom};
        pDC->DrawFocusRect(&rFocus);
    }

    rDraw.DeflateRect(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Draw Arrow
    rArrow.left		= rDraw.right - g_ciArrowSizeX - 2*::GetSystemMetrics(SM_CXEDGE);///2;
    rArrow.right	= rArrow.left + g_ciArrowSizeX;
    rArrow.top		= (rDraw.bottom + rDraw.top)/2 - g_ciArrowSizeY / 2;
    rArrow.bottom	= (rDraw.bottom + rDraw.top)/2 + g_ciArrowSizeY / 2;


    CSize SizeArrow = GetArrowSize();
    int nArrowWidth = SizeArrow.cx; //g_ciArrowSizeX;
    int nArrowLeft = rArrow.left + 1;
    int nArrowTop = rArrow.top;
    while (nArrowWidth > 0)
    {
        CPoint point(nArrowLeft, nArrowTop);
        nArrowLeft += 1;
        nArrowTop += 1;
        pDC->MoveTo(point);
        point.x += nArrowWidth;
        nArrowWidth -= 2;
        pDC->LineTo(point);
    }

    rDraw.right = rArrow.left - ::GetSystemMetrics(SM_CXEDGE)/2;

    // Draw Separator
    pDC->DrawEdge(&rDraw,
        EDGE_ETCHED,
        BF_RIGHT);

    rDraw.right -= (::GetSystemMetrics(SM_CXEDGE) * 2) + 1 ;

    // Draw Color
    if ((state & ODS_DISABLED) == 0)
    {
        pDC->FillSolidRect(&rDraw, (m_crColor == CLR_DEFAULT) ? m_DefaultColor : m_crColor);

        ::FrameRect(pDC->m_hDC, &rDraw, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }
}
