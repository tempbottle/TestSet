
#include "stdafx.h"
#include "XTTreeCtrl.h"
#include ".\xttreectrl.h"


UINT WM_CLK_TREE = ::RegisterWindowMessage(_T("WM_CLK_TREE"));
UINT WM_DBCLK_TREE = ::RegisterWindowMessage(_T("WM_DBCLK_TREE"));


IMPLEMENT_DYNAMIC(CXTTreeCtrl, CTreeCtrl)
CXTTreeCtrl::CXTTreeCtrl(void)
    :m_hRoot(NULL)
{    
}

CXTTreeCtrl::~CXTTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CXTTreeCtrl, CTreeCtrl)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// �豸ͼ���б�
// uImageId    [IN] ͼ���б�ID
// uImageCount [IN] ͼ������
bool CXTTreeCtrl::SetImage(UINT uImageId, UINT uImageCount)
{
    // ����ͼ���б�ؼ�
    m_ImageList.Create(uImageId, 16, uImageCount, RGB(255,0,255));
    m_ImageList.SetBkColor(RGB(255,255,255));

    this->SetImageList(&m_ImageList, TVSIL_NORMAL); 
    return true;
}

// ����
void CXTTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    TVHITTESTINFO HitTestInfo = {0};
    HitTestInfo.pt = point;

    HTREEITEM hSelItem = this->HitTest(&HitTestInfo);

    if (NULL != hSelItem)
    {
        DWORD dwItemData = (DWORD)GetItemData(hSelItem);
        CWnd *ParentWnd = this->GetParent();

        ParentWnd->PostMessage(WM_CLK_TREE, (WPARAM)hSelItem, (LPARAM)dwItemData);
    }

    CTreeCtrl::OnLButtonDown(nFlags, point);
}

// ˫��
void CXTTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    HTREEITEM hSelItem = this->GetSelectedItem();
    DWORD dwItemData = (DWORD)GetItemData(hSelItem);
    CWnd *ParentWnd = this->GetParent();

    ParentWnd->PostMessage(WM_DBCLK_TREE, (WPARAM)hSelItem, (LPARAM)dwItemData);

    CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

// ��ӽڵ�
// hParent [IN] ���ڵ�
// sName   [IN] �ڵ���
// dwData  [IN] �ڵ���
// dwIcon  [IN] �ڵ�ͼ��
HTREEITEM CXTTreeCtrl::AddNode(HTREEITEM hParent, CString sName, DWORD dwData, DWORD dwIcon)
{
    HTREEITEM hItem = InsertItem(sName, dwIcon, dwIcon, hParent, TVI_LAST);
    SetItemData(hItem, dwData);

    return hItem;
}

// ɾ���ڵ�
// dwData [IN] �ڵ�����
void CXTTreeCtrl::DelNode(DWORD dwData)
{
    if (NULL == m_hRoot) m_hRoot = this->GetRootItem();

    HTREEITEM hItem = this->FindNode(m_hRoot, NULL, dwData);

    if (NULL != hItem) this->DeleteItem(hItem);
}

// ���ýڵ�����
// hItem  [IN] �ڵ���
// sParam [IN] ����
BOOL CXTTreeCtrl::SetNode(HTREEITEM hItem, LPCTSTR sParam, ...)
{
    if (NULL == sParam) return FALSE;

    va_list argptr;
    va_start(argptr, sParam);

    char szName[1000] = {""};
    
    int nRet = _vsntprintf(szName, sizeof(szName), sParam, argptr);

    if (nRet >= 0) this->SetItemText(hItem, szName);

    va_end(argptr);

    return TRUE;
}

// ֻ������
HTREEITEM CXTTreeCtrl::GetNode(DWORD dwData)
{
    if (NULL == m_hRoot) m_hRoot = this->GetRootItem();

    return FindNode(m_hRoot, NULL, dwData);
}

// ֻ������
HTREEITEM CXTTreeCtrl::GetNode(LPCTSTR sName)
{
    if (NULL == m_hRoot) m_hRoot = this->GetRootItem();

    return FindNode(m_hRoot, sName, 0);
}

// �����ڵ�
HTREEITEM CXTTreeCtrl::FindNode(HTREEITEM hItem, LPCTSTR sName, DWORD dwData)
{
    while (hItem != NULL)
    {
        if ((NULL != sName) && (this->GetItemText(hItem).Compare(sName) == 0)) return hItem;

        if (this->GetItemData(hItem) == dwData) return hItem;

        if (this->ItemHasChildren(hItem))
        {
            HTREEITEM hSubItem = this->GetChildItem(hItem);

            HTREEITEM hFand = FindNode(hSubItem, sName, dwData);
            if (NULL != hFand) return hFand;
        }

        hItem = this->GetNextItem(hItem, TVGN_NEXT);
    }

    return NULL;
}

