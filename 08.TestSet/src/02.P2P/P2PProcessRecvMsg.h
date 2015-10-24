
#pragma once
#include "P2PBase.h"
#include "P2PDataInfo.h"
#include "03.Network/IOCP/Srv.h"

namespace XT_P2P
{
    class P2PLayoutBase;

    /**
     *\class  P2PProcessRecvMsg
     *\brief  ������Ҫ��P2PЭ����մ���(������������)
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PProcessRecvMsg
    {
    public:
        /**
         *\fn         P2PProcessRecvMsg()
         *\brief      ȱʡ���캯����
         */
        P2PProcessRecvMsg();

        /**
         *\fn         ~P2PProcessRecvMsg()
         *\brief      ����������
         */
        ~P2PProcessRecvMsg();

    public:
        XT_IOCP::PackQueue *recvQueue_;
        XT_IOCP::BuffPool  *buffPool_;
        P2PLayoutBase      *pre_;
        P2PDataInfo        dataInfo_;
        bool               running_;

    public:
        /**
		 *\fn         void setRecvQueue(XT_IOCP::PackQueue *recvQueue)
		 *\brief      ���ý��ն���
		 *\param[in]  recvQueue	    ���ն���ָ�롣
         *\return     �ޡ�
		 */
        void setRecvQueue(XT_IOCP::PackQueue *recvQueue) { recvQueue_ = recvQueue; }

        /**
		 *\fn         setBuffPool(XT_IOCP::BuffPool *buffPool)
		 *\brief      �����ڴ��
		 *\param[in]  buffPool	    �ڴ��ָ�롣
         *\return     �ޡ�
		 */
        void setBuffPool(XT_IOCP::BuffPool *buffPool) { buffPool_ = buffPool; }

        /**
		 *\fn         setPre(P2PLayoutBase *pre)
		 *\brief      ���ý��ն���
		 *\param[in]  buffPool	    �ڴ��ָ�롣
         *\return     �ޡ�
		 */
        void setPre(P2PLayoutBase *pre) { pre_ = pre; }

        /**
         *\fn         int Start()
         *\brief      ��ʼ������Ϣ��
         *\param[in]  �ޡ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        int Start();

        /**
         *\fn         int Stop()
         *\brief      ֹͣ��
         *\param[in]  �ޡ�
         *\return     �ޡ�
         */
        void Stop();

        /**
		 *\fn         RecvThreadFunc()
		 *\brief      ʵ�ִ�������
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		int RecvThreadFunc();
		
		/**
		 *\fn         ProcessPackThread(void *pParam)
		 *\brief      �����̡߳�
		 *\param[in]  param  �̲߳�����
		 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
		 */
		static unsigned __stdcall RecvThread(void *param);

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