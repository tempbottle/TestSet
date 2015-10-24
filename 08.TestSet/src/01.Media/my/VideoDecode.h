#pragma once

#include "MediaLayout.h"
#include "H264.h"

enum
{
    E_VD_PARAM_NULL = 4000, /*����Ϊ��*/
    E_VD_DECODE_FAIL,       /*����ʧ��*/
};

/**
 H264����
**/
class VideoDecode : public MeidaLayout
{
public:
    VideoDecode();
    ~VideoDecode();

protected:
    BYTE *buf_;     // ������
    CH264 H264_;    // ��ƵH264�����

public:
    /**
     *\fn           int init()
     *\brief        ��ʼ��
     *\return       int 0�ɹ�,����ʧ��
     */
    int init(int cx, int cy);

    /**
     *\fn           void uninit()
     *\brief        ����ʼ��
     *\return       void ��
     */
    void uninit();

    /**
     *\fn           int send(void *data)
     *\brief        ��������
     *\param[in]    void * data ��Ƶ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int send(void *data);

    /**
     *\fn           int recv(void *data)
     *\brief        ��������
     *\param[in]    void * data ��Ƶ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int recv(void *data);
};