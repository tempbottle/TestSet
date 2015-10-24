#include "StdAfx.h"
#include "srv_iocp.h"


namespace XT_IOCP
{

//#define USE_TCP

	/**
	 *\fn         CSrv_HTTP()
	 *\brief      ȱʡ���캯����
	 */
	CSrv_IOCP::CSrv_IOCP(void)
		:num_(0),
        client_(NULL)
	{
	}

	/**
	 *\fn         ~CSrv_IOCP()
	 *\brief      ����������
	 */
	CSrv_IOCP::~CSrv_IOCP(void)
	{
	}

	/**
	 *\fn         Start(const char *ip, unsigned short port)
	 *\brief      ��Ӽ���
	 *\param[in]  ip		����IP��
	 *\param[in]  port		�����˿ڡ�
	 *\return     �Ƿ�ɹ�(0�ɹ�,����ʧ��)
	 */
	int CSrv_IOCP::Start(const char *ip, unsigned short port)
	{
		CSrv::Start(CMD_TYPE_MYSELF, 4000);

#ifdef USE_TCP
        return iocp_.AddTcpListen(ip, port, &client_);
#else
        return iocp_.AddUdpListen(ip, port, &client_);
#endif
	}

	/**
	 *\fn         Test(const char *ip, unsigned short port)
	 *\brief      ����
	 *\param[in]  ip		�Է�IP��
	 *\param[in]  port		�Է��˿ڡ�
	 */
	void CSrv_IOCP::Test(const char *ip, unsigned short port)
	{
		PCLIENT client = NULL;

#ifdef USE_TCP
        if (0 == iocp_.ConnectTo(ip, port, &client))
#endif
        {
			PPACK pack = (PPACK)buffPool_.Get(1);
			if (NULL != pack)
			{
				pack->clear();
				pack->protoId = 123;
				pack->dataLen = buffPool_.getSize()-sizeof(PACK)+1;

				LOG(INFO) << "datalen:" << pack->dataLen 
					<< "packsize:" << sizeof(PACK) 
					<< "buffsize:" << buffPool_.getSize();

				for (int i = 0; i < pack->dataLen - 1; i++)
				{
					pack->data[i] = '1';
				}

				pack->data[pack->dataLen - 1] = '2'; // ���һ������

				int len = 0;
				char *data = NULL;

				if (zipFile_.Compress((char*)pack->data, pack->dataLen, &data, (UINT&)len))
				{
					pack->dataLen = len;
				}

                memcpy(pack->data, data, len);

#ifdef USE_TCP
                outputQueue_.Add(client, pack, pack->getPackLen());
#else
                outputQueue_.Add(client_, pack, pack->getPackLen(), inet_addr(ip), htons(port));
#endif
            }
		}
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
	void CSrv_IOCP::ProcessPack(void *client, void *buf, int len, int ip, short port)
	{
		PPACK pack = (PPACK)buf;

		switch (pack->protoId)
		{
		case 123:
			{
				++num_;

				char data[1024*10] = "";
				int len = sizeof(data);

// 				if (!zipFile_.UnCompress((char*)pack->data, pack->dataLen, data, (UINT&)len))
// 				{
// 					TRACE(_T("UnCompress not ok\n"));
// 				}

#ifdef USE_TCP
				outputQueue_.Add(client, pack, pack->getPackLen());
#else
				outputQueue_.Add(client, pack, pack->getPackLen(), ip, port);
#endif
				break;
			}
		case CLIENT_CLOSE_SOCKET:
		default:
			{
				TRACE("CSrv_IOCP::ProcessPack default\n");
				buffPool_.Free(pack);
				break;
			}
		}
	}
}