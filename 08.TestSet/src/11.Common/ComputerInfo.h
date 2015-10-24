
#ifndef DESKDEPLOY_COMPUTERINFO_H_
#define DESKDEPLOY_COMPUTERINFO_H_

#pragma once
#include <vector>
#include <string>

typedef std::vector<std::string> VECTOR_STR;


/**
 *\class  ComputerInfo
 *\brief  ������Ҫ�õ�����IP��MAC��ַ
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class ComputerInfo
{
public:
	/**
	 *\fn         ComputerInfo()
	 *\brief      ȱʡ���캯����
     */
	ComputerInfo();

	/**
	 *\fn         ~ComputerInfo()
	 *\brief      ����������
	 */
	~ComputerInfo();

	/**
	 *\fn         getIpCount()
	 *\brief      ����XPϵͳ��������
	 *\return     ���ر���IP��ַ������
	 */
	int getIpCount() { return ipAddr_.size(); }

	/**
	 *\fn         getIpStr()
	 *\brief      �õ�IP��ַ��
	 *\param[in]  data		��š�
	 *\return     ���ر���IP��ַ����
	 */
	const char * getIpStr(int id) { return ipAddr_[id].c_str(); }

	/**
	 *\fn         getMacStr()
	 *\brief      �õ�MAC��ַ��
	 *\param[in]  data		��š�
	 *\return     ���ر���MAC��ַ����
	 */
	const char * getMacStr(int id) { return macAddr_[id].c_str(); }

	/**
	 *\fn         GetComputerName()
	 *\brief      ����Win7ϵͳ��������
	 *\return     ���ر���MAC��ַ����
	 */
	const char * getComputerName() { return computerName_.c_str(); }

	/**
	 *\fn         Init()
	 *\brief      ��ʹ����
	 */
	void Init();

	/**
	*\fn         GetUpSpeed()
	*\brief      �õ��������������ٶ�
	*\return     �������������ٶ�
	*/
	int GetUpSpeed();

	/**
	*\fn         GetDownSpeed()
	*\brief      �õ��������������ٶ�
	*\return     �������������ٶ�
	*/
	int GetDownSpeed();

private:
	/**
	*\fn         GetUpBytes()
	*\brief      �õ��������������ֽ���
	*\return     �������������ֽ���
	*/
	int GetUpBytes();

	/**
	*\fn         GetDownBytes()
	*\brief      �õ��������������ֽ���
	*\return     �������������ֽ���
	*/
	int GetDownBytes();

	VECTOR_STR ipAddr_;
	VECTOR_STR macAddr_;
	std::string computerName_;

	int upBytes_;
	int downBytes_;
	int upTick_;
	int downTick_;
};



#endif // DESKDEPLOY_COMPUTERINFO_H_