#include "StdAfx.h"
#include "ClientPool.h"
#include "PrePack.h"


namespace XT_IOCP
{
	/**
	 *\fn         ClientPool()
	 *\brief      ȱʡ���캯����
	 */
	ClientPool::ClientPool(void)
		:Pool(10),
		buffPool_(NULL),
		inQueue_(NULL),
		cmdType_(CMD_TYPE_MYSELF)
	{
	}

	/**
	 *\fn         ~ClientPool()
	 *\brief      ����������
	 */
	ClientPool::~ClientPool(void)
	{
		DeleteAll();
	}

	/**
	 *\fn         NewItem()
	 *\brief      newһ���ͻ��˶���,���麯����
	 *\return     �ڴ��ָ�롣
	 */
	void* ClientPool::NewItem()
	{
		PCLIENT client = new CLIENT[addCount_];

		client->sock = 0;
        client->data = NULL;
		client->prePack = new PrePack;
		client->prePack->Init(cmdType_, inQueue_, buffPool_);

		return client;
	}

	/**
	 *\fn         DeleteItem()
	 *\brief      ɾ��һ����Ԫ,���麯����
	 *\param[in]  data ����ָ�롣
	 *\return     ��Ԫָ�롣
	 */
	void ClientPool::DeleteItem(void *data)
	{
		PCLIENT client = (PCLIENT)data;
		delete client->prePack;
		delete client;
	}

	/**
	 *\fn         Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
	 *\brief      ��ʹ���ͻ��˶���ء�
	 *\param[in]  cmdType  �������͡�
	 *\param[in]  inQueue  ���ݰ�������С�
	 *\param[in]  buffPool �ڴ�ء�
	 *\return     �Ƿ�ɹ�
	 */
	bool ClientPool::Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
	{
		if (NULL == inQueue || NULL == buffPool) return false;

		EnterCriticalSection(&csUsed_);
		cmdType_ = cmdType;
		inQueue_ = inQueue;
		buffPool_ = buffPool;    
		LeaveCriticalSection(&csUsed_);

		return true;
	}

	/**
	 *\fn         Get(int sock)
	 *\brief      �ӳ���ȡ��һ���ͻ��˶���
	 *\param[in]  sock  �ͻ���socket�����
	 *\return     �ͻ��˶���ָ�롣
	 */
	PCLIENT ClientPool::Get(int sock)
	{
		PCLIENT client = (PCLIENT)GetItem();

		client->sock = sock;

		return client;
	}
}