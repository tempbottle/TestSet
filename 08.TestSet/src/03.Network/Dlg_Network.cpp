// Dlg_HTTP.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "Dlg_Network.h"
#include <afxinet.h> // CHttpConnection CFtpConnection



IMPLEMENT_DYNAMIC(CDlg_Network, CDialog)
    CDlg_Network::CDlg_Network(CWnd* pParent /*=NULL*/)
    : CDialog(CDlg_Network::IDD, pParent)
{
}

CDlg_Network::~CDlg_Network()
{
}

void CDlg_Network::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlg_Network, CDialog)
    ON_BN_CLICKED(IDC_NETWORK_BTN_HTTP_SERVER,  &OnBnClickedButHttpServer)
    ON_BN_CLICKED(IDC_NETWORK_BTN_DOWNLOAD,     &OnBnClickedButDown)
    ON_BN_CLICKED(IDC_NETWORK_BTN_POST,         &OnBnClickedBtnPost)
    ON_BN_CLICKED(IDC_NETWORK_BTN_FTP,          &OnBnClickedBtnFtp)
    ON_BN_CLICKED(IDC_NETWORK_BTN_MONGOOSE,     &OnBnClickedBtnMongoose)
    ON_BN_CLICKED(IDC_NETWORK_BTN_WEBSERVICE,   &OnBnClickedBtnWebservice)
    ON_BN_CLICKED(IDC_NETWORK_BTN_IOCP_SERVER,  &OnBnClickedBtnIocpServer)
    ON_BN_CLICKED(IDC_NETWORK_BTN_IOCP_CLIENT,  &OnBnClickedBtnIocpClient)
    ON_BN_CLICKED(IDC_NETWORK_BTN_RTSP_SERVER,  &OnBnClickedBtnRtspServer)
    ON_BN_CLICKED(IDC_NETWORK_BTN_RTSP_CLIENT,  &OnBnClickedBtnRtspClient)
END_MESSAGE_MAP()

BOOL CDlg_Network::OnInitDialog()
{
    CDialog::OnInitDialog();

    std::string path = g_ProFile.getPath() + std::string("\\res");
    GetDlgItem(IDC_NETWORK_IPADDRESS)->SetWindowText(g_ProFile.getIp(0));
    GetDlgItem(IDC_NETWORK_EDIT_PORT)->SetWindowText("8080");
    GetDlgItem(IDC_NETWORK_EDIT_PATH)->SetWindowText(path.c_str());// HTTPЭ�������·�����ͻ�������ļ���

    return TRUE;
}

void CDlg_Network::OnBnClickedButHttpServer()
{
//     CString strIP;
//     CString strPort;
//     CString strPath;
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
//     GetDlgItem(IDC_NETWORK_EDIT_PATH)->GetWindowText(strPath);
// 
//     int ret = m_SrvHTTP.Start(strIP, _ttoi(strPort), strPath);
// 
//     if (0 == ret)
//     {
//         TRACE("HTTP Server Start %s:%s %s\n", strIP, strPort, strPath);
//         GetDlgItem(IDC_NETWORK_BTN_HTTP_SERVER)->EnableWindow(FALSE);
//     }
//     else
//     {
//         CString msg;
//         msg.Format("HTTP Server Start Error\nErrorCode:%d", ret);
//         MessageBox(msg);
//     }
}

typedef std::list<std::string> LIST_STR;

void CDlg_Network::OnBnClickedButDown()
{
//     char temp[1024] = {""};
//     int updateNum = 0;
//     WORD port = 0;
//     DWORD type = 0;
//     CString strServer;
//     CString strPath;
//     CString strFile;
//     LIST_STR fileList;
// 
//     std::string strfile = g_ProFile.getPath() + std::string("\\res\\config\\Config.xml");
// 
//     m_XmlCfg.OpenFile(strfile.c_str());
// 
//     updateNum = m_XmlCfg.GetUpdateNum();
// 
//     for (int i = 0; i < updateNum; i++)
//     {
//         const char *file = m_XmlCfg.GetUpdateFile(i);// Ҫ���µ��ļ�
//         fileList.push_back(file);
//     }
// 
//     const char *url = m_XmlCfg.GetUpdateSrv();
// 
//     AfxParseURL(url, type, strServer, strPath, port);	// ��һ��URL���еõ�Э�����ͣ����������˿ڣ�·��
// 
//     CInternetSession cis(NULL, 1, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);//��ʼ��m_cis,ֱ���������磬��ͨ������
//     cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5);
//     CHttpConnection *http = cis.GetHttpConnection(strServer, port);
// 
//     BeginWaitCursor();
// 
//     LIST_STR::iterator iter = fileList.begin();
//     for (; iter != fileList.end(); ++iter)
//     {
//         if (!UpdateFile(iter->c_str(), http))
//         {
//             MessageBox("���³���!");
//             EndWaitCursor();
//             return;
//         }
//     }
// 
//     MessageBox("�������!");
//     EndWaitCursor();
}

// bool CDlg_Network::UpdateFile(const char *file, CHttpConnection *http)
// {
//     DWORD httpRequestFlags = HSR_DOWNLOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_KEEP_CONNECTION;
// 
//     // ���ݵ�ַ����http����
//     CHttpFile *pFile = http->OpenRequest(CHttpConnection::HTTP_VERB_GET, file, NULL, 1, NULL, NULL, httpRequestFlags);
//     try
//     {
//         pFile->SendRequest();
//     }
//     catch (CInternetException* pEx)
//     {
//         TCHAR* szError = new TCHAR[1024];
//         pEx->GetErrorMessage(szError,1024);
//         AfxMessageBox(szError);
//         pFile->Close();
//         delete [] szError;
//         return FALSE;
//     }
// 
//     if (NULL != pFile)
//     {
//         DWORD dwResult = 0;
// 
//         pFile->QueryInfo(HTTP_QUERY_STATUS_CODE, dwResult); // �õ�״̬��
// 
//         if (404 == dwResult)
//         {
//             MessageBox("����ʧ�ܣ�û���ҵ������ļ�");
//             pFile->Close();
//             return FALSE;
//         }
// 
//         pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, dwResult); // �õ����ݳ�
// 
//         std::string path = g_ProFile.getPath() + std::string(file);
// 
//         CStdioFile csf;
//         if (!csf.Open(path.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyWrite))
//         {
//             AfxMessageBox(path.c_str(), MB_ICONSTOP);
//             pFile->Close();
//             return FALSE;
//         }
// 
//         char buf[2048];
//         int len = 0;
//         int read = 0;
// 
//         while ((len = pFile->Read(buf,sizeof(buf))) > 0)
//         {
//             read += len;
// 
//             MSG msg;
//             for (int i = 0; i < 10; i++)
//             {
//                 if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//                 {
//                     TranslateMessage(&msg);
//                     DispatchMessage(&msg);
//                 }
//             }
// 
//             csf.Write(buf, len);
//         }
// 
//         pFile->Close();
//     }
// 
//     return TRUE;
// }

//#include "Common/HttpPost.h"

void CDlg_Network::OnBnClickedBtnPost()
{
//     //m_SrvHTTP.Post();
// 
//     CString strIP;
//     CString strPort;
//     CString strPath;
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
//     GetDlgItem(IDC_NETWORK_EDIT_PATH)->GetWindowText(strPath);
// 
//     CHttpPost post(strIP, atoi(strPort), strPath);
// 
//     std::string strfile = g_ProFile.getPath() + std::string("\\res\\config\\Config.xml");
// 
//     FILE *file = fopen(strfile.c_str(), "rb");
// 
//     char data[MAX_PATH*10] = "";
//     int len = sizeof(data);
// 
//     if (NULL != file)
//     {
//         len = fread(data, len, 1, file); // len = 0
//         fclose(file);
// 
//         len = strlen(data);
//     }
// 
//     if (post.SendPostRequest(data, len, "data.xml"))
//     {
//         char *response = (char*)post.getResponse();
//         int responeLen = post.getResponseLen();
//         int status = post.getStatusCode();
//     }
}

void CDlg_Network::OnBnClickedBtnFtp()
{
    CString strIP;
    CString strPort;

    GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
    GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);

    CInternetSession cis(NULL, 1, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);//��ʼ��m_cis,ֱ���������磬��ͨ������
    cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10);
    CFtpConnection *ftp = cis.GetFtpConnection(strIP, "Anonymous", "", _tstoi(strPort));

    ftp->SetCurrentDirectory("\\eDesign");

    std::string dir;
    std::string remotePath("Previews\\111\\222\\333\\444");

    CFtpFileFind finder(ftp);

    int last = 0;
    while (true)
    {
        int index = remotePath.find('\\', last);
        if (-1 != index)
        {
            dir = remotePath.substr(last, index - last);

            last = index;

            if (!finder.FindFile(dir.c_str()))  // û�д�Ŀ¼�򴴽�
            {
                if (!ftp->CreateDirectory(dir.c_str()))
                {
                    LOG(ERROR) << "CreateDirNotOK" << dir.c_str() << GetLastError();
                }
            }

            if (!ftp->SetCurrentDirectory(dir.c_str())) // ���õ�ǰĿ¼
            {
                LOG(ERROR) << "SetDirNotOK" << dir.c_str() << GetLastError();
            }
        }
        else
        {
            break;
        }
    }

    // �����ļ��ϴ�
    if (ftp->PutFile("E:\\File\\������ѵ\\IDOͨѶ�ṹ.pptx", "IDOͨѶ�ṹ.doc"))
    {
        TRACE("put OK\n");
    }
    else
    {
        TRACE("put don't OK error:%d\n", GetLastError());
    }

    ftp->Close();
    delete ftp;
    cis.Close();
}

#include "mongoose\mongoose.h"

static void VRF_MyInterface(struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data)
{
    char szContent[] = "������Mongoose Http���������õ��û��Լ����õĻص��������ɵ��ַ���";
    int nLen = (int)strlen(szContent);

    mg_printf(conn, "HTTP/1.1 200 OK\r\nDate: Thu, 05 Aug 2010 01:33:00 China Standard Time\r\nLast-Modified: Thu, 05 Aug 2010 01:32:47 China Standard Time\r\nContent-Type: application/octet-stream\r\nContent-Length: %du\r\nConnection: close\r\nAccept-Ranges: bytes\r\n\r\n",nLen);

    mg_write(conn, szContent, nLen);
}

void CDlg_Network::OnBnClickedBtnMongoose()
{
    // Mongoose ��һ��С�͵�Http�����������п���ע���Լ��Ļص�

    struct mg_context *ctx = mg_start();

    if(!mg_set_option(ctx, "ports", "8080")) return;

    mg_set_uri_callback(ctx, "/", &VRF_MyInterface, NULL);

    // �ⲿ�ֱȽϺ�ʱ����ʱ�ɴ�ֱ����
    // mg_stop(ctx);
}

#include "WebService/WebServiceCtrl.h"

void CDlg_Network::OnBnClickedBtnWebservice()
{
    WebServiceCtrl webService;
    webService.getWeatherbyCityName("����");
}

void CDlg_Network::OnBnClickedBtnIocpServer()
{
//     CString strIP;
//     CString strPort;
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
// 
//     int ret = m_SrvIOCP.Start(strIP, _ttoi(strPort));
//     if (0 == ret)
//     {
//         TRACE("IOCP Server Start %s:%s\n", strIP, strPort);
//         GetDlgItem(IDC_NETWORK_BTN_IOCP_SERVER)->EnableWindow(FALSE);
//     }
//     else
//     {
//         CString msg;
//         msg.Format("Init IOCP Error\nErrorCode:%d", ret);
//         MessageBox(msg);
//     }
}

void CDlg_Network::OnBnClickedBtnIocpClient()
{
//     CString strIP;
//     CString strPort;
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
// 
//     m_SrvIOCP.Test(strIP, _ttoi(strPort));
}

void CDlg_Network::OnBnClickedBtnRtspServer()
{
//     CString strIP;
//     CString strPort;
//     CString strPath("D:\\film");
// 
//     GetDlgItem(IDC_NETWORK_EDIT_PATH)->SetWindowText(strPath);
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
//     GetDlgItem(IDC_NETWORK_EDIT_PATH)->GetWindowText(strPath);
// 
//     int ret = m_SrvRTSP.Start(strIP, _ttoi(strPort), strPath);
// 
//     if (0 == ret)
//     {
//         TRACE("RTSP Server Start %s:%s %s\n", strIP, strPort, strPath);
//         GetDlgItem(IDC_NETWORK_BTN_RTSP_SERVER)->EnableWindow(FALSE);
//     }
//     else
//     {
//         CString msg;
//         msg.Format("RTSP Server Start Error\nErrorCode:%d", ret);
//         MessageBox(msg);
//     }
}

void CDlg_Network::OnBnClickedBtnRtspClient()
{
//     // �����ͻ���
//     AfxEnableControlContainer();
// 
//     CString strURL("rtsp://");
//     CString strIP;
//     CString strPort;
//     CString strFileName;
// 
//     GetDlgItem(IDC_NETWORK_IPADDRESS)->GetWindowText(strIP);
//     GetDlgItem(IDC_NETWORK_EDIT_PORT)->GetWindowText(strPort);
//     GetDlgItem(IDC_NETWORK_EDIT_PATH)->GetWindowText(strFileName);
// 
//     strURL += strIP;
//     strURL += ":";
//     strURL += strPort;
//     strURL += "/";
//     strURL += strFileName;
// 
//     CRect m_rcVideo;
//     this->GetClientRect(m_rcVideo);
//     m_rcVideo.top += 60; 
// 
//     if (!m_Player.Create(NULL, "Player", WS_CHILD, m_rcVideo, this, -1))
//     {
//         MessageBox("Failed to Load MediaPlayer ActiveX Control", "Notification");
//         return;
//     }
// 
//     m_Player.SetControls("ImageWindow");
//     m_Player.SetNoLogo(TRUE);
//     m_Player.SetEnableContextMenu(TRUE);
//     m_Player.SetImageStatus(FALSE);
//     m_Player.SetWantErrors(FALSE);
//     m_Player.SetCenter(TRUE);
//     m_Player.SetMaintainAspect(TRUE);
// 
//     //m_hMsgHook = SetWindowsHookEx(WH_CALLWNDPROC, MsgHook, AfxGetInstanceHandle(), GetCurrentThreadId());
// 
//     m_Player.ShowWindow(SW_SHOW);
//     m_Player.SetSource(strURL);
//     m_Player.Play();
// 
//     GetDlgItem(IDC_NETWORK_BTN_RTSP_CLIENT)->EnableWindow(FALSE);
}
