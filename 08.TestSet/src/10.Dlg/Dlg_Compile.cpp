// Dlg_Compile.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestSet.h"
#include "Dlg_Compile.h"
#include ".\dlg_compile.h"


extern UINT WM_CLK_TREE;

// CDlg_Compile �Ի���

IMPLEMENT_DYNCREATE(CDlg_Compile, CDialog)

CDlg_Compile::CDlg_Compile(CWnd* pParent /*=NULL*/)
: CDialog(CDlg_Compile::IDD, pParent),
m_bIsLoad(FALSE),
m_nTreePage(-1)
{
}

CDlg_Compile::~CDlg_Compile()
{
}

BEGIN_MESSAGE_MAP(CDlg_Compile, CDialog)
    ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_COMPLIE_COMBO0, IDC_COMPLIE_COMBO17, OnCbnSetfocusCompileCombo)
    ON_REGISTERED_MESSAGE(WM_CLK_TREE,      OnClickTree)
    ON_BN_CLICKED(IDC_COMPLIE_BTN_SAVE,     OnBnClickedComplieBtnSave)
    ON_BN_CLICKED(IDC_COMPLIE_BTN_OPEN,     OnBnClickedComplieBtnOpen)
    ON_BN_CLICKED(IDC_COMPLIE_BTN_CREATE,   OnBnClickedComplieBtnCreate)
    ON_BN_CLICKED(IDC_COMPLIE_BTN_MAKE,     OnBnClickedComplieBtnMake)
END_MESSAGE_MAP()

void CDlg_Compile::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX,IDC_COMPLIE_TREE,       m_treRoom);
    DDX_Control(pDX,IDC_COMPLIE_COMBO_CFG,  m_cbbConfig);
    DDX_Control(pDX,IDC_COMPLIE_INFO,       m_staInfo);

    DDX_Control(pDX,IDC_COMPLIE_NAME0,      m_staName[0]);
    DDX_Control(pDX,IDC_COMPLIE_NAME1,      m_staName[1]);
    DDX_Control(pDX,IDC_COMPLIE_NAME2,      m_staName[2]);
    DDX_Control(pDX,IDC_COMPLIE_NAME3,      m_staName[3]);
    DDX_Control(pDX,IDC_COMPLIE_NAME4,      m_staName[4]);
    DDX_Control(pDX,IDC_COMPLIE_NAME5,      m_staName[5]);
    DDX_Control(pDX,IDC_COMPLIE_NAME6,      m_staName[6]);
    DDX_Control(pDX,IDC_COMPLIE_NAME7,      m_staName[7]);
    DDX_Control(pDX,IDC_COMPLIE_NAME8,      m_staName[8]);
    DDX_Control(pDX,IDC_COMPLIE_NAME9,      m_staName[9]);
    DDX_Control(pDX,IDC_COMPLIE_NAME10,     m_staName[10]);
    DDX_Control(pDX,IDC_COMPLIE_NAME11,     m_staName[11]);
    DDX_Control(pDX,IDC_COMPLIE_NAME12,     m_staName[12]);
    DDX_Control(pDX,IDC_COMPLIE_NAME13,     m_staName[13]);
    DDX_Control(pDX,IDC_COMPLIE_NAME14,     m_staName[14]);
    DDX_Control(pDX,IDC_COMPLIE_NAME15,     m_staName[15]);
    DDX_Control(pDX,IDC_COMPLIE_NAME16,     m_staName[16]);
    DDX_Control(pDX,IDC_COMPLIE_NAME17,     m_staName[17]);

    DDX_Control(pDX,IDC_COMPLIE_COMBO0,     m_cbbData[0]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO1,     m_cbbData[1]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO2,     m_cbbData[2]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO3,     m_cbbData[3]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO4,     m_cbbData[4]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO5,     m_cbbData[5]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO6,     m_cbbData[6]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO7,     m_cbbData[7]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO8,     m_cbbData[8]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO9,     m_cbbData[9]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO10,    m_cbbData[10]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO11,    m_cbbData[11]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO12,    m_cbbData[12]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO13,    m_cbbData[13]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO14,    m_cbbData[14]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO15,    m_cbbData[15]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO16,    m_cbbData[16]);
    DDX_Control(pDX,IDC_COMPLIE_COMBO17,    m_cbbData[17]);

}

BOOL CDlg_Compile::OnInitDialog()
{
    CDialog::OnInitDialog();

    //m_treRoom.SetImageList(g_ProFile.GetImageList(), TVSIL_NORMAL);

    int nIndex = 1;

    HTREEITEM Node[15] = {0};

    Node[0] = m_treRoom.AddNode(TVI_ROOT, _T("ͨ������"), nIndex, 0);
    Node[3] = m_treRoom.AddNode(Node[0], _T("����"), nIndex++, 0);

    Node[1] = m_treRoom.AddNode(TVI_ROOT, _T("��������"), nIndex, 0);
    Node[4] = m_treRoom.AddNode(Node[1], _T("����"), nIndex++, 0);
    //Node[4] = m_treRoom.AddNode(Node[1], _T("����"), nIndex++, 0);

    Node[2] = m_treRoom.AddNode(TVI_ROOT, _T("�ļ�����"), nIndex, 0);
    Node[5] = m_treRoom.AddNode(Node[2], _T("����"), nIndex, 0);    

    Node[6] = m_treRoom.AddNode(Node[1], _T("C/C++"), nIndex, 0);
    m_treRoom.AddNode(Node[6], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("�Ż�"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("Ԥ������"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("��������"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("Ԥ����ͷ"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("����ļ�"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("�����Ϣ"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("�߼�"), nIndex++, 0);
    m_treRoom.AddNode(Node[6], _T("������"), nIndex++, 0);

    Node[7] = m_treRoom.AddNode(Node[1], _T("������"), nIndex, 0);
    m_treRoom.AddNode(Node[7], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("�嵥�ļ�"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("ϵͳ"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("�Ż�"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("Ƕ���IDL"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("�߼�"), nIndex++, 0);
    m_treRoom.AddNode(Node[7], _T("������"), nIndex++, 0);

    Node[8] = m_treRoom.AddNode(Node[1], _T("�嵥����"), nIndex, 0);
    m_treRoom.AddNode(Node[8], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[8], _T("��������"), nIndex++, 0);
    m_treRoom.AddNode(Node[8], _T("����COM"), nIndex++, 0);
    m_treRoom.AddNode(Node[8], _T("�߼�"), nIndex++, 0);
    m_treRoom.AddNode(Node[8], _T("������"), nIndex++, 0);

    Node[9] = m_treRoom.AddNode(Node[1], _T("��Դ"),nIndex, 0);
    m_treRoom.AddNode(Node[9], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[9], _T("������"), nIndex++, 0);

    Node[10] = m_treRoom.AddNode(Node[1], _T("XML�ĵ�����"), nIndex, 0);
    m_treRoom.AddNode(Node[10], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[10], _T("������"), nIndex++, 0);

    Node[11] = m_treRoom.AddNode(Node[1], _T("�����Ϣ"), nIndex, 0);
    m_treRoom.AddNode(Node[11], _T("����"), nIndex++, 0);
    m_treRoom.AddNode(Node[11], _T("������"), nIndex++, 0);

    Node[12] = m_treRoom.AddNode(Node[1], _T("�����¼�"), nIndex, 0);
    m_treRoom.AddNode(Node[12], _T("Ԥ�����¼�"), nIndex++, 0);
    m_treRoom.AddNode(Node[12], _T("Ԥ�����¼�"), nIndex++, 0);
    m_treRoom.AddNode(Node[12], _T("���ɺ��¼�"), nIndex++, 0);

    Node[13] = m_treRoom.AddNode(Node[1], _T("�Զ������ɲ���"), nIndex, 0);
    m_treRoom.AddNode(Node[13], _T("����"), nIndex++, 0);

    Node[14] = m_treRoom.AddNode(Node[1], _T("WEB����"), nIndex, 0);
    m_treRoom.AddNode(Node[14], _T("����"), nIndex++, 0); 

    m_treRoom.Expand(Node[0], TVE_EXPAND);
    m_treRoom.Expand(Node[1], TVE_EXPAND);
    m_treRoom.Expand(Node[2], TVE_EXPAND);
    m_treRoom.SelectItem(Node[3]);

    //------------------------------------------------------
    this->SelNewPage(1);
    //------------------------------------------------------

    return TRUE;
}

// ���TreeCtrl
LRESULT CDlg_Compile::OnClickTree(WPARAM wParam, LPARAM lParam)
{
    this->SelNewPage((int)lParam);
    return 0;
}

// ���ComboBox
void CDlg_Compile::OnCbnSetfocusCompileCombo(UINT nIndex)
{
    //m_staInfo.SetWindowText(m_xmlComplie.GetExplain(m_nTreePage, nIndex-IDC_COMPLIE_COMBO0));
}

// "Ӧ��"��ť
void CDlg_Compile::OnBnClickedComplieBtnSave()
{
    CString strSelect;
    CString strValues;
    CStringArray arValue;
    CStringArray arSelect;

    int nCfg = m_cbbConfig.GetCurSel();

    if (-1 != nCfg)
    {
        int nUse = nCfg + 1;
        int nCount = 0;//m_xmlComplie.GetAttribCount(m_nTreePage);

        for (int i = 0; i < nCount; i++)
        {
            int nItemCount = m_cbbData[i].GetCount();
            int nSel = m_cbbData[i].GetCurSel();

//             strSelect = m_xmlComplie.GetSelect(m_nTreePage, i);     // �����ѡ��
//             strValues = m_xmlComplie.GetXmlValue(m_nTreePage, i);   // ��������
//             m_xmlComplie.CStringToArray(strValues, arValue);
//             m_xmlComplie.CStringToArray(strSelect, arSelect);

            if (1 == nItemCount)    // ֻ��һ������Edit���͵�
            {
                m_cbbData[i].GetWindowText(arValue[nUse]);
            }
            else if (nSel == nItemCount-1)   // ѡ��<����ĿĬ�����ü̳�>,��ʾΪĬ��ֵ
            {
                arValue[nUse] = arValue[0];

                if (_T("true") == arValue[0])
                {
                    for (int j = 0; j < arSelect.GetCount(); j++)
                    {
                        if ((_T("��") == arSelect[j].Left(2)) || (_T("��") != arSelect[j].Left(2)))
                        {
                            nSel = j;
                            break;
                        }
                    }
                }
                else if (_T("false") == arValue[0])
                {
                    for (int j = 0; j < arSelect.GetCount(); j++)
                    {
                        if ((_T("��") == arSelect[j].Left(2)) || (_T("��") != arSelect[j].Left(2)))
                        {
                            nSel = j;
                            break;
                        }
                    }
                }
                else
                {
                    nSel = atoi(arValue[0]);
                }

                m_cbbData[i].SetCurSel(nSel);
            }
            else if (3 == nItemCount)   // ��|��|<����ĿĬ�����ü̳�> ���ͣ���ûѡ��
            {
                arValue[nUse] = (0 == nSel) ? _T("true") : _T("true");
            }
            else
            {
                arValue[nUse].Format(_T("%d"), nSel);
            }

            strValues = _T("");
            for (int j = 0; j < arValue.GetCount(); j++)
            {
                strValues += _T("|") + arValue[j];
            }

 //           m_xmlComplie.SetXmlValue(m_nTreePage, i, strValues);    // д��������
        }
    }
}

// "��vcproj"��ť
void CDlg_Compile::OnBnClickedComplieBtnOpen()
{
    CStringArray saConfig;
    CStringArray saPlatform;

    CString strCaption;
    CString strTemp = this->GetProjectFileName();

    if (_T("") != strTemp)
    {
        this->CleanConfig();
// 
//         if (m_xmlComplie.OpenVcProj(strTemp))
//         {
//             m_xmlComplie.GetConfigs(saConfig, TRUE);
// 
//             for (int i = 0; i < saConfig.GetCount(); i++)
//             {
//                 m_cbbConfig.InsertString(i, saConfig[i]);
//             }
// 
//             m_cbbConfig.SetCurSel(0);
// 
//             this->SelNewPage(1);
//         }
    }
}

// "����vcproj"��ť
void CDlg_Compile::OnBnClickedComplieBtnCreate()
{
    CString strPathFile = this->GetProjectFileName();

    if (_T("") != strPathFile)
    {
        if (-1 == strPathFile.Find(_T(".vcproj")))
        {
            strPathFile += _T(".vcproj");
        }

        //m_xmlComplie.CreateVcProj(strPathFile);
    }
}

// "����make"��ť
void CDlg_Compile::OnBnClickedComplieBtnMake()
{
}

// ��һ��ѡ���ļ����壬Ȼ��ѡ��һ���ļ�
CString CDlg_Compile::GetProjectFileName(void)
{
    CFileDialog FileDlg(TRUE, NULL, NULL, NULL, _T("VC��Ŀ(*.vcproj)|*.vcproj|�����ļ�(*.*)|*.*||"));

    if (FileDlg.DoModal())
    {
        return FileDlg.GetPathName();
    }
    else
    {
        return CString(_T(""));
    }
}

// ѡ��һ���µ�ҳ
void CDlg_Compile::SelNewPage(int nNewPage)
{
    if (m_nTreePage != nNewPage)
    {
        m_nTreePage = nNewPage;

        int nUse = m_cbbConfig.GetCurSel() + 1;
        int nCount = 0;//m_xmlComplie.GetAttribCount(m_nTreePage);

        for (int i = 0; i < nCount; i++)
        {
//             m_staName[i].SetWindowText(m_xmlComplie.GetName(m_nTreePage, i));
// 
//             this->SetComboData(i, m_xmlComplie.GetSelect(m_nTreePage, i), m_xmlComplie.GetXmlValue(m_nTreePage, i), nUse);

            m_staName[i].ShowWindow(SW_SHOW);
            m_cbbData[i].ShowWindow(SW_SHOW);
        }

        for (int i = 17; i >= nCount; i--)
        {
            m_staName[i].ShowWindow(SW_HIDE);
            m_cbbData[i].ShowWindow(SW_HIDE);
        }

//        m_staInfo.SetWindowText(m_xmlComplie.GetExplain(m_nTreePage, 0));
    }
}

// ����ComboBox�ؼ�������
void CDlg_Compile::SetComboData(int nIndex, CString strSelect, CString strValue, int nUse)
{
    this->CleanComboData(nIndex);

    CStringArray arrValue;
    CStringArray arrSelect;

//     m_xmlComplie.CStringToArray(strValue, arrValue);
//     m_xmlComplie.CStringToArray(strSelect, arrSelect);

    int nValCount = (int)arrValue.GetCount();
    int nSelCount = (int)arrSelect.GetCount();

    if (0 == nSelCount)
    {
        if ((nValCount != 0) && (nUse >= 0) && (nUse < nValCount))
        {
            m_cbbData[nIndex].InsertString(0, arrValue[nUse]);
            m_cbbData[nIndex].SetCurSel(0);
        }
    }
    else
    {
        int i = 0;
        for (; i < nSelCount; i++)
        {
            m_cbbData[nIndex].InsertString(i, arrSelect[i]);
        }

        m_cbbData[nIndex].InsertString(i, _T("<����ĿĬ�����ü̳�>"));

        CString strTemp = arrValue[nUse];
        int nSel = 0;

        if (_T("true") == strTemp)
        {
            for (int i = 0; i < arrSelect.GetCount(); i++)
            {
                if ((_T("��") == arrSelect[i].Left(2)) || (_T("��") != arrSelect[i].Left(2)))
                {
                    nSel = i;
                    break;
                }
            }
        }
        else if (_T("false") == strTemp)
        {
            for (int i = 0; i < arrSelect.GetCount(); i++)
            {
                if ((_T("��") == arrSelect[i].Left(2)) || (_T("��") != arrSelect[i].Left(2)))
                {
                    nSel = i;
                    break;
                }
            }
        }
        else
        {
            nSel = atoi(strTemp);
        }

        m_cbbData[nIndex].SetCurSel(nSel);
    }
}

// ���ComboBox�ؼ�������
void CDlg_Compile::CleanComboData(int nIndex)
{
    int nCount = m_cbbData[nIndex].GetCount();
    for (int i = 0; i < nCount; i++)
    {
        m_cbbData[nIndex].DeleteString(0);
    }
}

// �������ComboBox
void CDlg_Compile::CleanConfig(void)
{
    int nCount = m_cbbConfig.GetCount();
    for (int i = 0; i < nCount; i++)
    {
        m_cbbConfig.DeleteString(0);
    }
}
