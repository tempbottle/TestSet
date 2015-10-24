
#pragma once
#include "P2PEnginer.h"
#include "03.Network/IOCP/BuffPool.h"
#include "03.Network/IOCP/PackQueue.h"

namespace XT_P2P
{
	/**
	 *\class  P2PApplication
	 *\brief  ������Ҫ��P2P���ϲ�Ӧ����
	 *\date   2013-04-15
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2013-04-15   �ź���   ��������,ʹ��P2P��
	 */
	class P2PApplication
	{
	public:
		/**
		 *\fn         P2PApplication()
		 *\brief      Ĭ�Ϲ��캯����
		 */
		P2PApplication();

		/**
		 *\fn         ~P2PApplication()
		 *\brief      ����������
		 */
		~P2PApplication();

	protected:
		HANDLE		        thread_;	    // �����߳�
		bool 		        running_;	    // �Ƿ���������

		P2PEnginer          enginer_;	    // P2P����
        XT_IOCP::BuffPool   buffPool_;      // �����
		XT_IOCP::PackQueue	inputQueue_;	// �����ж�

	public:
		/**
	     *\fn         Start(LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port)
	     *\brief      ��ʼ���з���
	     *\param[in]  rootPath  �ļ���·����
         *\param[in]  ip        IP��ַ��
         *\param[in]  port      �˿ڵ�ַ��
	     *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	     */
	    int Start(LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port);

		/**
		 *\fn         Stop()
		 *\brief      ֹͣ���з���
		 */
		void Stop();

		/**
		 *\fn         Stop(HANDLE thread)
		 *\brief      ֹͣ�̡߳�
		 *\param[in]  thread  �߳̾����
		 */
		void StopThread(HANDLE thread);

		/**
		 *\fn         ProcessPackThreadPoc()
		 *\brief      �����̴߳�������
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int ProcessPackThreadProc();

		/**
		 *\fn         ProcessPackThread(LPVOID pParam)
		 *\brief      �����̡߳�
		 *\param[in]  param  �̲߳�����
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		static unsigned __stdcall ProcessPackThread(LPVOID param);

		/**
		 *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
		 *\brief      �������ݰ���
		 *\param[in]  client  �ͻ��ˡ�
		 *\param[in]  buf     ���ݡ�
		 *\param[in]  len     ���ݳ���
		 *\param[in]  ip	  IP��ַ(UDP)��
		 *\param[in]  port    �˿ڵ�ַ(UDP)��
		 */
		virtual void ProcessPack(void *client, void *buf, int len, int ip, short port);
	};

}
