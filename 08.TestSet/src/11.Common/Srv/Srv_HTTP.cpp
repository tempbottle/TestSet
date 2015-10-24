#include "StdAfx.h"
#include "Srv_HTTP.h"
#include "Common\StrConver.h"


namespace XT_IOCP
{

	/**
	 *\fn         CSrv_HTTP()
	 *\brief      ȱʡ���캯����
	 */
	CSrv_HTTP::CSrv_HTTP(void)
		:postData_(NULL),
		postDataLen_(0),
		client_(NULL)
	{
		CleanHead();

		// Init MIME Types
		mimeTypes_["doc"]	= "application/msword";
		mimeTypes_["bin"]	= "application/octet-stream";
		mimeTypes_["dll"]	= "application/octet-stream";
		mimeTypes_["exe"]	= "application/octet-stream";
		mimeTypes_["pdf"]	= "application/pdf";
		mimeTypes_["p7c"]	= "application/pkcs7-mime";
		mimeTypes_["ai"]	= "application/postscript";
		mimeTypes_["eps"]	= "application/postscript";
		mimeTypes_["ps"]	= "application/postscript";
		mimeTypes_["rtf"]	= "application/rtf";
		mimeTypes_["fdf"]	= "application/vnd.fdf";
		mimeTypes_["arj"]	= "application/x-arj";
		mimeTypes_["gz"]	= "application/x-gzip";
		mimeTypes_["class"]	= "application/x-java-class";
		mimeTypes_["js"]	= "application/x-javascript";
		mimeTypes_["lzh"]	= "application/x-lzh";
		mimeTypes_["lnk"]	= "application/x-ms-shortcut";
		mimeTypes_["tar"]	= "application/x-tar";
		mimeTypes_["hlp"]	= "application/x-winhelp";
		mimeTypes_["cert"]	= "application/x-x509-ca-cert";
		mimeTypes_["zip"]	= "application/zip";
		mimeTypes_["rar"]	= "application/rar";
		mimeTypes_["cab"]	= "application/x-compressed";
		mimeTypes_["arj"]	= "application/x-compressed";
		mimeTypes_["aif"]	= "audio/aiff";
		mimeTypes_["aifc"]	= "audio/aiff";
		mimeTypes_["aiff"]	= "audio/aiff";
		mimeTypes_["au"]	= "audio/basic";
		mimeTypes_["snd"]	= "audio/basic";
		mimeTypes_["mid"]	= "audio/midi";
		mimeTypes_["rmi"]	= "audio/midi";
		mimeTypes_["mp3"]	= "audio/mpeg";
		mimeTypes_["vox"]	= "audio/voxware";
		mimeTypes_["wav"]	= "audio/wav";
		mimeTypes_["ra"]	= "audio/x-pn-realaudio";
		mimeTypes_["ram"]	= "audio/x-pn-realaudio";
		mimeTypes_["bmp"]	= "image/bmp";
		mimeTypes_["gif"]	= "image/gif";
		mimeTypes_["jpeg"]	= "image/jpeg";
		mimeTypes_["jpg"]	= "image/jpeg";
		mimeTypes_["tif"]	= "image/tiff";
		mimeTypes_["tiff"]	= "image/tiff";
		mimeTypes_["xbm"]	= "image/xbm";
		mimeTypes_["wrl"]	= "model/vrml";
		mimeTypes_["htm"]	= "text/html";
		mimeTypes_["html"]	= "text/html";
		mimeTypes_["c"]		= "text/plain";
		mimeTypes_["cpp"]	= "text/plain";
		mimeTypes_["def"]	= "text/plain";
		mimeTypes_["h"]		= "text/plain";
		mimeTypes_["txt"]	= "text/plain";
		mimeTypes_["rtx"]	= "text/richtext";
		mimeTypes_["rtf"]	= "text/richtext";
		mimeTypes_["java"]	= "text/x-java-source";
		mimeTypes_["css"]	= "text/css";
		mimeTypes_["mpeg"]	= "video/mpeg";
		mimeTypes_["mpg"]	= "video/mpeg";
		mimeTypes_["mpe"]	= "video/mpeg";
		mimeTypes_["avi"]	= "video/msvideo";
		mimeTypes_["mov"]	= "video/quicktime";
		mimeTypes_["qt"]	= "video/quicktime";
		mimeTypes_["shtml"]	= "wwwserver/html-ssi";
		mimeTypes_["asa"]	= "wwwserver/isapi";
		mimeTypes_["asp"]	= "wwwserver/isapi";
		mimeTypes_["cfm"]	= "wwwserver/isapi";
		mimeTypes_["dbm"]	= "wwwserver/isapi";
		mimeTypes_["isa"]	= "wwwserver/isapi";
		mimeTypes_["plx"]	= "wwwserver/isapi";
		mimeTypes_["url"]	= "wwwserver/isapi";
		mimeTypes_["cgi"]	= "wwwserver/isapi";
		mimeTypes_["php"]	= "wwwserver/isapi";
		mimeTypes_["wcgi"]	= "wwwserver/isapi";

		defaultHtmlFile_ = "/htm/Demo.htm";

		html404_ = "<HTML><HEAD><TITLE>404 Resource not found</TITLE></HEAD><BODY>404 Resource not found</BODY></HTML>";
	
#ifdef _DEBUG
        buffPool_.name_ = _T("http_buffPool_");
        inputQueue_.name_ = _T("http_inputQueue_");
        outputQueue_.name_ = _T("http_outputQueue_");
#endif // _DEBUG        
    }

	/**
	 *\fn         ~CSrv_HTTP()
	 *\brief      ����������
	 */
	CSrv_HTTP::~CSrv_HTTP(void)
	{
		MAP_STR_VOID::iterator iter = htmlFiles_.begin();

		for (; iter != htmlFiles_.end(); ++iter)
		{
			if (iter->second != NULL)
			{
				delete[] (char*)iter->second;
			}
		}
	}

	/**
	 *\fn         CleanHead()
	 *\brief      ������ݡ�
	 */
	void CSrv_HTTP::CleanHead()
	{
		// Ӧ����ͻ���:״̬��Ϊ206,Content-Range: bytes 5275648-15143085/15143086,��5275648��15143085,�ļ�ȫ��15143086

		headers_["Accept"] = "";				// �ͻ��˿�ʶ������������б�,�����ȼ�����
		headers_["Accept-Language"] = "";		// �ͻ������ڽ��ܵ���Ȼ�����б�,��:zh,cn
		headers_["Accept-Encoding"] = "";		// �ͻ��˿������ı������,��:gzip,deflate compress
		headers_["Accept-Charset"] = "";		// �ͻ��˿��Դ�����ַ���,�����ȼ�����
		headers_["Cache-Control"] = "";			// ����:no-cache
		headers_["Connection"] = "";			// ���Ӳ���:keep-alive,close
		headers_["Content-Disposition"] = "";	// ��������,POSTʱ,Content-Disposition: form-data; name="test.txt"
		headers_["Content-Length"] = "";		// ���ݳ�
		headers_["Content-Type"] = "";			// ���ݱ�������
		headers_["Cookie"] = "";				// COOKIE
		headers_["Date"] = "";					// ����ʱ��
		headers_["Host"] = "";					// ������ַ
		headers_["Keep-Alive"] = "";			// ����������ӵ�ʱ��
		headers_["Pragma"] = "";				// ��������no-cache
		headers_["Range"] = "";					// http�ϵ�����,�Ŀ�ʼ��,�� Range: bytes=5275648-
		headers_["Referer"] = "";				// ���ĸ���ַת��
		headers_["UA-CPU"] = "";				// �ͻ���CPU�ܹ�
		headers_["User-Agent"] = "";			// �ͻ��˱�ʶ

		reqFile_.clear();
		boundary_.clear();
		boundaryEnd_.clear();

		postFile_.clear();
	}

	/**
	 *\fn         Start(const char *ip, const int port, const char *root)
	 *\brief      ��ʼhttp����
	 *\param[in]  ip	ip��ַ��
	 *\param[in]  port	�˿ڡ�
	 *\param[in]  root	��Ŀ¼��
	 */
	int CSrv_HTTP::Start(const char *ip, const int port, const char *root)
	{
		root_ = root;

        LOG(INFO) << "HTTP����ʼ��������";

		LoadFile(root);

        LOG(INFO) << "HTTP��������������";

        google::FlushLogFiles(0);

		CSrv::Start(CMD_TYPE_HTTP, 1024*20);
		
		PCLIENT client = NULL;

		return iocp_.AddTcpListen(ip, port, &client);
	}

	/**
	 *\fn         LoadFile(const char *root)
	 *\brief      ��ʼhttp����
	 *\param[in]  root	��Ŀ¼��
	 */
	void CSrv_HTTP::LoadFile(const char *root)
	{
		LIST_STR files;

		if (this->GetFiles(root, "", files))
		{
			FILE *file = NULL;
			std::string path;

			LIST_STR::iterator iter = files.begin();

			for (; iter != files.end(); ++iter)
			{
				path = root_ + iter->c_str();

				if (file = fopen(path.c_str(), "rb"))
				{
					fseek(file, 0, SEEK_END);

					unsigned int fileLen = ftell(file);

					char *fileData = new char[fileLen+4];

					memcpy(fileData, &fileLen, sizeof(fileLen));	// ��¼�ļ�����

					fseek(file, 0, SEEK_SET);

					fread(&fileData[4], fileLen, 1, file);	// �ļ�����

					fclose(file);

					htmlFiles_[iter->c_str()] = fileData;

                    TRACE("Load file: %s\n", iter->c_str());
				}
			}
		}
	}

	/**
	 *\fn         GetFiles(const char *root, const char *path, LIST_STR &files)
	 *\brief      �����ļ���������Ŀ¼��
	 *\param[in]  root	��Ŀ¼��
	 *\param[in]  path	���Ŀ¼��
	 *\param[out] files	�ļ���
	 */
	bool CSrv_HTTP::GetFiles(const char *root, const char *path, LIST_STR &files)
	{
		bool ret = true;
		bool finished = false;    

		std::string name;

		WIN32_FIND_DATA fileData = {0};

		char fullPath[MAX_PATH] = "";
		strcpy(fullPath, root);
		strcat(fullPath, path);

		//int ret1 = chdir(fullPath);    // ���õ�ǰĿ¼
		SetCurrentDirectory(fullPath);

		HANDLE search = FindFirstFile("*.*", &fileData);
		if (search == INVALID_HANDLE_VALUE)
		{
			ret = false;
		}

		while (!finished && ret) 
		{
			if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)// ���ļ�
			{
				char file[MAX_PATH] = "";
				strcpy(file, path);
				strcat(file, "/");
				strcat(file, fileData.cFileName);
				files.push_back(file);
			}
			else// ��Ŀ¼
			{
				if ((strcmp(fileData.cFileName, ".") != 0) && (strcmp(fileData.cFileName, "..") != 0))
				{
					char shortPath[MAX_PATH] = "";	// ���·��
					strcpy(shortPath, path);
					strcat(shortPath, "/");
					strcat(shortPath, fileData.cFileName);
					GetFiles(root, shortPath, files);  // �ݹ����
				}
			}

			if (!FindNextFile(search, &fileData))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
				{
					finished = true;
				}
				else
				{
					ret = false;
				}
			}
		}

		FindClose(search);

		return ret;
	}

	/**
	 *\fn         GetHeadContent(const char *data, std::string &head, std::string &content)
	 *\brief      �õ�ͷ����Ϣ���ݡ�
	 *\param[in]  data		ͷ���ı���
	 *\param[out] head		ͷ�ڵ����ơ�
	 *\param[out] content	ͷ�ڵ����ݡ�
	 */
	void CSrv_HTTP::GetHeadContent(const char *data, std::string &head, std::string &content)
	{
		std::string dataStr(data);

		int index = dataStr.find(":");

		if (index != -1)
		{
			char name[MAX_PATH] = "";
			strncpy(name, data, index);

			head = name;
			content = &data[index + 1];

			return;
		}

		// ������GET��POST
		index = dataStr.find(" ");

		if (index != -1)
		{
			char name[MAX_PATH] = "";
			strncpy(name, data, index);

			head = name;
			content = &data[index + 1];

			index = content.find(" ");
			if (index != -1)
			{
				content.erase(index, content.length());
			}

			index = content.find(" ");
			if (index != -1)
			{
				content.erase(index, content.length());
			}
		}
		else    // POST�ı��,��:-----------------------------7db1f467094c
		{
			head = data;
		}
	}

	/**
	 *\fn         SendResponseData(PCLIENT client, const char *head, int headLen, const char *data, int dataLen, char *pack)
	 *\brief      ����Ӧ������
	 *\param[in]  client	�ͻ��˶���
	 *\param[in]  head		ͷ�ڲ����ݡ�
	 *\param[in]  headLen	ͷ�ڲ����ݳ���
	 *\param[in]  data		�������ݡ�
	 *\param[in]  dataLen	�������ݳ���
	 *\param[in]  pack		���ݰ���
	 */
	void CSrv_HTTP::SendResponseData(PCLIENT client, const char *head, int headLen, 
		const char *data, int dataLen, char *pack)
	{
		if (headLen + dataLen < buffPool_.getSize())	// һ�����ݰ��ɷ���
		{
			memcpy(pack, head, headLen);
			memcpy(&pack[headLen], data, dataLen);

			iocp_.SendData(client, pack, headLen + dataLen, 0, 0);
		}
		else	// �ö��������
		{
			// ��һ����,������
			memcpy(pack, head, headLen);
			memcpy(&pack[headLen], data, buffPool_.getSize() - headLen);

			iocp_.SendData(client, pack, buffPool_.getSize(), 0, 0);

			int remain = dataLen - (buffPool_.getSize() - headLen);	// ʣ�����ݳ�

			for (int pos = buffPool_.getSize() - headLen; remain > 0; pos += buffPool_.getSize())
			{
                Sleep(50); // ��ɶ˿ڶ��̷߳������ݣ���ʱ������������ʱ

				void *buf = buffPool_.Get(1);

				int len = (remain >= buffPool_.getSize()) ? buffPool_.getSize() : remain;	// ���һ������

				memcpy(buf, &data[pos], len);

				iocp_.SendData(client, buf, len);

				remain -= len;

				TRACE(_T("client:%d pos:%d len:%d remain:%d\n"), client, pos, len, remain);
			}
		}
	}

	/**
	 *\fn         GetResponseHead(const char *statusCode, const char *reqFile, int dataLen)
	 *\brief      �õ�Ӧ��ͷ
	 *\param[in]  statusCode	״̬�롣
	 *\param[in]  reqFile		������ļ�����
	 *\param[in]  dataLen		���ݳ���
	 *\return	  �õ�Ӧ��ͷ������
	 */
	std::string CSrv_HTTP::GetResponseHead(const char *statusCode, const char *reqFile, int dataLen)
	{
		// �õ���չ��
		const char *ext = strrchr(reqFile, '.');

		MAP_STR_STR::iterator iter = mimeTypes_.find(ext + 1);

		std::string contentType = (iter != mimeTypes_.end()) ? iter->second : "text/html";

		char head[MAX_PATH] = "";

		sprintf(head, "HTTP/1.1 %s\r\nServer: %s\r\nDate: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
			statusCode,
			CTime::GetCurrentTime().FormatGmt(_T("%a, %d %b %Y %H:%M:%S GMT")),
			SERVERNAME,
			contentType.c_str(),
			dataLen);

		return head;
	}

	/**
	 *\fn         ParseRequstHead(const char *data, int len)
	 *\brief      ��������ͷ
	 *\param[in]  data		���ݡ�
	 *\param[in]  len		���ݳ���
	 *\return     ��������
	 */
	std::string CSrv_HTTP::ParseRequstHead(const char *data, int len)
	{
		if ((strncmp("GET", data, 3) == 0) || 
			(strncmp("POST", data, 4) == 0) || 
			(strncmp("----", data, 4) == 0))
		{
			(char)data[len] = '\0';
			char *sub = (char*)strstr(data, "\r\n\r\n");	// û��\r\n\r\n,����POST��������
			int index = (NULL == sub)?-1:(sub-data);

			postData_ = (char*)&data[index + 4];
			postDataLen_ = len - index - 4;
		}
		else    // ֻ������û��ͷ�ṹ
		{
			postData_ = (char*)data;
			postDataLen_ = len;
			return "POST";
		}

		VECTOR_STR headerNodes;
		CStrConver::TokenizeData(data, "\r\n", headerNodes);

		std::string cmd;
		std::string header;
		std::string content;

		for (unsigned int i = 0; i < headerNodes.size(); i++)
		{
			GetHeadContent(headerNodes[i].c_str(), header, content);

			if (header == "GET")                         // GET
			{
				CleanHead();

				int index = content.find("?");
				if (-1 != index)
				{
					content = content.substr(0, index);
				}

				reqFile_ = content;
				cmd = header;
			}
			else if (header == "POST")                   // POST
			{
				CleanHead();
				reqFile_ = content;
				cmd = header;
			}
			else if (header == "Content-Disposition")    // ��������,POSTʱ,Content-Disposition: form-data; name="test.txt"
			{
				int begin = content.find("name=\"");
				int end = content.find("\"", begin + 6);

				if ((-1 != begin) && (-1 != end))
				{
					postFile_ = content.substr(begin + 6, end - begin - 6);
					cmd = "POST";
				}
			}
			else   
			{
				MAP_STR_STR::iterator iter = headers_.find(header);

				if (iter != headers_.end())
				{
					headers_[header] = content;
				}

				TRACE(_T("Head:%s Value:%s\n"), header.c_str(), content.c_str());
			}
		}

		return cmd;
	}

	/**
	 *\fn         ProcessGET(PCLIENT client, const char *data, int len);
	 *\brief      ����GET����
	 *\param[in]  client	�ͻ��˶���
	 *\param[in]  data		���ݡ�
	 *\param[in]  len		���ݳ���
	 *\return     ��������
	 */
	void CSrv_HTTP::ProcessGET(PCLIENT client, const char *data, int len)
	{
		if ("/" == reqFile_)
		{
			reqFile_ = defaultHtmlFile_;
		}
// 		else
// 		{
// 			CStrConver::StringReplace(reqFile_, "/", "\\");
// 		}

		MAP_STR_VOID::iterator iter = htmlFiles_.find(reqFile_);

		if (iter != htmlFiles_.end())
		{
			char *fileData = (char*)iter->second;

			unsigned int fileLen = 0;

			memcpy(&fileLen, fileData, 4);

			fileData += 4;

			std::string responseHead = GetResponseHead("200 OK", reqFile_.c_str(), fileLen);
			SendResponseData(client, responseHead.c_str(), responseHead.length(), fileData, fileLen, (char*)data);
		}
		else
		{
			std::string responseHead = GetResponseHead("200 OK", reqFile_.c_str(), html404_.length());
			SendResponseData(client, responseHead.c_str(), responseHead.length(), html404_.c_str(), html404_.length(), (char*)data);
		}
	}

	/**
	 *\fn         ProcessPOST(PCLIENT client, const char *data, int len);
	 *\brief      ����POST����
	 *\param[in]  client	�ͻ��˶���
	 *\param[in]  data		���ݡ�
	 *\param[in]  len		���ݳ���
	 *\return     ��������
	 */
	std::string CSrv_HTTP::ProcessPOST(PCLIENT client, const char *data, int len)
	{
		std::string cmd;

		std::string contentType = headers_["Content-Type"];

		if ("" != contentType)
		{
			int index = contentType.find("boundary=");
			if (-1 != index)
			{
				boundary_ = "--" + contentType.substr(index+9);
				boundaryEnd_ = "\r\n" + boundary_ + "--\r\n";
			}
		}

		if ((0 != postDataLen_) && (NULL != postData_))
		{
			char *end = postData_ + postDataLen_ - boundaryEnd_.length();

			if (0 == strncmp(end, boundaryEnd_.c_str(), boundaryEnd_.length()))
			{
				cmd = "GET";
				postDataLen_ -= boundaryEnd_.length();
			}

			//-----------------------------------------------

			std::string filePath = root_ + "\\" + postFile_;
			bool open = false;
			UINT bModeCmd = 0;

			FILE *file = fopen(filePath.c_str(), "ab");

			if (NULL != file)
			{
				fseek(file, 0, SEEK_END);
				fwrite(postData_, postDataLen_, 1, file);
				fclose(file);
			}
		}

		return cmd;
	}

	/**
	 *\fn         ProcessPack(void *client, void *buf, int len, int ip, short port)
	 *\brief      �������ݰ�
	 *\param[in]  client	�ͻ��˶���
	 *\param[in]  buf		���ݡ�
	 *\param[in]  len		���ݳ���
	 *\param[in]  ip		�Է�IP(UDP)��
	 *\param[in]  port		�Է��˿�(UDP)��
	 */
	void CSrv_HTTP::ProcessPack(void *client, void *buf, int len, int ip, short port)
	{
		PPACK pack = (PPACK)buf;

		if ((packFlag == pack->flag) && (CLIENT_CLOSE_SOCKET == pack->protoId))
		{
			buffPool_.Free(pack);
			return;
		}

		std::string cmd = ParseRequstHead((char*)buf, len);

		if ("POST" == cmd)
		{
			cmd = ProcessPOST((PCLIENT)client, (char*)buf, len);
		}

		if ("GET" == cmd)
		{
			ProcessGET((PCLIENT)client, (char*)buf, len);
		}

		if (headers_["Connection"] == "close") // Keep-Alive
		{
			::Sleep(500);
			iocp_.CloseClient((PCLIENT)client, NULL);
		}

		return;
	}

	/**
	 *\fn         Post()
	 *\brief      Post���ݡ�
	 */
	void CSrv_HTTP::Post(void)
	{
		if (0 == iocp_.ConnectTo(g_ProFile.getIp(0), 8080, &client_))
		{
			CString strLen;
			CString strPost1;
			CString strPost2;        
			CString strData("����Ĳ��Ե�Ҫ�ϴ�������");

			strPost1.AppendFormat("POST /post.aspx HTTP/1.1\r\n");
			strPost1.AppendFormat("Accept: */*\r\n");
			strPost1.AppendFormat("Referer: http://%s:8080/post.aspx\r\n", g_ProFile.getIp(0));
			strPost1.AppendFormat("Accept-Language: zh-cn\r\n");
			strPost1.AppendFormat("Content-Type: multipart/form-data; boundary=---------------------------7d8186a20320\r\n");   // �������������--
			strPost1.AppendFormat("UA-CPU: x86\r\n");
			strPost1.AppendFormat("Accept-Encoding: gzip, deflate\r\n");
			strPost1.AppendFormat("User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Maxthon; .NET CLR 1.1.4322; InfoPath.1)\r\n");
			strPost1.AppendFormat("Host: 127.0.0.1:86\r\n");
			strPost1.AppendFormat("Content-Length: ****\r\n");
			strPost1.AppendFormat("Connection: Keep-Alive\r\n");//close");//
			strPost1.AppendFormat("Cache-Control: no-cache\r\n");
			strPost1.AppendFormat("\r\n");

			strPost2.AppendFormat("-----------------------------7d8186a20320\r\n");
			strPost2.AppendFormat("Content-Disposition: form-data; name=\"\\test.txt\"\r\n");
			strPost2.AppendFormat("\r\n");
			strPost2.AppendFormat("%s\r\n", strData);
			strPost2.AppendFormat("-----------------------------7d8186a20320--\r\n");

			strLen.Format("%d", strData.GetLength());

			strPost1.Replace("****", strLen);

			BYTE* pbyBuf = (BYTE*)buffPool_.Get(1);
			memcpy(pbyBuf, (LPCSTR)strPost1, strPost1.GetLength());
			outputQueue_.Add(client_, pbyBuf, strPost1.GetLength());

			pbyBuf = (BYTE*)buffPool_.Get(1);
			memcpy(pbyBuf, (LPCSTR)strPost2, strPost2.GetLength());
			outputQueue_.Add(client_, pbyBuf, strPost2.GetLength());
		}
	}
}