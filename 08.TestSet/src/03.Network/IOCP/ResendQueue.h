
/**
��Ϣ�Ѿ����ͳ�ȥ���ȴ�����Ϣ��ȷ�ϣ�ÿ����Ϣ���б�����ȷ�ϣ���ҪӦ����UDP�����У�
���û�еõ�����Ϣ��ȷ������ CHECK_TIMEOUT ������·��ʹ���Ϣ
����ط��� SEND_TIMES ����û�еõ�����Ϣ��ȷ������ɾ��
*/

#pragma once

#define RESEND_TIME     2000    // ���볬ʱ
#define MAX_SEND_TIMES  5       // ���ʹ���

typedef struct tagClientContext *PCLIENT;
class CBufferPool;
class CSrv;

typedef struct tagAckPack
{
    DWORD   dwUserId;   // �û�ID
    DWORD   dwPackId;   // ��ID 
    DWORD   dwDataLen;  // ���ݳ�
    DWORD   dwNextTime; // �´η���ʱ��
    WORD    wSendTimes; // �Ѿ����͵Ĵ���
    WORD    wProtoNum;  // Э���
    ADDR    Addr;       // ��ַ
    PCLIENT pClient;    // ���Ͷ���
    PPACK   pData;      // ���ݰ�

}ACKPACK,*PACKPACK;


//��ϢIDΪ����
typedef CMap<DWORD, DWORD&, ACKPACK, ACKPACK> CWAITACKMAP;

class CResendQueue
{
public: 
    CResendQueue();
    ~CResendQueue();

    CRITICAL_SECTION    m_csUsedFlag;
    CWAITACKMAP         m_mapAckPack;
    CBuffPool           *m_pBuffPool;
    CSrv                *m_pSrv;

    int  Init(CSrv *pSrv, CBuffPool *pBuffPool);
    void UnInit(void);

    BOOL Add(DWORD dwUserId, DWORD dwPackId, PPACK pPack, PCLIENT pClient, ADDR &Addr);
    BOOL Add(DWORD dwUserId, DWORD dwPackId, PPACK pPack, PCLIENT pClient, UINT64 *pnIP, WORD wPort);
    BOOL Del(DWORD dwPackId);
    BOOL Del(DWORD dwUserId, WORD wProtoNum);
    static unsigned __stdcall CheckPackThread(LPVOID pParam);   // �������߳�

protected:
    void CheckPack();
    HANDLE  m_hExit;    // ɱ���߳̾��
    HANDLE  m_hThread;  // �ȴ���Ϣ�����߳�
    BOOL    m_bInit;    // �Ƿ��ʹ��
};
