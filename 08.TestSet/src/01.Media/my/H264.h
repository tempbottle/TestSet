
#pragma once
#include "H264\avcodec.h"
#include "H264\swscale.h"

// ��Ҫ���ÿ�avcodec.lib,avformat.lib,avutil.lib
// Ӧ��ʱҪ��avcodec.dll,avutil.dll

enum
{
    E_H264_PARAM_NULL = 1000, /*����Ϊ��*/
    E_H264_INITED,            /*�Ѿ���ʼ��*/
    E_H264_FIND_ENCODER,
    E_H264_FIND_DECODER,
    E_H264_OPEN_ENCODER,
    E_H264_OPEN_DECODER,
};

class CH264
{
private:
    int cx_;
    int cy_;

    AVCodec *m_pEncoder;
    AVCodec *m_pDecoder;
    
    AVCodecContext *m_pEncoder_c;
    AVCodecContext *m_pDecoder_c;

    AVFrame *m_pRgb_En_picture;
    AVFrame *m_pRgb_De_picture;
    AVFrame *m_pYuv_En_picture;
    AVFrame *m_pYuv_De_picture;

    UCHAR *m_pbyEncodeBuf;
    int m_nEncodeBufSize;
    bool m_bInit;

public:
    CH264();
    ~CH264();

public:
    /**
     *\fn           int init(int cx, int cy)
     *\brief        ��ʼ��
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\return       int 0�ɹ�,����ʧ��
     */
    int init(int cx, int cy);

    /**
     *\fn           void uninit()
     *\brief        ����ʼ��
     *\return       void ��
     */
    void uninit();

    /*                         s
     *\fn         BYTE* encode(BYTE *data, int &outLen)
     *\brief      H264����
     *\param[in]  BYTE * data  ��Ƶ����
     *\param[out] int & outLen ���������ݳ�
     *\return     BYTE* ����������
     */
    BYTE* encode(BYTE *data, int &outLen);
    
    /*
     *\fn         BYTE* decode(BYTE *data, int inLen, int &outLen)
     *\brief      H264����
     *\param[in]  BYTE * data ����������
     *\param[in]  int inLen ���������ݳ�
     *\param[out] int & outLen ���������ݳ�
     *\return     BYTE* 0�ɹ�,����ʧ��
     */
    BYTE* decode(BYTE *data, int inLen, int &outLen);
};
