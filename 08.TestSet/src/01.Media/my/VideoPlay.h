#pragma once

#include "MediaLayout.h"
#include <vfw.h>

enum
{
    E_VP_PARAM_NULL = 2000, /*����Ϊ��*/
    E_VP_INITED,            /*�Ѿ���ʼ��*/
    E_VP_CREATECAPTURE,     /*��������ʧ��*/
    E_VP_DRIVERCONNECT,     /*�����豸ʧ��*/
    E_VP_SETCAPTUREPARAM,   /*���ò���ʧ��*/
    E_VP_STARTCAPTURE,      /*��ʼ����ʧ��*/
    E_VP_OPENFILE,          /*���ļ�ʧ��*/
};

//������Ƶ
class VideoPlay : public MeidaLayout
{
public:
    VideoPlay();
    virtual ~VideoPlay();

protected:
    int x_;
    int y_;
    HDC dc_;
    HDRAWDIB drawDib_;
    BITMAPINFOHEADER bmpInfo_;

public:

    /**
     *\fn           int init(int cx, int cy, HDC dc, LPBITMAPINFOHEADER bmpInfo)
     *\brief        ��ʼ��
     *\param[in]    int x ������
     *\param[in]    int y ������
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\param[in]    HDC dc DC���
     *\param[in]    LPBITMAPINFOHEADER bmpInfo ͼ����Ϣ
     *\return       int 0�ɹ�,����ʧ��
     */
    int init(int x, int y, HDC dc, LPBITMAPINFOHEADER bmpInfo);

    /**
     *\fn           void uninit()
     *\brief        ����ʼ��
     *\return       void ��
     */
    void uninit();

    /**
     *\fn           int send(void *data)
     *\brief        ������Ƶ
     *\param[in]    void * data ��Ƶ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int send(void *data);

    /**
     *\fn           int recv(void *data)
     *\brief        ������Ƶ
     *\param[in]    void * data ��Ƶ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int recv(void *data);
};
