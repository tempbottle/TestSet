
#include "StdAfx.h"
#include "P2PBase.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PLayoutBase()
	 *\brief      ȱʡ���캯����
	 */
	P2PLayoutBase::P2PLayoutBase()
        :pre_(NULL),
        next_(NULL)
	{
	}

	/**
	 *\fn         ~P2PLayoutBase()
	 *\brief      ����������
	 */
	P2PLayoutBase::~P2PLayoutBase()
	{
	}

    /**
     *\fn         int send(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PLayoutBase::send(P2PMsg *msg)
    {
        if (NULL == next_ || NULL == msg) return -1;

        return next_->send(msg);
    }

    /**
     *\fn         int recv(P2PMsg *msg)
     *\brief      �������
     *\param[in]  msg           �������
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    int P2PLayoutBase::recv(P2PMsg *msg)
    {
        if (NULL == pre_ || NULL == msg) return -1;

        return pre_->recv(msg);
    }
}