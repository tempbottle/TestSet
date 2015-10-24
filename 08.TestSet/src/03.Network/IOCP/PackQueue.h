
#pragma once
#include "Pool.h"
#include <WinSock2.h> // PSOCKADDR_IN


namespace XT_IOCP
{

	/**
	 *\struct tagQUEUE_NODE
	 *\brief  ���нڵ�
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	typedef struct tagQueueNode
	{
		int		len;		// ������
		void	*pack;		// ������
		void	*client;	// �ͻ��˶���
		long	ip;			// UDP��ַIP
		short	port;		// UDP��ַ�˿�
		
	}QueueNode, *PQueueNode;

	/**
	 *\class  PackQueue
	 *\brief  ������Ҫ�����ݰ�����
	 *\	      �ɵײ���������࣬����Ϣ��ӣ������ϲ����Ϣ�����ദ���𵽻���ͷֲ��ʹ��
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class PackQueue : public Pool
	{
	public:
		/**
		 *\fn         PackQueue()
		 *\brief      ȱʡ���캯����
	     */
		PackQueue();

		/**
		 *\fn         ~PackQueue()
		 *\brief      ����������
	     */
		~PackQueue();

	private:
		LIST_VOID	busyNodeList_;	// �ڶ�����İ�
		bool		used_;			// ʹ�ñ��

		/**
		 *\fn         LockQueue()
		 *\brief      �������С�
	     */
		void  LockQueue();

		/**
		 *\fn         UnLockQueue()
		 *\brief      �������С�
	     */
		void  UnLockQueue();

		/**
		 *\fn         NewItem()
		 *\brief      newһ����Ԫ,���麯����
		 *\return     ��Ԫָ�롣
		 */
		virtual void* NewItem();

		/**
		 *\fn         DeleteItem()
		 *\brief      ɾ��һ����Ԫ,���麯����
		 *\param[in]  data ����ָ�롣
		 *\return     ��Ԫָ�롣
		 */
		virtual void DeleteItem(void *data);

	public:
		/**
		 *\fn         Add(PQUEUE_NODE node)
		 *\brief      ����ӡ�
		 *\param[in]  data ����ָ�롣
		 */
		void Add(PQueueNode node);

		/**
		 *\fn         Add(void *client, void *buf, int len)
		 *\brief      TCP����ӡ�
		 *\param[in]  client �ͻ��˶���
		 *\param[in]  len    ���ݳ��ȡ�
		 *\param[in]  buf    ����ָ�롣
		 */
		void Add(void *client, void *buf, int len);
		
		/**
		 *\fn         Add(void *client, void *buf, int len, PSOCKADDR_IN addr)
		 *\brief      UDP����ӡ�
		 *\param[in]  client �ͻ��˶���
		 *\param[in]  buf    ����ָ�롣
		 *\param[in]  len    ���ݳ��ȡ�		 
		 *\param[in]  addr   �Է���ַ��
		 */
		void Add(void *client, void *buf, int len, PSOCKADDR_IN addr);

		/**
		 *\fn         Add(void *client, void *buf, int len, int ip, short port)
		 *\brief      UDP����ӡ�
		 *\param[in]  client �ͻ��˶���
		 *\param[in]  buf    ����ָ�롣
		 *\param[in]  len    ���ݳ��ȡ�		 
		 *\param[in]  ip     �Է�IP��ַ��
		 *\param[in]  port   �Է��˿ڡ�
		 */
		void Add(void *client, void *buf, int len, int ip, short port);

		/**
		 *\fn         Get(PQUEUE_NODE *node)
		 *\brief      �����ӡ�
		 *\param[OUT] node ����ָ���ָ�롣
		 */
		bool Get(PQueueNode *node);

		/**
		 *\fn         Get(void **client, void **buf, int &len)
		 *\brief      TCP�����ӡ�
		 *\param[OUT] client �ͻ�����ָ���ָ�롣
		 *\param[OUT] buf    ����ָ���ָ�롣
		 *\param[OUT] len    ���ݳ���		 
		 */
		bool Get(void **client, void **buf, int &len);

		/**
		 *\fn         Get(void **client, void **buf, int &len)
		 *\brief      UDP�����ӡ�
		 *\param[OUT] client �ͻ�����ָ���ָ�롣
		 *\param[OUT] buf    ����ָ���ָ�롣
		 *\param[OUT] len    ���ݳ���		 
		 *\param[OUT] addr   �Է���ַ��
		 */
		bool Get(void **client, void **buf, int &len, PSOCKADDR_IN addr);

		/**
		 *\fn         Get(void **client, void **buf, int &len, int &ip, short &port)
		 *\brief      UDP�����ӡ�
		 *\param[OUT] client �ͻ�����ָ���ָ�롣
		 *\param[OUT] buf    ����ָ���ָ�롣
		 *\param[OUT] len    ���ݳ���		 
		 *\param[OUT] ip     �Է�IP��ַ��
		 *\param[OUT] port   �Է��˿ڡ�
		 */
		bool Get(void **client, void **buf, int &len, int &ip, short &port);

		/**
		 *\fn         Start()
		 *\brief      ��ʼ������
		 */
		void Start() { used_ = true; }

		/**
		 *\fn         Stop()
		 *\brief      ֹͣ������
		 */
		void Stop() { used_ = false; }
	};

}