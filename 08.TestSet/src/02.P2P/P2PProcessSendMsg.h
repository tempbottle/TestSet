
#pragma once
#include "P2PBase.h"
#include "P2PDataInfo.h"
#include "03.Network/IOCP/Srv.h"

namespace XT_P2P
{
    class P2PLayoutBase;

    /**
     *\class  P2PProcessSendMsg
     *\brief  ������Ҫ��P2PЭ�鷢�ʹ���(������������)
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PProcessSendMsg
    {
    public:
        /**
         *\fn         P2PProcessSendMsg()
         *\brief      ȱʡ���캯����
         */
        P2PProcessSendMsg();

        /**
         *\fn         ~P2PProcessSendMsg()
         *\brief      ����������
         */
        ~P2PProcessSendMsg();

    public:
        XT_IOCP::PackQueue *sendQueue_;
        XT_IOCP::BuffPool  *buffPool_;
        XT_IOCP::IOCP      *iocp_;
        P2PDataInfo        dataInfo_;
        bool               running_;

    public:
        /**
		 *\fn         void setSendQueue(XT_IOCP::PackQueue *sendQueue)
		 *\brief      ���÷��Ͷ���
		 *\param[in]  sendQueue	    ���Ͷ���ָ�롣
         *\return     �ޡ�
		 */
        void setSendQueue(XT_IOCP::PackQueue *sendQueue) { sendQueue_ = sendQueue; }

        /**
		 *\fn         setBuffPool(XT_IOCP::BuffPool *buffPool)
		 *\brief      �����ڴ��
		 *\param[in]  buffPool	    �ڴ��ָ�롣
         *\return     �ޡ�
		 */
        void setBuffPool(XT_IOCP::BuffPool *buffPool) { buffPool_ = buffPool; }

        /**
		 *\fn         setIocp(XT_IOCP::IOCP *iocp)
		 *\brief      ���ý������ݵĶ���
		 *\param[in]  iocp	        ��ɶ˿�ָ�롣
         *\return     �ޡ�
		 */
        void setIocp(XT_IOCP::IOCP *iocp) { iocp_ = iocp; }

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
		 *\fn         SendThread(LPVOID data)
		 *\brief      �����߳�,����������ж������ݲ�Ͷ��д������
		 *\param[in]  data    �̲߳�����
		 *\return     �Ƿ�ɹ�
		 */
		static unsigned __stdcall SendThread(LPVOID data);

        /**
		 *\fn         SendThreadProc()
		 *\brief      �����̴߳�������
		 *\return     �Ƿ�ɹ�
		 */
		unsigned SendThreadProc();
    };

}