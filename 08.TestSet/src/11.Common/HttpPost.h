#pragma once


/**
 *\class  CHttpPost
 *\brief  ������Ҫ����HTTP��POST����
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class CHttpPost
{
public:
	/**
	 *\fn         CHttpPost()
	 *\brief      ȱʡ���캯����
     */
	CHttpPost();

	/**
	 *\fn         CHttpPost(const char *ip, unsigned short port, const char *path)
	 *\brief      ���캯����
	 *\param[in]  ip   IP��ַ��
	 *\param[in]  port �˿ڵ�ַ��
	 *\param[in]  path ·����
     */
	CHttpPost(const char *ip, unsigned short port, const char *path);

	/**
	 *\fn         ~CHttpPost()
	 *\brief      ����������
     */
	~CHttpPost();

private:
	std::string ip_;

	unsigned short port_;

	std::string path_;

	int status_;

	char *response_;

	DWORD responseLen_;


public:
	/**
	 *\fn         setAddr(const char *ip, unsigned short port, const char *path)
	 *\brief      ���õ�ַ������
	 *\param[in]  ip   IP��ַ��
	 *\param[in]  port �˿ڵ�ַ��
	 *\param[in]  path ·����
	 */
	void setAddr(const char *ip, unsigned short port, const char *path) { ip_ = ip; port_ = port; path_ = path; }

	/**
	 *\fn         SendPostRequest(void *data, int len, const char *filename)
	 *\brief      �������ݡ�
	 *\param[in]  data     ���ݡ�
	 *\param[in]  len      ���ݳ���
	 *\param[in]  filename �ļ�����
	 *\return     �Ƿ�ɹ�
	 */
	bool SendPostRequest(void *data, int len, const char *filename);

	/**
	 *\fn         getResponse()
	 *\brief      �õ�Ӧ�����ݡ�
	 *\return     Ӧ������
	 */
	void* getResponse() { return response_; }

	/**
	 *\fn         getResponseLen()
	 *\brief      �õ�Ӧ�����ݳ��ȡ�
	 *\return     Ӧ�����ݳ���
	 */
	int getResponseLen() { return responseLen_; }

	/**
	 *\fn         getStatusCode()
	 *\brief      �õ�״̬�롣
	 *\return     ״̬��
	 */
	int  getStatusCode() { return status_; }
};
