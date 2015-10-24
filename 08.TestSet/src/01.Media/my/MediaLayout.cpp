
#include "StdAfx.h"
#include "MediaLayout.h"


/**
 *\fn         MeidaLayout()
 *\brief      ȱʡ���캯����
 */
MeidaLayout::MeidaLayout()
    :pre_(NULL),
    next_(NULL)
{
}

/**
 *\fn         ~MeidaLayout()
 *\brief      ����������
 */
MeidaLayout::~MeidaLayout()
{
}

/**
 *\fn         int send(void *data)
 *\brief      �������ݡ�
 *\param[in]  data           ���ݰ���
 *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
 */
int MeidaLayout::send(void *data)
{
    if (NULL == next_ || NULL == data) return -1;

    return next_->send(data);
}

/**
 *\fn         int recv(void *data)
 *\brief      �������ݡ�
 *\param[in]  data           ���ݰ���
 *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
 */
int MeidaLayout::recv(void *data)
{
    if (NULL == pre_ || NULL == data) return -1;

    return pre_->recv(data);
}
