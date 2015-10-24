
#pragma once

typedef unsigned char uint8;

#define PI  3.141593
#define EPI 57.29578

/*
 *\fn         int loadBitmap(const char *filename, uint8 *data, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *cx, int *cy)
 *\brief      ����ͼ���ļ�
 *\param[in]  const char * filename �ļ���
 *\param[out] uint8 * data �ļ�����
 *\param[out] BITMAPFILEHEADER * bfh ͼ���ļ�ͷ
 *\param[out] BITMAPINFOHEADER * bih ͼ�����Ϣͷ
 *\param[out] int * cx ͼ���
 *\param[out] int * cy ͼ���
 *\return     int 0�ɹ�,����ʧ��
 */
int loadBitmap(const char *filename, uint8 *data, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *cx, int *cy);

/**
 *\fn           int saveBitmap(const char *filename, const uint8 *data, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, 
                               int cx, int cy, bool color)
 *\brief        ����ͼ���ļ�
 *\param[in]    const char * filename �ļ���
 *\param[in]    const uint8 * data ����
 *\param[in]    BITMAPFILEHEADER * bfh ͼ���ļ�ͷ
 *\param[in]    BITMAPINFOHEADER * bih ͼ�����Ϣͷ
 *\param[in]    int cy ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int color �Ƿ��ǲ�ɫ��
 *\return       int 0�ɹ�,����ʧ��
 */
int saveBitmap(const char *filename, const uint8 *data, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, 
    int cx, int cy, bool color);

/**
 *\fn           void zoomout(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ����СΪԭ����1/4
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ��С���ͼ������
 *\param[in]    int cx ԭͼ��
 *\param[in]    int cy ԭͼ��
 *\return       void ��
 */
void zoomout(uint8 *input, uint8 *output, int cx, int cy);

/**
 *\fn           void getGrads(uint8* input, uint8* output, int cx, int cy, long r)
 *\brief        �õ��ݶȳ�
 *\param[in]    uint8 * input ��������,��Ϊԭͼ��1/4ͼ��
 *\param[out]   uint8 * output �������,�ݶȳ�
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    long r �뾶��
 *\return       void ��
 */
void getGrads(uint8* input, uint8* output, int cx, int cy, long r);
void getGradsFromZoomout(uint8* input, uint8* output, int cx, int cy, long r);
/**
 *\fn           void getOrient(uint8* input, uint8* output, int cx, int cy, long r)
 *\brief        �õ�����
 *\param[in]    uint8 * input ��������,��Ϊԭͼ��1/4ͼ��
 *\param[out]   uint8 * output �������,�ݶȳ�
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    long r �뾶��
 *\return       void ��
 */
void getOrient(uint8 *input, uint8* output, int cx, int cy, long r);
void getOrientFromZoomout(uint8 *input, uint8* output, int cx, int cy, long r);

/**
 *\fn           int getBackgroundTemplate(uint8 *input, uint8 *output, int r, int threshold, int cx, int cy)
 *\brief        ���ɱ���ģ��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ����ģ��:255Ϊ����,0Ϊǰ��
 *\param[in]    int r �Է�ֵͼ��߶�ƽ���˲����˲����뾶
 *\param[in]    int threshold �ָ����ֵ
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       int 0�ɹ�,����ʧ��
 */
int getBackgroundTemplate(uint8 *input, uint8 *output, int r, int threshold, int cx, int cy);

/**
 *\fn           void clearBackground(uint8 *input, uint8 *output, uint8* mask, int cx, int cy)
 *\brief        �������
 *\param[in]    uint8 * input ����ԭͼ��
 *\param[out]   uint8 * output �������ͼ��
 *\param[in]    uint8 * mask ����ģ��
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void clearBackground(uint8 *input, uint8 *output, uint8* mask, int cx, int cy);

/**
 *\fn           void equalize(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ�����
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[out]   uint8 * output ������ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void equalize(uint8 *input, uint8 *output, int cx, int cy);

/**
 *\fn           int getGaussWindowSize(double sigma)
 *\brief        �õ�һά��˹ģ�崰�ڴ�С
 *\n            ���������������һ��һά�ĸ�˹�������������ݣ������ϸ�˹���ݵĳ���Ӧ
 *\n            �������޳��ģ�����Ϊ�˼���ļ򵥺��ٶȣ�ʵ�ʵĸ�˹����ֻ�������޳���
 *\n            ���鳤�ȣ����ݸ����۵�֪ʶ��ѡȡ[-3*sigma, 3*sigma]���ڵ����ݡ�
 *\n            ��Щ���ݻḲ�Ǿ��󲿷ֵ��˲�ϵ��
 *\param[in]    double sigma ��˹�����ı�׼��
 *\return       int ��˹ģ�崰�ڴ�С
 */
int getGaussWindowSize(double sigma);

/**
 *\fn           void makeGauss(double sigma, int nWindowSize, double *output)
 *\brief        ���ɸ�˹ģ��
 *\param[in]    double sigma ��˹�����ı�׼��
 *\param[in]    int nWindowSize ���ڴ�С
 *\param[out]   double * output ��˹ģ��
 *\return       void ��
 */
void makeGauss(double sigma, int nWindowSize, double *output);

/**
 *\fn           void gaussSmooth(uint8 *input, uint8 *output, int cx, int cy, double sigma)
 *\brief        ��˹ƽ��
 *\param[in]    uint8 * input ��������ԭͼ
 *\param[out]   uint8 * output �������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    double sigma ��˹�����ı�׼��
 *\return       void ��
 */
void gaussSmooth(uint8 *input, uint8 *output, int cx, int cy, double sigma);

/**
 *\fn           void smooth(uint8 *input, uint8 *output, int cx, int cy, int r, int d)
 *\brief        ��ֵƽ���˲�
 *\param[in]    uint8 * input Ҫƽ����ͼ�����ݻ�����
 *\param[out]    uint8 * output ƽ�����ͼ�����ݻ�����
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int r ƽ���˲����뾶
 *\param[in]    int d ƽ���˲�������
 *\return       void ��
 */
void smooth(uint8 *input, uint8 *output, int cx, int cy, int r, int d);

/**
 *\fn           int DDIndex(int angle)
 *\brief        ���Ҷ�ֵ��������12������ֵ
 *\param[in]    int angle �Ƕ� ��0 - 180��
 *\return       int 12������ֵ(0-11)
 */
int DDIndex(int angle);

/**
 *\fn           void orientEnhance(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy)
 *\brief        ͨ��������ǿͼ��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[out]   uint8 * output �����ǿ���ͼ������
 *\param[in]    uint8 * orient ��������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void orientEnhance(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy);

/**
 *\fn           void binary(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy)
 *\brief        ��ֵ��ͼ��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[out]   uint8 * output �����ֵ�����ͼ������
 *\param[in]    uint8 * orient ���뷽������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void binary(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy);

/**
 *\fn           void binaryClear(uint8 *input, uint8 *output, uint8 *mask, int cx, int cy)
 *\brief        �Զ�ֵ��ָ��ͼ�����ȥ����,ȥ��������
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ���ȥ������ͼ������
 *\param[in]    uint8 * mask ����ģ��
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void binaryClear(uint8 *input, uint8 *output, uint8 *mask, int cx, int cy);

/**
 *\fn           void thin(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ��ϸ��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ���ϸ�����ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void thin(uint8 *input, uint8 *output, int cx, int cy);

/**
 *\fn           void thinClear(uint8 *input, uint8 *output, int cx, int cy, int len)
 *\brief        ���ϸ��ͼ���ж̰���ë��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ���������ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int len �̰���ë�̵���󳤶�
 *\return       void ��
 */
void thinClear(uint8 *input, uint8 *output, int cx, int cy, int len);

/**
 *\fn           bool isEnd(uint8 *input, int cx)
 *\brief        �жϵ�ǰ���Ƿ�Ϊ�˵�
 *\param[in]    uint8 * input ��ǰ��
 *\param[in]    int cx ͼ���
 *\return       bool �Ƿ�Ϊ�˵�
 */
bool isEnd(uint8 *input, int cx);

/**
 *\fn           bool isFork(uint8 *input, int cx)
 *\brief        ĳ���Ƿ�Ϊ���
 *\param[in]    uint8 * input ĳ��
 *\param[in]    int cx ͼ���
 *\return       bool �Ƿ�Ϊ���
 */
bool isFork(uint8 *input, int cx);

/**
 *\fn           int getNextPt(uint8 *pt, uint8 *last, uint8 **next, int cx, int cy)
 *\brief        �õ����ߵ���һ����
 *\param[in]    uint8 * pt ��ǰ��ĵ�ַ
 *\param[in]    uint8 * last ǰ����ĵ�ַ
 *\param[in]    uint8 * * next ��һ����ĵ�ַ��ָ��
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       bool �Ƿ��ҵ���̵�
 */
bool getNextPt(uint8 *pt, uint8 *last, uint8 **next, int cx, int cy);
