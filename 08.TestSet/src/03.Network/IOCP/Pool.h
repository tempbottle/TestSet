
#pragma once
#include <map>
#include <list>
#include <string>

#define WIN32_LEAN_AND_MEAN	// ȥ��winsock.h����
#include <Windows.h>


namespace XT_IOCP
{

	typedef std::map<void*, long>	MAP_ADDR_NUM;

	typedef std::list<void*>		LIST_VOID;


	/**
	 *\class  Pool
	 *\brief  ������Ҫ�ǳ���Ļ���
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class Pool
	{
	public:
		/**
		 *\fn         Pool()
		 *\brief      ȱʡ���캯����
	     */
		Pool();

		/**
		 *\fn         Pool(int addCount)
		 *\brief      ���캯����
	     */
		Pool(int addCount);

		/**
		 *\fn         ~Pool()
		 *\brief      ����������
	     */
		~Pool();

	public:
		CRITICAL_SECTION	csUsed_;	// �ٽ���
		MAP_ADDR_NUM		allData_;	// ���еĵ�Ԫ
		MAP_ADDR_NUM		freeData_;	// ���еĵ�Ԫ
		int					addCount_;	// ÿ����ӵ�����

#ifdef _DEBUG
		std::string			name_;		// �������
#endif

	public:
		/**
		 *\fn         GetItem()
		 *\brief      �õ�һ����Ԫ��
		 *\return     ��Ԫָ�롣
		 */
		void* GetItem();

		/**
		 *\fn         FreeItem(void *data)
		 *\brief      �ͷ�һ����Ԫ��
		 *\param[in]  data ����ָ�롣
		 *\return     �ͷ��Ƿ�ɹ���
		 */
		bool  FreeItem(void *data);

		/**
		 *\fn         NewItem()
		 *\brief      newһ����Ԫ,���麯����
		 *\return     ��Ԫָ�롣
		 */
		virtual void* NewItem() = 0;

		/**
		 *\fn         DeleteItem()
		 *\brief      ɾ��һ����Ԫ,���麯����
		 *\param[in]  data ����ָ�롣
		 *\return     ��Ԫָ�롣
		 */
		virtual void DeleteItem(void *data) = 0;

		/**
		 *\fn         DeleteAll()
		 *\brief      ɾ�����е����ݡ�
		 *\return     �Ƿ�ɹ���
		 */
		bool  DeleteAll();

		/**
		 *\fn         GetNumber(unsigned long &allCount, unsigned long &freeCount)
		 *\brief      �õ�������ʣ��������
		 *\param[out] allCount  ������
		 *\param[out] freeCount ʣ������
		 */
		void  GetNumber(unsigned long &allCount, unsigned long &freeCount);
		
		/**
		 *\fn         setAddCount(int addCount)
		 *\brief      �õ�һ��new������Ԫ��
		 *\param[in]  addCount ������
		 */
		void  setAddCount(int addCount) { addCount_ = addCount; }
	};

}