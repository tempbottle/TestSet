
#include "stdafx.h"
#include "PackQueue.h"


namespace XT_IOCP
{

	/**
	 *\fn         PackQueue()
	 *\brief      ȱʡ���캯����
	 */
	PackQueue::PackQueue()
		:Pool(100),
		used_(false)
	{
	}

	/**
	 *\fn         ~PackQueue()
	 *\brief      ����������
	 */
	PackQueue::~PackQueue()
	{
		DeleteAll();
	}

	/**
	 *\fn         LockQueue()
	 *\brief      �������С�
	 */
	void PackQueue::LockQueue()
	{
		EnterCriticalSection(&csUsed_);
	}

	/**
	 *\fn         UnLockQueue()
	 *\brief      �������С�
	 */
	void PackQueue::UnLockQueue()
	{
		LeaveCriticalSection(&csUsed_);
	}

	/**
	 *\fn         NewItem()
	 *\brief      newһ���ڴ��,���麯����
	 *\return     �ڴ��ָ�롣
	 */
	void* PackQueue::NewItem()
	{
		return new QueueNode;
	}

	/**
	 *\fn         DeleteItem()
	 *\brief      ɾ��һ����Ԫ,���麯����
	 *\param[in]  data ����ָ�롣
	 *\return     ��Ԫָ�롣
	 */
	void PackQueue::DeleteItem(void *data)
	{
		delete data;
	}

	/**
	 *\fn         Add(PQUEUE_NODE node)
	 *\brief      ����ӡ�
	 *\param[in]  data ����ָ�롣
	 */
	void PackQueue::Add(PQueueNode node)
	{
		if (!used_) return;

		LockQueue();

		busyNodeList_.push_back(node);

		UnLockQueue();
	}
	
	/**
	 *\fn         Add(void *client, int len, void *buf)
	 *\brief      TCP����ӡ�
	 *\param[in]  client �ͻ��˶���
	 *\param[in]  buf    ����ָ�롣
	 *\param[in]  len    ���ݳ��ȡ�
	 */
	void PackQueue::Add(void *client, void *buf, int len)
	{
		if (!used_) return;

		LockQueue();

		PQueueNode node = (PQueueNode)GetItem();
		node->client	= client;
		node->pack		= buf;
		node->len		= len;
		node->ip		= 0;
		node->port      = 0;

		busyNodeList_.push_back(node);

		UnLockQueue();
	}

	/**
	 *\fn         Add(void *client, void *buf, int len, PSOCKADDR_IN addr)
	 *\brief      UDP����ӡ�
	 *\param[in]  client �ͻ��˶���
	 *\param[in]  buf    ����ָ�롣
	 *\param[in]  len    ���ݳ��ȡ�	 
	 *\param[in]  addr   �Է���ַ��
	 */
	void PackQueue::Add(void *client, void *buf, int len, PSOCKADDR_IN addr)
	{
		if (!used_) return;

		LockQueue();

		PQueueNode node = (PQueueNode)GetItem();
		node->client    = client;
		node->pack      = buf;
		node->len       = len;
		node->ip		= addr->sin_addr.S_un.S_addr;
		node->port      = addr->sin_port;

		busyNodeList_.push_back(node);

		UnLockQueue();
	}
	
	/**
	 *\fn         Add(void *client, void *buf, int len, int ip, short port)
	 *\brief      UDP����ӡ�
	 *\param[in]  client �ͻ��˶���
	 *\param[in]  buf    ����ָ�롣
	 *\param[in]  len    ���ݳ��ȡ�	 
	 *\param[in]  ip     �Է�IP��ַ��
	 *\param[in]  port   �Է��˿ڡ�
	 */
	void PackQueue::Add(void *client, void *buf, int len, int ip, short port)
	{
		if (!used_) return;

		LockQueue();

		PQueueNode node = (PQueueNode)GetItem();
		node->client	= client;
		node->pack		= buf;
		node->len		= len;
		node->ip		= ip;
		node->port		= port;

		busyNodeList_.push_back(node);

		UnLockQueue();
	}

	/**
	 *\fn         Get(PQUEUE_NODE *node)
	 *\brief      �����ӡ�
	 *\param[OUT] node ����ָ���ָ�롣
	 */
	bool PackQueue::Get(PQueueNode *node)
	{
		LockQueue();

		bool ret = false;

		if (!busyNodeList_.empty())
		{
			LIST_VOID::iterator iter = busyNodeList_.begin();

			if (iter != busyNodeList_.end())
			{
				PQueueNode node = (PQueueNode)*iter;

				busyNodeList_.erase(iter);

				ret = true;
			}
		}

		UnLockQueue();

		return ret;
	}

	/**
	*\fn         Get(void **client, void **buf, int &len)
	*\brief      TCP�����ӡ�
	*\param[OUT] client �ͻ�����ָ���ָ�롣
	*\param[OUT] buf    ����ָ���ָ�롣
	*\param[OUT] len    ���ݳ���	
	*/
	bool PackQueue::Get(void **client, void **buf, int &len)
	{
		LockQueue();

		bool ret = false;

		if (!busyNodeList_.empty())
		{
			LIST_VOID::iterator iter = busyNodeList_.begin();

			if (iter != busyNodeList_.end())
			{
				PQueueNode node = (PQueueNode)*iter;
				len     = node->len;
				*buf    = node->pack;
				*client = node->client;

				FreeItem(node);
				
				busyNodeList_.erase(iter);

				ret = true;
			}
		}

		UnLockQueue();

		return ret;
	}

	/**
	 *\fn         Get(void **client, void **buf, int &len, PSOCKADDR_IN addr)
	 *\brief      UDP�����ӡ�
	 *\param[OUT] client �ͻ�����ָ���ָ�롣
	 *\param[OUT] buf    ����ָ���ָ�롣
	 *\param[OUT] len    ���ݳ���	 
	 *\param[OUT] addr   �Է���ַ��
	 */
	bool PackQueue::Get(void **client, void **buf, int &len, PSOCKADDR_IN addr)
	{
		LockQueue();

		bool ret = false;

		if (!busyNodeList_.empty())
		{
			LIST_VOID::iterator iter = busyNodeList_.begin();

			if (iter != busyNodeList_.end())
			{
				PQueueNode node            = (PQueueNode)*iter;
				len                        = node->len;
				*buf                       = node->pack;
				*client                    = node->client;
				addr->sin_addr.S_un.S_addr = node->ip;
				addr->sin_port             = node->port;

				FreeItem(node);

				busyNodeList_.erase(iter);

				ret = true;
			}
		}

		UnLockQueue();

		return ret;
	}

	/**
	 *\fn         Get(void **client, void **buf, int &len, int &ip, short &port)
	 *\brief      UDP�����ӡ�
	 *\param[OUT] client �ͻ�����ָ���ָ�롣
	 *\param[OUT] buf    ����ָ���ָ�롣
	 *\param[OUT] len    ���ݳ���	 
	 *\param[OUT] ip     �Է�IP��ַ��
	 *\param[OUT] port   �Է��˿ڡ�
	 */
	bool PackQueue::Get(void **client, void **buf, int &len, int &ip, short &port)
	{
		LockQueue();

		bool ret = false;

		if (!busyNodeList_.empty())
		{
			LIST_VOID::iterator iter = busyNodeList_.begin();

			if (iter != busyNodeList_.end())
			{
				PQueueNode node = (PQueueNode)*iter;
				len     = node->len;
				*buf    = node->pack;
				*client = node->client;
				ip      = node->ip;
				port    = node->port;

				FreeItem(node);

				busyNodeList_.erase(iter);

				ret = true;
			}
		}

		UnLockQueue();

		return ret;
	}
}