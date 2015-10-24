
#pragma once
#include "Pool.h"
#include <ws2def.h>	// WSABUF


namespace XT_IOCP
{

	/**
	 *\enum   OperateType
	 *\brief  ��������
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	enum OperateType 
	{
		OP_ACCEPT,	    // ��������
		OP_READ,	    // ��
		OP_WRITE,	    // д
        OP_WRITE_FINISH,// д�����
        OP_IDLE		    // ����
	};

// 	typedef struct _WSABUF {
// 		ULONG len;     /* the length of the buffer */
// 		__field_bcount(len) CHAR FAR *buf; /* the pointer to the buffer */
// 	} WSABUF, FAR * LPWSABUF;

	/**
	 *\struct tagOverLappedPlus
	 *\brief  ��չ�ص��ṹ������IOCP�Ķ���
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	typedef struct tagOverLappedPlus
	{
		OVERLAPPED	overlapped;	// �ص��ṹ
		OperateType	type;		// ��������
		WSABUF      buf;		// ������
		SOCKADDR_IN remoteAddr; // �Է���ַ(UDP)
        int         addrSize;   // ��ַ�ṹ��С
        int			sock;		// TCP�ͻ���(TCP�ͻ������Ӽ����˿�ʱ,�����˿�ִ��IOAccept����,�ڲ���¼�ľ��Ƿ��������ͻ��˵�SOCKET)
		bool		sent;		// �������
		bool		udp;		// UDP���ݰ�

        tagOverLappedPlus::tagOverLappedPlus()
        {
            memset(this, 0, sizeof(tagOverLappedPlus));
            addrSize = sizeof(remoteAddr);
            remoteAddr.sin_family = AF_INET;
        }

        void tagOverLappedPlus::setIp(int ip)
        {
            remoteAddr.sin_addr.S_un.S_addr = ip;
        }

        void tagOverLappedPlus::setPort(int port)
        {
            remoteAddr.sin_port = port;
        }

        int tagOverLappedPlus::getIp()
        {
            return remoteAddr.sin_addr.S_un.S_addr;
        }

        USHORT tagOverLappedPlus::getPort()
        {
            return remoteAddr.sin_port;
        }

	}OPERATE,*POPERATE;

	/**
	 *\class  PackQueue
	 *\brief  �ص��ṹ����չ��ÿһ������һ�ֲ�����
	 *\	      ����socketһһ��Ӧ��ֻ��Ͷ��ĳ�ֲ���ʱ���õ�
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class OperatePool : public Pool
	{
	public:
		/**
		 *\fn         OperatePool()
		 *\brief      ȱʡ���캯����
	     */
		OperatePool();

		/**
		 *\fn         ~OperatePool()
		 *\brief      ����������
	     */
		~OperatePool();

	private:
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
		 *\fn         Get()
		 *\brief      �ӳ���ȡ��һ����������
		 *\return     ��������ָ�롣
		 */
		POPERATE Get();
	};

}