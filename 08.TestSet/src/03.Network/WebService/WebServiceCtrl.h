
#ifndef TESTSET_WEBSERVICE_H_
#define TESTSET_WEBSERVICE_H_

#pragma once


/**
 *\class  WebServiceCtrl
 *\brief  ������ҪWebService����
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class WebServiceCtrl
{
public:
	/**
	 *\fn         WebServiceCtrl()
	 *\brief      ȱʡ���캯����
     */
	WebServiceCtrl(void);

	/**
	 *\fn         WebServiceCtrl()
	 *\brief      ����������
     */
	~WebServiceCtrl(void);

	/**
	 *\fn         getWeatherbyCityName(const char *city)
	 *\brief      ͨ���������õ�������
	 *\param[in]  city ��������
	 */
	void getWeatherbyCityName(const char *city);

};

#endif // TESTSET_WEBSERVICE_H_

