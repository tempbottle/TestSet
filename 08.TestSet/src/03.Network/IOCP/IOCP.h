
#pragma once
#include "ClientPool.h"
#include "OperatePool.h"
#include <set>
#include <list>
#include <winsock2.h>
#include <mswsock.h> // LPFN_ACCEPTEX
#pragma comment(lib,"ws2_32.lib")


namespace XT_IOCP
{

#define MAX_THREAD_COUNT    50      // ����߳�����
#define MAX_LISTEN_COUNT    100     // ����������
#define SOCKET_ADD_NUM      100     // һ�����SOCKET������
#define CLIENT_CLOSE_SOCKET 0xFFFF  // IOCP֪ͨ�ϲ�ͻ���׳־������SOCKET

	/**
	 *\struct tagListenInfo
	 *\brief  �����������ݽṹ
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������
	 */
	typedef struct tagListenInfo
	{
		SOCKET  listenSocket;
		HANDLE  acceptEvent;
		PCLIENT listenClient;

	}LISTEN_INFO, *PLISTEN_INFO;


	typedef std::list<HANDLE>			WORK_THREAD_LIST;
	typedef std::map<int, LISTEN_INFO>	LISTEN_INFO_MAP;
	typedef struct tagQueueNode			*PQueueNode;	

	/**
	��ɶ˿ڣ�����Initialize(),Ȼ����AddTcpListen()��AddUdpListen()

	1-  ��ҪΪÿ��С���ݰ�����һ��IOCP����,���������׺ľ�IOCP���ڲ�����.....�Ӷ�����10055����.
	2-  ��Ҫ��ͼ�ڷ��ͳ�IOCP����֮��,�յ����֪֮ͨǰ�޸�������ʹ�õ����ݻ��������,��Ϊ�����ʱ��,ϵͳ���ܻ�����ȡ��Щ����.
	3-  Ϊ�˱����ڴ濽��,���Գ��Թر�SOCKET�ķ��ͺͽ��ջ�����,����������,����Ҫ����Ľ�������POST��һ�����������Ƚϴ��SOCKET,
	�Ӷ���֤ϵͳһֱ�����ҵ�BUFFER����ȡ����������.
	4-  �ڷ���������������ʱ��,������WORKTHREAD��ֹһ��,һ��Ҫʹ��һЩ�ֶ�����֤������ɵ����ݰ��շ��ͽ��������˳����,����,
	����������ݰ��û��ҵ�˳����������Ĵ��������.....
	5-  ˵�����߳�, ���Ҫ����MS�Ľ���, �� CPU����*2+2 ��, ����㲻�˽�IOCP�Ĺ���ԭ��Ļ�. 
	6-  IOCP�Ĺ����߳���ϵͳ�Ż��͵��ȵ�, �Լ��Ͳ���Ҫ���ж���Ĺ�����.��������������ǻۺ;��鳬��MS�Ĺ���ʦ, ���㻹��ҪIOCPô....
	7-  ����һ��Send����֮�󣬾Ͳ���Ҫ��ȥ����Ƿ�����������Ϊiocp�������������飬��Щ��˵iocpû����������飬���iocp�ĸ�Ч������㣵ģ�
	�����������������β��Ա�����IocpҪô�Ͽ����ӣ�Ҫô�Ͱ����ÿ���������󶼷���������
	8-  �������ݴ��ҵ�ʱ�򣬲�Ҫ�ţ�Ҫ�Ӷ��̵߳ĽǶȼ����Ľ����ͷ������ݰ��Ĵ��룬�����ǲ�����˳���ϵ����⡣
	9-  ��������ֵ��ڴ�����ʱ���𽥵ļ��ٹ����̵߳����������԰��������������ⷢ����Ǳ��λ�á�
	10- ͬ���������ڴ�����ʱ������ȥ�����Ŀͻ����ڷ��������ڲ�ӳ�������ͷ��Ƿ������⡣����ҪС�ĵı�дiocp���ʧ�ܵĴ�����룬
	��ֹ����һ��������ڲ�ӳ�����ĵ�ַ��
	11- overlapped����һ��Ҫ�����ڳ־õ�λ�ã����Ҳ���������ɣ����ܳɹ�����ʧ�ܣ���Ҫ�ͷţ�������ܻ�����������ֵ����⡣
	12- IOCP�����й��������ڻ�ȡ���״̬���Ǹ������ڲ����е��Ⱥ���ɵģ����Գ���ע�⹤���̵߳�����֮�⣬��Ҫע�⣬
	���������㹻��Ĺ����̴߳��ڻ�ȡ���״̬���Ǹ��ȴ����棬����������Ҫ���ٹ����̵߳ĸ�����ȷ�������߳��ڲ�Ҫ�����ʱ�Ĺ�����
	���ҵĽ����ǹ����̺߳��߼��̳߳������ֿ���
	13- overlapped����ҪΪÿ�ε�send��recv������׼��һ��ȫ�µģ�����ͼ�����ظ����á�
	14- ��������send��recv�Ļ���Ĵ�С��ϵͳҳ���С�ı�������Ϊϵͳ���ͻ��߽������ݵ�ʱ�򣬻����û��ڴ�ģ�
	��ҳ��С�Ļ�����˷ѵ�����һ��ҳ�档����Ϊ��һ���Ĳ��䣬�����С���ϲ��ɴ�����ͣ�
	**/

	enum
	{
		IOCP_THREAD_PARA_ERROR  = 1000, // �߳������������ȷ
		IOCP_SEND_THREAD_EXIT,          // �����߳������˳�
		IOCP_IDLE_THREAD_EXIT,          // ����߳������˳�
		IOCP_WORK_THREAD_EXIT,          // �����߳������˳�
		IOCP_CORECE_THREAD_EXIT,        // ǿ���ر��߳�

		IOCP_PARA_ERROR         = -1999,// �����������ȷ
		IOCP_INIT_CLIENT_POOL_FAIL,     // ��ʹ���ͻ��˳�ʧ��    
		IOCP_SET_SOCK_FAIL,             // ����SOCKETʧ��
		IOCP_GET_SOCK_ADDR_FAIL,        // �õ�SOCKET��ַʧ��
		IOCP_GET_SOCK_ACCEPT_FAIL,      // �õ�SOCKET��չ����ָ��ʧ��
		IOCP_GET_CLIENT_DATA_FAIL,      // �õ��ͻ��˽ṹʧ��
		IOCP_ADD_ACCEPT_EVENT_FAIL,     // ���ACCEPT�¼�ʧ��
		IOCP_POST_READ_OPERTE_FAIL,     // ��Ӷ�ȡ����ʧ��
		IOCP_POST_ACCEPT_SOCKET_FAIL,   // ��Ӷ�ȡ����ʧ��
		IOCP_SOCKET_LISTEN_OUT,         // ����SOCKET����̫��    
		IOCP_SOCKET_BIND_FAIL,          // SCOKET��ʧ��
		IOCP_SOCKET_LISTEN_FAIL,        // SCOKET����ʧ��
		IOCP_SOCKET_CONNECT_FAIL,       // SCOKET����ʧ��
		IOCP_SOCKET_INTO_IOCP_FAIL,     // SCOKET������IOCPʧ��
		IOCP_CREATE_EXIT_EVENT_FAIL,    // �����˳��¼�ʧ��
		IOCP_CREATE_IOCP_FAIL,          // ������ɶ˿�ʧ��
		IOCP_CREATE_SOCK_FAIL,          // ����SOCKETʧ��
		IOCP_CREATE_WORK_THREAD_FAIL,   // ���������߳�ʧ��
		IOCP_CREATE_SEND_THREAD_FAIL,   // ���������߳�ʧ��
		IOCP_CREATE_IDLE_THREAD_FAIL,   // ��������߳�ʧ��
		IOCP_SEND_THREAD_EXCEPT,        // �����߳��쳣�˳�
		IOCP_IDLE_THREAD_EXCEPT,        // ����߳��쳣�˳�
		IOCP_WORK_THREAD_EXCEPT,        // �����߳���ʳ��˳�

	};

	class PackQueue;

	/**
	 *\class  IOCP
	 *\brief  ��ɶ˿���
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   �������ࡣ
	 */
	class IOCP
	{
	public:
		/**
		 *\fn         IOCP()
		 *\brief      Ĭ�Ϲ��캯����
		 */
		IOCP(void);

		/**
		 *\fn         ~IOCP()
		 *\brief      ����������
		 */
		~IOCP(void);

	private:
		CRITICAL_SECTION    csAccept_;		
		CRITICAL_SECTION    csRead_;
		CRITICAL_SECTION    csWrite_;
		CRITICAL_SECTION    csPostRead_;
		CRITICAL_SECTION    csWork_;
		CRITICAL_SECTION    csListenSocket_;

		bool                running_;		    // �Ƿ���������
		HANDLE              completionPort_;	// ��ɶ˿ھ��
		HANDLE              idelThread_;		// ���������¼�,�����������߳�
		HANDLE              sendThread_;		// ���Ͱ��߳�

		LPFN_ACCEPTEX       acceptEx_;			// ��ɶ˿ڰ�������

		WORK_THREAD_LIST	workThreadList_;    // �����߳�HANDLE
		LISTEN_INFO_MAP     listenInfoMap_;		// ����SOCKET��Ϣ
		HANDLE              acceptEventes_[MAX_LISTEN_COUNT];
		int                 acceptEventesCount_;// �¼�����

		PackQueue          *inputQueue_;		// �ѽ��հ�����
		PackQueue          *outputQueue_;		// �����Ͱ�����
		BuffPool           *buffPool_;			// �ڴ��
		ClientPool          clientPool_;		// �û��ṹ��
		OperatePool         operatePool_;		// �����ṹ��

		int					buffSize_;			// ��������С
		int					postSocketCount_;	// �Ѿ�Ͷ�ݵ�socket����

	protected:
		/**
		 *\fn         InitIOCP()
		 *\brief      ��ʹ����ɶ˿ڡ�
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int InitIOCP();		

		/**
		 *\fn         AddAccepEvent(int listenSocket)
		 *\brief      ��ӿͻ��˾�������¼���
		 *\n		  ������FD_ACCEPT�¼�ע�ᵽhAcceptEvent��
		 *\n		  ����������AcceptEx���ñ��ľ���ʱ�򣬾ͻᴥ��FD_ACCEPT�¼���
		 *\n		  Ȼ��IdleThread���WaitForSingleObject�ͻ�ɹ����أ�
		 *\n		  ����PostAcceptEx�����ã��ִη���SOKCET
		 *\param[in]  listenSocket  ���������
		 *\return     �¼����
		 */
		HANDLE AddAccepEvent(int listenSocket);

		/**
		 *\fn         PostAcceptSocket(int listenSocket, PCLIENT listenClient)
		 *\brief      ��Ӵ��󶨵Ŀͻ��˾��,��������SOCKET_ADD_NUM��AcceptEx���á�
		 *\param[in]  listenSocket  ���������
		 *\param[in]  listenClient  �����ͻ��˶���
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int PostAcceptSocket(int listenSocket, PCLIENT listenClient);

		/**
		 *\fn         PostReadOperate(PCLIENT client, POPERATE operate)
		 *\brief      Ͷ��һ��������,ÿ��ִ��������������ҪΪԭ����socketͶ��һ����������
		 *\n		  ׼�������´ε����ݡ�
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  operate ��������
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int PostReadOperate(PCLIENT client, POPERATE operate);

		/**
		 *\fn         GetTcpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
		 *\brief      �õ�TCP��ַ����Ҫ������IPV6��Ҳ������IPV4��
		 *\param[in]  ip      IP��ַ��
		 *\param[in]  port    �˿ڵ�ַ��
		 *\param[in]  ipv6    �Ƿ���IPV6��
		 *\param[out] addr    ��ַ�ṹ��
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int GetTcpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr);

		/**
		 *\fn         GetUdpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
		 *\brief      �õ�UDP��ַ����Ҫ������IPV6��Ҳ������IPV4��
		 *\param[in]  ip      IP��ַ��
		 *\param[in]  port    �˿ڵ�ַ��
		 *\param[in]  ipv6    �Ƿ���IPV6��
		 *\param[out] addr    ��ַ�ṹ��
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int GetUdpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr);

        /**
	     *\fn         GetIpAddr(addrinfo *addr)
	     *\brief      �õ�IP��ַ��
	     *\param[in]  addr      ��ַ��ַ��
	     *\return     IP��ַ��
	     */
        int GetIpAddr(addrinfo *addr);

        /**
	     *\fn         GetPort(addrinfo *addr)
	     *\brief      �õ��˿ڡ�
	     *\param[in]  addr      ��ַ��ַ��
	     *\return     �˿ڡ�
	     */
        int GetPort(addrinfo *addr);
		
		/**
		 *\fn         EndAllThread()
		 *\brief      ���������߳�(�����̣߳������̣߳������¼��߳�)��
		 */
		void EndAllThread();

	protected:
		/**
		 *\fn         OPAccept(PCLIENT client, POPERATE operate)
		 *\brief      �󶨲���,�����ӵĿͻ��ˡ�
		 *\param[in]  client  ��������Ŀͻ��˶���(client->socketΪ����socket)��
		 *\param[in]  operate ��������(operate->socketΪ�ͻ���socket)��
		 *\return     ���ӵĿͻ��˶���
		 */
		PCLIENT OPAccept(PCLIENT client, POPERATE operate);

		/**
		 *\fn         OPRead(int size, PCLIENT client, POPERATE operate)
		 *\brief      ��������
		 *\param[in]  size    ���ݳ���
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  operate ��������
		 *\return     �Ƿ�ɹ�
		 */
		bool OPRead(int size, PCLIENT client, POPERATE operate);

		/**
		 *\fn         OPWrite(PCLIENT client, POPERATE operate)
		 *\brief      д������
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  operate ��������
		 *\return     �Ƿ�ɹ�
		 */
		bool OPWrite(PCLIENT client, POPERATE operate);

        /**
		 *\fn         OPWriteFinish(PCLIENT client, POPERATE operate)
		 *\brief      д��ɲ�����
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  operate ��������
		 *\return     �Ƿ�ɹ�
		 */
		bool OPWriteFinish(PCLIENT client, POPERATE operate);

		/**
		 *\fn         WorkThreadProc()
		 *\brief      �����̴߳�������
		 *\return     �Ƿ�ɹ�
		 */
		unsigned WorkThreadProc();

		/**
		 *\fn         SendThreadProc()
		 *\brief      �����̴߳�������
		 *\return     �Ƿ�ɹ�
		 */
		unsigned SendThreadProc();

		/**
		 *\fn         IdleThreadProc()
		 *\brief      �ȴ��̴߳�������
		 *\return     �Ƿ�ɹ�
		 */
		unsigned IdleThreadProc();

		/**
		 *\fn         WorkThread(LPVOID data)
		 *\brief      �����̡߳�
		 *\param[in]  data    �̲߳�����
		 *\return     �Ƿ�ɹ�
		 */
		static unsigned __stdcall WorkThread(LPVOID data);

		/**
		 *\fn         SendThread(LPVOID data)
		 *\brief      �����߳�,����������ж������ݲ�Ͷ��д������
		 *\param[in]  data    �̲߳�����
		 *\return     �Ƿ�ɹ�
		 */
		static unsigned __stdcall SendThread(LPVOID data);

		/**
		 *\fn         IdleThread(LPVOID data)
		 *\brief      �ȴ��߳�,�ػ�hAcceptEvent�����¼�������FD_ACCEPTʱ������
		 *\n		  ˵�����󶨵Ŀͻ����Ѿ��þ�������Ͷ�ݡ�
		 *\param[in]  data    �̲߳�����
		 *\return     �Ƿ�ɹ�
		 */
		static unsigned __stdcall IdleThread(LPVOID data);

	public:
		/**
		 *\fn         Init(int cmdType, PackQueue *inputQueue, PackQueue *outputQueue, BuffPool *buffPool)
		 *\brief      ��ʹ����
		 *\param[in]  cmdType      �������͡�
		 *\param[in]  inputQueue   ������С�
		 *\param[in]  outputQueue  ������С�
		 *\param[in]  buffPool     ����ء�
		 *\return     �Ƿ�ɹ�
		 */
		int     Init(int cmdType, PackQueue *inputQueue, PackQueue *outputQueue, BuffPool *buffPool);

		/**
		 *\fn         Stop()
		 *\brief      ֹͣ��
		 *\return     �Ƿ�ɹ�
		 */
		bool    Stop();

		/**
		 *\fn         SendData(PQueueNode node)
		 *\brief      �������ݡ�
		 *\param[in]  node      ���нڵ㡣
		 *\return     �Ƿ�ɹ�
		 */
		bool    SendData(PQueueNode node);

		/**
		 *\fn         SendData(PQueueNode node)
		 *\brief      ����TCP���ݡ�
		 *\param[in]  client   �ͻ��ˡ�
		 *\param[in]  buf      ���ݡ�
		 *\param[in]  len      ���ݳ���
		 *\return     �Ƿ�ɹ�
		 */
		bool    SendData(PCLIENT client, void *buf, int len);

		/**
		 *\fn         SendData(PQueueNode node)
		 *\brief      ����UDP���ݡ�
		 *\param[in]  client   �ͻ��ˡ�
		 *\param[in]  buf      ���ݡ�
		 *\param[in]  len      ���ݳ���
		 *\param[in]  ip       IP��ַ��
		 *\param[in]  port     �˿ڵ�ַ��
		 *\return     �Ƿ�ɹ�
		 */
		bool    SendData(PCLIENT client, void *buf, int len, int ip, unsigned short port);

		/**
		 *\fn         ConnectTo(const char *ip, unsigned short port, PCLIENT *client)
		 *\brief      ���ӷ�������
		 *\param[in]  ip       IP��ַ��
		 *\param[in]  port     �˿ڵ�ַ��
		 *\param[in]  client   �ͻ��˶���
		 *\return     �Ƿ�ɹ�
		 */
		int     ConnectTo(const char *ip, unsigned short port, PCLIENT *client);

		/**
		 *\fn         AddTcpListen(const char *ip, unsigned short port, PCLIENT *client)
		 *\brief      ���TCP������
		 *\param[in]  ip       IP��ַ��
		 *\param[in]  port     �˿ڵ�ַ��
		 *\param[in]  client   �ͻ��˶���
		 *\return     �Ƿ�ɹ�
		 */
		int     AddTcpListen(const char *ip, unsigned short port, PCLIENT *client);

		/**
		 *\fn         AddUdpListen(const char *ip, unsigned short port, PCLIENT *client)
		 *\brief      ���UDP������
		 *\param[in]  ip       IP��ַ��
		 *\param[in]  port     �˿ڵ�ַ��
		 *\param[in]  client   �ͻ��˶���
		 *\return     �Ƿ�ɹ�
		 */
		int     AddUdpListen(const char *ip, unsigned short port, PCLIENT *client);
		
		/**
		 *\fn         CloseClient(PCLIENT client, POPERATE operate)
		 *\brief      �رն���
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  operate ��������
		 *\return     �Ƿ�ɹ�
		 */
		bool    CloseClient(PCLIENT client, POPERATE operate);
	};
}