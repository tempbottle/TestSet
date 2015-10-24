
#pragma once
#include "P2PDataInfo.h"


namespace XT_P2P
{
    class P2PMsg;

    /**
     *\class  P2PBase
     *\brief  ������Ҫ��P2P�������
     *\date   2012-12-26
     *\author �ź���
     *\par    Revision History:
     */
    class P2PLayoutBase
    {
    public:
        /**
         *\fn         P2PLayoutBase()
         *\brief      ȱʡ���캯����
         */
        P2PLayoutBase();

        /**
         *\fn         ~P2PLayoutBase()
         *\brief      ����������
         */
        ~P2PLayoutBase();

    protected:
        P2PLayoutBase *pre_;        // ��һ���ڵ�
        P2PLayoutBase *next_;       // ��һ���ڵ�
        P2PDataInfo   dataInfo_;    // ͳ��������Ϣ

    public:
        /**
         *\fn         int send(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        virtual int send(P2PMsg *msg);

        /**
         *\fn         int recv(P2PMsg *msg)
         *\brief      �������
         *\param[in]  msg           �������
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
         */
        virtual int recv(P2PMsg *msg);

        /**
         *\fn         void setPre(P2PLayoutBase *layout)
         *\brief      ������һ��ڵ㡣
         *\param[in]  layout        ��ڵ㡣
         *\return     �ޡ�
         */
        void setPre(P2PLayoutBase *layout) { pre_ = layout; }

        /**
         *\fn         void setNext(P2PLayoutBase *layout)
         *\brief      ������һ��ڵ㡣
         *\param[in]  layout        ��ڵ㡣
         *\return     �ޡ�
         */
        void setNext(P2PLayoutBase *layout) { next_ = layout; }

        /**
         *\fn         P2PLayoutBase* getPre()
         *\brief      �õ���һ��ڵ㡣
         *\param[in]  �ޡ�
         *\return     ��һ��ڵ�ָ�롣
         */
        P2PLayoutBase* getPre() { return pre_; }

        /**
         *\fn         P2PLayoutBase* getNext()
         *\brief      �õ���һ��ڵ㡣
         *\param[in]  �ޡ�
         *\return     ��һ��ڵ�ָ�롣
         */
        P2PLayoutBase* getNext() { return next_; }

        /**
         *\fn         void addRecvPack(UINT size)
         *\brief      ���ý��յ������ݳ���
         *\param[in]  size          ���ݳ���
         *\return     �ޡ�
         */
        void addRecvPack(UINT size) { dataInfo_.addRecvPack(size); }

        /**
         *\fn         void addSendPack(UINT size)
         *\brief      ���÷��͵����ݳ���
         *\param[in]  size          ���ݳ���
         *\return     �ޡ�
         */
        void addSendPack(UINT size) { dataInfo_.addSendPack(size); }

        /**
         *\fn         P2PDataInfo* getDataInfo()
         *\brief      �õ�ͳ�����ݡ�
         *\param[in]  �ޡ�
         *\return     ͳ�����ݡ�
         */
        P2PDataInfo* getDataInfo() { return &dataInfo_; }
    };

}