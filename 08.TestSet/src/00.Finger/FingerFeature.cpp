#include "stdafx.h"
#include "FingerFeature.h"
#include "FingerImageProcess.h"


/**
 *\fn           void flagMinutia(uint8 *image, int cx, int cy, PFEATURE end, PFEATURE fork)
 *\brief        ���������
 *\param[in]    uint8 * image ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    PFEATURE end �˵�
 *\param[in]    PFEATURE fork ���
 *\return       void ��
 */
void flagMinutia(uint8 *image, int cx, int cy, PFEATURE end, PFEATURE fork)
{
    int i = 0;
    int x = 0;
    int y = 0;
    int r = 5;

    for (int i = 0; i < end->minutiaNum; i++)
    {
        for (int x = -r; x < r; x++)
        {
            *(image + end->minutia[i].y * cx + end->minutia[i].x - r * cx + x) = 1;
            *(image + end->minutia[i].y * cx + end->minutia[i].x + r * cx + x) = 1;
        }

        for (int y = -r; y < r; y++)
        {
            *(image + end->minutia[i].y * cx + end->minutia[i].x + y * cx - r) = 1;
            *(image + end->minutia[i].y * cx + end->minutia[i].x + y * cx + r) = 1;
        }
    }

    for (int i = 0; i < fork->minutiaNum; i++)
    {
        for (int x = -2*r; x < 2*r; x++)
        {
            *(image + fork->minutia[i].y * cx + fork->minutia[i].x - r * cx + x) = 2;
        }

        for (int y = r, x = 0; y > -r; y--, x++)
        {
            *(image + fork->minutia[i].y * cx + fork->minutia[i].x + y * cx - x) = 2;
            *(image + fork->minutia[i].y * cx + fork->minutia[i].x + y * cx + x) = 2;
        }
    }
}

/**
 *\fn           int GetJiajiao(int angle1, int angle2)
 *\brief        �������Ƕȵļн�(0 - 90)
 *\param[in]    int angle1 �Ƕ�1
 *\param[in]    int angle2 �Ƕ�2
 *\return       int �н�
 */
int GetJiajiao(int angle1, int angle2)
{
    int a = abs(angle1-angle2);

    if (a > 90)
    {
        a = 180 - a;
    }

    return a;
}

/**
 *\fn           int getDistancePt(uint8 *end, uint8 **pt, int d, int cx, int cy)
 *\brief        ���������ҵ����뵱ǰ�˵�Ϊd�ĵ�ĵ�ַ
 *\param[in]    uint8 * end ��ǰ�˵�ĵ�ַ
 *\param[out]   uint8 * * pt Ŀ�ĵ�ĵ�ַ(����ھ���d���ж˵���߲�㣬�򷵻����ǵ��ڵ�)
 *\param[in]    int d ���루������
 *\param[in]    int cx 
 *\param[in]    int cy 
 *\return       int 0�ɹ�,����ʧ��
 */
int getDistancePt(uint8 *end, uint8 **pt, int d, int cx, int cy)
{
    int i = 0;
    int n = 0;
    int siteD8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1};
    uint8 *now = NULL;
    uint8 *last = NULL;
    uint8 *next = NULL;

    *pt = NULL;

    // �ҵ���ǰ����Χ���Ǻڵ㲢�Ҳ���ǰ����ĺڵ㣬���浽����
    for (i = 0; i < 8; i++)
    {
        if (*(end + siteD8[i]) == 0)
        {
            now = end + siteD8[i];
            n++;
        }
    }

    if (n != 1) // ��Ŀ�������1�򷵻ش���
    {
        return 1;
    }

    // �ҵ��ĺڵ��ַΪnow
    // �˵���Ϊǰ����,��ַ���浽last
    last = end;

    // ѭ����d�κ�̵�
    for (i = 0; i < d; i++)
    {
        // ����ǲ��������ѭ��
        if (isFork(now, cx))
        {
            break;
        }

        // �ɹ��ҵ���һ��������ĵ�ǰ���ǰ����
        if (getNextPt(now, last, &next, cx, cy))
        {
            last = now;
            now = next;
        }
        else
        {
            break;
        }
    }

    // ����ҵ��ĵ�Ϊ���,�򷵻���ǰ����,���򷵻ص�ǰ��
    *pt = (isFork(now, cx)) ? last : now;

    return 0;
}

/**
 *\fn           int getPtX(uint8 *input, uint8 *pos, int cx)
 *\brief        ���ݵ�ַ�õ���ǰ��ĺ�����
 *\param[in]    uint8 * input ͼ��ʼ
 *\param[in]    uint8 * pos ��ǰ���ַ
 *\param[in]    int cx ͼ���
 *\return       int ������
 */
int getPtX(uint8 *input, uint8 *pos, int cx)
{
    return ((pos - input) % cx);
}

/**
 *\fn           int getPtY(uint8 *input, uint8 *pos, int cx)
 *\brief        ���ݵ�ַ�õ���ǰ���������
 *\param[in]    uint8 * input ͼ��ʼ
 *\param[in]    uint8 * pos ��ǰ���ַ
 *\param[in]    int cx ͼ���
 *\return       int ������
 */
int getPtY(uint8 *input, uint8 *pos, int cx)
{
    return ((pos - input) % cx);
}

/**
 *\fn           int getAngleAbs(int angle1, int angle2)
 *\brief        �������Ƕȵļн�(0 - 360)(��ʱ�뷽��)
 *\param[in]    int angle1 �Ƕ�һ(0 - 360)
 *\param[in]    int angle2 �Ƕȶ�(0 - 360)
 *\return       int �н�
 */
int getAngleAbs(int angle1, int angle2)
{
    int a = angle1 - angle2;

    if (a < 0)
    {
        a += 360;
    }

    return a;
}

/**
 *\fn           int getAngleRelative(int angle1, int angle2)
 *\brief        �������Ƕȵļн�(0 - 180)
 *\param[in]    int angle1 �Ƕ�һ(0 - 360)
 *\param[in]    int angle2 �Ƕȶ�(0 - 360)
 *\return       int �н�
 */
int getAngleRelative(int angle1, int angle2)
{
    int a = abs(angle1 - angle2);

    return ((a > 180) ? (360 - a) : a);
}

/**
 *\fn           int getAngle(int x0, int y0, int x1, int y1)
 *\brief        ������������x�᷽��ĽǶ�
 *\param[in]    int x0 ��һ���������
 *\param[in]    int y0 ��һ����������
 *\param[in]    int x1 �ڶ����������
 *\param[in]    int y1 �ڶ�����������
 *\return       int �Ƕ�
 */
int getAngle(int x0, int y0, int x1, int y1)
{
    double angle = atan2((double)(y1-y0), (double)(x1-x0));

    // ����ת����0 - 2*PI
    if (angle < 0)
    {
        angle += 2*PI;
    }

    // ����ת��Ϊ�Ƕ�
    return (int)(angle*EPI + 0.5);
}

/**
 *\fn           bool isNearEdge(int x, int y, int cx, int cy, int r, uint8 *orient_pt)
 *\brief        ����Ƿ񿿽���Ե
 *\param[in]    int x ��ǰ��x����
 *\param[in]    int y ��ǰ��y����
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int r ���뾶
 *\param[in]    uint8 * orient_pt ��ǰ���򳡵�
 *\return       bool �Ƿ񿿽���Ե
 */
bool isNearEdge(int x, int y, int cx, int cy, int r, uint8 *orient_pt)
{
    int i = 0;
    int j = 0;
    bool flag = true;

    for (i = -r; i <= r && flag; i++)
    {
        for (j = -r; j <= r && flag; j++)
        {
            if ((y+i)<0 || (y+i)>=cy || (x+j)<0 || (x+j)>=cx)
            {
                continue;
            }

            if (*(orient_pt + i*cx + x + j) == 255) // ����
            {
                flag = false;
                break;
            }
        }
    }

    return !flag;
}

/**
 *\fn           bool isSevereChange(uint8 *orient_pt, int *siteR5)
 *\brief        ���õ���Χ�����Ƿ�仯����,�仯���Һܿ�������ٵ�
 *\param[in]    uint8 * orient_pt ��ǰ���򳡵�
 *\param[in]    int *siteR5 ĳ��ΪԲ��,�뾶Ϊ5��Բ�����е�ĵ�ַƫ��
 *\return       bool �Ƿ�仯����
 */
bool isSevereChange(uint8 *orient_pt, int *siteR5)
{
    int i = 0;
    int sum = 0;

    for (i = 0; i < 28; i++)
    {
        sum += GetJiajiao(*(orient_pt+siteR5[(i+1)%28]), *(orient_pt+siteR5[i]));
    }

    return (sum > 96);
}

/*
 *\fn         int getRawMinutia(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork)
 *\brief      �õ�ԭʼ������
 *\param[in]  uint8 * input ͼ������
 *\param[in]  uint8 * orient ����
 *\param[in]  int cx ͼ���
 *\param[in]  int cy ͼ���
 *\param[in]  PFEATURE end �˵�
 *\param[in]  PFEATURE fork ���
 *\return     int 0�ɹ�,����ʧ��
 */
int getRawMinutia(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork)
{
    int x = 0;
    int y = 0;
    int lineBegin = 0;
    bool flag = false;
    uint8 *curInput = NULL;
    uint8 *curOrient = NULL;

    // ĳ��ΪԲ�ģ��뾶Ϊ5��Բ�����е�ĵ�ַƫ��
    int siteR5[28] =
    {
        -5, cx-5, 2*cx-5, 3*cx-4, 4*cx-3, 5*cx-2, 5*cx-1, 5*cx,
        5*cx+1, 5*cx+2, 4*cx+3, 3*cx+4, 2*cx+5, cx+5, 5, -cx+5,
        -2*cx+5, -3*cx+4, -4*cx+3, -5*cx+2, -5*cx+1, -5*cx,
        -5*cx-1, -5*cx-2, -4*cx-3, -3*cx-4, -2*cx-5, -cx-5
    };

    end->minutiaNum = 0;
    fork->minutiaNum = 0;

    for (y = 17; y < cy-17; y++)
    {
        lineBegin = y*cx;

        for (x = 17; x < cx-17; x++)
        {
            curInput  = input + lineBegin + x;
            curOrient = orient + lineBegin + x;

            // ���Ǻڵ��򲻿���
            if (*curInput != 0)
            {
                continue;
            }

            // �ǲ��
            if (isFork(curInput, cx))
            {
                // ����Ƿ񿿽���Ե,������Ե�򲻿���
                flag = isNearEdge(x, y, cx, cy, 16, (orient + lineBegin));

                if (!flag)
                {
                    // ���õ���Χ�����Ƿ�仯����,�仯�����򲻿���,��Ϊ�ܿ�������ٵ�
                    flag = isSevereChange(curOrient, siteR5);
                }

                // �ﵽ����Ҫ�����¼�����õ�
                if (!flag)
                {
                    fork->minutia[fork->minutiaNum].type = MINUTIA_FORK;
                    fork->minutia[fork->minutiaNum].x = x;
                    fork->minutia[fork->minutiaNum].y = y;
                    fork->minutiaNum++;

                    // ��������Ѿ��������������Ŀ���򷵻ش���
                    if (fork->minutiaNum >= MAXRAWminutiaNum)
                    {
                        fork->minutiaNum = 0;
                        return -1;
                    }
                }
            }
            else if (isEnd(curInput, cx)) // �Ƕ˵�
            {
                flag = isNearEdge(x, y, cx, cy, 16, (orient + lineBegin));

                if (!flag)
                {
                    flag = isSevereChange(curOrient, siteR5);
                }

                if (!flag)
                {
                    end->minutia[end->minutiaNum].type = MINUTIA_END;
                    end->minutia[end->minutiaNum].x = x;
                    end->minutia[end->minutiaNum].y = y;
                    end->minutiaNum++;

                    if (end->minutiaNum >= MAXRAWminutiaNum)
                    {
                        end->minutiaNum = 0;
                        return -2;
                    }
                }
            }
        }
    }

    return 0;
}

/**
 *\fn           int filteMinutiaEnd(uint8 *input, int cx, int cy, int minDis, PFEATURE end)
 *\brief        �����Ż�ָ�ƶ˵�����
 *\param[in]    uint8 * input ָ��ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int minDis �������������������ľ���
 *\param[in]    PFEATURE end �˵�����
 *\return       int �˵�����
 */
int filteMinutiaEnd(uint8 *input, int cx, int cy, int minDis, PFEATURE end)
{
    int i = 0;
    int j = 0;
    int d = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int num = 0;
    int end_num = end->minutiaNum;
    int edge = 16; // ���������㿿��ǰ����Ե���������
    int angle1 = 0;
    int angle2 = 0;
    int angle3 = 0;
    int angle4 = 0;
    bool flag = false;
    uint8 *curPoint = NULL;
    uint8 *disPoint = NULL;

    for (i = 0; i < (end_num - 1); i++)
    {
        flag = false;

        for (j = i+1; j < end_num; j++)
        {
            x1 = end->minutia[i].x;
            y1 = end->minutia[i].y;
            x2 = end->minutia[j].x;
            y2 = end->minutia[j].y;

            // �������˵�ľ���,���ɶ���
            d = (int)sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));

            // �����㹻��������һ���˵�
            if (d > edge && d > minDis)
            {
                continue;
            }

            // ����̫С������������Ϊ��0��0��
            if (d <= minDis)
            {
                end->minutia[j].x = 0;
                end->minutia[j].y = 0;
                flag = true;
                continue;
            }

            // ���һ���˵��������ߵĽǶ�
            curPoint = input + y1*cx + x1;

            // �õ������Ͼ���õ�8����ĵ�
            if (getDistancePt(curPoint, &disPoint, 8, cx, cy) != 0)
            {
                break;
            }

            angle1 = getAngle(x1, y1, getPtX(input, disPoint, cx), getPtY(input, disPoint, cy));

            // ��ڶ����˵��������ߵĽǶ�
            curPoint = input + y2*cx + x2;

            if (getDistancePt(curPoint, &disPoint, 8, cx, cy) != 0)
            {
                continue;
            }

            angle2 = getAngle(x2, y2, getPtX(input, disPoint, cx), getPtY(input, disPoint, cy));

            // �������Ƕȼ�ľ���
            angle3 = getAngleAbs(angle1, angle2);

            // ��������Ƕȼ����ǣ��������������
            if (angle3 > 270 || angle3 < 90)
            {
                continue;
            }

            // �������˵����ߵĽǶ�
            angle3 = getAngle(x1, y1, x2, y2);

            // ���һ���˵����������ߵļн�
            angle3 = getAngleAbs(angle1, angle3);

            // ����нǽϴ�������ٵ�
            if (angle3 < 150 || angle3 > 210)
            {
                continue;
            }

            // ��ڶ����˵����������ߵļн�
            angle4 = getAngle(x2, y2, x1, y1);

            angle4 = getAngleAbs(angle2, angle4);

            // ����нǽϴ�������ٵ�
            if (angle4 < 150 || angle4 > 210)
            {
                continue;
            }

            // �����ʾ����������ͬһ�������ϵĶ��Ѵ��������˵�,������ԭ��
            end->minutia[j].x = 0;
            end->minutia[j].y = 0;
            flag = true;
        }

        if (flag)// ��ʾ����������ͬһ�������ϵĶ��Ѵ��������˵�,������ԭ��
        {
            end->minutia[i].x = 0;
            end->minutia[i].y = 0;
        }
    }

    // ͳ���µĶ˵���Ŀ
    for (i = 0; i < end_num; i++)
    {
        if (end->minutia[i].x == 0 || end->minutia[i].y == 0)
        {
            continue;
        }

        curPoint = input + end->minutia[i].y*cx + end->minutia[i].x;

        // �õ������Ͼ���õ�8����ĵ�
        if (getDistancePt(curPoint, &disPoint, 8, cx, cy) != 0)
        {
            end->minutia[i].x = 0;
            end->minutia[i].y = 0;
            continue;
        }

        num++;
    }

    return num;
}

/**
 *\fn           int filteMinutiaFork(uint8 *input, int cx, int cy, int minDis, PFEATURE fork)
 *\brief        �����Ż�ָ�ƶ˵�����
 *\param[in]    uint8 * input ָ��ͼ������
 *\param[in]    int cx ͼ���
 *\param[in]    int cy ͼ���
 *\param[in]    int minDis �������������������ľ���
 *\param[in]    PFEATURE fork �������
 *\return       int �������
 */
int filteMinutiaFork(uint8 *input, int cx, int cy, int minDis, PFEATURE fork)
{
    int i = 0;
    int j = 0;
    int n = 0;
    int d = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int num = 0;
    int fork_num = fork->minutiaNum;
    int siteU8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1}; // ĳ����Χ8����ĵ�ַƫ��
    bool flag = false;
    uint8 *temp[9] = {0};
    uint8 *curPoint = NULL;

    for (i = 0; i < fork_num-1; i++)
    {
        flag = false;

        for (j = i+1; j < fork_num; j++)
        {
            x1 = fork->minutia[i].x;
            y1 = fork->minutia[i].y;
            x2 = fork->minutia[j].x;
            y2 = fork->minutia[j].y;

            d = sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));

            if (d <= (minDis-2))
            {
                fork->minutia[j].x = 0;
                fork->minutia[j].y = 0;
                flag = true;
            }
        }

        if (flag)
        {
            fork->minutia[i].x = 0;
            fork->minutia[i].y = 0;
        }
    }

    // ͳ���µ������Ĳ��
    for (i = 0; i < fork_num; i++)
    {
        if (fork->minutia[i].x == 0 || fork->minutia[i].y == 0)
        {
            continue;
        }

        curPoint = input + fork->minutia[i].y*cx + fork->minutia[i].x;

        temp[0] = curPoint;
        *temp[0] = 255; // ���óɰ׵�

        // ����Ƿ��������
        for (j = 0, n = 1; j < 8; j++)
        {
            if (*(curPoint + siteU8[j]) == 0) // �ڵ�
            {
                temp[n] = curPoint + siteU8[j];
                *temp[n] = 255;
                n++;
            }
        }

        for (j = 0; j < 4; j++)
        {
            *temp[j] = 0; // ���óɺڵ�
        }

        if (n != 4)
        {
            fork->minutia[i].x = 0;
            fork->minutia[i].y = 0;
            continue;
        }

        num++;
    }

    return num;
}

/*
 *\fn         void clearMinutia(PFEATURE data)
 *\brief      ���������
 *\param[in]  PFEATURE data ����������
 *\return     void 0�ɹ�,����ʧ��
 */
void clearMinutia(PFEATURE data)
{
    int last = data->minutiaNum - 1;
    int j = last;
    int i = 0;

    for (i = 0; i < j; i++)
    {
        if (data->minutia[i].x != 0 && data->minutia[i].y != 0)
        {
            continue;
        }

        for (j = last; j > i; j--)
        {
            if (data->minutia[j].x == 0 || data->minutia[j].y == 0)
            {
                continue;
            }

            last = j - 1;
            data->minutia[i] = data->minutia[j];
            data->minutia[j].x = 0;
            data->minutia[j].y = 0;
            break;
        }
    }

    data->minutiaNum = i - (0 == data->minutia[i-1].x || 0 == data->minutia[i-1].y);
}

/*
 *\fn         int filteMinutia(uint8 *input, int cx, int cy, PFEATURE end, PFEATURE fork)
 *\brief      ����������
 *\param[in]  uint8 * input ͼ������
 *\param[in]  int cx ͼ���
 *\param[in]  int cy ͼ���
 *\param[in]  PFEATURE end �˵�
 *\param[in]  PFEATURE fork ���
 *\return     int 0�ɹ�,����ʧ��
 */
int filteMinutia(uint8 *input, int cx, int cy, PFEATURE end, PFEATURE fork)
{
    int i = 0;
    int j = 0;
    int last = 0;
    int count = 0;

    for (i = 7; i < (7+32); i++)
    {
        count = filteMinutiaEnd(input, cx, cy, i, end);

        if (count < MAXminutiaNum) break;
    }

    for (i = 7; i < (7+32); i++)
    {
        count = filteMinutiaFork(input, cx, cy, i, fork);

        if (count < MAXminutiaNum) break;
    }

    clearMinutia(end);
    clearMinutia(fork);

    return 0;
}

/*
 *\fn         void makeFeatureEnd(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end)
 *\brief      ���ɶ˵���������
 *\param[in]  uint8 * input ͼ������
 *\param[in]  uint8 * orient ����
 *\param[in]  int cx ͼ���
 *\param[in]  int cy ͼ���
 *\param[in]  PFEATURE end �˵�
 *\return     void ��
 */
void makeFeatureEnd(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end)
{
    int i = 0;
    int x = 0;
    int y = 0;
    int x1 = 0;
    int y1 = 0;
    int angle1 = 0;
    int angle2 = 0;
    int angle3 = 0;
    int trilen = 16;
    double a = 0;
    uint8 *curPoint = NULL;
    uint8 *disPoint = NULL;

    for (i = 0; i < end->minutiaNum; i++)
    {
        x = end->minutia[i].x;
        y = end->minutia[i].y;

        // �������ҵ���8����
        curPoint = input + y*cx + x;

        if (getDistancePt(curPoint, &disPoint, 8, cx, cy) != 0)
        {
            continue;
        }

        // ��õ���˵����ߵĽǶ�
        angle1 = getAngle(x, y, getPtX(input, disPoint, cx), getPtX(input, disPoint, cx));

        // ����
        angle2 = *(orient + y * cx + x);

        angle3 = getAngleAbs(angle1, angle2);

        // ����ɶ۽ǣ����ʾ�������㷽����õ㷽�򳡷����෴
        // �������ǣ����ʾ�������㷽����õ㷽�򳡷�����ͬ
        if (angle3 > 90 && angle3 < 270)
        {
            angle2 += 180;
        }

        end->minutia[i].direction = angle2;

        a = angle2 / EPI + PI/4.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        end->minutia[i].triangle[0] = *(orient + y1*cx + x1);

        a += PI*2/3.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        end->minutia[i].triangle[1] = *(orient + y1*cx + x1);

        a += PI*2/3.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        end->minutia[i].triangle[2] = *(orient + y1*cx + x1);
    }
}

/*
 *\fn         void makeFeatureFork(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE fork)
 *\brief      ���ɲ����������
 *\param[in]  uint8 * input ͼ������
 *\param[in]  uint8 * orient ����
 *\param[in]  int cx ͼ���
 *\param[in]  int cy ͼ���
 *\param[in]  PFEATURE fork ���
 *\return     void ��
 */
void makeFeatureFork(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE fork)
{
    int i = 0;
    int j = 0;
    int n = 0;
    int x = 0;
    int y = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int x3 = 0;
    int y3 = 0;
    int d0 = 0;
    int d1 = 0;
    int d2 = 0;
    int angle1 = 0;
    int angle2 = 0;
    int angle3 = 0;
    int trilen = 16;
    int siteU8[8] = {cx-1, cx, cx+1, 1, -cx+1, -cx, -cx-1, -1}; // ĳ����Χ8����ĵ�ַƫ��
    uint8 *temp[9] = {0};
    uint8 *curPoint = NULL;
    uint8 *disPoint[8] = {0};
    double a = 0;

    for (i = 0; i < fork->minutiaNum; i++)
    {
        x = fork->minutia[i].x;
        y = fork->minutia[i].y;

        curPoint = input + y*cx + x;

        temp[0] = curPoint;
        *temp[0] = 255; // ���óɰ׵�

        for (j = 0, n = 1; j < 8; j++)
        {
            if (*(curPoint + siteU8[j]) == 0) // �ڵ�
            {
                temp[n] = curPoint + siteU8[j];
                *temp[n] = 255; // ���óɰ׵�
                n++;
            }
        }

        if (n != 4)
        {
            continue;
        }

        // �ҵ�����������������Ͼ���Ϊ8�ĵ�
        for (j = 0; j < 3; j++)
        {
            getDistancePt(temp[j+1], &disPoint[j], 8, cx, cy);
        }

        for (j = 0; j < 4; j++)
        {
            *temp[j] = 0; // ���óɺڵ�
        }

        // ���������������ľ���
        x1 = getPtX(input, disPoint[0], cx);
        y1 = getPtY(input, disPoint[0], cx);
        x2 = getPtX(input, disPoint[1], cx);
        y2 = getPtY(input, disPoint[1], cx);
        x3 = getPtX(input, disPoint[2], cx);
        y3 = getPtY(input, disPoint[2], cx);

        d0 = sqrt((double)((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
        d1 = sqrt((double)((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3)));
        d2 = sqrt((double)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)));

        // ������̵ı߶�Ӧ���ǲ��߷���
        if (d0 < d1 && d0 < d2)
        {
            angle1 = getAngle(x, y, (x2+x3)/2, (y2+y3)/2);
        }
        else if (d1 < d0 && d1 < d2)
        {
            angle1 = getAngle(x, y, (x1+x3)/2, (y1+y3)/2);
        }
        else
        {
            angle1 = getAngle(x, y, (x1+x2)/2, (y1+y2)/2);
        }

        // ����̱��е��������ߵķ�����õ㷽�򳡷���ļн�
        //angle1 = getAngle(x, y, (x11+x21)/2, (y11+y21)/2);

        angle2 = *(orient + y*cx + x);

        angle3 = getAngleAbs(angle1, angle2);

        // ����ɶ۽ǣ����ʾ�������㷽����õ㷽�򳡷����෴
        // �������ǣ����ʾ�������㷽����õ㷽�򳡷�����ͬ
        if (angle3 > 90 && angle3 < 270)
        {
            angle2 += 180;
        }

        fork->minutia[i].direction = angle2;

        // ��¼��������Ϊ�������Բ�뾶Ϊtrilen������������������ķ���
        a = angle2 / EPI + PI/4.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        fork->minutia[i].triangle[0] = *(orient + y1*cx + x1);

        a += PI*2/3.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        fork->minutia[i].triangle[1] = *(orient + y1*cx + x1);

        a += PI*2/3.0;
        x1 = x + (int)(trilen*cos(a)+ 0.5);
        y1 = y + (int)(trilen*sin(a)+ 0.5);
        fork->minutia[i].triangle[2] = *(orient + y1*cx + x1);
    }
}

/*
 *\fn         void makeFeature(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork)
 *\brief      ������������
 *\param[in]  uint8 * input ͼ������
 *\param[in]  uint8 * orient ����
 *\param[in]  int cx ͼ���
 *\param[in]  int cy ͼ���
 *\param[in]  PFEATURE end �˵�
 *\param[in]  PFEATURE fork ���
 *\return     void 0�ɹ�,����ʧ��
 */
void makeFeature(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork)
{
    makeFeatureEnd(input, orient, cx, cy, end);
    makeFeatureFork(input, orient, cx, cy, fork);
}


/*
 *\fn         void align(PFEATURE feature, PFEATURE aligned, PMINUTIA featureCore, int rotation, int transx, int transy)
 *\brief      ����
 *\param[in]  PFEATURE feature ��������
 *\param[in]  PFEATURE aligned ������������
 *\param[in]  PMINUTIA featureCore ����ת��������������
 *\param[in]  int rotation ��ת�Ƕ�
 *\param[in]  int transx ˮƽƫ��
 *\param[in]  int transy ��ֱƫ��
 *\return     void ��
 */
void align(PFEATURE feature, PFEATURE aligned, PMINUTIA featureCore, int rotation, int transx, int transy)
{
    int i = 0;
    int x = 0;
    int y = 0;
    int cx = featureCore->x;        // ����ת�������ĵ�����
    int cy = featureCore->y;        // ����ת�������ĵ�����
    double rota = rotation / EPI;   // ��ת�Ļ���
    double sinv = sin(rota);;       // ��ת���ȵ�sinֵ
    double cosv = cos(rota);        // ��ת���ȵ�cosֵ

    // ���������ṹ��Ϣ
    *aligned = *feature;

    for (i = 0; i < feature->minutiaNum; i++)
    {
        x = feature->minutia[i].x;
        y = feature->minutia[i].y;

        //  ����ת�����������
        aligned->minutia[i].x = (int)(cx + cosv*(x-cx) - sinv*(y-cy) + transx + 0.5);
        aligned->minutia[i].y = (int)(cy + sinv*(x-cx) + cosv*(y-cy) + transy + 0.5);

        // ��ת����������·���
        aligned->minutia[i].direction = (feature->minutia[i].direction + rotation) % 360;
    }
}

// Ϊ��ʡʱ�䣬�̾��������������ò��
// DisTbl[m][n] = (int)(sqrt(m*m+n*n)+0.5)
int  DisTbl[10][10] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
    1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
    2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 
    3, 3, 3, 4, 5, 5, 6, 7, 8, 9, 
    4, 4, 4, 5, 5, 6, 7, 8, 8, 9, 
    5, 5, 5, 5, 6, 7, 7, 8, 9, 10, 
    6, 6, 6, 6, 7, 7, 8, 9, 10, 10, 
    7, 7, 7, 7, 8, 8, 9, 9, 10, 11, 
    8, 8, 8, 8, 8, 9, 10, 10, 11, 12, 
    9, 9, 9, 9, 9, 10, 10, 11, 12, 12
};

/*
 *\fn         void alignMatch(PFEATURE feature, PFEATURE templat, PMATCHRESULT matchResult, int matchMode)
 *\brief      ��������ϵ�����ָ���������бȶ�
 *\param[in]  PFEATURE feature ��������
 *\param[in]  PFEATURE templat ģ������
 *\param[in]  PMATCHRESULT matchResult �ȶԽ��
 *\param[in]  int matchMode 
 *\return     void 0�ɹ�,����ʧ��
 */
void alignMatch(PFEATURE feature, PFEATURE templat, PMATCHRESULT matchResult, int matchMode)
{
    int i = 0;
    int j = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int s = 0;          // ���ƶ�
    int dis = 0;        // ����
    int angle = 0;      // �Ƕ�
    int score = 0;      // �ܷ�
    int matchNum = 0;   // ƥ����������
    int num1 = feature->minutiaNum;  // ����������
    int num2 = templat->minutiaNum;  // ģ������������
    BYTE flagA[MAXminutiaNum] = {0}; // ���feature���������Ƿ��Ѿ���ƥ�����
    BYTE flagT[MAXminutiaNum] = {0}; // ���templat���������Ƿ��Ѿ���ƥ�����

    // ��ͬ�����������ƥ�����
    for (i = 0; i < templat->minutiaNum; i++)
    {
        // �Ƿ�����ƥ�����
        if (flagT[i]) continue;

        for (j = 0; j < feature->minutiaNum; j++)
        {
            // �Ƿ�����ƥ�����
            if (flagA[j]) continue;

            // �����������Ƿ���ͬ
            if (templat->minutia[i].type != feature->minutia[j].type) continue;

            // �����㷽��н�
            angle = getAngleAbs(templat->minutia[i].direction, feature->minutia[j].direction);

            // �н�>=10��ƥ��
            if (angle >= 10) continue;

            x1 = templat->minutia[i].x;
            y1 = templat->minutia[i].y;
            x2 = feature->minutia[j].x;
            y2 = feature->minutia[j].y;

            // ˮƽ����>=10��ƥ��
            if (abs(x1-x2) >= 10) continue;

            // ��ֱ����>=10��ƥ��
            if (abs(y1-y2) >= 10) continue;

            // ���������ľ���
            dis = DisTbl[abs(y1-y2)][abs(x1-x2)];

            // ����>=10��ƥ��
            if (dis >= 10) continue;

            // ������������������ǣ���ʾ�Ѿ���ƥ�����
            flagA[j] = 1;
            flagT[i] = 1;

            // �ּܷ��ϴ������������ƥ�����
            score += (10-angle);
            score += (10-dis);

            // ƥ������������1
            matchNum++;

            // ����ǿ��ٱȶ�ģʽ
            if (matchMode == MATCHMODE_FAST && matchNum >= 8)
            {
                // �������ƶ�
                s = 4 * score * matchNum * MAXminutiaNum / ((num1+num2)*(num1+num2));

                if (s > 100)  // ���ƶ��㹻���򷵻رȶԽ��
                {
                    matchResult->matchNum = matchNum;
                    matchResult->rotation = 0;
                    matchResult->similarity = s;
                    matchResult->transX = 0;
                    matchResult->transY = 0;
                    return;
                }
            }
        }
    }

    if (matchMode != MATCHMODE_FAST)
    {
        // ����ͼ��������¿��ܶ˵㴦��ɲ����㴦��ɶ˵㣬�������Ϊ50%������
        // ��������ķ���
        for (i = 0; i < templat->minutiaNum; i++)
        {
            // �Ƿ�����ƥ�����
            if (flagT[i]) continue;

            for (j = 0; j < feature->minutiaNum; j++)
            {
                // �Ƿ�����ƥ�����
                if (flagA[j]) continue;

                // �Ƿ����Ͳ�ͬ
                if (templat->minutia[i].type == feature->minutia[j].type) continue;
                    
                // �����㷽��н�
                angle = getAngleAbs(templat->minutia[i].direction, feature->minutia[j].direction);

                // �н�>=10��ƥ��
                if (angle >= 10) continue;
                    
                x1 = templat->minutia[i].x;
                y1 = templat->minutia[i].y;
                x2 = feature->minutia[j].x;
                y2 = feature->minutia[j].y;

                // ˮƽ����>=10��ƥ��
                if (abs(x1-x2) >= 10) continue;
                    
                // ��ֱ����>=10��ƥ��
                if (abs(y1-y2) >= 10) continue;
                    
                // ���������ľ���
                dis = DisTbl[abs(y1-y2)][abs(x1-x2)];

                // ����>=10��ƥ��
                if (dis >= 10) continue;
                    
                // ������������������ǣ���ʾ�Ѿ���ƥ�����
                flagA[j] = 1;
                flagT[i] = 1;

                // �ּܷ��ϴ������������ƥ�����
                score += ((10-angle)/2);
                score += ((10-dis)/2);

                // ƥ������������1
                matchNum++;
            }
        }
    }

    // �������ƶȣ����رȶԽ��
    s = 4 * score * matchNum * MAXminutiaNum / ((num1+num2)*(num1+num2));

    matchResult->matchNum = matchNum;
    matchResult->rotation = 0;
    matchResult->similarity = s;
    matchResult->transX = 0;
    matchResult->transY = 0;
}

/*
 *\fn         int matchFeature(PFEATURE feature, PFEATURE templat, MATCHRESULT *matchResult, int matchMode)
 *\brief      �Ƚ���������
 *\param[in]  PFEATURE feature1 ��������
 *\param[in]  PFEATURE templat ģ����������
 *\param[in]  MATCHRESULT * matchResult �ȽϽ��
 *\param[in]  int matchMode �Ƚ�ģʽ,Ĭ����ֵΪ50�����ƶ�>=50����Ϊ�ȶԳɹ�������ȶ�ʧ��
 *\return     bool �Ƿ�ƥ��
 */
bool matchFeature(PFEATURE feature, PFEATURE templat, MATCHRESULT *matchResult, int matchMode)
{
    int i = 0;
    int j = 0;
    int a1 = 0;
    int a2 = 0;
    int d1 = 0;
    int d2 = 0;
    int t11 = 0;
    int t12 = 0;
    int t13 = 0;
    int t21 = 0;
    int t22 = 0;
    int t23 = 0;
    int num = 0;
    int agate = 8;                  // �������˽ṹ�Ƕ����
    int rotation = 0;;              // ��ת�Ƕ�
    int transx = 0;                 // ˮƽƫ��
    int transy = 0;                 // ��ֱƫ��
    FEATURE alignFeature = {0};     // ������ָ������
    MATCHRESULT max = {0};          // ���ƶ����ıȶԽ��
    MATCHRESULT result = {0};       // �ȶԽ��

    // ����ͬ���͵�ָ������������Ϊͬһ��ָ���������ж���ȶ�
    for (i = 0; i < templat->minutiaNum; i++)
    {
        for (j = 0; j < feature->minutiaNum; j++)
        {
            // ��ͬ�����򲻱ȶ�
            if (feature->minutia[j].type != templat->minutia[i].type) continue;

            d1 = feature->minutia[j].direction;
            d2 = feature->minutia[i].direction;

            if (matchMode == MATCHMODE_FAST)
            {
                t11 = feature->minutia[j].triangle[0];
                t12 = feature->minutia[j].triangle[1];
                t13 = feature->minutia[j].triangle[2];
                t21 = feature->minutia[i].triangle[0];
                t22 = feature->minutia[i].triangle[1];
                t23 = feature->minutia[i].triangle[2];

                // �������������˽ṹ�ȶԣ������������׼
                if (t11 != 255 && t21 != 255)
                {
                    a1 = GetJiajiao(t11, d1 % 180);
                    a2 = GetJiajiao(t21, d2 % 180);

                    if (abs(a1-a2) > agate) continue;
                }

                if (t11 != 255 && t21 != 255 && t12 != 255 && t22 != 255)
                {
                    a1 = GetJiajiao(t11, t12);
                    a2 = GetJiajiao(t21, t22);

                    if(abs(a1-a2) > agate) continue;
                }

                if (t13 != 255 && t23 != 255 && t12 != 255 && t22 != 255)
                {
                    a1 = GetJiajiao(t12, t13);
                    a2 = GetJiajiao(t22, t23);

                    if(abs(a1-a2) > agate) continue;
                }

                if (t11 != 255 && t11 != 255 && t13 != 255 && t23 != 255)
                {
                    a1 = GetJiajiao(t11, t13);
                    a2 = GetJiajiao(t21, t23);

                    if(abs(a1-a2) > agate) continue;
                }
            }

            alignFeature.minutiaNum = 0;

            // ��ת�Ƕ�
            rotation = getAngleAbs(feature->minutia[j].direction, templat->minutia[i].direction);

            // λ��ƫ��
            transx = (templat->minutia[i].x - feature->minutia[j].x);
            transy = (templat->minutia[i].y - feature->minutia[j].y);

            // ��feature��templat����
            align(feature, &alignFeature, &feature->minutia[j], rotation, transx, transy);

            // �����������ָ���������бȶ�
            alignMatch(&alignFeature, templat, &result, matchMode);

            // ����ȶԽ������õıȶԽ�����ã������alignMax
            if (result.similarity > max.similarity)
            {
                max.matchNum = result.matchNum;
                max.similarity = result.similarity;
                max.rotation = rotation;
                max.transX = transx;
                max.transY = transy;

                // ����ǿ��ٱȶ�ģʽ�������ƶȴﵽһ���̶����˳�
                if (matchMode == MATCHMODE_FAST && max.matchNum >= 8)
                {
                    if (max.similarity >100)
                    {
                        *matchResult = max;
                        return TRUE;
                    }
                }
            }
        }
    }

    // ���ձȶԽ��
    *matchResult = max;

    // ���ƥ�������������٣���ԱȶԽ�����ƶȽ�������
    if (matchMode != MATCHMODE_FAST && matchResult->matchNum < 15)
    {
        matchResult->similarity = (int)(matchResult->similarity*sin(matchResult->matchNum*PI/30.0) + 0.5);
    }

    // Ĭ����ֵΪ50�����ƶ�>=50����Ϊ�ȶԳɹ�������ȶ�ʧ��,
    // һ������£���35���ϼ�����Ϊ����, Ϊ��ȫ����������50-100
    return (matchResult->similarity > 50);
}

/*
int getOriChange(int angle1, int angle2, char flag)
{
    int  d;

    d = angle2 - angle1;
    if(flag >= 1)
    {
        if(d < 0)
        {
            d += 10;
        }

    }
    else if(flag <= -1)
    {
        if(d > 0)
        {
            d -= 10;
        }
    }

    return d;
}

static int D3[8][2] = 
{
    -1,-1, -1,0,  -1,1,  0,1,  1,1,  1,0,  1,-1,  0,-1
};

static int D5[12][2] = 
{
    -2,-1, -2,0,  -2,1, -1,2,  0,2,  1,2, 2,1,  2,0,  2,-1, 1,-2, 0,-2, -1,-2
};

static int D7[24][2] = 
{
    -3,-3, -3,-2, -3,-1, -3,0, -3,1, -3,2, -3,3, -2,3, -1,3, 0,3, 1,3, 2,3, 3,3, 3,2, 3,1, 3,0,
    3,-1, 3,-2, 3,-3, 2,-3, 1,-3, 0,-3, -1,-3, -2,-3
};

// �õ������������
int getSingularMinutia(BYTE *lpOrient, int Width, int Height, char flag, LPVOID feature)
{
    int    x, y, i;
    int    num;
    int    sum1, sum2;
    int    d;
    unsigned char *pOriMap;

    int    oriV;
    int    a1, a2;
    DBLPOINT  singuArr[30];

    int    value;
    double dis;
    bool   bFound = false;
    bool   fg = false;
    int    D3[8][2] = {
        -1,-1, -1,0,  -1,1,  0,1,  1,1,  1,0,  1,-1,  0,-1
    };
    int    D5[12][2] = {
        -2,-1, -2,0,  -2,1, -1,2,  0,2,  1,2, 2,1,  2,0,  2,-1, 1,-2, 0,-2, -1,-2
    };
    int    D7[24][2] = {
        -3,-3, -3,-2, -3,-1, -3,0, -3,1, -3,2, -3,3, -2,3, -1,3, 0,3, 1,3, 2,3, 3,3, 3,2, 3,1, 3,0,
        3,-1, 3,-2, 3,-3, 2,-3, 1,-3, 0,-3, -1,-3, -2,-3
    };

    FEATURE *g_Feature = (FEATURE*)(feature);


    num = 0;

    memset(singuArr, 0, sizeof(singuArr));				//��ʼ����������飬����

    for(y = 3; y < Height-3; y++)
    {
        for(x = 3; x < Width-3; x++)					//��������ͼ
        {
            pOriMap = lpOrient + y*Width + x;			//���ĳ��ķ���ָ��
            oriV = *pOriMap;							//��ȡĳ��ķ��򳡵�ֵ

            if(oriV == 255)								//���Ǳ���������������һ��ѭ��
            {
                continue;
            }

            fg = false;
            for(i = 0; i < 24; i++)
            {
                if(pOriMap[D7[i][1]*Width + D7[i][0]]==255)
                {
                    fg = true;
                    break;
                }
            }
            if(fg)
            {
                continue;
            }

            sum1 = 0;
            for(i = 0; i < 8; i++)
            {
                a1 = pOriMap[D3[i][1]*Width + D3[i][0]]/24;
                a2 = pOriMap[D3[(i+1)%8][1]*Width + D3[(i+1)%8][0]]/24;
                d = getOriChange(a1, a2, flag);

                if(abs(d) > 5)
                {
                    break;
                }

                sum1 += d;
            }
            sum2 = 0;
            for(i = 0; i < 12; i++)
            {
                a1 = pOriMap[D5[i][1]*Width + D5[i][0]]/24;
                a2 = pOriMap[D5[(i+1)%12][1]*Width + D5[(i+1)%12][0]]/24;
                d = getOriChange(a1, a2, flag);

                if(abs(d) > 5)
                {
                    break;
                }

                sum2 += d;
            }

            if(flag == -1)									//���ĵ�
            {
                value = -10;
            }
            else if(flag == 1)								//���ǵ�
            {
                value = 10;
            }
            if(sum2 == value && sum1 == value)
            {
                bFound = false;
                for(i = 0; i < num; i++)
                {
                    dis = sqrt((double)((x - singuArr[i].x)*(x - singuArr[i].x) +
                        (y - singuArr[i].y)*(y - singuArr[i].y)));
                    if(dis < 4)
                    {
                        singuArr[i].x = (singuArr[i].x + x)/2.0;
                        singuArr[i].y = (singuArr[i].y + y)/2.0;
                        bFound = true;
                        break;
                    }

                }
                if(!bFound)
                {
                    singuArr[num].x = x;
                    singuArr[num].y = y;
                    num++;
                    if(num >= MAX_SINGULARYNUM)				//���������������������ƣ���ֹͣѰ�������
                    {
                        goto next;
                    }
                }

            }
        }
    }

next:

    int  trilen = 16;
    int N = 0;											//�����ĸ���
    for(i = 0; i < num; i++)								//���ҵ�������������������
    {
        //lpArr[*lpNum + i].x = (long)singuArr[i].x;
        //lpArr[*lpNum + i].y = (long)singuArr[i].y;	

        int x = (long)singuArr[i].x;
        int y = (long)singuArr[i].y;

        //if (*(g_lpDivide + y * Width + x) == 255) continue;

        if (((x + 0.0) / Width < 0.1) || ((x + 0.0) / Width > 0.9)) 
            continue;

        if (((y + 0.0) / Height < 0.1 ) || ((y + 0.0) / Height > 0.9))
            continue;



        g_Feature->minutia[g_Feature->minutiaNum + N].x = x;
        g_Feature->minutia[g_Feature->minutiaNum + N].y = y;

        g_Feature->minutia[g_Feature->minutiaNum + N].type = (flag == -1) ? VF_MINUTIA_CORE : VF_MINUTIA_DELTA;				
        g_Feature->minutia[g_Feature->minutiaNum + N].direction = 0;				

        N++;
    }

    g_Feature->minutiaNum += N;

    return 0;
}

*/

