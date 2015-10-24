
#pragma once

#include "IOCP.h"
#include "PackQueue.h"
#include "BuffPool.h"
#include "PrePack.h"


namespace XT_IOCP
{
	/**
	 *\class  CSrv
	 *\brief  ������Ҫ����ɶ˿ڷ�����
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������,ʹ����ɶ˿ڡ�
	 */
	class CSrv
	{
	public:
		/**
		 *\fn         CSrv()
		 *\brief      Ĭ�Ϲ��캯����
		 */
		CSrv();

		/**
		 *\fn         ~CSrv()
		 *\brief      ����������
		 */
		~CSrv();

	protected:
		HANDLE		thread_;		// �����߳�
		bool 		running_;	    // �Ƿ���������

		IOCP        iocp_;			// ��ɶ˿�
		PackQueue	inputQueue_;	// �����ж�
		PackQueue	outputQueue_;	// ����ж�
		BuffPool    buffPool_;		// �ڴ��
		int			cmdType_;		// ��������

	public:
		/**
		 *\fn         Start(int cmdType, int buffSize)
		 *\brief      ��ʼ���з���
		 *\param[in]  cmdType  �������͡�
		 *\param[in]  buffSize ��������С��
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int Start(int cmdType, int buffSize);

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
		 *\brief      ʵ�ִ�������
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int ProcessPackThreadPoc();
		
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
