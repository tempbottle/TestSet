#pragma once
#include "Network\Srv.h"
#include "Base\MD5.h"
#include <map>
#include <list>
#include <string>


namespace XT_IOCP
{

#define SERVERNAME		"MyWebServer/0.1"


	typedef std::map<std::string, void*>		MAP_STR_VOID;
	typedef std::map<std::string, std::string>	MAP_STR_STR;
	typedef std::list<std::string>				LIST_STR;

	/**
	 *\class  CSrv_HTTP
	 *\brief  ������Ҫ��HTTP����
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������
	 */
	class CSrv_HTTP : public CSrv
	{
	public:
		/**
		 *\fn         CSrv_HTTP()
		 *\brief      ȱʡ���캯����
	     */
		CSrv_HTTP();

		/**
		 *\fn         ~CSrv_HTTP()
		 *\brief      ����������
	     */
		~CSrv_HTTP();

	private:
		std::string		defaultHtmlFile_;	// Ĭ�ϵ��ļ�
		std::string		html404_;			// û���ҵ��ļ�
		std::string		root_;				// ��·��
		std::string		reqFile_;			// ������ļ�

		std::string		boundary_;			// POST���
		std::string		boundaryEnd_;		// POST���
		std::string		postFile_;			// POST���ļ���
		char			*postData_;			// POST�ļ�����
		int				postDataLen_;		// POST�ļ���С
		
		PCLIENT			client_;			// �ͻ���
		MAP_STR_VOID    htmlFiles_;			// ���ص��ļ�
		MAP_STR_STR		mimeTypes_;			// ����
		MAP_STR_STR		headers_;			// ����ͷ

    protected:
		/**
		 *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
		 *\brief      �������ݰ�
		 *\param[in]  client	�ͻ��˶���
		 *\param[in]  buf		���ݡ�
		 *\param[in]  len		���ݳ���
		 *\param[in]  ip		�Է�IP(UDP)��
		 *\param[in]  port		�Է��˿�(UDP)��
		 */
		void ProcessPack(void *client, void *buf, int len, int ip, short port);

		/**
		 *\fn         ParseRequstHead(const char *data, int len)
		 *\brief      ��������ͷ
		 *\param[in]  data		���ݡ�
		 *\param[in]  len		���ݳ���
		 *\return     ��������
		 */
		std::string ParseRequstHead(const char *data, int len);

		/**
		 *\fn         ProcessGET(PCLIENT client, const char *data, int len);
		 *\brief      ����GET����
		 *\param[in]  client	�ͻ��˶���
		 *\param[in]  data		���ݡ�
		 *\param[in]  len		���ݳ���
		 *\return     ��������
		 */
		void ProcessGET(PCLIENT client, const char *data, int len);
		
		/**
		 *\fn         ProcessPOST(PCLIENT client, const char *data, int len);
		 *\brief      ����POST����
		 *\param[in]  client	�ͻ��˶���
		 *\param[in]  data		���ݡ�
		 *\param[in]  len		���ݳ���
		 *\return     ��������
		 */
		std::string ProcessPOST(PCLIENT client, const char *data, int len);

		/**
		 *\fn         SendResponseData(PCLIENT client, const char *head, int headLen, 
		 *\n                           const char *data, int dataLen, char *pack)
		 *\brief      ����Ӧ������
		 *\param[in]  client	�ͻ��˶���
		 *\param[in]  head		ͷ�ڲ����ݡ�
		 *\param[in]  headLen	ͷ�ڲ����ݳ���
		 *\param[in]  data		�������ݡ�
		 *\param[in]  dataLen	�������ݳ���
		 *\param[in]  pack		���ݰ���
		 */
		void CSrv_HTTP::SendResponseData(PCLIENT client, const char *head, int headLen, 
			const char *data, int dataLen, char *pack);

		/**
		 *\fn         GetResponseHead(const char *statusCode, const char *reqFile, int dataLen)
		 *\brief      �õ�Ӧ��ͷ
		 *\param[in]  statusCode	״̬�롣
		 *\param[in]  reqFile		������ļ�����
		 *\param[in]  dataLen		���ݳ���
		 *\return	  �õ�Ӧ��ͷ������
		 */
		std::string GetResponseHead(const char *statusCode, const char *reqFile, int dataLen);

		/**
		 *\fn         GetHeadContent(const char *data, std::string &head, std::string &content)
		 *\brief      �õ�ͷ����Ϣ���ݡ�
		 *\param[in]  data		ͷ���ı���
		 *\param[out] head		ͷ�ڵ����ơ�
		 *\param[out] content	ͷ�ڵ����ݡ�
		 */
		void GetHeadContent(const char *data, std::string &head, std::string &content);

		/**
		 *\fn         CleanHead()
		 *\brief      ������ݡ�
		 */
		void CleanHead();

		/**
		 *\fn         LoadFile(const char *root)
		 *\brief      ��ʼhttp����
		 *\param[in]  root	��Ŀ¼��
		 */
		void LoadFile(const char *root);

		/**
		 *\fn         GetFiles(const char *root, const char *path, LIST_STR &files)
		 *\brief      �����ļ���������Ŀ¼��
		 *\param[in]  root	��Ŀ¼��
		 *\param[in]  path	���Ŀ¼��
		 *\param[out] files	�ļ���
		 */
		bool GetFiles(const char *root, const char *path, LIST_STR &files);

	public:
		/**
		 *\fn         Start(const char *ip, const int port, const char *root)
		 *\brief      ��ʼhttp����
		 *\param[in]  ip	ip��ַ��
		 *\param[in]  port	�˿ڡ�
		 *\param[in]  root	��Ŀ¼��
		 */
		int Start(const char *ip, const int port, const char *root);

		/**
		 *\fn         Post()
		 *\brief      Post���ݡ�
		 */
		void Post();

        /**
		 *\fn         getFileList()
		 *\brief      �õ��Ѿ����ص��ļ���
         *\return     �Ѿ����ص��ļ�
		 */
        MAP_STR_VOID* getFileList() { return &htmlFiles_; }
	};

}