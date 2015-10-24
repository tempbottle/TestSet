#pragma once

#include "MediaLayout.h"
#include "H264.h"

enum
{
    E_VE_PARAM_NULL = 3000, /*����Ϊ��*/
    E_VE_ENCODE_FAIL,       /*����ʧ��*/
};

/**
 H264����
**/
class VideoEncode : public MeidaLayout
{
public:
    VideoEncode();
    ~VideoEncode();

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