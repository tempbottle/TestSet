#pragma once
#include "Network/Srv.h"
#include "ZipFile/CZip.h"


namespace XT_IOCP
{

	/**
	 *\class  CSrv_IOCP
	 *\brief  ������Ҫ��IOCP����
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������
	 */
	class CSrv_IOCP : public CSrv
	{
	public:
		/**
		 *\fn         CSrv_HTTP()
		 *\brief      ȱʡ���캯����
		 */
		CSrv_IOCP(void);

		/**
		 *\fn         ~CSrv_IOCP()
		 *\brief      ����������
		 */
		~CSrv_IOCP(void);

		int  num_;		// ����İ���

		CZip zipFile_;	// ZIPѹ���ļ�

        PCLIENT	client_;// �ͻ��˶���
		
		/**
		 *\fn         getNum()
		 *\brief      ����İ���
		 *\return	  ����İ���
		 */
		int  getNum() { return num_; }
		
		/**
		 *\fn         Start(const char *ip, unsigned short port)
		 *\brief      ��Ӽ���
		 *\param[in]  ip		����IP��
		 *\param[in]  port		�����˿ڡ�
		 *\return     �Ƿ�ɹ�(0�ɹ�,����ʧ��)
		 */
		int  Start(const char *ip, unsigned short port);

		/**
		 *\fn         Test(const char *ip, unsigned short port)
		 *\brief      ����
		 *\param[in]  ip		�Է�IP��
		 *\param[in]  port		�Է��˿ڡ�
		 */
		void Test(const char *ip, unsigned short port);

		/**
		 *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
		 *\brief      �������ݰ�
		 *\param[in]  client	�ͻ��˶���
		 *\param[in]  buf		���ݡ�
		 *\param[in]  len		���ݳ���
		 *\param[in]  ip		�Է�IP(UDP)��
		 *\param[in]  port		�Է��˿�(UDP)��
		 */
		void ProcessPack(void *client, void *buf, int len, int ip, short port);
	};
}