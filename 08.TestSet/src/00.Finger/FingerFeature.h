
#pragma once

typedef unsigned char uint8;


#define MAXminutiaNum       60  // �����������
#define MAXRAWminutiaNum    100 // �������������ĳ�ʼ�����������Ŀ

#define MINUTIA_END         1   // �˵�
#define MINUTIA_FORK        2   // ���

#define MATCHMODE_FAST      1   // ���ٱȽ�

// ������ṹ
typedef struct tagMinutiae
{
    int x;                      // ������
    int y;                      // ������
    int direction;              // ����
    int triangle[3];            // ������Ϊ�������Բ�뾶Ϊ��ֵ������������������ķ���
    int type;                   // ����

} MINUTIA, *PMINUTIA;

// ָ������(ģ��)�ṹ
typedef struct tagFeature
{
    int     minutiaNum;             // ��������
    MINUTIA minutia[MAXminutiaNum]; // ����������

} FEATURE, *PFEATURE;

typedef struct tagMatchResult
{
    int    similarity;              // ���ƶ�
    int    rotation;                // ��ת�Ƕ�
    int    transX;                  // ˮƽƫ��
    int    transY;                  // ��ֱƫ��
    int    matchNum;                // ƥ����������

} MATCHRESULT, *PMATCHRESULT;


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
int getRawMinutia(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork);

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
int filteMinutia(uint8 *input, int cx, int cy, PFEATURE end, PFEATURE fork);

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
void makeFeature(uint8 *input, uint8 *orient, int cx, int cy, PFEATURE end, PFEATURE fork);

/*
 *\fn         int matchFeature(PFEATURE feature, PFEATURE templat, MATCHRESULT *matchResult, int matchMode)
 *\brief      �Ƚ���������
 *\param[in]  PFEATURE feature1 ��������
 *\param[in]  PFEATURE templat ģ����������
 *\param[in]  MATCHRESULT * matchResult �ȽϽ��
 *\param[in]  int matchMode �Ƚ�ģʽ,Ĭ����ֵΪ50�����ƶ�>=50����Ϊ�ȶԳɹ�������ȶ�ʧ��
 *\return     bool �Ƿ�ƥ��
 */
bool matchFeature(PFEATURE feature, PFEATURE templat, MATCHRESULT *matchResult, int matchMode);

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
void flagMinutia(uint8 *image, int cx, int cy, PFEATURE end, PFEATURE fork);
