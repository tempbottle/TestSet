
#include "stdafx.h"
#include "VideoDecode.h"


VideoDecode::VideoDecode()
{
    buf_ = new BYTE[1024*1024];
}

VideoDecode::~VideoDecode()
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
int VideoDecode::init(int cx, int cy)
{
    return H264_.init(cx, cy);
}

/**
 *\fn           void uninit()
 *\brief        ����ʼ��
 *\return       void ��
 */
void VideoDecode::uninit()
{
    H264_.uninit();
}

/**
 *\fn           int send(void *data)
 *\brief        ��������
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoDecode::send(void *data)
{
    FRAMEINFO out = {0};
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VD_PARAM_NULL;

    out.len = 1024*1024;

    out.data = H264_.decode((BYTE*)frame->data, frame->len, out.len);

    if (NULL == out.data) return E_VD_DECODE_FAIL;

    return MeidaLayout::send(&out);
}

/**
 *\fn           int recv(void *data)
 *\brief        ��������
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoDecode::recv(void *data)
{
    FRAMEINFO out = {0};
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VD_PARAM_NULL;

    out.data = H264_.decode((BYTE*)frame->data, frame->len, out.len);

    if (NULL == out.data) return E_VD_DECODE_FAIL;

    return MeidaLayout::recv(&out);
}