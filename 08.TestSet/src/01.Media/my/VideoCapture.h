#pragma once

#include "MediaLayout.h"
#include <vfw.h>

#define IMAGE_WIDTH     176
#define IMAGE_HEIGHT    144

enum
{
    E_VC_PARAM_NULL = 1000, /*����Ϊ��*/
    E_VC_INITED,            /*�Ѿ���ʼ��*/
    E_VC_CREATECAPTURE,     /*��������ʧ��*/
    E_VC_DRIVERCONNECT,     /*�����豸ʧ��*/
    E_VC_SETCAPTUREPARAM,   /*���ò���ʧ��*/
    E_VC_STARTCAPTURE,      /*��ʼ����ʧ��*/
    E_VC_OPENFILE,          /*���ļ�ʧ��*/
};


//������Ƶ
class VideoCapture : public MeidaLayout
{
public:
    VideoCapture();
    virtual ~VideoCapture();

private:
    BYTE *buf_;
    bool isStop_;
    HWND wndCap_;
    BITMAPINFO bmpInfo_;

public:
    /**
     *\fn           int init(int cx, int cy)
     *\brief        ��ʼ����Ƶ����
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

    /**
     *\fn           int startCapture()
     *\brief        ��ʼ������Ƶ
     *\return       int 0�ɹ�,����ʧ��
     */
    int startCapture();

    /**
     *\fn           int stopCapture()
     *\brief        ֹͣ������Ƶ
     *\return       int 0�ɹ�,����ʧ��
     */
    int stopCapture();

    /**
     *\fn           bool isStop()
     *\brief        �Ƿ��Ѿ�ֹͣ
     *\return       bool trueֹͣ
     */
    bool isStop() { return isStop_; }

    /**
     *\fn           int send(void *data)
     *\brief        ��������
     *\param[in]    void * data ��Ƶ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int send(void *data);

    /**
     *\fn           BITMAPINFOHEADER* getBmpInfoHeader()
     *\brief        �õ�λͼ��Ϣ
     *\return       BITMAPINFOHEADER* λͼ��Ϣ
     */
    BITMAPINFOHEADER* getBmpInfoHeader();

    /**
     *\fn           int saveBmp(LPCSTR fileName, BITMAPINFOHEADER *bitInfo, LPBYTE data)
     *\brief        ����bmpͼ��
     *\param[in]    LPCSTR fileName �ļ���
     *\param[in]    BITMAPINFOHEADER * bitInfo ͼƬ��Ϣ
     *\param[in]    LPBYTE data ͼƬ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int saveBmp(LPCSTR fileName, BITMAPINFOHEADER *bitInfo, LPBYTE data);

private:
    /**
     *\fn           int setCaptureParam(int cx, int cy)
     *\brief        ���ò���
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\return       int 0�ɹ�,����ʧ��
     */
    int setCaptureParam(int cx, int cy);

    /**
     *\fn           LRESULT CALLBACK OnCaptureVideo(HWND wnd, LPVIDEOHDR lphdr)
     *\brief        �豸�ص�
     *\param[in]    HWND wnd �û��Զ�������
     *\param[in]    LPVIDEOHDR lphdr ��Ƶ����
     *\return       LRESULT CALLBACK 0�ɹ�,����ʧ��
     */
    friend LRESULT CALLBACK OnCaptureVideo(HWND wnd, LPVIDEOHDR lphdr);
};
