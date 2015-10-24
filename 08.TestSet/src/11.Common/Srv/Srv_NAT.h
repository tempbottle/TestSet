
#pragma once

#include <list>
#include "Network\Srv.h"
#include "Network\PackId.h"
#include "Network\ResendQueue.h"
#include "Common\AVList.h"
#include "Common\RecvAVMap.h"

#define SVR_IP          _T("211.100.28.61")
#define SVR_PORT        6000
#define MY_MTU          1400

#include <map>


// Э��ţ����ֽڳ���
// PT_������_������
// U-�û��� S-������
// ��¼��¼�������˿�������������,�Ϳ�����Ҫͨ�ŷ�����ͨ��
// �˹�����˫���ͬʱ����

#define PT_U_S_LOGON_SERVER_REQUEST 0x1000  // �û���¼������������˽��IP,Port,UserID,RoomNum

#define PT_S_U_IN_ROOM_USER         0x1010  // ���������е��ˣ������û���Ϣ
#define PT_S_U_INTO_ROOM_USER       0x1011  // �½��뷿����ˣ������û���Ϣ

#define PT_U_S_IN_ROOM_ACK          0x1020  // Ӧ�� PT_S_U_IN_ROOM_USER ��Ϣ
#define PT_U_S_INTO_ROOM_ACK        0x1021  // Ӧ�� PT_S_U_INTO_ROOM_USER ��Ϣ

#define PT_S_U_OUT_ROOM_USER        0x1032  // �û��˳��������û�ID
#define PT_U_S_OUT_ROOM_USER        0x1033  // �û��˳��������û�ID

#define PT_U_U_HELLO_REQUEST        0x1040  // ��������
#define PT_U_U_HELLO_ANSWER         0x1041  // ����Ӧ��

#define PT_S_U_HEART_REQUEST        0x1050  // �����������û�ID
#define PT_U_S_HEART_ANSWER         0x1051  // �����������û�ID

#define PT_ACK_MSG                  0x1060  // Ӧ����Ϣ

#define PT_U_U_TEXT                 0x1070  // �ı���Ϣ
#define PT_U_S_VIDEO                0x1071  // ��Ƶ��Ϣ
#define PT_S_U_VIDEO                0x1072  // ��Ƶ��Ϣ
#define PT_U_U_VIDEO                0x1073  // ��Ƶ��Ϣ
#define PT_U_S_AUDIO                0x1074  // ��Ƶ��Ϣ
#define PT_S_U_AUDIO                0x1075  // ��Ƶ��Ϣ
#define PT_U_U_AUDIO                0x1076  // ��Ƶ��Ϣ

#define PT_U_S_RESEND               0x1080  // ͨ��������ת��
#define PT_S_U_RESEND               0x1081  // ͨ��������ת��
#define PT_U_U_RESEND               0x1082  // ͨ��������ת��

enum
{
    UNCOMMUNICATION,
    COMMUNICING,
    COMMUNICED,
    IN_LAN
};

enum
{
    TYPE_AUDIO,
    TYPE_VIDEO,
    TYPE_COMMAND
};

typedef struct _tagUserInfo
{
    ULONG       lPublicIp;      // ����IP
    WORD        wPublicPort;        // �����˿�
    ULONG       lPrivateIp;     // ˽��IP
    WORD        wPrivatePort;       // ˽���˿�
    DWORD       dwAckTime;      // ����Ӧ��ʱ��
    DWORD       dwUserId;       // �û�ID
    WORD        wRoomNum;       // �����   
    BYTE        byCommState;        // ͨ��״̬ 0-ĩֱͨ 1-������ 2-��ֱͨ

}USER_INFO,*PUSER_INFO;


typedef std::map<DWORD, USER_INFO> USER_MAP;    // �û�IDΪ����
typedef std::map<WORD, USER_MAP>   ROOM_MAP;    // �����Ϊ����
typedef std::pair<USER_MAP::iterator, bool> USER_MAP_INSERT;
typedef std::pair<ROOM_MAP::iterator, bool> ROOM_MAP_INSERT;

// ��¼��Ϣ�����ڵ�½������
typedef struct _tagLogonInfo
{
    DWORD       dwUserId;       // �û�ID
    WORD        wRoomNum;       // �����
    ULONG       lPrivateIp;     // ˽��IP
    WORD        wPrivatePort;   // ˽���˿�

}LOGON_INFO,*PLOGON_INFO;

// �û�ID�����
typedef struct _tagUserId_RoomNum
{
    DWORD       dwUserId;       // �û�ID
    WORD        wRoomNum;       // �����

}USERID_ROOMNUM,*PUSERID_ROOMNUM;

// ��ƵӦ�ð�
typedef struct _tagSubPackInfo
{
    DWORD       dwVideoId;      // ��ƵID
    WORD        wRoomNum;       // �����
    UCHAR       ucSubPackNum;   // �Ӱ�����
    UCHAR       ucSubPackId;    // �Ӱ�ID
    UCHAR       ucData[1];      // ����

}SUBPACKINFO,*PSUBPACKINFO;

// �ش�Ӧ�ð�
typedef struct _tagResendPack
{
    WORD        wRoomNum;       // �����
    DWORD       dwRecvUserId;   // �������û�ID
    DWORD       dwPackId;       // �ش�����ϢID
    WORD        wPackType;      // �ش�����

}RESENDPACK,*PRESENDPACK;



class CDlg_NAT;

class CSrv_NAT : public CSrv
{
public:
    CSrv_NAT(void);
    ~CSrv_NAT(void);

private:
    CRITICAL_SECTION m_csCheckNum;      // �ٽ���
    CResendQueue    m_ResendQueue;      // ������Ϣ���ط�����û���յ�ȷ�ϰ���
    CPackID         m_PackId;           // ���ڵõ���ID
    CDlg_NAT*       m_pDlg;             // ����
    CListCtrl*      m_pList;            // �б�

    ROOM_MAP        m_RoomMap;          // ����MAP(��������)
    USER_MAP        m_UserMap;          // �û�MAP(�ͻ�����)    
    HANDLE          m_hLockRoomMap;     // ��������MAP
    HANDLE          m_hLockUserMap;     // �����û�MAP
    HANDLE          m_hCommThread;      // ����,�ͻ��������߳�
    HANDLE          m_hSvrHeartBeatThread;  // ������������߳� 

    CLIENTCONTEXT   *m_pMyUdpCC;        // UDP�ͻ���
    ULONG           m_lMyPubIp;         // �Լ��Ĺ���IP�������ֽ�˳��
    WORD            m_wMyPubPort;       // �Լ��Ĺ����˿ڣ������ֽ�˳��
    ULONG           m_lMyPriIp;         // �Լ���˽��IP�������ֽ�˳��
    WORD            m_wMyPriPort;       // �Լ���˽���˿ڣ������ֽ�˳��
    ULONG           m_lServerIp;        // ��������IP�������ֽ�˳��
    WORD            m_wServerPort;      // �������Ķ˿ڣ������ֽ�˳��
    DWORD           m_dwMyId;           // �û�ID
    WORD            m_wMyRoomNum;       // �û������

    DWORD           m_dwAudioId;        // ��ƵID
    DWORD           m_dwVideoId;        // ��ƵID
    CAVList         m_AudioList;        // �Ѿ����͵���Ƶ����
    CAVList         m_VideoList;        // �Ѿ����͵���Ƶ����

    void Lock(HANDLE hOject);
    void UnLock(HANDLE hOject);
    bool AddList(DWORD dwUserId, DWORD dwPubIP, WORD wPubPort, DWORD dwPriIP, WORD wPriPort, LPCTSTR pszInfo);
    BOOL ModList(DWORD dwUserId, WORD wCol, LPCTSTR pszInfo);
    bool S_Broadcast(WORD wRoom, WORD wTimes, PPACK pPack);
    bool S_Transmit(WORD wRoom, DWORD dwUserId, WORD wTimes, PPACK pPack);
    bool S_NotfiyUserOutRoom(DWORD dwUserId, WORD wRoomNum, USER_MAP* pUserMap);
    bool PSU_ShowText(DWORD dwIP, WORD wPort, PPACK pPack);

    // ����������(PS)
    bool PS_UserLogonServer(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PS_UserIntoRoomAck(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PS_NotfiyUserOutRoom(PPACK pPack);
    bool PS_HeartBeatAnswer(PPACK pPack);
    bool PS_BroadcastVideo(PPACK pPack);
    bool PS_BroadcastAudio(PPACK pPack);
    bool PS_Resend(PPACK pPack);

    // �ͻ��˴���(PU)
    bool PU_UserIntoRoom(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PU_UserOutRoom(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PU_HelloRequest(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PU_HelloAnswer(DWORD dwIP, WORD wPort, PPACK pPack);
    bool PU_Resend(PPACK pPack);

public: 
    CRecvAVMap m_RecvMap;       // ���������û�����������Ƶ
    BYTE Init(LPCTSTR pszIp, const WORD wPort, BOOL bSrv, CDlg_NAT *pDlg);
    WORD GetCheckNum(LPBYTE byData, DWORD dwDataLen);
    void ProcessPack(PCLIENTCONTEXT pClient, DWORD dwIP, WORD wPort, DWORD dwDataLen, PBYTE pbyBuf);
    static unsigned __stdcall CommThread(LPVOID pParam);
    static unsigned __stdcall SvrHeartBeatThread(LPVOID pParam);

    // �ͻ���Ӧ��
    bool U_LogonServer(LPCTSTR pszIp, WORD wPort, UINT nUserId, WORD wRoomNum);
    bool U_RequestOutRoom(void);
    bool U_SendText(LPCTSTR pszIp, WORD wPort, LPCSTR pszText);
    bool U_SendVideo(DWORD dwUserId, WORD wProtoNum, BYTE *pbyVideo, WORD wDataLen);
    bool U_SendAudio(DWORD dwUserId, WORD wProtoNum, UCHAR *pAudio, int nDataLen);
    bool U_RequestResend(DWORD dwUserId, DWORD dwPackId, WORD wType);
};
