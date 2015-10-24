#include "stdafx.h"
#include "FingerImageProcess.h"
#include <math.h>


// ���ݷ�������12�����������,
// ����12��������ȡ7����,ÿ��������λ��
static const int g_DDSite[12][7][2] =
{
    -3, 0,  -2, 0,  -1, 0,   0, 0,   1, 0,   2, 0,   3, 0,
    -3,-1,  -2,-1,  -1, 0,   0, 0,   1, 0,   2, 1,   3, 1,
    -3,-2,  -2,-1,  -1,-1,   0, 0,   1, 1,   2, 1,   3, 2,
    -3,-3,  -2,-2,  -1,-1,   0, 0,   1, 1,   2, 2,   3, 3,
    -2,-3,  -1,-2,  -1,-1,   0, 0,   1, 1,   1, 2,   2, 3,
    -1,-3,  -1,-2,   0,-1,   0, 0,   0, 1,   1, 2,   1, 3,
     0,-3,   0,-2,   0,-1,   0, 0,   0, 1,   0, 2,   0, 3,
    -1, 3,  -1, 2,   0, 1,   0, 0,   0,-1,   1,-2,   1,-3,
    -2, 3,  -1, 2,  -1, 1,   0, 0,   1,-1,   1,-2,   2,-3,
    -3, 3,  -2, 2,  -1, 1,   0, 0,   1,-1,   2,-2,   3,-3,
    -3, 2,  -2, 1,  -1, 1,   0, 0,   1,-1,   2,-1,   3,-2,
    -3, 1,  -2, 1,  -1, 0,   0, 0,   1, 0,   2,-1,   3,-1
};

/**
 *\fn           void zoomout(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ����СΪԭ����1/4
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ��С���ͼ������
 *\param[in]    int cx ԭͼ��
 *\param[in]    int cy ԭͼ��
 *\return       void ��
 */
void zoomout(uint8 *input, uint8 *output, int cx, int cy)
{
    int x = 0;
    int y = 0;
    int sum = 0;
    int temp1 = 0;
    int temp2 = 0;
    int SiteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};
    uint8 *src = NULL;
    uint8 *dst = NULL;

    // ��Ե����
    temp2 = cx / 2;
    for (y = 0; y < cy; y += 2)
    {
        src = input + y*cx;
        dst = output + (y/2)*temp2;
        *dst = *src;

        src = input + y*cx + cx - 1;
        dst = output + (y/2)*temp2 + (temp2 - 1);
        *dst = *src;
    }

    temp1 = (cy-1)*cx;
    temp2 = (cy/2-1)*(cx/2);
    for (x = 0; x < cx; x += 2)
    {
        src = input + x;
        dst = output + x/2;
        *dst = *src;

        src = input + x + temp1;
        dst = output + x/2 + temp2;
        *dst = *src;
    }

    // �Ǳ�Ե�����ø�˹ģ����ȡ��Ƶ��Ϣ
    for (y = 2; y < cy-2; y += 2)
    {
        for (x = 2; x < cx-2; x += 2)
        {
            src = input + y*cx + x;
            dst = output + (y/2)*(cx/2) + x/2;

            // ��˹ģ��
            // 1 2 1
            // 2 4 2
            // 1 2 1
            sum = *src*4 + *(src + SiteD8[0]) +
                *(src + SiteD8[1])*2 + *(src + SiteD8[2]) +
                *(src + SiteD8[3])*2 + *(src + SiteD8[4]) +
                *(src + SiteD8[5])*2 + *(src + SiteD8[6]) +
                *(src + SiteD8[7])*2;

            *dst = (uint8)(sum >> 4);
        }
    }
}

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
void getGradsFromZoomout(uint8* input, uint8* output, int cx, int cy, long r)
{
    long x = 0;
    long y = 0;
    long i = 0;
    long j = 0;
    long vx = 0;
    long vy = 0;
    long lvx = 0;
    long lvy = 0;;

    uint8 *src = NULL; // ԭ����
    uint8 *dst = NULL; // Ŀ¼����

    int grad = 0;       // �ݶ�
    int gradSum = 0;    // �ݶȺ�
    int gradNum = 0;    // �ݶȼ���

    for (y = 0; y < cy / 2; y++)
    {
        for (x = 0; x < cx / 2; x++)
        {
            lvx = 0;
            lvy = 0;
            gradNum = 0;
            gradSum = 0;

            for (i = -r; i <= r; i++) // Ϊ����ٶȣ�����Ϊ2
            {
                if ((y+i) < 1 || (y+i) >= (cy/2-1)) continue;

                for (j = -r; j <= r; j++) // Ϊ����ٶȣ�����Ϊ2
                {
                    if ((x+j) < 1 || (x+j) >= (cx/2-1)) continue;

                    src = input + (y+i)*(cx/2) + x+j;

                    // ��x����ƫ��
                    // -1 0 1
                    // -2 0 2
                    // -1 0 1
                    vx = *(src + cx/2 + 1) - *(src + cx/2 - 1) +
                        *(src + 1)*2 - *(src - 1)*2 +
                        *(src - cx/2 + 1) - *(src - cx/2 - 1);

                    // ��y����ƫ��
                    // -1 -2 -1
                    //  0  0  0
                    //  1  2 -1
                    vy = *(src + cx/2 - 1) - *(src - cx/2 - 1) +
                        *(src + cx/2)*2 - *(src - cx/2)*2 +
                        *(src + cx/2 + 1) - *(src - cx/2 + 1);

                    gradSum += (abs(vx)+abs(vy));
                    gradNum++;
                }
            }

            if (gradNum == 0) gradNum = 1;

            grad = gradSum / gradNum;

            if (grad > 255) grad = 255;

            dst = output + 2*y*cx + 2*x;

            *(dst) = (uint8)grad;
            *(dst + 1) = (uint8)grad;
            *(dst + cx) = (uint8)grad;
            *(dst + cx + 1) = (uint8)grad;
        }
    }
}

void getGrads(uint8* input, uint8* output, int cx, int cy, long r)
{
}

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
void getOrientFromZoomout(uint8 *input, uint8* output, int cx, int cy, long r)
{
    int x = 0;
    int y = 0;
    int i = 0;
    int j = 0;
    long vx = 0;
    long vy = 0;
    long lvx = 0;
    long lvy = 0;
    long num = 0;
    long angle = 0;
    double fAngle = 0.0;
    uint8 *src = NULL;
    uint8 *dst = NULL;

    for (y = 0; y < cy / 2; y++)
    {
        for (x = 0; x < cx / 2; x++)
        {
            lvx = 0;
            lvy = 0;
            num = 0;

            for (i = -r; i <= r; i++) // Ϊ����ٶȣ�����Ϊ2
            {
                if ((y+i) < 1 || (y+i) >= (cy/2-1)) continue;

                for (j = -r; j <= r; j++) // Ϊ����ٶȣ�����Ϊ2
                {
                    if ((x+j) < 1 || (x+j) >= (cx/2-1)) continue;

                    src = input + (y+i)*(cx/2) + x+j;

                    //��x����ƫ��
                    // -1 0 1
                    // -2 0 2
                    // -1 0 1
                    vx = *(src + cx/2 + 1) - *(src + cx/2 - 1) +
                        *(src + 1)*2 - *(src - 1)*2 +
                        *(src - cx/2 + 1) - *(src - cx/2 - 1);

                    //��y����ƫ��
                    // -1 -2 -1
                    //  0  0  0
                    //  1  2 -1
                    vy = *(src + cx/2 - 1) - *(src - cx/2 - 1) +
                        *(src + cx/2)*2 - *(src - cx/2)*2 +
                        *(src + cx/2 + 1) - *(src - cx/2 + 1);

                    lvx += vx * vy * 2;//sin(2sita)
                    lvy += vx*vx - vy*vy;//cos(2sita)
                    num++;
                }
            }

            if (num == 0) num = 1;

            // �󻡶�(-pi - pi)
            fAngle = atan2((double)lvy, (double)lvx);

            // �任��(0 - 2*pi)
            if(fAngle < 0) fAngle += 2*PI;

            // �ɻ���ת�ɽǶ�,�����߽Ƕ�
            angle = (long)(fAngle*EPI*0.5 + 0.5);

            // ��Ϊ����sobel���ӣ����ԽǶ�ƫת�˶ȣ�����Ҫ��ת��õĽǶ�
            angle -= 135;

            // �Ƕȱ任����-180��
            if(angle <= 0) angle += 180;

            angle = 180-angle;

            // �������߽Ƕ�
            dst = output + 2 * y * cx + 2 * x;

            *(dst) = (uint8)angle;
            *(dst + 1) = (uint8)angle;
            *(dst + cx) = (uint8)angle;
            *(dst + cx + 1) = (uint8)angle;
        }
    }
}

void getOrient(uint8 *input, uint8* output, int cx, int cy, long r)
{
    int x = 0;
    int y = 0;
    int i = 0;
    int j = 0;
    long vx = 0;
    long vy = 0;
    long lvx = 0;
    long lvy = 0;
    long num = 0;
    long angle = 0;
    double fAngle = 0.0;
    uint8 *src = NULL;
    uint8 *dst = NULL;

    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            lvx = 0;
            lvy = 0;
            num = 0;

            for (i = -r; i <= r; i++)
            {
                if ((y+i) < 1 || (y+i) >= (cy-1)) continue;

                for (j = -r; j <= r; j++)
                {
                    if ((x+j) < 1 || (x+j) >= (cx-1)) continue;

                    src = input + (y+i)*cx + x+j;

                    //��x����ƫ��
                    // -1 0 1
                    // -2 0 2
                    // -1 0 1
                    vx = *(src + cx + 1) - *(src + cx - 1) +
                        *(src + 1)*2 - *(src - 1)*2 +
                        *(src - cx + 1) - *(src - cx - 1);

                    //��y����ƫ��
                    // -1 -2 -1
                    //  0  0  0
                    //  1  2 -1
                    vy = *(src + cx - 1) - *(src - cx - 1) +
                        *(src + cx)*2 - *(src - cx)*2 +
                        *(src + cx + 1) - *(src - cx + 1);

                    lvx += vx * vy * 2;//sin(2sita)
                    lvy += vx*vx - vy*vy;//cos(2sita)
                    num++;
                }
            }

            if (num == 0) num = 1;

            // �󻡶�(-pi - pi)
            fAngle = atan2((double)lvy, (double)lvx);

            // �任��(0 - 2*pi)
            if(fAngle < 0) fAngle += 2*PI;

            // �ɻ���ת�ɽǶ�,�����߽Ƕ�
            angle = (long)(fAngle*EPI*0.5 + 0.5);

            // ��Ϊ����sobel���ӣ����ԽǶ�ƫת�˶ȣ�����Ҫ��ת��õĽǶ�
            angle -= 135;

            // �Ƕȱ任����-180��
            if(angle <= 0) angle += 180;

            angle = 180-angle;

            // �������߽Ƕ�
            dst = output + y * cx + x;

            *(dst) = (uint8)angle;
//             *(dst + 1) = (uint8)angle;
//             *(dst + cx) = (uint8)angle;
//             *(dst + cx + 1) = (uint8)angle;
        }
    }
}

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
int getBackgroundTemplate(uint8 *input, uint8 *output, int r, int threshold, int cx, int cy)
{
    int x = 0;
    int y = 0;
    int num = 0;
    int lineBegin= 0;
    uint8 *src = NULL;

    // �Է��򳡷�ֵͼ����и߶�ƽ���˲�
    smooth(input, output, cx, cy, r, 2);

    // ͼ���Ե������Ϊ����

    num = cx - 1;

    for (y = 0; y < cy; y++)
    {
        src = output + y*cx;

        *(src) = 255;
        *(src + num) = 255;
    }

    num = (cy-1)*cx;

    for (x = 0; x < cx; x++)
    {
        src = output + x;

        *(src) = 255;
        *(src + num) = 255;
    }

    num = 0;
    lineBegin = cx;

    for (y = 1; y < cy-1; y++)
    {
        for (x = 1; x < cx-1; x++)
        {
            src = output + lineBegin + x;

            // ���ݷ�ֵ����ֵ��С�ж��Ƿ�Ϊ��������
            if (*src < threshold)
            {
                *src = 0;
            }
            else
            {
                *src = 255;
                num++;
            }
        }

        lineBegin += cx;
    }

    // ���ǰ���������С���������ʮ��֮һ�����ʾǰ������̫С�����ش���
    return ((num < (cx * cy / 10)) ? 1 : 0);
}

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
void clearBackground(uint8 *input, uint8 *output, uint8* mask, int cx, int cy)
{
    int x = 0;
    int y = 0;
    int pos = 0;
    int lineBegin = 0;

    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            pos = lineBegin + x;

            // ����Ǳ����������øõ�Ϊ�׵�
            if (*(mask + pos) == 0)
            {
                *(output + pos) = 255;
            }
            else
            {
                *(output + pos) = *(input + pos);
            }
        }

        lineBegin += cx;
    }
}

/**
 *\fn           void equalize(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ�����
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[out]   uint8 * output ������ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void equalize(uint8 *input, uint8 *output, int cx, int cy)
{
    // �Ҷȼ�����
    int lCount[256] = {0};

    // �Ҷ�ӳ���
    uint8 bMap[256] = {0};

    // ָ��Դͼ���ָ��
    uint8* src = NULL;
    uint8* dst = NULL;

    // ��ʱ����
    int lTemp = 0;
    int lineBegin = 0;

    // ѭ������
    int i = 0;
    int j = 0;
    int x = 0;
    int y = 0;

    // ��������Ҷ�ֵ�ļ���
    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            src = input + lTemp + x;

            // ������1
            lCount[*(src)]++;
        }

        lineBegin += cx;
    }

    // ����Ҷ�ӳ���
    for (i = 0; i < 256; i++)
    {
        // ��ʼΪ0
        lTemp = 0;

        for (j = 0; j <= i; j++)
        {
            lTemp += lCount[j];
        }

        // �����Ӧ���»Ҷ�ֵ
        bMap[i] = (uint8)(lTemp * 255 / cy / cx);
    }

    lineBegin = 0;

    // �����µ�ͼ��
    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            src = input + lineBegin + x;
            dst = output + lineBegin + x;

            // �����µĻҶ�ֵ
            *dst = bMap[*src];
        }

        lineBegin += cx;
    }
}

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
int getGaussWindowSize(double sigma)
{
    return (int)(1 + 2 * ceil(3 * sigma));
}

/**
 *\fn           void makeGauss(double sigma, int nWindowSize, double *output)
 *\brief        ���ɸ�˹ģ��
 *\param[in]    double sigma ��˹�����ı�׼��
 *\param[in]    int nWindowSize ���ڴ�С
 *\param[out]   double * output ��˹ģ��
 *\return       void ��
 */
void makeGauss(double sigma, int nWindowSize, double *output)
{
    // ��������ĵ�
    int nCenter = nWindowSize / 2;

    // �����ĳһ�㵽���ĵ�ľ���
    double dDis = 0.0;

    // �м����
    double dSum = 0.0;
    double dValue = 0.0;

    // ��ʱ����
    int i = 0;

    // ����һά��˹��������ģ��
    for (i = 0; i < nWindowSize; i++)
    {
        dDis = (double)(i - nCenter);
        dValue = exp(-(1/2)*dDis*dDis/(sigma*sigma)) / (sqrt(2 * PI) * sigma);
        output[i] = dValue;
        dSum += dValue;
    }

    // ��һ��
    for (i = 0; i < nWindowSize; i++)
    {
        output[i] /= dSum;
    }
}

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
void gaussSmooth(uint8 *input, uint8 *output, int cx, int cy, double sigma)
{
    // ��˹�˲��������鳤��
    int nWindowSize = getGaussWindowSize(sigma);

    // ���ڳ��ȵ�1/2
    int nHalfLen = nWindowSize / 2;

    // һά��˹�����˲���
    double *pdKernel = new double[nWindowSize];

    // ��˹ϵ����ͼ�����ݵĵ��
    double dDotMul = 0.0;

    // ��˹�˲�ϵ�����ܺ�
    double dWeightSum = 0.0;

    // �м����
    double *pdTmp = new double[cx*cy];

    // ��ʱ����
    int x = 0;
    int y = 0;
    int i = 0;

    // ����һά��˹�����˲���
    makeGauss(sigma, nWindowSize, pdKernel);

    // x��������˲�
    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            dDotMul = 0;
            dWeightSum = 0;

            for (i = (-nHalfLen); i <= nHalfLen; i++)
            {
                // �ж��Ƿ���ͼ���ⲿ
                if ((x+i) < 0 || (x+i) >= cx) continue;

                dDotMul += (double)input[y*cx + (x+i)] * pdKernel[nHalfLen+i];
                dWeightSum += pdKernel[nHalfLen+i];
            }

            pdTmp[y*cx + x] = dDotMul/dWeightSum;
        }
    }

    // y��������˲�
    for (x = 0; x < cx; x++)
    {
        for (y = 0; y < cy; y++)
        {
            dDotMul = 0;
            dWeightSum = 0;

            for (i = (-nHalfLen); i <= nHalfLen; i++)
            {
                // �ж��Ƿ���ͼ���ⲿ
                if ((y+i) < 0 || (y+i) >= cy) continue;
                
                dDotMul += (double)pdTmp[(y+i)*cx + x] * pdKernel[nHalfLen+i];
                dWeightSum += pdKernel[nHalfLen+i];
            }

            output[y*cx + x] = (BYTE)(int)(dDotMul / dWeightSum);
        }
    }

    // �ͷ��ڴ�
    delete[] pdKernel;
    pdKernel = NULL;

    delete[] pdTmp;
    pdTmp = NULL;
}

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
void smooth(uint8 *input, uint8 *output, int cx, int cy, int r, int d)
{
    int x = 0;
    int y = 0;
    int i = 0;
    int j = 0;
    int sum = 0;
    int num = 0;
    uint8 *src = NULL;
    uint8 *dst = NULL;

    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            sum = 0;
            num = 0;
            src = input + y*cx + x;
            dst = output + y*cx + x;

            for (i = -r; i <= r; i += d)
            {
                if ((y+i) < 0 || (y+i) >= cy) continue;

                for (j = -r; j <= r; j += d)
                {
                    if ((x+j) < 0 || (x+j) >= cx) continue;

                    sum += *(src + i*cx + j);
                    num++;
                }
            }

            *dst = (uint8)(sum/num);
        }
    }
}

/**
 *\fn           int DDIndex(int angle)
 *\brief        ���Ҷ�ֵ��������12������ֵ
 *\param[in]    int angle �Ƕ� ��0 - 180��
 *\return       int 12������ֵ(0-11)
 */
int DDIndex(int angle)
{
    if (angle >= 173 || angle < 8)
    {
        return 0;
    }
    else
    {
        return ((angle-8)/15 + 1);
    }
}

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
void orientEnhance(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy)
{
    int Hw[7] = {1, 1, 1, 1, 1, 1, 1}; // ���߷����Ͻ���ƽ���˲���ƽ���˲���
    int Vw[7] = {-3, -1, 3, 9, 3, -1, -3}; // ���߷���Ĵ�ֱ�����Ͻ������˲������˲���
    uint8 *src = NULL;
    uint8 *dst = NULL;
    int x = 0;
    int y = 0;
    int i = 0;
    int d = 0;
    int sum = 0;
    int hsum = 0;
    int vsum = 0;
    int lineBegin = 0;
    uint8 *temp = new uint8[cx * cy];

    // ���߷����Ͻ���ƽ���˲�
    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            sum = 0;
            hsum = 0;
            src = input + lineBegin + x;
            d = DDIndex(*(orient + lineBegin + x)); // ���߷��������

            for (i = 0; i < 7; i++)
            {
                if (y+g_DDSite[d][i][1] < 0 || y+g_DDSite[d][i][1] >= cy ||
                    x+g_DDSite[d][i][0] < 0 || x+g_DDSite[d][i][0] >= cx)
                {
                    continue;
                }

                sum += Hw[i] * (*(src + g_DDSite[d][i][1]*cx + g_DDSite[d][i][0]));
                hsum += Hw[i];
            }

            if (hsum != 0)
            {
                *(temp + lineBegin + x) = (uint8)(sum / hsum);
            }
            else
            {
                *(temp + lineBegin + x) = 255;
            }
        }

        lineBegin += cx;
    }

    // ���߷���Ĵ�ֱ�����Ͻ������˲�
    lineBegin = 0;
    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            sum = 0;
            vsum = 0;
            src = temp + lineBegin + x;
            d = (DDIndex(*(orient + lineBegin + x))+6) % 12; // ���߷���Ĵ�ֱ���������

            for (i = 0; i < 7; i++)
            {
                if (y+g_DDSite[d][i][1] < 0 || y+g_DDSite[d][i][1] >= cy ||
                    x+g_DDSite[d][i][0] < 0 || x+g_DDSite[d][i][0] >= cx)
                {
                    continue;
                }

                sum += Vw[i] * (*(src + g_DDSite[d][i][1]*cx + g_DDSite[d][i][0]));
                vsum += Vw[i];
            }

            if (vsum > 0)
            {
                sum /= vsum;

                if (sum > 255)
                {
                    *(output + lineBegin + x) = 255;
                }
                else if (sum < 0)
                {
                    *(output + lineBegin + x) = 0;
                }
                else
                {
                    *(output + lineBegin + x) = (uint8)sum;
                }
            }
            else
            {
                *(output + lineBegin + x) = 255;
            }
        }

        lineBegin += cx;
    }

    delete[] temp;
}

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
void binary(uint8 *input, uint8 *output, uint8 *orient, int cx, int cy)
{
    int Hw[7] = {2, 2, 3, 4, 3, 2, 2}; // ���߷����ϵ�7�����Ȩֵ
    int Vw[7] = {1, 1, 1, 1, 1, 1, 1}; // ���߷���Ĵ�ֱ�����ϵ�7�����Ȩֵ
    int hsum = 0;   // ���߷����ϵ�7����ļ�Ȩ��
    int vsum = 0;   // ���߷���Ĵ�ֱ�����ϵ�7����ļ�Ȩ��
    int Hv = 0;     // ���߷����ϵ�7����ļ�Ȩƽ��ֵ
    int Vv = 0;     // ���߷���Ĵ�ֱ�����ϵ�7����ļ�Ȩƽ��ֵ
    uint8  *src = NULL; // ָ��ͼ�����ص�ָ��
    uint8  *dst = NULL; // ���߷���ָ��
    int x = 0;
    int y = 0;
    int i = 0;
    int d = 0;
    int sum = 0;
    int lineBegin = 0;

    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            src = input + lineBegin + x;

            // ����õ�ǳ��ڣ�������ʱ���������øõ�Ϊ�ڵ㣬ֵΪ0
            if (*src < 4)
            {
                *(output + lineBegin + x) = 0;
                continue;
            }

            // ���㷽������������Ϊ12������
            d = DDIndex(*(orient + lineBegin + x));

            // ���㵱ǰ�������߷����ϵļ�Ȩƽ��ֵ
            sum = 0;
            hsum = 0;

            for (i = 0; i < 7; i++)
            {
                // �����Ƿ�Խ��
                if (y+g_DDSite[d][i][1] < 0 || y+g_DDSite[d][i][1] >= cy ||
                    x+g_DDSite[d][i][0] < 0 || x+g_DDSite[d][i][0] >= cx)
                {
                    continue;
                }

                sum += Hw[i] * (*(src + g_DDSite[d][i][1]*cx + g_DDSite[d][i][0]));
                hsum += Hw[i];
            }

            Hv = (hsum != 0) ? (sum / hsum) : 255;

            // ���߷���Ĵ�ֱ���������
            d = (d+6)%12;

            // ���㵱ǰ�������߷���Ĵ�ֱ�����ϵļ�Ȩƽ��ֵ
            sum = 0;
            vsum = 0;

            for (i = 0; i < 7; i++)
            {
                if (y+g_DDSite[d][i][1] < 0 || y+g_DDSite[d][i][1] >= cy ||
                    x+g_DDSite[d][i][0] < 0 || x+g_DDSite[d][i][0] >= cx)
                {
                    continue;
                }

                sum += Vw[i] * (*(src + g_DDSite[d][i][1]*cx + g_DDSite[d][i][0]));
                vsum += Vw[i];
            }

            Vv = (vsum != 0) ? (sum / vsum) : 255;

            dst = output + lineBegin + x;

            // ���߷����ϼ�Ȩƽ��ֵ��С���õ�ǰ��Ϊ�ڵ�,�ϴ����õ�ǰ��Ϊ�׵�
            *dst = (Hv < Vv) ? 0 : 255;
        }

        lineBegin += cx;
    }
}

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
void binaryClear(uint8 *input, uint8 *output, uint8 *mask, int cx, int cy)
{
    int x = 0;
    int y = 0;
    int i = 0;
    int n = 0;
    int num = 0;
    int lineBegin = 0;
    bool working = true;
    uint8 *src = NULL;
    uint8 *dst = NULL;

    // ĳ����Χ8����ĵ�ַƫ��
    int SiteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};

    // ��ʼ����ʱ������
    memset(output, 0xFF, cx*cy);

    // ѭ������ֱ��������ϻ��ߴ����������8
    while (working && n < 8)
    {
        n++;
        working = false;
        lineBegin = cx;

        for (y = 1; y < cy-1; y++)
        {
            for (x = 1; x < cx-1; x++)
            {
                // �����ĵ㲻����
                if ((NULL != mask) && (*(mask + lineBegin + x) == 0)) continue;

                // ͳ�Ƶ�ǰ����Χ������ͬ���͵�ĸ���
                num = 0;
                src = input + lineBegin + x;

                for (i = 0; i < 8; i++)
                {
                    if (*(src+SiteD8[i]) == *src)
                    {
                        num++;
                    }
                }

                // ��ͬ�����С�ڶ���ı䵱ǰ������
                dst = output + lineBegin + x;

                if (num < 2)
                {
                    *dst = 255 - *src;
                    working = true;
                }
                else
                {
                    *dst = *src;
                }
            }

            lineBegin += cx;
        }
    }
}

/**
 *\fn           void thin(uint8 *input, uint8 *output, int cx, int cy)
 *\brief        ͼ��ϸ��
 *\param[in]    uint8 * input ����ԭͼ������
 *\param[in]    uint8 * output ���ϸ�����ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\return       void ��
 */
void thin(uint8 *input, uint8 *output, int cx, int cy)
{
    uint8 eraseTable[256] =
    {
        0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
        0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
        1,1,0,0,1,1,0,0,             0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,             0,0,0,0,0,0,0,0,
        1,1,0,0,1,1,0,0,             1,1,0,1,1,1,0,1,
        0,0,0,0,0,0,0,0,             0,0,0,0,0,0,0,0,
        0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
        0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
        1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,0,
        1,1,0,0,1,1,0,0,             0,0,0,0,0,0,0,0,
        1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,0,
        1,1,0,0,1,1,0,0,             1,1,0,1,1,1,0,0,
        1,1,0,0,1,1,1,0,             1,1,0,0,1,0,0,0
    };

    int     x = 0;
    int     y = 0;
    int     num = 0;
    int     lineBegin = cx;
    bool    finished = false;
    uint8   nw,n,ne,w,e,sw,s,se;
    uint8   *src = NULL;
    uint8   *dst = NULL;
    uint8   *buffer = new uint8[cx*cy];

    memcpy(buffer, input, cx*cy);
    memcpy(output, input, cx*cy);

    while (!finished) // ��û�н���
    {
        // ������־�óɼ�
        finished = true;
        lineBegin = 0;

        // �Ƚ���ˮƽ�����ϸ��
        for (y = 1; y < (cy-1); y++) //ע��Ϊ��ֹԽ�磬y�ķ�Χ��1���߶�-2
        {
            for (x = 1; x < (cx-1); ) // ע��Ϊ��ֹԽ�磬x�ķ�Χ��1�����-2
            {
                src = buffer + lineBegin + x;
                dst = output + lineBegin + x;

                if (*src == 0) // �Ǻڵ��������
                {
                    w = *(src - 1); // ���ڵ�
                    e = *(src + 1); // ���ڵ�

                    if ((w == 255)|| (e == 255)) // ���������ھ���������һ���ǰ׵�Ŵ���
                    {
                        nw = *(src+cx-1); // �����ڵ�
                        n  = *(src+cx);   // ���ڵ�
                        ne = *(src+cx+1); // �����ڵ�
                        sw = *(src-cx-1); // �����ڵ�
                        s  = *(src-cx);   // ���ڵ�
                        se = *(src-cx+1); // �����ڵ�

                        // ��������
                        num = nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+
                            sw/255*32+s/255*64+se/255*128;

                        if (eraseTable[num] == 1) // ���������ɾ��
                        {
                            *src = 255; // ��ԭͼ�������н��úڵ�ɾ��
                            *dst = 255; // ���ͼ�иúڵ�Ҳɾ��
                            finished = false; // �иĶ���������־�óɼ�
                            x++; // ˮƽ��������һ������
                        }
                    }
                }

                x++; // ɨ����һ������
            }

            lineBegin += cx;
        }

        lineBegin = 0;

        // �ٽ��д�ֱ�����ϸ��
        for (x = 1; x < (cx-1); x++)
        {
            for (y = 1; y < (cy-1);)
            {
                src = buffer + lineBegin + x;
                dst = output + lineBegin + x;

                if (*src == 0) // �Ǻڵ��������
                {
                    n = *(src + cx); // ���ڵ�
                    s = *(src - cx); // ���ڵ�

                    if ((n == 255) || (s == 255)) // ������������ھ���������һ���ǰ׵�Ŵ���
                    {
                        nw = *(src+cx-1); // �����ڵ�
                        ne = *(src+cx+1); // �����ڵ�
                        w  = *(src-1);    // ���ڵ�
                        e  = *(src+1);    // ���ڵ�
                        sw = *(src-cx-1); // �����ڵ�
                        se = *(src-cx+1); // �����ڵ�

                        // ��������
                        num = nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+
                            sw/255*32+s/255*64+se/255*128;

                        if (eraseTable[num] == 1) // ���������ɾ��
                        {
                            *src = 255; // ��ԭͼ�������н��úڵ�ɾ��
                            *dst = 255; // ���ͼ�иúڵ�Ҳɾ��
                            finished = false;   // �иĶ���������־�óɼ�
                            y++;                // ��ֱ��������һ������
                        }
                    }
                }

                y++; // ɨ����һ������
            }

            lineBegin += cy;
        }
    }

    delete[] buffer;
}

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
void thinClear(uint8 *input, uint8 *output, int cx, int cy, int len)
{
    int SiteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};
    int x = 0;
    int y = 0;
    int i = 0;
    int n = 0;
    int lineBegin = 0;
    uint8 *linePt[25] = {0}; // ���߸�����
    uint8 *temp[8] = {0}; // ĳ����ڵ�
    uint8 *pt = NULL;
    uint8 *last = NULL;
    uint8 *next = NULL;
    uint8 *src = NULL;

    memcpy(output, input, cx*cy);

    for (y = 0; y < cy; y++)
    {
        for (x = 0; x < cx; x++)
        {
            src = output + lineBegin + x;

            if (*src != 0) continue;

            n = 0;
            linePt[0] = src;

            // ͳ�Ƶ�ǰ�����Χ�ڵ����
            for (i = 0; i < 8; i++)
            {
                pt = src + SiteD8[i];

                if (*pt == 0)
                {
                    temp[n] = pt;
                    n++;
                }
            }

            // �ڵ����Ϊ�㣬��ʾ��ǰ���ǹµ㣬��Ϊ��ɫ
            if (n == 0)
            {
                *src = 255;
                continue;
            }
            else if (n == 1) // �ڵ����Ϊ1����ʾΪ�˵�
            {
                n = 0;
                last = src;
                pt = temp[0];

                // �����߸���len����
                for (i = 0; i < len; i++)
                {
                    // ����������������ѭ��
                    if (isFork(pt, cx))
                    {
                        break;
                    }

                    n++;
                    linePt[n] = pt;

                    if (getNextPt(pt, last, &next, cx, cy))
                    {
                        last = pt;
                        pt = next;
                    }
                    else // ��������쳣����ѭ��
                    {
                        break;
                    }
                }

                // ���߽϶̣���ʾΪ�̰�������ë��
                if (n < len)
                {
                    for(i = 0; i <= n; i++)
                    {
                        *linePt[i] = 255;
                    }
                }
            }
        }

        lineBegin += cx;
    }
}

/**
 *\fn           bool isEnd(uint8 *input, int cx)
 *\brief        �жϵ�ǰ���Ƿ�Ϊ�˵�
 *\param[in]    uint8 * input ��ǰ��
 *\param[in]    int cx ͼ���
 *\return       bool �Ƿ�Ϊ�˵�
 */
bool isEnd(uint8 *input, int cx)
{
    int i = 0;
    int sum = 0;
    int siteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};// ĳ����Χ8����ĵ�ַƫ��

    // 8������������������Ĳ�ľ���ֵ�ĺ����Ϊ2*255��Ϊ�˵�
    for(i = 0; i < 8; i++)
    {
        sum += abs(*(input + siteD8[(i+1)%8]) - *(input + siteD8[i]));
    }

    return (sum == 255*2);
}

/**
 *\fn           bool isFork(uint8 *input, int cx)
 *\brief        ��ǰ���Ƿ�Ϊ���
 *\param[in]    uint8 * input ��ǰ������
 *\param[in]    int cx ͼ���
 *\return       bool �Ƿ�Ϊ���
 */
bool isFork(uint8 *input, int cx)
{
    int i = 0;
    int sum = 0;
    int siteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1}; // ĳ����Χ8����ĵ�ַƫ��

    // 8������������������Ĳ�ľ���ֵ�ĺ����Ϊ6*255��Ϊ���
    for (i = 0; i < 8; i++)
    {
        sum += abs(*(input + siteD8[(i+1)%8]) - *(input + siteD8[i]));
    }

    return (sum == 255*6);
}

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
bool getNextPt(uint8 *pt, uint8 *last, uint8 **next, int cx, int cy)
{
    int siteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};
    int i = 0;
    int n = 0;
    uint8 *src = NULL;
    uint8 *temp[8] = {0}; // ĳ����Χ8����ĵ�ַ

    *next = NULL;

    // �ҵ���ǰ����Χ���Ǻڵ㲢�Ҳ���ǰ����ĺڵ㣬���浽����
    for (i = 0; i < 8; i++)
    {
        src = pt + siteD8[i];

        if (*src == 0 && src != last)
        {
            temp[n] = src;
            n++;
        }
    }

    if (n == 1) // �ҵ���һ��������Ϊ��̵�
    {
        *next = temp[0];
        return true;
    }

    // û���ҵ������ڵ����ʾû�к�̵�
    // ���ҵ�������򷵻ش���
    return false;
}

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
int loadBitmap(const char *filename, uint8 *data, BITMAPFILEHEADER *bfh, BITMAPINFOHEADER *bih, int *cx, int *cy)
{
    if (NULL == filename || NULL == bfh || NULL == bih || NULL == cx || NULL == cy) return -1;

    // ���ļ�
    FILE *fp = fopen(filename, "rb");

    if (NULL == fp) return -2;

    // ���ļ�ͷ
    fread((void*)bfh, sizeof(BITMAPFILEHEADER), 1, fp);
    fread((void*)bih, sizeof(BITMAPINFOHEADER), 1, fp);

    // ����Ƿ�ΪBMP�ļ�
    if (bfh->bfType != 19778)
    {
        fclose(fp);
        return -3;
    }

    // ����Ƿ�Ϊ256ɫ�Ҷ�ͼ��
    if (bih->biBitCount != 8 && bih->biClrUsed != 256)
    {
        fclose(fp);
        return -4;
    }

    // �õ�ͼ���С
    *cx = bih->biWidth;
    *cy = bih->biHeight;

    // �õ�һ�е��ֽ���,4���ֽڶ���
    int lineBytes = (bih->biWidth + 3) / 4 * 4;

    // ������ָ�벻Ϊ�����ȡͼ�����ݵ�������
    if (NULL == data)
    {
        fclose(fp);
        return -5;
    }

    // ���ж�ȡ��ÿ��ֻ��ȡcx���ֽ�
    for (int i = 0; i < bih->biHeight; i++)
    {
        fseek(fp, 1078+lineBytes*i, SEEK_SET);
        fread((void*)(data+i*bih->biWidth), 1, bih->biWidth, fp);
    }

    fclose(fp);

    return 0;
}

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
    int cx, int cy, bool color)
{
    if (NULL == filename || NULL == data || NULL == bfh || NULL == bih || NULL == cx || NULL == cy) return -1;

    // �õ�һ�е��ֽ���,4���ֽڶ���
    int lineBytes = (cx + 3) / 4 * 4;

    // �ļ�ͷ
    BITMAPFILEHEADER bfh_temp = *bfh;
    BITMAPINFOHEADER bih_temp = *bih;
    bfh_temp.bfSize = lineBytes * cy + 1078;
    bih_temp.biWidth = cx;
    bih_temp.biHeight = cy;
    bih_temp.biSizeImage = lineBytes * cy;

    // �����ɫ������
    uint8 palatte[1024];
    for(int i = 0; i < 256; i++)
    {
        if (1 == i && color)
        {
            palatte[i*4]   =(unsigned char)255;
            palatte[i*4+1] =(unsigned char)0;
            palatte[i*4+2] =(unsigned char)0;
            palatte[i*4+3] =0;
        }
        else if (2 == i && color)
        {
            palatte[i*4]   =(unsigned char)0;
            palatte[i*4+1] =(unsigned char)255;
            palatte[i*4+2] =(unsigned char)0;
            palatte[i*4+3] =0;
        }
        else if (3 == i && color)
        {
            palatte[i*4]   =(unsigned char)0;
            palatte[i*4+1] =(unsigned char)0;
            palatte[i*4+2] =(unsigned char)255;
            palatte[i*4+3] =0;
        }
        else
        {
            palatte[i*4]   =(unsigned char)i;
            palatte[i*4+1] =(unsigned char)i;
            palatte[i*4+2] =(unsigned char)i;
            palatte[i*4+3] =0;
        }
    }

    // ���ļ�
    FILE *fp = fopen(filename, "wb");

    if (NULL == fp) return -2;

    // д�ļ�ͷ
    fwrite((void*)&bfh_temp, sizeof(bfh_temp), 1, fp);
    fwrite((void*)&bih_temp, sizeof(bih_temp), 1, fp);
    fwrite((void*)&palatte,  sizeof(palatte),  1, fp);

    // ���ж�ȡ��ÿ��lineBytes���ֽ�
    for (int i = 0; i < cy; i++)
    {
        fwrite((void*)(data+i*cx), 1, lineBytes, fp);
    }

    fclose(fp);

    return 0;
}
