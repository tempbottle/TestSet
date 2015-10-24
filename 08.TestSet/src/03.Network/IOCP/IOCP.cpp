
#include "StdAfx.h"
#include "IOCP.h"
#include "BuffPool.h"
#include "PrePack.h"
#include "PackQueue.h"
#include <process.h>
#include <WS2tcpip.h> // freeaddrinfo


namespace XT_IOCP
{

	/**
	 *\fn         IOCP()
	 *\brief      Ĭ�Ϲ��캯����
	 */
	IOCP::IOCP(void)
		:running_(false),
		completionPort_(NULL),
		idelThread_(NULL),
		sendThread_(NULL),
		acceptEx_(NULL),
		acceptEventesCount_(0),
		inputQueue_(NULL),
		outputQueue_(NULL),
		buffPool_(NULL),
		buffSize_(0),
		postSocketCount_(0)
	{
		::InitializeCriticalSection(&csAccept_);
		::InitializeCriticalSection(&csRead_);
		::InitializeCriticalSection(&csWrite_);
		::InitializeCriticalSection(&csPostRead_);
		::InitializeCriticalSection(&csWork_);
		::InitializeCriticalSection(&csListenSocket_);

		// ��ʹ��socket
		WSADATA wsd = {0};
		::WSAStartup(MAKEWORD(2, 2), &wsd);
	}

	/**
	 *\fn         ~IOCP()
	 *\brief      ����������
	 */
	IOCP::~IOCP(void)
	{
		Stop();

		if (NULL != completionPort_)
		{
			CloseHandle(completionPort_);
			completionPort_ = NULL;
		}

		::DeleteCriticalSection(&csAccept_);
		::DeleteCriticalSection(&csRead_);
		::DeleteCriticalSection(&csWrite_);
		::DeleteCriticalSection(&csPostRead_);
		::DeleteCriticalSection(&csWork_);
		::DeleteCriticalSection(&csListenSocket_);

		::WSACleanup();
	}

	/**
	 *\fn         Stop()
	 *\brief      ֹͣ��
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::Stop(void)
	{
        running_ = false;

		EndAllThread();

		::EnterCriticalSection(&csListenSocket_);

		LISTEN_INFO_MAP::iterator iter = listenInfoMap_.begin();

		for (; iter != listenInfoMap_.end(); ++iter)
		{
			CloseHandle(iter->second.acceptEvent);
			CloseClient(iter->second.listenClient, NULL);			
		}

		listenInfoMap_.clear();

		::LeaveCriticalSection(&csListenSocket_);

		return true;
	}

	/**
	 *\fn         EndAllThread()
	 *\brief      ���������߳�(�����̣߳������̣߳������¼��߳�)��
	 */
	void IOCP::EndAllThread(void)
	{
		for (unsigned int i = 0; i < workThreadList_.size(); i++)
		{
			::PostQueuedCompletionStatus(completionPort_, 0, NULL, NULL);
		}

		workThreadList_.clear();

		::TerminateThread(idelThread_, IOCP_IDLE_THREAD_EXIT);
		::CloseHandle(idelThread_);
		idelThread_ = NULL;

		::TerminateThread(sendThread_, IOCP_SEND_THREAD_EXIT);
		::CloseHandle(sendThread_);
		sendThread_ = NULL;
	}

    /**
	 *\fn         GetTcpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
	 *\brief      �õ�TCP��ַ����Ҫ������IPV6��Ҳ������IPV4��
	 *\param[in]  ip      IP��ַ��
	 *\param[in]  port    �˿ڵ�ַ��
	 *\param[in]  ipv6    �Ƿ���IPV6��
	 *\param[out] addr    ��ַ�ṹ��
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int IOCP::GetTcpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
	{
		char portStr[10] = "";
		sprintf(portStr, "%d", port);

		struct addrinfo info = {0};
		info.ai_family = ipv6?AF_INET6:AF_INET;
		info.ai_socktype = SOCK_STREAM;
		info.ai_protocol = IPPROTO_TCP;

		return getaddrinfo(ip, portStr, &info, addr);
	}

	/**
	 *\fn         GetUdpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
	 *\brief      �õ�UDP��ַ����Ҫ������IPV6��Ҳ������IPV4��
	 *\param[in]  ip      IP��ַ��
	 *\param[in]  port    �˿ڵ�ַ��
	 *\param[in]  ipv6    �Ƿ���IPV6��
	 *\param[out] addr    ��ַ�ṹ��
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int IOCP::GetUdpAddr(const char *ip, unsigned short port, bool ipv6, PADDRINFOA *addr)
	{
		char portStr[10] = "";
		sprintf(portStr, "%d", port);

		struct addrinfo info = {0};
		info.ai_family = ipv6?AF_INET6:AF_INET;
		info.ai_socktype = SOCK_DGRAM;
		info.ai_protocol = IPPROTO_UDP;

		return getaddrinfo(ip, portStr, &info, addr);
	}

    /**
	 *\fn         GetIpAddr(addrinfo *addr)
	 *\brief      �õ�IP��ַ��
	 *\param[in]  addr      ��ַ��ַ��
	 *\return     IP��ַ��
	 */
    int IOCP::GetIpAddr(addrinfo *addr)
    {
        if (NULL == addr) return 0;

        return ((UCHAR)addr->ai_addr->sa_data[5] << 24) | 
            ((UCHAR)addr->ai_addr->sa_data[4] << 16) |
            ((UCHAR)addr->ai_addr->sa_data[3] << 8) |
            (UCHAR)addr->ai_addr->sa_data[2];
    }

    /**
	 *\fn         GetPort(addrinfo *addr)
	 *\brief      �õ��˿ڡ�
	 *\param[in]  addr      ��ַ��ַ��
	 *\return     �˿ڡ�
	 */
    int IOCP::GetPort(addrinfo *addr)
    {
        if (NULL == addr) return 0;

        return ((UCHAR)addr->ai_addr->sa_data[1] << 8) | (UCHAR)addr->ai_addr->sa_data[0];
    }

	/**
	 *\fn         Init(int cmdType, PackQueue *inputQueue, PackQueue *outputQueue, BuffPool *buffPool)
	 *\brief      ��ʹ����
	 *\param[in]  cmdType      �������͡�
	 *\param[in]  inputQueue   ������С�
	 *\param[in]  outputQueue  ������С�
	 *\param[in]  buffPool     ����ء�
	 *\return     �Ƿ�ɹ�
	 */
	int IOCP::Init(int cmdType, PackQueue *inputQueue, PackQueue *outputQueue, BuffPool *buffPool)
	{
		if (NULL == inputQueue || NULL == outputQueue || NULL == buffPool)
		{
			return IOCP_PARA_ERROR;
		}

		inputQueue_  = inputQueue;
		outputQueue_ = outputQueue;
		buffPool_    = buffPool;
		buffSize_    = buffPool->getSize();

#ifdef _DEBUG
		clientPool_.name_ = "clientPool_";
		operatePool_.name_ = "operatePool_";
#endif		

		if (!clientPool_.Init(cmdType, inputQueue, buffPool))
		{
			return IOCP_INIT_CLIENT_POOL_FAIL;
		}

		return InitIOCP();
	}

	/**
	 *\fn         InitIOCP()
	 *\brief      ��ʹ����ɶ˿ڡ�
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int IOCP::InitIOCP()
	{
		if (running_)
		{
			return IOCP_CREATE_EXIT_EVENT_FAIL;
		}
        
        running_ = true;

		// ������ɶ˿�
		completionPort_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (NULL == completionPort_)
		{
			return IOCP_CREATE_IOCP_FAIL;
		}

		// ȡ��ϵͳ��CPU����Ŀ,����CPU����Ŀ*2+2���߳�
		SYSTEM_INFO SysInfo = {0};
		::GetSystemInfo(&SysInfo);
		int cpuCount = SysInfo.dwNumberOfProcessors;
		int nWorkerThreadCount = 1;//SysInfo.dwNumberOfProcessors * 2 + 2;

		// ���������߳�
		for (int i = 0; i < nWorkerThreadCount; i++) 
		{
			unsigned int threadID = 0;
			HANDLE workThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, &threadID);
			if (NULL != workThread)
			{
				workThreadList_.push_back(workThread);
			}
		}

		// ���������̲߳��ɹ�
		if (workThreadList_.size() <= 0)
		{
			return IOCP_CREATE_WORK_THREAD_FAIL;
		}

		// ���������߳�(��ȫ����,ջ��Сʹ��Ĭ�ϵ�,�̺߳���,����,���,�߳�ID)
// 		unsigned int threadId = 0;
// 		sendThread_ = (HANDLE)_beginthreadex(NULL, 0, SendThread, this, 0, &threadId);
// 		if (NULL == sendThread_)
// 		{
//             running_ = false;
// 			this->EndAllThread();
// 			return IOCP_CREATE_SEND_THREAD_FAIL;
// 		}

		// ����Idle�߳�(��ȫ����,ջ��Сʹ��Ĭ�ϵ�,�̺߳���,����,���,�߳�ID)
		unsigned int threadId = 0;
		idelThread_ = (HANDLE)_beginthreadex(NULL, 0, IdleThread, this, 0, &threadId);
		if (NULL == idelThread_)
		{
            running_ = false;
			this->EndAllThread();
			return IOCP_CREATE_IDLE_THREAD_FAIL;
		}

		return S_OK;
	}

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
	HANDLE IOCP::AddAccepEvent(int listenSocket)
	{   
		HANDLE acceptEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == acceptEvent)
		{
			return NULL;
		}

		int ret = WSAEventSelect(listenSocket, acceptEvent, FD_ACCEPT);
		if (SOCKET_ERROR == ret)
		{
			CloseHandle(acceptEvent);
			return NULL;
		}

		return acceptEvent;
	}

	/**
	 *\fn         PostAcceptSocket(int listenSocket, PCLIENT listenClient)
	 *\brief      ��Ӵ��󶨵Ŀͻ��˾��,��������SOCKET_ADD_NUM��AcceptEx���á�
	 *\param[in]  listenSocket  ���������
	 *\param[in]  listenClient  �����ͻ��˶���
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int IOCP::PostAcceptSocket(int listenSocket, PCLIENT listenClient)
	{
		if (NULL == listenClient)
		{
			return IOCP_PARA_ERROR;
		}

		if (NULL == acceptEx_)
		{
			DWORD result = 0;
			GUID GUIDAcceptEx = WSAID_ACCEPTEX;

			// ��ȡ΢��SOCKET��չ����ָ�룬��һ������
			int ret = WSAIoctl(listenSocket, 
				SIO_GET_EXTENSION_FUNCTION_POINTER, 
				&GUIDAcceptEx, 
				sizeof(GUIDAcceptEx),
				&acceptEx_, 
				sizeof(acceptEx_), 
				&result, 
				NULL, 
				NULL);

			if (SOCKET_ERROR == ret)
			{
				closesocket(listenSocket);
				return IOCP_GET_SOCK_ACCEPT_FAIL;
			}
		}

		int successCount = 0;

		for (int i = 0; i < SOCKET_ADD_NUM; i++)
		{
			SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == acceptSocket)
			{
				continue;
			}

			// û����ʱ(��С�İ���װΪ�����֡Ȼ���ٷ���)
			int option = 1;
			int ret = setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
			if (SOCKET_ERROR == ret)
			{
				closesocket(acceptSocket);
				TRACE(_T("setsockopt error:%d\n"), ::GetLastError());
				continue;
			}

			// ���û�������С
			option = 1024*1024;
			ret = setsockopt(acceptSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&option, sizeof(option)); 
			if (SOCKET_ERROR == ret)
			{
				closesocket(acceptSocket);
				TRACE(_T("setsockopt error:%d\n"), ::GetLastError());
				continue;
			}
			
			// ��socket����Ϊ������
			option = 1;
			ret = ioctlsocket(acceptSocket, FIONBIO, (u_long*)&option);
			if (SOCKET_ERROR == ret)
			{
				closesocket(acceptSocket);
				TRACE(_T("ioctlsocket error:%d\n"), ::GetLastError());
				continue;
			}

			POPERATE operation = operatePool_.Get();
			operation->sock    = acceptSocket;
			operation->type    = OP_ACCEPT;
			operation->buf.buf = (char*)buffPool_->Get(1);
			operation->buf.len = buffSize_;
			operation->udp     = false;
			operation->sent    = false;

			/*
			typedef
				BOOL
				(PASCAL FAR * LPFN_ACCEPTEX)(
				__in SOCKET sListenSocket,
				__in SOCKET sAcceptSocket,
				__in PVOID lpOutputBuffer,
				__in DWORD dwReceiveDataLength,
				__in DWORD dwLocalAddressLength,
				__in DWORD dwRemoteAddressLength,
				__out LPDWORD lpdwBytesReceived,
				__inout LPOVERLAPPED lpOverlapped
				);
			*/

			// Ͷ��IoAccept,����ṩ�˵��ĸ������������������һֱ�ȵ��յ���һ�����ݲŻ᷵�ء�   
			int bytes = 1;
			int addrSize = sizeof(SOCKADDR_IN) + 16;

			ret = acceptEx_(listenSocket, 
				operation->sock, 
				(void*)operation->buf.buf,
				operation->buf.len - addrSize * 2,	// ������������ɾ��Buffʱ�����
				addrSize, 
				addrSize,							// �������16���122����
				(DWORD*)&bytes, 
				&(operation->overlapped));

			if (FALSE == ret)
			{
				int error = ::WSAGetLastError();
				if (ERROR_IO_PENDING != error)
				{
					//g_ProFile.m_pLog->println(LOG_ERROR, _T("PostAcceptSocket Error:%d Socket:%d"), error, listenSocket);
					closesocket(acceptSocket);
					continue;
				}
			}

			successCount++;
		}

		postSocketCount_ += successCount;

		TRACE(_T("PostAcceptSocket all:%d\n"), postSocketCount_);
		return successCount;
	}

	/**
	 *\fn         SendThread(LPVOID data)
	 *\brief      �����߳�,����������ж������ݲ�Ͷ��д������
	 *\param[in]  data    �̲߳�����
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::SendThread(LPVOID data)
	{
		if (NULL == data)
		{
			return IOCP_PARA_ERROR;
		}

		IOCP *pThis = reinterpret_cast<IOCP*>(data);

		try
		{
			return pThis->SendThreadProc();
		}
		catch (...)
		{
			//g_ProFile.m_pLog->println(LOG_EXCEPT, _T("SendThreadProc ErrorCode:%d"), ::GetLastError());
			return IOCP_SEND_THREAD_EXCEPT;
		}
	}

	/**
	 *\fn         SendThreadProc()
	 *\brief      �����̴߳�������
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::SendThreadProc(void)
	{
		PCLIENT client = NULL;
		void   *buf       = NULL;
		int   len   = 0;
		int     ip      = {0};
		short    port       = 0;
		short    waitTime   = 1;
		short    times      = 0;

		while (running_)
		{
			if (outputQueue_->Get((void**)&client, &buf, len, ip, port))
			{
				SendData(client, buf, len, ip, port);
				waitTime = 0;
			}
			else
			{
				::Sleep(waitTime); // ���������CPU��100%
				if (++times > 10000)
				{
					times = 0;
					waitTime = 1;
				}
			}
		}

		return IOCP_SEND_THREAD_EXIT;
	}

	/**
	 *\fn         SendData(PQueueNode node)
	 *\brief      �������ݡ�
	 *\param[in]  node      ���нڵ㡣
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::SendData(PQueueNode node)
	{
		bool ret = SendData((PCLIENT)node->client, 
			node->pack,
			node->len,
			node->ip,
			node->port);

		outputQueue_->FreeItem(node);

		return ret;
	}

	/**
	 *\fn         SendData(PQueueNode node)
	 *\brief      ����TCP���ݡ�
	 *\param[in]  client   �ͻ��ˡ�
	 *\param[in]  buf      ���ݡ�
	 *\param[in]  len      ���ݳ���
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::SendData(PCLIENT client, void *buf, int len)
	{
		// ���������ͳɹ����ٽ���
		POPERATE operate = operatePool_.Get();
		operate->sock    = client->sock;
		operate->type    = OP_WRITE;
		operate->buf.buf = (char*)buf;
		operate->buf.len = len;
		operate->udp     = false;
		operate->sent    = false;

		::PostQueuedCompletionStatus(completionPort_, 0, (DWORD)client, &(operate->overlapped));
		return true;
	}

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
	bool IOCP::SendData(PCLIENT client, void *buf, int len, int ip, unsigned short port)
	{
		// ���������ͳɹ����ٽ���
		POPERATE operate = operatePool_.Get();
		operate->sock    = client->sock;
		operate->type    = OP_WRITE;
		operate->buf.buf = (char*)buf;
		operate->buf.len = len;
		operate->udp     = (0 != ip);
		operate->sent    = false;
		operate->setIp(ip);
		operate->setPort(port);

		::PostQueuedCompletionStatus(completionPort_, 0, (DWORD)client, &(operate->overlapped));
		return true;
	}

	/**
	 *\fn         IdleThread(LPVOID data)
	 *\brief      �ȴ��߳�,�ػ�hAcceptEvent�����¼�������FD_ACCEPTʱ������
	 *\n		  ˵�����󶨵Ŀͻ����Ѿ��þ�������Ͷ�ݡ�
	 *\param[in]  data    �̲߳�����
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::IdleThread(LPVOID data)
	{
		if (NULL == data)  return 1;

		IOCP* pThis = reinterpret_cast<IOCP*>(data);

		try
		{
			return pThis->IdleThreadProc();
		}
		catch (...)
		{
			//g_ProFile.m_pLog->println(LOG_EXCEPT, _T("IdleThreadProc ErrorCode:%d"), ::GetLastError());
			return IOCP_IDLE_THREAD_EXCEPT;
		}
	}

	/**
	 *\fn         IdleThreadProc()
	 *\brief      �ȴ��̴߳�������
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::IdleThreadProc(void)
	{
		while (running_)
		{
			if (0 == acceptEventesCount_)
			{
				Sleep(100);
				continue;
			}

			DWORD ret = ::WaitForMultipleObjects(acceptEventesCount_, acceptEventes_, false, 10000); // 10��

			switch (ret)
			{
			case WAIT_FAILED:
				{
					//g_ProFile.m_pLog->println(LOG_INFO, _T("IdleThreadProc WaitForMultipleObjects Err:%d\n"), ::GetLastError());
					continue;
				}
			case WAIT_TIMEOUT:  // ��ʱ���һ�������߳��Ƿ�������
				{
					DWORD code = 0;
					if (!GetExitCodeThread(idelThread_, &code) || (code != STILL_ACTIVE))
					{
						//g_ProFile.m_pLog->println(LOG_INFO, _T("IdelThread dead:%d ExitCode:%d"), idelThread_, code);
					}

					code = 0;
					if (!GetExitCodeThread(sendThread_, &code) || (code != STILL_ACTIVE))
					{
						//g_ProFile.m_pLog->println(LOG_INFO, _T("SendThread dead:%d ExitCode:%d"), sendThread_, code);
					}
					break;
				}
			default:
				{
					int index = ret - WAIT_OBJECT_0;
					this->PostAcceptSocket(listenInfoMap_[index].listenSocket, listenInfoMap_[index].listenClient);

					// ͬʱ����������¼�
					while (index < acceptEventesCount_)
					{
						ret = WaitForMultipleObjects(acceptEventesCount_ - index, &acceptEventes_[index], false, 0);
						
						switch (ret)
						{
						case WAIT_FAILED:
							{
								continue;
							}
						case WAIT_TIMEOUT:
							{
								index = acceptEventesCount_; // �˳����,��Ϊû�б������Ķ�����.
								break;
							}
						default:
							{
								index = ret - WAIT_OBJECT_0;
								PostAcceptSocket(listenInfoMap_[index].listenSocket, listenInfoMap_[index].listenClient);
							}
							break;
						}
					}
					break;
				}
			}
		}

		return IOCP_IDLE_THREAD_EXIT;
	}

	/**
	 *\fn         WorkThread(LPVOID data)
	 *\brief      �����̡߳�
	 *\param[in]  data    �̲߳�����
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::WorkThread(LPVOID data)
	{
		if (NULL == data)  return IOCP_THREAD_PARA_ERROR;

		IOCP* pThis = reinterpret_cast<IOCP*>(data);

		try
		{
			return pThis->WorkThreadProc();
		}
		catch (...)
		{
			//g_ProFile.m_pLog->println(LOG_EXCEPT, _T("CIOCP::WorkThread ErrorCode:%d"), ::GetLastError());
			return IOCP_WORK_THREAD_EXCEPT;
		}
	}

	/**
	 *\fn         WorkThreadProc()
	 *\brief      �����̴߳�������
	 *\return     �Ƿ�ɹ�
	 */
	unsigned IOCP::WorkThreadProc(void)
	{
		BOOL        ret     = 0;
		DWORD       size    = 0;
		DWORD       error   = 0;
		POPERATE    operate = NULL;
		PCLIENT     client  = NULL;

		while (running_)
		{
			ret = ::GetQueuedCompletionStatus(completionPort_, &size, (PULONG_PTR)&client, (LPOVERLAPPED*)&operate, INFINITE);

			if (!ret)          // �������ɹ�
			{
				error = ::WSAGetLastError();

				if (64 == error)
				{
					CloseClient(client, operate);
				}

				// 64    ָ�������������ٿ���
				// 234   ��ʾ�����ݻ�û�н����ꡣӦ���ٴε���WSARecv
				// 995   �����߳��˳���Ӧ�ó��������ѷ��� I/O ����
				// 998   �ڴ���������Ч
				// 10014 WSARecv Ͷ�ݻ�����Ҫ���벻Ȼ��10014
				// 10035 �޷��������һ�����赲���׽��ֲ���
				// 10038 ��һ�����׽����ϳ�����һ������
				// 10054 Զ������ǿ�ȹر���һ�����е�����
				// 10061 ����Ŀ����������ܾ����޷�����
				TRACE(_T("IOCP Error:%d 0x%x 0x%x\n"), error, client, operate);
			}
			else if (NULL != client && NULL != operate)
			{
				switch (operate->type)
				{
				case OP_WRITE:          // ��������
					{
						OPWrite(client, operate);
						break;
					}
                case OP_WRITE_FINISH:   // �������
                    {
                        OPWriteFinish(client, operate);
                        break;
                    }
				case OP_READ:           // ��������
					{
						if (!OPRead(size, client, operate))
							continue;
						else
							PostReadOperate(client, operate);
						break;
					}
				case OP_ACCEPT:         // ���յ����ݺ�Żᴥ��(TCP)
					{
						if (0 != size)
						{
							PCLIENT newClient = OPAccept(client, operate);

							if (NULL == newClient) continue;

							if (!OPRead(size, newClient, operate)) continue;

							if (!PostReadOperate(newClient, operate)) continue;
						}
						else     // ֱ�ӹرմ˿ͻ��ˣ��������ϲ�֪ͨ(û������,Ҳû���ӳɹ�)
						{
							CloseClient(NULL, operate);
							TRACE(_T("OP_Accept: close\n"));
						}
						break;
					}
				default:
					break;
				}
			}
			else                // ���ͻ���Ϣ�������Ϣ���ǿ�ʱ���Լ�Ͷ�ݵģ��˳����߳�
			{
				TRACE(_T("CIOCP.WorkThreadProc Quit\n"));
				break;
			}
		}
		return IOCP_WORK_THREAD_EXIT;
	}

	/**
	*\fn         OPAccept(PCLIENT client, POPERATE operate)
	*\brief      �󶨲���,�����ӵĿͻ��ˡ�
	*\param[in]  client  ��������Ŀͻ��˶���(client->socketΪ����socket)��
	*\param[in]  operate ��������(operate->socketΪ�ͻ���socket)��
	*\return     ���ӵĿͻ��˶���
	*/
	PCLIENT IOCP::OPAccept(PCLIENT client, POPERATE operate)
	{
		//::EnterCriticalSection(&csAccept_);

		// ��������
		int ret = setsockopt(operate->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(client->sock), sizeof(client->sock));
		if (SOCKET_ERROR == ret)    // ���û����ӳɹ���û�з�������ʱ(��ʱ),��ִ�д˺���
		{
			closesocket(operate->sock);
			operatePool_.FreeItem(operate);
			//::LeaveCriticalSection(&csAccept_);
			return NULL;
		}

		PCLIENT newClient = clientPool_.Get(operate->sock);
		if (NULL == newClient)
		{
			//g_ProFile.m_pLog->println(LOG_ERROR, _T("Accept GetClient error"));
			closesocket(operate->sock);
			operatePool_.FreeItem(operate);
			//::LeaveCriticalSection(&csAccept_);
			return NULL;
		}

		// ���½����׽��ֹ�������ɶ˿�
		ret = (int)CreateIoCompletionPort((HANDLE)newClient->sock, completionPort_, (DWORD_PTR)newClient, 0);
		if (NULL == ret)
		{
			CloseClient(newClient, operate);
			//::LeaveCriticalSection(&csAccept_);
			return NULL;
		}

		//::LeaveCriticalSection(&csAccept_);
		return newClient;
	}

	/**
	 *\fn         OPRead(int size, PCLIENT client, POPERATE operate)
	 *\brief      ��������
	 *\param[in]  size    ���ݳ���
	 *\param[in]  client  �ͻ��˶���
	 *\param[in]  operate ��������
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::OPRead(int size, PCLIENT client, POPERATE operate)
	{
		//::EnterCriticalSection(&csRead_);

		if (0 == size)       // ���ϲ�֪ͨ�Ͷ��ѹر�(TCP)
		{
			TRACE(_T("IOCP recv null data Error:%d\n"), ::GetLastError());
			CloseClient(client, operate);
			//::LeaveCriticalSection(&csRead_);
			return false;
		}

		if (!operate->udp) // TCP��
		{
			client->prePack->Add(client, size, operate->buf.buf);
		}
		else // UDP��
		{
			inputQueue_->Add(client, operate->buf.buf, size, operate->getIp(), operate->getPort());

            //static int a = 0;
            //TRACE("%d\n", a++);
		}

		operate->buf.buf = (char*)buffPool_->Get(1);
		operate->buf.len = buffSize_;

		//::LeaveCriticalSection(&csRead_);
		return true;
	}

	/**
	 *\fn         OPWrite(PCLIENT client, POPERATE operate)
	 *\brief      д������
	 *\param[in]  client  �ͻ��˶���
	 *\param[in]  operate ��������
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::OPWrite(PCLIENT client, POPERATE operate)
	{
		//::EnterCriticalSection(&csWrite_);

		// �������
// 		if (operate->sent)
// 		{
// 			operate->sent = false;
// 			operatePool_.FreeItem(operate);
// 			//::LeaveCriticalSection(&csWrite_);
// 			return true;
// 		}

SEND_BEGIN:
		int ret = 0;
		int sentLen = 0;
		int dataLen = operate->buf.len;
		int flags = MSG_PARTIAL;

        operate->type = OP_WRITE_FINISH; // �������

		if (operate->udp) // UDP��
		{
            //static int b = 0;
            //TRACE("write %d\n", b++);

			ret = ::WSASendTo(client->sock, 
				&(operate->buf), 
				1, 
				(DWORD*)&sentLen, 
				0,//flags,
				(LPSOCKADDR)&(operate->remoteAddr), 
				sizeof(SOCKADDR_IN), 
				&(operate->overlapped),
				NULL);
		}
		else
		{
			ret = WSASend(client->sock, 
				&(operate->buf), 
				1, 
				(DWORD*)&sentLen, 
				flags, 
				&(operate->overlapped), 
				NULL);
		}

		if (SOCKET_ERROR == ret)
		{
			static int lockTimes = 0;

			int error = ::WSAGetLastError();
			if (10055 == error)// IOCP���ڲ����кľ�(������)
			{
				Sleep(++lockTimes * 1000);
				goto SEND_BEGIN;
			}
			else if (error != WSA_IO_PENDING)
			{
				CloseClient(client, operate);
				//::LeaveCriticalSection(&csWrite_);
				return false;
			}

			lockTimes = 0;
		}
        else if (sentLen != dataLen)
		{
			if (operate->udp) // UDP��
			{
                OPWriteFinish(client, operate); // ����

				TRACE(_T("IOCP delete UDP pack sent:%d data len:%d error:%d client:%d operate:%d\n"), 
                    sentLen, dataLen, ::WSAGetLastError(), client, operate);
			}
			else // TCP ��
			{
				operate->buf.len = dataLen - sentLen;
				memcpy(operate->buf.buf, operate->buf.buf + sentLen, dataLen - sentLen);
				goto SEND_BEGIN;
			}
		}
// 		else                         // ���ͳɹ�
// 		{
// 			buffPool_->Free(operate->buf.buf);
// 			operate->sent = true;
// 			operate->buf.len = 0;
// 			operate->buf.buf = NULL;
// 
//             TRACE(_T("IOCP UDP pack sent:%d data len:%d error:%d client:%d operate:%d\n"), 
//                 sentLen, dataLen, GetLastError(), client, operate);
// 		}

		//::LeaveCriticalSection(&csWrite_);
		return true;
	}

    /**
	 *\fn         OPWriteFinish(PCLIENT client, POPERATE operate)
	 *\brief      д��ɲ�����
	 *\param[in]  client  �ͻ��˶���
	 *\param[in]  operate ��������
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::OPWriteFinish(PCLIENT client, POPERATE operate)
    {
        //static int c = 0;
        //TRACE("write finish %d\n", c++);

        buffPool_->Free(operate->buf.buf);
        operate->sent = true;
        operate->buf.len = 0;
        operate->buf.buf = NULL;
        operatePool_.FreeItem(operate);
        return true;
    }

	/**
	 *\fn         PostReadOperate(PCLIENT client, POPERATE operate)
	 *\brief      Ͷ��һ��������,ÿ��ִ��������������ҪΪԭ����socketͶ��һ��������
	 *\n		  ׼�������´ε����ݡ�
	 *\param[in]  client  �ͻ��˶���
	 *\param[in]  operate ��������
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int IOCP::PostReadOperate(PCLIENT client, POPERATE operate)
	{   
		int ret= 0;
		int sentLen = 0;
		int flags = MSG_PARTIAL;

		//::EnterCriticalSection(&csPostRead_);

RECV_BEGIN:
		operate->type = OP_READ;

		if (operate->udp) // UDP��
		{
			ret = WSARecvFrom(operate->sock, 
				&(operate->buf), 
				1, 
				(DWORD*)&sentLen,
				(DWORD*)&flags, 
				(LPSOCKADDR)&(operate->remoteAddr), 
				&(operate->addrSize), 
				&(operate->overlapped),
				NULL);
		}
		else
		{
			ret = WSARecv(operate->sock, 
				&(operate->buf), 
				1, 
				(DWORD*)&sentLen, 
				(DWORD*)&flags, 
				&(operate->overlapped), 
				NULL);
		}

		if (SOCKET_ERROR == ret)
		{
			static int lockTimes = 0;

			int error = WSAGetLastError();

			if (error == 10055)// IOCP���ڲ����кľ�(������)
			{
				::Sleep(++lockTimes * 1000);
				//g_ProFile.m_pLog->println(LOG_ERROR, _T("IOCP post recv operate error:10055, socket:%d, times:%d"), pOP->hSocket, m_nLockTimes);
				goto RECV_BEGIN;
			}
			else if (error != WSA_IO_PENDING)
			{
				CloseClient(client, operate);
				//g_ProFile.m_pLog->println(LOG_ERROR, _T("IOCP post recv operate Error:%d"), nError);
			}

			lockTimes = 0;

			ret = error;
		}

		//::LeaveCriticalSection(&csPostRead_);
		return ret;
	}

	/**
	 *\fn         CloseClient(PCLIENT client, POPERATE operate)
	 *\brief      �رն���
	 *\param[in]  client  �ͻ��˶���
	 *\param[in]  operate ��������
	 *\return     �Ƿ�ɹ�
	 */
	bool IOCP::CloseClient(PCLIENT client, POPERATE operate)
	{
		if (NULL != client)
		{			
			PPACK pack = (PPACK)buffPool_->Get(1);// ���ϲ㱨��ͻ����Ѿ��ر�
			pack->clear();
			pack->protoId = CLIENT_CLOSE_SOCKET;
			pack->dataLen = client->sock;

			inputQueue_->Add(NULL, pack, 0);

			closesocket(client->sock);
			clientPool_.FreeItem(client);		// ���տͻ��˶���
			client = NULL;
		}

		if (NULL != operate)
		{
			buffPool_->Free(operate->buf.buf);	// �����ڴ�
			operate->buf.buf = NULL;
			operate->buf.len = 0;

			operatePool_.FreeItem(operate);		// ���ղ�������
			operate = NULL;
		}

		return true;
	}

	/**
	 *\fn         ConnectTo(const char *ip, unsigned short port, PCLIENT *client)
	 *\brief      ���ӷ�������
	 *\param[in]  ip       IP��ַ��
	 *\param[in]  port     �˿ڵ�ַ��
	 *\param[in]  client   �ͻ��˶���
	 *\return     �Ƿ�ɹ�
	 */
	int IOCP::ConnectTo(const char *ip, unsigned short port, PCLIENT *client)
	{
		if (NULL == ip || NULL == client)
		{
			return IOCP_PARA_ERROR;
		}

		// �½�socket
		SOCKET connectSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == connectSocket)
		{
			return IOCP_CREATE_SOCK_FAIL;
		}

		// û����ʱ(��С�İ���װΪ�����֡Ȼ���ٷ���)
		int option = 1;
		int ret = setsockopt(connectSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(char)); // û����ʱ
		if (SOCKET_ERROR == ret)
		{
			closesocket(connectSocket);
			return IOCP_SET_SOCK_FAIL;
		}

		// ���û�������С
		option = 1024*1024;
		ret = setsockopt(connectSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&option, sizeof(option)); 
		if (SOCKET_ERROR == ret)
		{
			closesocket(connectSocket);
			return IOCP_SET_SOCK_FAIL;
		}

		// �õ���ַ�ṹ
		struct addrinfo *addr = NULL;
		ret = GetTcpAddr(ip, port, false, &addr);
		if (INVALID_SOCKET == ret)
		{
			closesocket(connectSocket);
			return IOCP_GET_SOCK_ADDR_FAIL;
		}

		// ���ӵ�������
		ret = connect(connectSocket, addr->ai_addr, (int)addr->ai_addrlen);

		// �ͷŵ�ַ��Ϣ�ṹ
		freeaddrinfo(addr);

		if (SOCKET_ERROR == ret)
		{
			closesocket(connectSocket);
			return IOCP_SOCKET_CONNECT_FAIL;
		}
		
		// ����Ϊ������,�������connectǰ�����connect��ֱ�ӷ���,��Ϊ�Ƿ�������
		option = 1;
		ret = ioctlsocket(connectSocket, FIONBIO, (u_long*)&option);
		if (SOCKET_ERROR == ret)
		{
			closesocket(connectSocket);
			return IOCP_SET_SOCK_FAIL;
		}
		
		// �õ�һ���ͻ������ݽṹ
		PCLIENT connectClient = clientPool_.Get(connectSocket);
		if (NULL == connectClient)
		{
			closesocket(connectSocket);
			return IOCP_GET_CLIENT_DATA_FAIL;
		}

		// �õ�һ���������ݽṹ
		POPERATE operate = operatePool_.Get();
		operate->sock    = connectSocket;
		operate->type    = OP_READ;
		operate->buf.buf = (char*)buffPool_->Get(1);
		operate->buf.len = buffSize_;
		operate->udp     = false;
		operate->sent    = false;
		
		// Ͷ�ݶ�ȡ����
		ret = PostReadOperate(connectClient, operate);
		if (WSA_IO_PENDING != ret)
		{
			CloseClient(connectClient, NULL);
			return IOCP_POST_READ_OPERTE_FAIL;
		}
		
		// ���½������׽��ֹ�������ɶ˿�
		ret = (int)CreateIoCompletionPort((HANDLE)connectSocket, completionPort_, (DWORD_PTR)connectClient, 0);
		if (NULL == ret)
		{
			CloseClient(connectClient, NULL);
			return IOCP_SOCKET_INTO_IOCP_FAIL;
		}

		// ���ط���Ŀͻ��˶���
		*client = connectClient;

		return S_OK;
	}

	/**
	 *\fn         AddTcpListen(const char *ip, unsigned short port, PCLIENT *client)
	 *\brief      ���TCP������
	 *\param[in]  ip       IP��ַ��
	 *\param[in]  port     �˿ڵ�ַ��
	 *\param[in]  client   �ͻ��˶���
	 *\return     �Ƿ�ɹ�
	 */
	int IOCP::AddTcpListen(const char *ip, unsigned short port, PCLIENT *client)
	{
		if (NULL == ip  || NULL == client)
		{
			return IOCP_PARA_ERROR;
		}

		// �½�socket
		SOCKET listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == listenSocket)
		{
			return IOCP_CREATE_SOCK_FAIL;
		}

		// �õ���ַ��Ϣ�ṹ
		struct addrinfo *addr = NULL;
		int ret = GetTcpAddr(ip, port, false, &addr);
		if (INVALID_SOCKET == ret)
		{
			closesocket(listenSocket);
			return IOCP_GET_SOCK_ADDR_FAIL;
		}

		// �󶨼���socket
		ret = bind(listenSocket, addr->ai_addr, addr->ai_addrlen);
		
		// �ͷŵ�ַ��Ϣ�ṹ
		freeaddrinfo(addr);

		if (SOCKET_ERROR == ret)
		{
			closesocket(listenSocket);
			return IOCP_SOCKET_BIND_FAIL;
		}

		// ����
		ret = listen(listenSocket, 100);
		if (SOCKET_ERROR == ret)
		{
			closesocket(listenSocket);
			return IOCP_SOCKET_LISTEN_FAIL;
		}

		// �õ�һ���ͻ������ݽṹ
		PCLIENT listenClient = clientPool_.Get(listenSocket);
		if (NULL == listenClient)
		{
			closesocket(listenSocket);
			return IOCP_GET_CLIENT_DATA_FAIL;
		}

		// ��������¼�
		HANDLE acceptEvent = AddAccepEvent(listenSocket);
		if (NULL != acceptEvent)
		{
			LISTEN_INFO listenInfo = {0};
			listenInfo.acceptEvent = acceptEvent;
			listenInfo.listenClient = listenClient;
			listenInfo.listenSocket = listenSocket;
			listenInfoMap_[acceptEventesCount_] = listenInfo;
			acceptEventes_[acceptEventesCount_] = acceptEvent;
			acceptEventesCount_++;
		}
		else
		{
			CloseClient(listenClient, NULL);
			return IOCP_ADD_ACCEPT_EVENT_FAIL;
		}

		// ׼�����տͻ��˵�socket
		ret = PostAcceptSocket(listenSocket, listenClient);
		if (ret <= 0)
		{
			CloseClient(listenClient, NULL);
			return IOCP_POST_ACCEPT_SOCKET_FAIL;
		}
		
		// �������׽���listenSocket���Ѿ���������ɶ˿ڹ������� 
		ret = (int)CreateIoCompletionPort((HANDLE)listenSocket, completionPort_, (ULONG_PTR)listenClient, 0);
		if (NULL == ret)
		{
			CloseClient(listenClient, NULL);
			return IOCP_SOCKET_INTO_IOCP_FAIL;
		}

		// ���ط���Ŀͻ��˶���
		*client = listenClient;

		return S_OK;
	}

	/**
	 *\fn         AddUdpListen(const char *ip, unsigned short port, PCLIENT *client)
	 *\brief      ���UDP������
	 *\param[in]  ip       IP��ַ��
	 *\param[in]  port     �˿ڵ�ַ��
	 *\param[in]  client   �ͻ��˶���
	 *\return     �Ƿ�ɹ�
	 */
	int IOCP::AddUdpListen(const char *ip, unsigned short port, PCLIENT *client)
	{
		if (NULL == ip || NULL == client)
		{
			return IOCP_PARA_ERROR;
		}

		// �½�socket
		SOCKET listenSocket = ::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == listenSocket)
		{
			return IOCP_CREATE_SOCK_FAIL;
		}

		// �õ���ַ��Ϣ�ṹ
		struct addrinfo *addr = NULL;
		int ret = GetUdpAddr(ip, port, false, &addr);
		if (INVALID_SOCKET == ret)
		{
			closesocket(listenSocket);
			return IOCP_GET_SOCK_ADDR_FAIL;
		}

		// �󶨼���socket
		ret = bind(listenSocket, addr->ai_addr, addr->ai_addrlen);

		if (SOCKET_ERROR == ret)
		{
			closesocket(listenSocket);
			TRACE(_T("bind error:%d\n"), ::GetLastError());
			return IOCP_SOCKET_BIND_FAIL;
		}

		// �õ�һ�����ݽṹ
		PCLIENT listenClient = clientPool_.Get(listenSocket);
		if (NULL == listenClient)
		{
			closesocket(listenSocket);
			return IOCP_GET_CLIENT_DATA_FAIL;
		}

		// �õ�һ���������ݽṹ
		POPERATE operate = operatePool_.Get();
		operate->sock    = listenSocket;
		operate->type    = OP_READ;
		operate->buf.buf = (char*)buffPool_->Get(1);
		operate->buf.len = buffSize_;
		operate->udp     = true;
		operate->sent    = false;
        operate->setIp(GetPort(addr));
        operate->setPort(GetIpAddr(addr));

        // �ͷŵ�ַ��Ϣ�ṹ
		freeaddrinfo(addr);

		// Ͷ�ݶ�ȡ����
		ret = PostReadOperate(listenClient, operate);
		if (WSA_IO_PENDING != ret)
		{
			CloseClient(listenClient, operate);
			return IOCP_POST_READ_OPERTE_FAIL;
		}
		
		// �������׽���listenSocket���Ѿ���������ɶ˿ڹ������� 
		ret = (int)CreateIoCompletionPort((HANDLE)listenSocket, completionPort_, (ULONG_PTR)listenClient, 0);
		if (NULL == ret)
		{
			CloseClient(listenClient, operate);
			return IOCP_SOCKET_INTO_IOCP_FAIL;
		}

		// ���ط���Ŀͻ��˶���
		*client = listenClient;

		return S_OK;
	}

	/*
	// ��ַ����
	BOOL bFlag = TRUE;
	nResult = ::setsockopt(hUdpListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&bFlag, sizeof(bFlag));  
	if (SOCKET_ERROR == nResult)  
	{ 
	TRACE("setsockopt() SO_REUSEADDR failed, Err: %d\n",WSAGetLastError());
	closesocket(hUdpListenSocket);
	return IOCP_SET_SOCK_FAIL;
	}

	// ��socket����Ϊ������,test
	u_long argp = 1;
	nResult = ioctlsocket(hUdpListenSocket, FIONBIO, &argp);

	// ���÷��ͻ������Ĵ�С   
	DWORD maxBytes = 0;   
	setsockopt(hUdpListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&maxBytes, sizeof(maxBytes));

	// ��ȡ����������ջ�������С   
	int maxSend = sizeof(int);   
	getsockopt(hUdpListenSocket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&maxBytes, &maxSend);

	// ���ý��ջ������Ĵ�С   
	setsockopt(hUdpListenSocket, SOL_SOCKET, SO_RCVBUF, (char*)&maxBytes, sizeof(maxBytes));

	// ʹ��nt��IOCTL ��ֹ��WIN2000�¶Է�û�д򿪶˿ڵ������
	// ���ִ���(WSAECONNRESET)10054
	DWORD bytes = 0;
	BOOL inBuffer = FALSE;
	int ret = ::WSAIoctl(listenSocket, SIO_UDP_CONNRESET, &inBuffer, sizeof(inBuffer), NULL, 0, &bytes, NULL, NULL);
	if (SOCKET_ERROR == ret)
	{
	TRACE("AddUdpListen WSAIoctl() SIO_UDP_CONNRESET failed, Err: %d\n", WSAGetLastError());
	closesocket(listenSocket);
	return IOCP_SET_SOCK_FAIL;
	}
	*/
}