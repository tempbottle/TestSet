
#pragma once
#include "MySQL/mysql.h"
#include <vector>
#include <string>

typedef std::vector<std::string> VECTOR_STR;




/**
 *\class  CMySQLDB
 *\brief  ������Ҫ��MySQL��
 *\date   2012-07-03
 *\author �ź���
 *\par    Revision History:
 *\n      0.1     2007-07-03   �ź���   ��������
 */
class CMySQLDB
{
public:
	/**
	 *\fn         CMySQLDB()
	 *\brief      ȱʡ���캯����
	 */
	CMySQLDB();

	/**
	 *\fn         ~CMySQLDB()
	 *\brief      ����������
	 */
	~CMySQLDB();

protected:
	MYSQL			mysql_;	  // MySQL����
    int				nextSet_; // ��һ�������ַ���ʱ��
    int				timeout_; // ��ʱʱ��

public:
	/**
	 *\fn         OpenDB(const char *host, unsigned short port, const char *name, const char *username, const char *password)
	 *\brief      �����ݿ�
	 *\param[in]  host		������ַ��
	 *\param[in]  port		�����˿ڡ�
	 *\param[in]  name		���ݿ�����
	 *\param[in]  username	�û�����
	 *\param[in]  password	�û����롣
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int OpenDB(const char *host, unsigned short port, const char *name, const char *username, const char *password);	
	
	/**
	 *\fn         Close()
	 *\brief      �ر����ݿ�
	 */
	void Close();	

	/**
	 *\fn         Exec(const char *sql)
	 *\brief      ִ��SQL���
	 *\param[in]  sql		SQL��䡣
	 *\return     �Ƿ�ɹ�
	 */
	bool Exec(const char *sql);
	
	/**
	 *\fn         Procedure(const char *name, const char *param, ...)
	 *\brief      ���ô洢����
	 *\param[in]  name		�洢��������
	 *\param[in]  param		�洢���̲�����
	 *\return     �Ƿ�ɹ�
	 */
	bool Procedure(const char *name, const char *param, ...);

	/**
	 *\fn         InsertBlob(const char *tableName, const char *fieldName, char *data, int len)
	 *\brief      �����ݱ��в���BLOB����������,������ݴ���1M����Ҫ���� max_allowed_packet ����
	 *\param[in]  tableName		������
	 *\param[in]  fieldName		�ֶ�����
	 *\param[in]  data			���ݡ�
	 *\param[in]  len			���ݳ���
	 *\return     �Ƿ�ɹ�
	 */
	bool InsertBlob(const char *tableName, const char *fieldName, char *data, int len);
	
	/**
	 *\fn         GetBlob(const char *tableName, const char *fieldName, char *data, int len)
	 *\brief      �����ݱ��еõ�BLOB����������
	 *\param[in]  tableName		������
	 *\param[in]  fieldName		�ֶ�����
	 *\param[in]  data			���ݡ�
	 *\param[in]  len			���ݳ���
	 *\return     �Ƿ�ɹ�
	 */
	bool GetBlob(const char *tableName, const char *fieldName, char *data, int &len);

	/**
	 *\fn         GetWaitTimeout()
	 *\brief      �õ��ȴ���ʱֵ
	 *\return     ��ʱֵ
	 */
    int GetWaitTimeout();
	
	/**
	 *\fn         GetData(VECTOR_STR &data, int &rows, int &fields)
	 *\brief      �õ�����
	 *\param[out] data		���ݡ�
	 *\param[out] rows		��¼������
	 *\param[out] fields	�ֶθ�����
	 *\return     �Ƿ�ɹ�
	 */
	bool GetData(VECTOR_STR &data, int &rows, int &fields);

	/**
	 *\fn         GetDataStr()
	 *\brief      �õ�SELECT�������
	 *\return     �ַ�������
	 */
	std::string GetDataStr();
};

