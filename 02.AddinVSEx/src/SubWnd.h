/*************************************************
 * VS Addin
 * Copyright (C) XT. Co., Ltd.
 * File name:       SubWnd.h
 * Author:          zht
 * Version:         1.0.0
 * Date:            2014.9.4
 * Description:     VS���������ඨ��
*************************************************/

#pragma once

enum
{
    MOUSE_NORMAL,       /*�������*/
    MOUSE_DOWN,         /*��갴��*/
    MOUSE_UP,           /*���̧��*/
};

enum
{
    SUB_TYPE_TERMINAL,  /*�ն�*/
    SUB_TYPE_BROWSE,    /*�����*/
};

class CSubWnd
{
public:
    CSubWnd() { type_ = 0; }
    ~CSubWnd() {}

public:
    int type_;

public:
    /**
     *\fn           void destroy()
     *\brief        �ͷŴ���,����Դ
     *\return       void ��
     */
    virtual void destroy() {}
    /**
     *\fn           void showWindow(int cmd)
     *\brief        ��ʾ����
     *\param[in]    int cmd TRUE-��ʾ,FALSE-����
     *\return       void ��
     */
    virtual void showWindow(int /*cmd*/) {}

    /**
     *\fn           void mouseMove(int x, int y, int type)
     *\brief        ����ƶ�
     *\param[in]    int x Xλ��
     *\param[in]    int y Yλ��
     *\param[in]    int type 0-����,1-��갴��,2-���̧��
     *\return       void ��
     */
    virtual void mouseMove(int /*x*/, int /*y*/, int /*type*/) {}

    /**
     *\fn           void setWndRect(int x, int y, int cx, int cy)
     *\brief        ���ô����Сλ��
     *\param[in]    int x Xλ��
     *\param[in]    int y Yλ��
     *\param[in]    int cx ��
     *\param[in]    int cy ��
     *\return       void ��
     */
    virtual void setWndRect(int /*x*/, int /*y*/, int /*cx*/, int /*cy*/) {}
};


