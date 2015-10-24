
#include "stdafx.h"
#include "VideoPlay.h"

VideoPlay::VideoPlay()
    :x_(0),
    y_(0),
    dc_(NULL),
    drawDib_(NULL)
{
}

VideoPlay::~VideoPlay()
{

}

/**
 *\fn           int init(int cx, int cy, HDC dc, LPBITMAPINFOHEADER bmpInfo)
 *\brief        ��ʼ��
 *\param[in]    int x ������
 *\param[in]    int y ������
 *\param[in]    HDC dc DC���
 *\param[in]    LPBITMAPINFOHEADER bmpInfo ͼ����Ϣ
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoPlay::init(int x, int y, HDC dc, LPBITMAPINFOHEADER bmpInfo)
{
    if (NULL == dc) return E_VP_PARAM_NULL;
    if (NULL != drawDib_) return E_VP_INITED;

    x_ = x;
    y_ = y;
    dc_ = dc;
    bmpInfo_ = *bmpInfo;
    bmpInfo_.biBitCount = 24;
    bmpInfo_.biCompression = BI_RGB;
    bmpInfo_.biSizeImage = bmpInfo_.biWidth * bmpInfo_.biHeight * 3;

    drawDib_ = DrawDibOpen();

    return !::DrawDibBegin(drawDib_, dc, -1, -1, &bmpInfo_, bmpInfo_.biWidth, bmpInfo_.biHeight, 0);
}

/**
 *\fn           void uninit()
 *\brief        ����ʼ��
 *\return       void ��
 */
void VideoPlay::uninit()
{
    ::DrawDibClose(drawDib_);
}

/**
 *\fn           int send(void *data)
 *\brief        ������Ƶ
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoPlay::send(void *data)
{
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VP_PARAM_NULL;

    DrawDibDraw(drawDib_, dc_, x_, y_, -1, -1, &bmpInfo_, frame->data, 
        0, 0, bmpInfo_.biWidth, bmpInfo_.biHeight, DDF_SAME_DRAW);

    MeidaLayout::send(data);

    return 0;
}

/**
 *\fn           int recv(void *data)
 *\brief        ������Ƶ
 *\param[in]    void * data ��Ƶ����
 *\return       int 0�ɹ�,����ʧ��
 */
int VideoPlay::recv(void *data)
{
    PFRAMEINFO frame = (PFRAMEINFO)data;

    if (NULL == frame || NULL == frame->data || 0 == frame->len) return E_VP_PARAM_NULL;

    DrawDibDraw(drawDib_, dc_, x_, y_, -1, -1, &bmpInfo_, frame->data, 
        0, 0, bmpInfo_.biWidth, bmpInfo_.biHeight, DDF_SAME_DRAW);

    MeidaLayout::recv(data);

    return 0;
}
