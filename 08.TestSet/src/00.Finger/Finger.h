
#pragma once
#include "FingerFeature.h"


class FingerIdentify
{
public:
    FingerIdentify();
    ~FingerIdentify();

public:
    const char *path_;      /*�ļ�·��*/

protected:
    int cx_;                /*ͼ���*/
    int cy_;                /*ͼ���*/
    uint8 *data_;           /*ͼ������*/
    uint8 *thin_clear_;     /*ͼ��ϸ�����������*/
    uint8 *orient_smooth_;  /*ͼ��ƽ����ķ�������*/

    FEATURE end_;           /*�˵�����*/
    FEATURE fork_;          /*�������*/

    BITMAPFILEHEADER bfh_;  /*BMP�ļ�ͷ*/
    BITMAPINFOHEADER bih_;  /*BMP�ļ���Ϣͷ*/

public:
    /**
     *\fn           int loadBitmap(const char *filename)
     *\brief        ����ͼ���ļ�
     *\param[in]    const char * filename �ļ���
     *\return       int 0�ɹ�,����ʧ��
     */
    int loadBitmap(const char *filename);

    /**
     *\fn           int preprocess()
     *\brief        ͼ��Ԥ����
     *\return       int 0�ɹ�,����ʧ��
     */
    int preprocess();

    /**
     *\fn           int getFeature(PFEATURE feature)
     *\brief        ����������
     *\param[in]    PFEATURE feature ��������
     *\return       int 0�ɹ�,����ʧ��
     */
    int getFeature(PFEATURE feature);

    /**
     *\fn           bool match(PFEATURE feature1, PFEATURE feature2)
     *\brief        �Ƚ�
     *\param[in]    PFEATURE feature1 ��������1
     *\param[in]    PFEATURE feature2 ��������2
     *\return       int ���ƶ�,Ĭ����ֵΪ50�����ƶ�>=50����Ϊ�ȶԳɹ�������ȶ�ʧ��
     */
    int match(PFEATURE feature1, PFEATURE feature2);
};
