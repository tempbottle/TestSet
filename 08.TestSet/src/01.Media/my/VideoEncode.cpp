
#include "stdafx.h"
#include "VideoEncode.h"

VideoEncode::VideoEncode()
{
    buf_ = new BYTE[1024*1024];
}

VideoEncode::~VideoEncode()
{
    delete[] buf_;
}

/**
 *\fn           int init(int cx, int cy)
 *\brief        ��ʼ��
 *\param[in]    int cx ��
 *\param[in]    int cy ��
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoEncode::init(int cx, int cy)
{
    return H264_.init(cx, cy);
}

/**
 *\fn           void uninit()
 *\brief        ����ʼ��
 *\return       void ��
 */
void VideoEncode::uninit()
{
    H264_.uninit();
}

/**
 *\fn           int send(void *data)
 *\brief        ��������
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoEncode::send(void *data)
{
    FRAMEINFO out = {0};
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VE_PARAM_NULL;

    out.data = H264_.encode((BYTE*)frame->data, out.len);

    if (NULL == out.data) return E_VE_ENCODE_FAIL;

    return MeidaLayout::send(&out);
}

/**
 *\fn           int recv(void *data)
 *\brief        ��������
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoEncode::recv(void *data)
{
    FRAMEINFO out = {0};
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VE_PARAM_NULL;

    out.data = H264_.encode((BYTE*)frame->data, out.len);

    if (NULL == out.data) return E_VE_ENCODE_FAIL;

    return MeidaLayout::recv(&out);
}