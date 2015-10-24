
#include "StdAfx.h"
#include "OperatePool.h"


namespace XT_IOCP
{

	/**
	 *\fn         OperatePool()
	 *\brief      ȱʡ���캯����
	 */
	OperatePool::OperatePool(void)
		:Pool(100)
	{
	}

	/**
	 *\fn         ~OperatePool()
	 *\brief      ����������
	 */
	OperatePool::~OperatePool(void)
	{
		DeleteAll();
	}

	/**
	 *\fn         NewItem()
	 *\brief      newһ����Ԫ,���麯����
	 *\return     ��Ԫָ�롣
	 */
	void* OperatePool::NewItem()
	{
		POPERATE operate = new OPERATE;
		return operate;
	}

	/**
	 *\fn         DeleteItem()
	 *\brief      ɾ��һ����Ԫ,���麯����
	 *\param[in]  data ����ָ�롣
	 *\return     ��Ԫָ�롣
	 */
	void OperatePool::DeleteItem(void *data)
	{
		delete data;
	}

	/**
	 *\fn         Get()
	 *\brief      �ӳ���ȡ��һ����������
	 *\return     ��������ָ�롣
	 */
	POPERATE OperatePool::Get()
	{
		POPERATE operate = (POPERATE)GetItem();
		return operate;
	}
}
