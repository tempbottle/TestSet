#pragma once


/**
 *\class  PackID
 *\brief  ���ݰ�ID
 *\date   2012-07-17
 *\author �ź���
 *\par    Revision History:
 */
class PackID
{
public:
	/**
	 *\fn         Pool()
	 *\brief      ȱʡ���캯����
	 */
	PackID() {}

	/**
	 *\fn         Pool()
	 *\brief      ȱʡ���캯����
	 */
	~PackID() {}

private:
	unsigned int id_;
	CRITICAL_SECTION csUsed_;

public:
	unsigned int getPackId() { return ++id_; }
};
