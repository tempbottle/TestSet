
#include "stdafx.h"
#include "TestSet.h"
#include "Dlg_Main.h"
#include "00.Finger/Dlg_Finger.h"
#include "01.Media/my/Dlg_Media.h"
#include "02.P2P/Dlg_P2P.h"
#include "03.Network/Dlg_Network.h"
// #include "Dlg_UI.h"
#include "05.CPU/Dlg_CPU.h"
#include "06.EMule/Dlg/Dlg_Emule.h"
#include "07.Database/Dlg_DB.h"
// #include "Dlg_DataStruct.h"
#include "11.Common/mdump.h"

// ���ڼ���ڴ�й©,Visual Leak Detector
// #include "vld/vld.h"
// #pragma comment(lib, "../10.thirdParty/lib/vld/vld32.lib")

#ifdef _DEBUG
#pragma comment(lib, "../10.thirdParty/lib/glog/libglogd.lib")
#else
#pragma comment(lib, "../10.thirdParty/lib/glog/libglog.lib")
#endif

// �쳣ʱ����dmp�ļ�
extern CMiniDumper g_theCrashDumper;

// ������ͼ�����Ϣ
UINT WM_MY_NOTIFYICON = RegisterWindowMessage(_T("WM_MY_NOTIFYICON"));

// ���ı�exe�ļ��ṹ,���1������,�����������ж���ı������ǹ����
// ������ΪShared�����ݶ����ԡ�
// /SECTION�ͱ�ʾΪ����һ���������䣬R����Read����W����Writeд��S����Share����
#pragma data_seg("Shared")
volatile LONG g_lApplicationInstances = 0;
char szString[MAX_PATH] = "Shared string";
#pragma data_seg()

#pragma comment(linker, "/Section:Shared,RWS") 


CDlg_Main::CDlg_Main(CWnd* pParent /*=NULL*/)
    : CDialog(CDlg_Main::IDD, pParent),
    curDlg_(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon("CONTACT0");

    memset(dlg_, 0, sizeof(dlg_));
}

void CDlg_Main::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MAIN_COMBO, comboMain_);
}

BEGIN_MESSAGE_MAP(CDlg_Main, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_COMMAND(ID_MAIN_MENU_SHOW,               OnMenuShow)
    ON_COMMAND(ID_MAIN_MENU_EXIT,               OnMenuExit)
    ON_COMMAND(ID_MAIN_MENU_AUTORUN,            OnMenuAutorun)
    ON_COMMAND(ID_MAIN_MENU_CLOSE_MIN,          OnMenuCloseMin)
    ON_BN_CLICKED(IDC_MAIN_BTN_CREATE,          OnBnClickedMainBtnCreate)
    ON_CBN_SELCHANGE(IDC_MAIN_COMBO,            OnSelchangeComboMain)
    ON_REGISTERED_MESSAGE(WM_MY_NOTIFYICON,     OnNotifyIcon)
END_MESSAGE_MAP()


BOOL CDlg_Main::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��

    //-------------------------------------------------------------------------

    CoInitialize(NULL);
    AfxInitRichEdit();

    // ��ʹ��Google��־��
    google::InitGoogleLogging("");

    // ȫ�ֵĲ���
    LIST_STR list;
    list.push_back("CONTACT0");
    list.push_back("CONTACT1");
    list.push_back("CONTACT2");
    list.push_back("CONTACT3");
    list.push_back("CONTACT4");

    g_ProFile.Init(list);
    g_theCrashDumper.Enable("TestSet", true);

    //-------------------------------------------------------------------------

    // ������Сͼ��
    sysTray_.Create(NULL, WM_MY_NOTIFYICON, "���Լ�", NULL, IDR_MAIN_MENU);
    sysTray_.SetImageList(g_ProFile.getImageList());
    sysTray_.Animate(500);

    //-------------------------------------------------------------------------

    comboMain_.InsertString(DLG_FINGER,     "ָ��ʶ��");
    comboMain_.InsertString(DLG_MEDIA,      "��ý��");
    comboMain_.InsertString(DLG_P2P,        "P2P�������");
    comboMain_.InsertString(DLG_NETWORK,    "����");
    comboMain_.InsertString(DLG_UI,         "����");
    comboMain_.InsertString(DLG_CPU,        "����CPU");
    comboMain_.InsertString(DLG_EMULE,      "��¿");
    comboMain_.InsertString(DLG_DATABASE,   "���ݿ�");
    comboMain_.InsertString(DLG_ALGORITHM,  "�㷨");
    comboMain_.SetCurSel(0);

    return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CDlg_Main::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ��������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CDlg_Main::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// Сͼ�����Ϣ
LRESULT CDlg_Main::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
    switch(lParam)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        {
            this->ShowWindow(IsWindowVisible()?SW_HIDE:SW_SHOW);
        }
        break;
    }
    return 0;
}

// ע������
void CDlg_Main::OnDestroy()
{
    for (int i = 0; i < DLG_ALL; i++)
    {
        if (NULL != dlg_[i])
        {
            dlg_[i]->DestroyWindow();
            delete dlg_[i];
        }
    }

    sysTray_.DestroyWindow();

    CDialog::OnDestroy();
}

// �л���ǰ��ʾ�Ĵ���
void CDlg_Main::OnSelchangeComboMain()
{
    int nIndex = comboMain_.GetCurSel();

    if (NULL != curDlg_)
    {
        curDlg_->ShowWindow(SW_HIDE);
    }

    if (NULL == dlg_[nIndex])
    {
        return;
    }

    dlg_[nIndex]->ShowWindow(SW_SHOW);

    curDlg_ = dlg_[nIndex];
}

// ��ť��������
void CDlg_Main::OnBnClickedMainBtnCreate()
{
    CRect rcWnd;
    this->GetClientRect(&rcWnd);
    rcWnd.top += 50;

    int nIndex = comboMain_.GetCurSel();

    if (NULL != curDlg_)
    {
        curDlg_->ShowWindow(SW_HIDE);
    }

    if (NULL == dlg_[nIndex])
    {
        int nID = 0;
        curDlg_ = NewDlg(nIndex, nID);
        curDlg_->Create(nID, this);
        curDlg_->MoveWindow(rcWnd);
    }
    else
    {
        curDlg_ = dlg_[nIndex];
    }

    curDlg_->ShowWindow(SW_SHOW);
}

// ���������ڴ�
// nIndex [IN]  ���
// nID    [OUT] ������ԴID
CDialog* CDlg_Main::NewDlg(int nIndex, int &nID)
{
    if ((nIndex < 0) || (nIndex >= DLG_ALL)) return NULL;

    switch(nIndex)
    {
    case DLG_FINGER:
        {
            dlg_[nIndex] = new CDlg_Finger;
            nID = CDlg_Finger::IDD;
            break;
        }
    case DLG_MEDIA:
        {
            dlg_[nIndex] = new CDlg_Media;
            nID = CDlg_Media::IDD;
            break;
        }
    case DLG_P2P:
        {
            dlg_[nIndex] = new CDlg_P2P;
            nID = CDlg_P2P::IDD;
            break;
        }
    case DLG_NETWORK:
        {
            dlg_[nIndex] = new CDlg_Network;
            nID = CDlg_Network::IDD;
            break;
        }
//     case DLG_UI:
//         {
//             dlg_[nIndex] = new CDlg_UI;
//             nID = CDlg_UI::IDD;
//             break;
//         }
    case DLG_CPU:
        {
            dlg_[nIndex] = new CDlg_CPU;
            nID = CDlg_CPU::IDD;
            break;
        }
//     case DLG_EMULE:
//         {
//             dlg_[nIndex] = new CDlg_Emule;
//             nID = CDlg_Emule::IDD;
//             break;
//         }
    case DLG_DATABASE:
        {
            dlg_[nIndex] = new CDlg_DB;
            nID = CDlg_DB::IDD;
            break;
        }
//     case DLG_ALGORITHM:
//         {
//             dlg_[nIndex] = new CDlg_Algorithm;
//             nID = CDlg_Algorithm::IDD;
//             break;
//         }
    }

    return dlg_[nIndex];
}

// �˵�����ʾ�����ش���
void CDlg_Main::OnMenuShow()
{
    this->ShowWindow(IsWindowVisible()?SW_HIDE:SW_SHOW);
}

// �˵����˳�
void CDlg_Main::OnMenuExit()
{
    OnOK();
}

// �˵��������Զ�����
void CDlg_Main::OnMenuAutorun()
{
}

// �˵�����С����������
void CDlg_Main::OnMenuCloseMin()
{
}

