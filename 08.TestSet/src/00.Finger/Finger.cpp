#include "stdafx.h"
#include "Finger.h"
#include "FingerImageProcess.h"

FingerIdentify::FingerIdentify()
    :cx_(0),
    cy_(0),
    data_(NULL),
    thin_clear_(NULL),
    orient_smooth_(NULL)
{
    memset(&end_,  0, sizeof(end_));
    memset(&fork_, 0, sizeof(fork_));
    memset(&bfh_,  0, sizeof(bfh_));
    memset(&bih_,  0, sizeof(bih_));

    data_ = new uint8[1024*1024];
}

FingerIdentify::~FingerIdentify()
{
    if (NULL != data_)
    {
        delete data_;
    }

    if (NULL != thin_clear_)
    {
        delete thin_clear_;
    }

    if (NULL != orient_smooth_)
    {
        delete orient_smooth_;
    }
}

/**
 *\fn           int loadBitmap(const char *filename)
 *\brief        ����ͼ���ļ�
 *\param[in]    const char * filename �ļ���
 *\return       int 0�ɹ�,����ʧ��
 */
int FingerIdentify::loadBitmap(const char *filename)
{
    return ::loadBitmap(filename, data_, &bfh_, &bih_, &cx_, &cy_);
}

/**
 *\fn           int preprocess()
 *\brief        ͼ��Ԥ����
 *\return       int 0�ɹ�,����ʧ��
 */
int FingerIdentify::preprocess()
{
    if (NULL != thin_clear_) delete thin_clear_;
    if (NULL != orient_smooth_) delete orient_smooth_;

    // ͼ�����
    uint8 *equaliz = new uint8[cx_*cy_];
    equalize(data_, equaliz, cx_, cy_);

    // ��˹ƽ��
    uint8 *gauss = new uint8[cx_*cy_];
    gaussSmooth(data_, gauss, cx_, cy_, 0.4);

    // ͼ��ƽ��
    uint8 *smooth1 = new uint8[cx_*cy_];
    smooth(data_, smooth1, cx_, cy_, 1, 1);

    // ͼ��ƽ��
    uint8 *smooth2 = new uint8[cx_*cy_];
    smooth(smooth1, smooth2, cx_, cy_, 1, 1);

    // ͼ����С
    uint8 *zoom = new uint8[cx_*cy_];
    zoomout(smooth2, zoom, cx_, cy_);

    // �ݶȳ�
    uint8 *grads = new uint8[cx_*cy_];
    getGradsFromZoomout(zoom, grads, cx_, cy_, 6);

    // ����ģ��
    uint8 *background = new uint8[cx_*cy_];
    getBackgroundTemplate(grads, background, 12, 35, cx_, cy_);

    // ����
    uint8 *orient = new uint8[cx_*cy_];
    getOrientFromZoomout(zoom, orient, cx_, cy_, 6);

    // ����ƽ��
    orient_smooth_ = new uint8[cx_*cy_];
    smooth(orient, orient_smooth_, cx_, cy_, 1, 1);

    // ��2��ƽ����ͼ����ǿ
    uint8 *smooth2_enhance = new uint8[cx_*cy_];
    orientEnhance(smooth2, smooth2_enhance, orient, cx_, cy_);

    // ��ֵ��
    uint8 *binary_data = new uint8[cx_*cy_];
    binary(smooth2_enhance, binary_data, orient, cx_, cy_);

    // ȥ�����ͱ���
    uint8 *binary_clear = new uint8[cx_*cy_];
    binaryClear(binary_data, binary_clear, background, cx_, cy_);

    // ϸ��
    uint8 *thin_data = new uint8[cx_*cy_];
    thin(binary_clear, thin_data, cx_, cy_);

    // ȥ�̰���ë��
    thin_clear_ = new uint8[cx_*cy_];
    thinClear(thin_data, thin_clear_, cx_, cy_, 12);

    if (NULL != path_)
    {
        char path[1024] = "";
        sprintf(path, "%s\\finger_equaliz.bmp", path_);
        saveBitmap(path, equaliz, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_smooth_gauss.bmp", path_);
        saveBitmap(path, gauss, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_smooth1.bmp", path_);
        saveBitmap(path, smooth1, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_smooth2.bmp", path_);
        saveBitmap(path, smooth2, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_zoomout.bmp", path_);
        saveBitmap(path, zoom, &bfh_, &bih_, cx_ / 2, cy_ / 2, false);

        sprintf(path, "%s\\finger_grads.bmp", path_);
        saveBitmap(path, grads, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_background.bmp", path_);
        saveBitmap(path, background, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_orient.bmp", path_);
        saveBitmap(path, orient, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_orient_smooth.bmp", path_);
        saveBitmap(path, orient_smooth_, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_smooth2_enhance.bmp", path_);
        saveBitmap(path, smooth2_enhance, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_binary.bmp", path_);
        saveBitmap(path, binary_data, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_thin.bmp", path_);
        saveBitmap(path, thin_data, &bfh_, &bih_, cx_, cy_, false);

        sprintf(path, "%s\\finger_thin_clear.bmp", path_);
        saveBitmap(path, thin_clear_, &bfh_, &bih_, cx_, cy_, false);
    }

    delete equaliz;
    delete gauss;
    delete smooth1;
    delete smooth2;
    delete zoom;
    delete grads;
    delete background;
    delete orient;
    delete smooth2_enhance;
    delete binary_data;
    delete binary_clear;
    delete thin_data;

    return 0;
}

/**
 *\fn           int getFeature(PFEATURE feature)
 *\brief        ����������
 *\param[in]    PFEATURE feature ��������
 *\return       int 0�ɹ�,����ʧ��
 */
int FingerIdentify::getFeature(PFEATURE feature)
{
    getRawMinutia(thin_clear_, orient_smooth_, cx_, cy_, &end_, &fork_);
    filteMinutia(thin_clear_, cx_, cy_, &end_, &fork_);
    makeFeature(thin_clear_, orient_smooth_, cx_, cy_, &end_, &fork_);

    if (NULL != path_)
    {
        flagMinutia(thin_clear_, cx_, cy_, &end_, &fork_);

        char path[1024] = "";
        sprintf(path, "%s\\finger_minutia.bmp", path_);
        saveBitmap(path, thin_clear_, &bfh_, &bih_, cx_, cy_, true);
    }

    for (int i = 0; i < fork_.minutiaNum; i++)
    {
        end_.minutia[end_.minutiaNum + i] = fork_.minutia[i];
    }

    end_.minutiaNum += fork_.minutiaNum;

    if (NULL != feature)
    {
        *feature = end_;
    }

    return 0;
}

/**
 *\fn           bool match(PFEATURE feature1, PFEATURE feature2)
 *\brief        �Ƚ�
 *\param[in]    PFEATURE feature1 ��������1
 *\param[in]    PFEATURE feature2 ��������2
 *\return       int ���ƶ�,Ĭ����ֵΪ50�����ƶ�>=50����Ϊ�ȶԳɹ�������ȶ�ʧ��
 */
int FingerIdentify::match(PFEATURE feature1, PFEATURE feature2)
{
    if (NULL == feature1 || NULL == feature2 || feature1->minutiaNum <= 0 || feature2->minutiaNum <= 0)
    {
        return false;
    }

    MATCHRESULT result = {0};

    matchFeature(feature1, feature2, &result, 1);

    return result.similarity;
}
