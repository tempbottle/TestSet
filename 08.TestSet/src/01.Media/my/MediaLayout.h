
#pragma once

typedef struct _tagFRAMEINFO
{
    BYTE type;      // 1��Ƶ,2��Ƶ
    WORD sleep;     // �ȴ�ʱ��
    int  len;       // ֡���ݳ�,�������
    void *data;     // ����

} FRAMEINFO, *PFRAMEINFO;

/**
 *\class  MeidaLayout
 *\brief  ������Ҫ�ǲ������
 *\date   2012-12-26
 *\author �ź���
 *\par    Revision History:
 */
class MeidaLayout
{
public:
    /**
     *\fn         P2PLayoutBase()
     *\brief      ȱʡ���캯����
     */
    MeidaLayout();

    /**
     *\fn         ~P2PLayoutBase()
     *\brief      ����������
     */
    ~MeidaLayout();

protected:
    MeidaLayout *pre_;  // ��һ���ڵ�
    MeidaLayout *next_; // ��һ���ڵ�

public:
    /**
     *\fn         int send(void *data)
     *\brief      �������ݡ�
     *\param[in]  data           ���ݰ���
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    virtual int send(void *data);

    /**
     *\fn         int recv(void *data)
     *\brief      �������ݡ�
     *\param[in]  data           ���ݰ���
     *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ�ܡ�
     */
    virtual int recv(void *data);

    /**
     *\fn         void setPre(MeidaLayout *layout)
     *\brief      ������һ��ڵ㡣
     *\param[in]  layout        ��ڵ㡣
     *\return     �ޡ�
     */
    void setPre(MeidaLayout *layout) { pre_ = layout; }

    /**
     *\fn         void setNext(MeidaLayout *layout)
     *\brief      ������һ��ڵ㡣
     *\param[in]  layout        ��ڵ㡣
     *\return     �ޡ�
     */
    void setNext(MeidaLayout *layout) { next_ = layout; }

    /**
     *\fn         MeidaLayout* getPre()
     *\brief      �õ���һ��ڵ㡣
     *\param[in]  �ޡ�
     *\return     ��һ��ڵ�ָ�롣
     */
    MeidaLayout* getPre() { return pre_; }

    /**
     *\fn         MeidaLayout* getNext()
     *\brief      �õ���һ��ڵ㡣
     *\param[in]  �ޡ�
     *\return     ��һ��ڵ�ָ�롣
     */
    MeidaLayout* getNext() { return next_; }

};

