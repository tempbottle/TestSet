#pragma once

#include "resource.h"/*
#include "Srv\Srv_NAT.h"
#include "H264\H264.h"
#include "Common\VideoCapture.h"
#include "Sound\WaveIn.h"
#include "Sound\WaveOut.h"
#include "Sound\DirectSound1.h"


typedef struct _tagVideoInfo
{
    LONG    lLocX;	// ��Ƶ����x
    LONG    lLocY;	// ��Ƶ����y
    CH264	h264;	// H264ѹ��
}VIDEOINFO, *PVIDEOIFNO;
*/

// CDlg_NAT �Ի���

class CDlg_NAT : public CDialog//, public CWaveIn, public CVideoCapture
{
    DECLARE_DYNAMIC(CDlg_NAT)

public:
    CDlg_NAT(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CDlg_NAT();

    // �Ի�������
    enum { IDD = IDD_DLG_NAT };

private:
//	CSrv_NAT	        m_SrvNAT;		    // ������	
    DWORD		        m_dwCurSelUserId;	// ��ǰ�б�ѡ����û�ID
    bool		        m_bToServer;		// �������������
    CButton		        m_ccbIsServer;		// ѡ����ť"server"
/*
    CH264		        m_h264;			    // ֻ����ѹ���Լ�����Ƶ	
    CWaveOut	        m_WaveOut;		    // ��Ƶ
    CDirectSound	    m_DS;
*/
public:
    CListCtrl	        m_lstUser;		    // �б�ؼ�	
/*
    HDC		            m_hdc;			    // ������ʾ��Ƶ
    HDRAWDIB	        m_hdib;
    BITMAPINFOHEADER*	m_pbmpheader;
*/
    BOOL InitVideo(void);
    BOOL InitH264(void);
    void UninitH264(void);
    void ShowAudio(int nId, DWORD dwAudioId, LPBYTE pbyData, int nLen);
 //   virtual void ProcAudio(char *pszData, DWORD dwLen);
 //   virtual void ProcVideo(LPBYTE pbyData,int nLen);

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLvnItemchangedNatList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDestroy();

    afx_msg void OnBnClickedButStartServer();
    afx_msg void OnBnClickedButStartClient();
    afx_msg void OnBnClickedNatButSendText();	
    afx_msg void OnBnClickedNatButToClient();	
    afx_msg void OnBnClickedNatButToServer();
};
