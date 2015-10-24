#pragma once

#include <afxhtml.h>
#include <comdef.h>

#include "DocHostSite.h"
#include "OleDropTargetEx.h"

enum {FILE_GIF = 1, FILE_JPG, FILE_BMP, FILE_DOC};

class CHtmlViewEx : public CHtmlView
{
    DECLARE_DYNCREATE(CHtmlViewEx)
    DECLARE_DISPATCH_MAP()
    DECLARE_MESSAGE_MAP()

public:
    CHtmlViewEx(BOOL isview = TRUE);
    virtual ~CHtmlViewEx();

public:
    enum CONTEXT_MENU_MODE		// �����Ĳ˵�
    {
        NoContextMenu,			// �޲˵�
        DefaultMenu,			// Ĭ�ϲ˵�
        TextSelectionOnly,		// ���ı�ѡ��˵�
        CustomMenu				// �Զ���˵�
    };

protected:
    CONTEXT_MENU_MODE m_ContextMenuMode;// �����Ĳ˵�ģʽ
    CString m_DefaultMsgBoxTitle;		// Ĭ��MessageBox����
    DWORD	m_HostInfoFlag;				// �������һЩ�������Ƿ��й�������
    BOOL	m_IsView;					// �Ƿ�����ͼ

    CComPtr<IHTMLDocument2>	m_spDoc;
    COleDropTargetEx m_DropTarget;

    HWND m_hParentWnd;					// ��������				

public:
    // ���������Ĳ˵�ģʽ
    void SetContextMenuMode(CONTEXT_MENU_MODE MenuMode);
    // ȡ�����Ĳ˵�ģʽ
    CONTEXT_MENU_MODE GetContextMenuMode();
    // ����Ĭ��MessageBox����
    void SetDefaultMsgBoxTitle(CString strTitle);
    // �����������һЩ�������Ƿ��й�������
    void SetHostInfoFlag(DWORD dwFlag);

public:
    // ����һ�������ϴ���ʱ�����������һ���ؼ�������ָ��һ���ؼ�ID��ָ���Ŀؼ���Сλ��
    BOOL CreateFromStatic(UINT nID, CWnd* pParent);
    BOOL Create(CRect rect, CWnd* pParent, UINT nID);

public:
    virtual BOOL CreateControlSite(COleControlContainer* pContainer,COleControlSite** ppSite, UINT nID, REFCLSID clsid);
    // MesseBox
    virtual HRESULT OnShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT * plResult);
    // ����������Ϣ
    virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO * pInfo);
    // �����Ĳ˵�
    virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);
    // ��ʾ���������Ĳ˵�
    virtual HRESULT OnShowCustomContextMenu(LPPOINT ppt, LPUNKNOWN pcmdtReserved,LPDISPATCH pdispReserved);
    // ���ýű���չ
    virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
    // ��ȡ�Զ����ϷŶ���
    virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget,LPDROPTARGET* ppDropTarget);

protected:
    const CString GetSystemErrorMessage(DWORD dwError);
    CString GetNextToken(CString& strSrc, const CString strDelim,BOOL bTrim = FALSE, BOOL bFindOneOf = TRUE);
    BOOL SetScriptDocument();
    const CString GetLastError() const;
    BOOL GetJScript(CComPtr<IDispatch>& spDisp);
    BOOL GetJScripts(CComPtr<IHTMLElementCollection>& spColl);
    CString ScanJScript(CString& strAText, CStringArray& args);

public:
    BOOL JScriptOK();

public:
    BOOL CallJScript(const CString strFunc,_variant_t* pVarResult = NULL);
    BOOL CallJScript(const CString strFunc,const CString strArg1,_variant_t* pVarResult = NULL);
    BOOL CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,_variant_t* pVarResult = NULL);
    BOOL CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,const CString strArg3,_variant_t* pVarResult = NULL);
    BOOL CallJScript(const CString strFunc,const CStringArray& paramArray,_variant_t* pVarResult = NULL);

protected:
    virtual void PostNcDestroy();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnDocumentComplete(LPCTSTR lpszURL);

public:
    // �޸���ҳԪ�ص�����
    BOOL PutElementHtml(CString ElemID,CString Html);
    // ȡ��Ԫ�ص�ֵ
    BOOL GetElementValue(CString ElemID,CString& Value);
    // ����Ԫ�ص�ֵ
    BOOL PutElementValue(CString ElemID,CString Value);
    // ��Ԫ�����ý���
    void ElementSetFocus(CString EleName);
    IHTMLStyle* GetElementStyle(CString ElemID);
    HRESULT GetElement(LPCTSTR szElementId, IDispatch **ppdisp, BOOL *pbCollection = NULL);
    HRESULT GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement);
    HRESULT GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj);

public:
    afx_msg void OnDestroy();
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


