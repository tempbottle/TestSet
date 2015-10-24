
#include "stdafx.h"
#include "MySQLDB.h"

#pragma comment(lib,"../10.thirdParty/lib/mysql/libmysql.lib")

/**
	// SqlServer
	// Command�����ڽ��д洢���̵ĵ�����������������������
	m_pCommand.CreateInstance("ADODB.Command");
	_variant_t vNULL;
	vNULL.vt = VT_ERROR;
	vNULL.scode = DISP_E_PARAMNOTFOUND;									// ����Ϊ�޲���
	m_pCommand->ActiveConnection = m_pConnection;						// �ǳ��ؼ���һ�䣬�����������Ӹ�ֵ����
	m_pCommand->CommandText = "SELECT * FROM users";					// �����ִ�
	m_pRecordset = m_pCommand- >Execute(&vNULL, &vNULL, adCmdText);		// ִ�����ȡ�ü�¼��
	
	m_pRecordset->Update();												// �Լ�¼�����޸ģ�����ؿ���

	exec pr_checkuservip @username='123123'								// ���ô洢����,�洢���̶���ı���@username


	// MySQL
	// ���Ӱ���Ŀ¼��	D:\Program Files\MySQL\MySQL Server 5.0\include
	// ���ӿ�Ŀ¼��		D:\Program Files\MySQL\MySQL Server 5.0\lib\opt
	// ���������		libmysql.dll
	show grants for user					// ��ʾuser��Ȩ��
	show global variables,show variables	// ��ʾ����
	set  collation_connection = gbk_bin;	// ���ñ���
	flush privileges						// ��ʾȨ��

	// ֻҪcharactor set database Ϊgb2312�Ϳ��Ե��ô洢����
**/

CMySQLDB::CMySQLDB()
{	
	memset(&mysql_, 0, sizeof(mysql_));
}

CMySQLDB::~CMySQLDB()
{
	Close();
}

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
int CMySQLDB::OpenDB(const char *host, unsigned short port, 
	const char *name, const char *username, const char *password)
{
	if (!mysql_init(&mysql_))
	{
		return -1;
	}

	if (mysql_options(&mysql_, MYSQL_OPT_RECONNECT, "1")) // �Զ�����
	{
		return -2;
	}

	if (!mysql_real_connect(&mysql_,
		host,
		username,
		password,
		name,
		port,
		NULL,
		REFRESH_QUERY_CACHE_FREE)) // ���ӾͲ��ܵ��ô洢����
	{
		return -3;
	}

	if (0 != mysql_query(&mysql_, "SET NAMES 'gb2312'")) // ���������ַ���
	{
		return -4;
	}

	timeout_ = GetWaitTimeout();

	nextSet_ = ::GetTickCount() + timeout_ * 500;

	return 0;
}

/**
 *\fn         Close()
 *\brief      �ر����ݿ�
 */
void CMySQLDB::Close(void)
{
	mysql_close(&mysql_);
}

/**
 *\fn         Exec(const char *sql)
 *\brief      ִ��SQL���
 *\param[in]  sql		SQL��䡣
 *\return     �Ƿ�ɹ�
 */
bool CMySQLDB::Exec(const char *sql)
{
	if (NULL == sql) return false;

	if (::GetTickCount() > nextSet_)
	{
		if (0 != mysql_query(&mysql_, "SET NAMES 'gb2312'"))	// ���������ַ���
		{
			return false;
		}

		nextSet_ = ::GetTickCount() + timeout_;
	}

	if (0 == mysql_query(&mysql_, sql))// 0�ɹ�
	{
		TRACE("OK! %s\n", sql);
		return true;
	}
	else
	{
		LOG(ERROR) << "CMySQLDB::ExecSQL Fail! " << mysql_error(&mysql_) << sql;
	}

	return false;
}

/**
 *\fn         Procedure(const char *name, const char *param, ...)
 *\brief      ���ô洢����
 *\param[in]  name		�洢��������
 *\param[in]  param		�洢���̲�����
 *\return     �Ƿ�ɹ�
 */
bool CMySQLDB::Procedure(const char *name, const char *param, ...)
{
	if (NULL == name) return false;

	if (::GetTickCount() > nextSet_)
	{
		if (0 != mysql_query(&mysql_, "SET NAMES 'gb2312'"))	// ���������ַ���
		{
			return false;
		}

		nextSet_ = ::GetTickCount() + timeout_;
	}

	char fullParam[MAX_PATH] = "";

	if (NULL != param)
	{
		va_list argptr;
		va_start(argptr, param);
		_vstprintf(fullParam, param, argptr);
		va_end(argptr);
	}

	char fullSql[MAX_PATH*2] = "";
	sprintf(fullSql, "CALL %s (%s);", name, fullParam);

	if (0 == mysql_query(&mysql_, fullSql))// 0�ɹ�
	{
		return true;
	}
	else
	{
		LOG(ERROR) << "CMySQLDB::Procedure Fail! " << mysql_error(&mysql_) << fullSql;

		return false;
	}
}

/**
 *\fn         InsertBlob(const char *tableName, const char *fieldName, char *data, int len)
 *\brief      �����ݱ��в���BLOB����������,������ݴ���1M����Ҫ���� max_allowed_packet ����
 *\param[in]  tableName		������
 *\param[in]  fieldName		�ֶ�����
 *\param[in]  data			���ݡ�
 *\param[in]  len			���ݳ���
 *\return     �Ƿ�ɹ�
 */
bool CMySQLDB::InsertBlob(const char *tableName, const char *fieldName, char *data, int len)
{
	if ((NULL == tableName) ||
		(NULL == fieldName) ||
		(NULL == data))
	{
		return false;
	}

	MYSQL_STMT*	mysqlStmt = mysql_stmt_init(&mysql_); // Ԥ�������

	if (NULL != mysqlStmt)
	{
		char sql[MAX_PATH] = "";
		sprintf(sql, "UPDATE %s SET %s = ?", tableName, fieldName); // ?����ľ���Ҫ���������

		if (0 != mysql_stmt_prepare(mysqlStmt, sql, strlen(sql)))
		{
			TRACE("mysql_stmt_prepare failed!\n");
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;
			return false;
		}

		MYSQL_BIND mysqlBind = {0};
		mysqlBind.buffer_type = MYSQL_TYPE_BLOB;
		mysqlBind.buffer_length = len;
		mysqlBind.is_null = 0;
		mysqlBind.length = (unsigned long *)&len;
		mysqlBind.buffer = data;

		if (0 != mysql_stmt_bind_param(mysqlStmt, &mysqlBind))
		{
			TRACE("BIND ERROR!\n");
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;
			return false;
		}

		if (0 != mysql_stmt_execute(mysqlStmt))
		{
			TRACE("EXECUTE ERROR! %s\n", mysql_error(&mysql_));
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;				
			return false;
		}

		mysql_stmt_close(mysqlStmt);
		mysqlStmt = NULL;

		return true;
	}

	return false;
}

/**
 *\fn         GetBlob(const char *tableName, const char *fieldName, char *data, int len)
 *\brief      �����ݱ��еõ�BLOB����������
 *\param[in]  tableName		������
 *\param[in]  fieldName		�ֶ�����
 *\param[in]  data			���ݡ�
 *\param[in]  len			���ݳ���
 *\return     �Ƿ�ɹ�
 */
bool CMySQLDB::GetBlob(const char *tableName, const char *fieldName, char *data, int &len)
{
	if ((NULL == tableName) ||
		(NULL == fieldName) ||
		(NULL == data))
	{
		return false;
	}

	MYSQL_STMT*	mysqlStmt = mysql_stmt_init(&mysql_); // Ԥ�������

	if (NULL != mysqlStmt)
	{
		char sql[MAX_PATH] = "";
		sprintf(sql, "SELECT %s FROM %s", tableName, fieldName); // ?����ľ���Ҫ���������

		if (0 != mysql_stmt_prepare(mysqlStmt, sql, strlen(sql)))
		{
			TRACE("mysql_stmt_prepare failed!\n");
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;
			return false;
		}

		ULONG count= mysql_stmt_param_count(mysqlStmt);

		if (0 != count)
		{
			TRACE("mysql_stmt_param_count() failed!\n");
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;
			return false;
		}

		MYSQL_RES *mysqlResult = mysql_stmt_result_metadata(mysqlStmt);

		if (NULL == mysqlResult)
		{
			TRACE("mysql_stmt_result_metadata() failed!\n");
			mysql_stmt_close(mysqlStmt);
			mysqlStmt = NULL;
			return false;
		}

		if (0 != mysql_stmt_execute(mysqlStmt))
		{
			TRACE("EXECUTE ERROR!\n");
			mysql_free_result(mysqlResult);
			mysql_stmt_close(mysqlStmt);
			mysqlResult = NULL;
			mysqlStmt = NULL;				
			return FALSE;
		}

		my_bool bNULL = 0;

		MYSQL_BIND mysqlBind_;
		memset(&mysqlBind_, 0, sizeof(mysqlBind_));
		mysqlBind_.buffer_type = MYSQL_TYPE_BLOB;
		mysqlBind_.buffer_length = len;
		mysqlBind_.is_null = &bNULL;
		mysqlBind_.length = (unsigned long *)&len;
		mysqlBind_.buffer = data;
		len = 0;

		if (mysql_stmt_bind_result(mysqlStmt, &mysqlBind_))
		{
			TRACE(_T("mysql_stmt_bind_result() failed\n"));
			mysql_free_result(mysqlResult);
			mysql_stmt_close(mysqlStmt);
			mysqlResult = NULL;
			mysqlStmt = NULL;
			return false;
		}

		if (mysql_stmt_store_result(mysqlStmt))
		{
			TRACE("mysql_stmt_store_result() failed\n");
			mysql_free_result(mysqlResult);
			mysql_stmt_close(mysqlStmt);
			mysqlResult = NULL;
			mysqlStmt = NULL;
			return false;
		}

		while (!mysql_stmt_fetch(mysqlStmt))// �õ����е�һ��(�����ж���)
		{
		}

		mysql_free_result(mysqlResult);
		mysql_stmt_close(mysqlStmt);
		mysqlResult = NULL;
		mysqlStmt = NULL;

		return true;
	}

	return false;
}

/**
 *\fn         GetWaitTimeout()
 *\brief      �õ��ȴ���ʱֵ
 *\return     ��ʱֵ
 */
int CMySQLDB::GetWaitTimeout()
{
    if (0 != mysql_query(&mysql_, "show global variables like 'wait_timeout';"))// 0�ɹ�
    {
        LOG(ERROR) << "CMySQLDB::GetWaitTimeout Fail! Error:" << mysql_error(&mysql_);
        return 0;
    }
    else
    {
        VECTOR_STR data;
        int rows = 0;
        int fields = 0;

        GetData(data, rows, fields);

        return atoi(data[1].c_str());
    }
}

/**
 *\fn         GetData(VECTOR_STR &data, int &rows, int &fields)
 *\brief      �õ�����
 *\param[out] data		���ݡ�
 *\param[out] rows		��¼������
 *\param[out] fields	�ֶθ�����
 *\return     �Ƿ�ɹ�
 */
bool CMySQLDB::GetData(VECTOR_STR &data, int &rows, int &fields)
{
	MYSQL_RES *mysqlResult = mysql_store_result(&mysql_);

	if (NULL != mysqlResult)
	{
		rows = mysql_num_rows(mysqlResult);
		fields = mysql_num_fields(mysqlResult);

		for (int i = 0; i < rows; i++)
		{
			MYSQL_ROW mysqlRow = mysql_fetch_row(mysqlResult);

			for (int j = 0; j < fields; j++)
			{
				data.push_back(mysqlRow[j]);
			}
		}
	}

	mysql_free_result(mysqlResult);

	for( ; !mysql_next_result(&mysql_););//���û�д���䣬�Ͳ�����ִ��SQL�����

	if (rows > 0)
	{
		return true;
	}

	return false;
}

/**
 *\fn         GetDataStr()
 *\brief      �õ�SELECT�������
 *\return     �ַ�������
 */
std::string CMySQLDB::GetDataStr()
{
	std::string data;

	MYSQL_RES *mysqlResult = mysql_store_result(&mysql_);

	if (NULL != mysqlResult)
	{
		int rows = mysql_num_rows(mysqlResult);
		int fields = mysql_num_fields(mysqlResult);

		std::string data;

		MYSQL_FIELD *mysqlField = NULL;

		while (mysqlField = mysql_fetch_field(mysqlResult))
		{
			data += mysqlField->name;
			data += " ";
		}

		data += "\n";

		for (int i = 0; i < rows; i++)
		{
			MYSQL_ROW mysqlRow = mysql_fetch_row(mysqlResult);

			for (int j = 0; j < fields; j++)
			{
				data += mysqlRow[j];
				data += " ";
			}
		}
	}

	mysql_free_result(mysqlResult);
	for( ; ! mysql_next_result(&mysql_););//���û�д���䣬�Ͳ�����ִ��SQL�����

	return data;
}