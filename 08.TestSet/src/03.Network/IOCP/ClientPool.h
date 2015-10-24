
#pragma once
#include "Pool.h"


namespace XT_IOCP
{

	class BuffPool;
	class PackQueue;
	class PrePack;

	/**
	 *\struct tagClientContext
	 *\brief  �ͻ�����Ϣ
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	typedef struct tagClientContext
	{
		int sock;			// socket����
		PrePack *prePack;	// Ԥ�����
        void *data;         // �û��Զ�������

	}CLIENT,*PCLIENT;

	/**
	 *\class  ClientPool
	 *\brief  ������Ҫ�ǿͻ��˶������
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class ClientPool : public Pool
	{
	public:
		/**
		 *\fn         ClientPool()
		 *\brief      ȱʡ���캯����
	     */
		ClientPool();

		/**
		 *\fn         ~ClientPool()
		 *\brief      ����������
	     */
		~ClientPool();

	private:
		int			cmdType_;	// ��������
		PackQueue	*inQueue_;	// ���ݰ��������
		BuffPool	*buffPool_;	// �ڴ��
		
		/**
		 *\fn         NewItem()
		 *\brief      newһ���ͻ��˶���,���麯����
		 *\return     �ͻ��˶���ָ�롣
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
		 *\fn         Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
		 *\brief      ��ʹ���ͻ��˶���ء�
		 *\param[in]  cmdType  �������͡�
		 *\param[in]  inQueue  ���ݰ�������С�
		 *\param[in]  buffPool �ڴ�ء�
		 *\return     �Ƿ�ɹ�
		 */
		bool Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool);

		/**
		 *\fn         Get(int sock)
		 *\brief      �ӳ���ȡ��һ���ͻ��˶���
		 *\param[in]  sock  �ͻ���socket�����
		 *\return     �ͻ��˶���ָ�롣
		 */
		PCLIENT Get(int sock);
	};
}
