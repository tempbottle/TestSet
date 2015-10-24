
#include "StdAfx.h"
#include "BuffPool.h"



namespace XT_IOCP
{

	/**
	 *\fn         BuffPool()
	 *\brief      ȱʡ���캯����
	 */
	BuffPool::BuffPool(void)
		:Pool(50)
	{
		buffSize_ = 1024 - sizeof(BUFFHEAD);
	}

	/**
	 *\fn         BuffPool(int size)
	 *\brief      ȱʡ���캯����
	 *\param[in]  size �ڴ���С��
	 */
	BuffPool::BuffPool(int size)
		:Pool(50)
	{
		buffSize_ = size;
	}
	
	/**
	 *\fn         ~BuffPool()
	 *\brief      ����������
	 */
	BuffPool::~BuffPool(void)
	{
		DeleteAll();
	}

	/**
	 *\fn         NewItem()
	 *\brief      newһ���ڴ��,���麯����
	 *\return     �ڴ��ָ�롣
	 */
	void* BuffPool::NewItem()
	{
		return new char[buffSize_ + sizeof(BUFFHEAD)];
	}

	/**
	 *\fn         DeleteItem()
	 *\brief      ɾ��һ����Ԫ,���麯����
	 *\param[in]  data ����ָ�롣
	 *\return     ��Ԫָ�롣
	 */
	void BuffPool::DeleteItem(void *data)
	{
		delete data;
	}

	/**
	 *\fn         Get(unsigned short times)
	 *\brief      �õ�һ����Ԫ��
	 *\param[in]  times �ڴ��ʹ�ô�����
	 *\return     �ڴ��ָ�롣
	 */
	PVOID BuffPool::Get(unsigned short times)
	{
		PBUFFHEAD buff = (PBUFFHEAD)GetItem();

		buff->usedCount = times;

		return ++buff; // ʵ�ʴ�����ݵ�λ��
	}

	/**
	 *\fn         Free(void *buff)
	 *\brief      ����һ���ڴ�,����������һ�����Ϊһ���Ƶ���������
	 *\param[in]  buff ����ָ�롣
	 *\return     �ͷ��Ƿ�ɹ���
	 */
	bool BuffPool::Free(void *buff)
	{
		if (NULL == buff) return false;

		PBUFFHEAD buffHead = (PBUFFHEAD)buff;
		buffHead--;    // ʵ��ָ����newʱ��ָ��

		if (buffHead->usedCount > 1)
		{
			buffHead->usedCount--;
			return true;
		}
		else
		{
			return FreeItem(buffHead);
		}
	}

	/**
	 *\fn         SetTimes(void *buff, unsigned short newTimes = 1)
	 *\brief      ���������ڴ�鱻ʹ�ô�����
	 *\param[in]  buff ����ָ�롣
	 *\param[in]  newTimes �µ�ʹ�ô�����
	 *\return     �ͷ��Ƿ�ɹ���
	 */
	bool BuffPool::SetTimes(PVOID buff, unsigned short newTimes)
	{
		if (NULL == buff) return false;

		EnterCriticalSection(&csUsed_);

		PBUFFHEAD buffHead = (PBUFFHEAD)buff;

		(--buffHead)->usedCount = newTimes;

		LeaveCriticalSection(&csUsed_);

		return false;
	}
}