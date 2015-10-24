
#pragma once
#include "Pool.h"


namespace XT_IOCP
{

	/**
	 *\struct tagBuffHead
	 *\brief  �ڴ����Ϣͷ
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	typedef struct tagBuffHead
	{
		short usedCount;    // ʹ�ô���

	}BUFFHEAD,*PBUFFHEAD;

	/**
	 *\class  BuffPool
	 *\brief  ������Ҫ���ڴ����
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class BuffPool : public Pool
	{
	public:
		/**
		 *\fn         BuffPool()
		 *\brief      ȱʡ���캯����
	     */
		BuffPool();

		/**
		 *\fn         BuffPool(int size)
		 *\brief      ȱʡ���캯����
		 *\param[in]  size �ڴ���С��
	     */
		BuffPool(int size);

		/**
		 *\fn         ~BuffPool()
		 *\brief      ����������
	     */
		~BuffPool();

	private:
		unsigned long buffSize_;	// ��������С

		/**
		 *\fn         NewItem()
		 *\brief      newһ���ڴ��,���麯����
		 *\return     �ڴ��ָ�롣
		 */
		virtual void* NewItem();

		/**
		 *\fn         DeleteItem()
		 *\brief      ɾ��һ����Ԫ,���麯����
		 *\param[in]  data ����ָ�롣
		 *\return     ��Ԫָ�롣
		 */
		void DeleteItem(void *data);

	public:
		/**
		 *\fn         Get(unsigned short times)
		 *\brief      �õ�һ����Ԫ��
		 *\param[in]  times �ڴ��ʹ�ô�����
		 *\return     �ڴ��ָ�롣
		 */
		void* Get(unsigned short times);

		/**
		 *\fn         Free(void *buff)
		 *\brief      ����һ���ڴ档
		 *\param[in]  buff ����ָ�롣
		 *\return     �ͷ��Ƿ�ɹ���
		 */
		bool Free(void *buff);
		
		/**
		 *\fn         SetTimes(void *buff, unsigned short newTimes = 1)
		 *\brief      ���������ڴ�鱻ʹ�ô�����
		 *\param[in]  buff ����ָ�롣
		 *\param[in]  newTimes �µ�ʹ�ô�����
		 *\return     �ͷ��Ƿ�ɹ���
		 */
		bool SetTimes(void *buff, unsigned short newTimes);
		
		/**
		 *\fn         setSize(unsigned long size)
		 *\brief      �����ڴ���С��
		 *\param[in]  size �ڴ���С��
		 *\return     �Ƿ�ɹ���
		 */
		void setSize(unsigned long size) { buffSize_ = size; }
		
		/**
		 *\fn         getSize()
		 *\brief      �õ��ڴ���С��
		 *\param[in]  size ����ָ�롣
		 *\return     �ڴ���С��
		 */
		unsigned long getSize() { return buffSize_; }
	};
}