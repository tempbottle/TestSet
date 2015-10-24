
#include "StdAfx.h"
#include "PrePack.h"
#include "BuffPool.h"
#include "PackQueue.h"



namespace XT_IOCP
{

	/**
	 *\fn         PrePack()
	 *\brief      ȱʡ���캯����
	 */
	PrePack::PrePack(void)
		:cmdType_(CMD_TYPE_MYSELF),
		inQueue_(NULL),
		buffPool_(NULL),
		oneBuffSize_(0),
		dataLen_(0),
		buffSize_(0),
		buff_(NULL)
	{
		InitializeCriticalSection(&csUsed_);
	}

	/**
	*\fn         ~PrePack()
	*\brief      ����������
	*/
	PrePack::~PrePack(void)
	{
		if (NULL != buff_)
		{
			delete[] buff_;
			buff_ = NULL;
		}
		DeleteCriticalSection(&csUsed_);
	}

	/**
	 *\fn         Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
	 *\brief      ��ʹ��Ԥ�������
	 *\param[in]  cmdType  �������͡�
	 *\param[in]  inQueue  ���ݰ�������С�
	 *\param[in]  buffPool �ڴ�ء�
	 *\return     �Ƿ�ɹ�
	 */
	bool PrePack::Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
	{
		if (NULL == inQueue || NULL == buffPool) return false;

		cmdType_ = cmdType;
		inQueue_ = inQueue;
		buffPool_ = buffPool;

		oneBuffSize_ = 10;//buffPool->getSize();
		buffSize_ = oneBuffSize_ * 4;
		buff_ = new char[buffSize_];

		return true;
	}

	/**
	*\fn         Add(PCLIENT client, long len, void *buffer)
	*\brief      ���һ�����ݰ���
	*\param[in]  client  �ͻ��˶���
	*\param[in]  len     ���ݳ���
	*\param[in]  buffer  ���ݰ���
	*\return     �Ƿ�ɹ�
	*/
	bool PrePack::Add(PCLIENT client, long len, PVOID buffer)
	{
		EnterCriticalSection(&csUsed_);

		switch(cmdType_)
		{
		case CMD_TYPE_MYSELF:
			{
				long newDataLen = 0;

				PPACK pack = (PPACK)buffer;
				long packLen = pack->getPackLen();
				if (packLen == len && pack->flag == packFlag)	// ��һ�������İ�
				{
					inQueue_->Add(client, buffer, len);
				}
				else
				{
					newDataLen = dataLen_ + len;			// ���뵱ǰ���ݺ�����ݳ�

					if (newDataLen > buffSize_)				// �ϲ����ݺ󳤶ȴ��ڻ�����
					{
						long newBuffNum = (newDataLen / oneBuffSize_) + 1;
						buffSize_ = newBuffNum * oneBuffSize_;
						char *newBuff = new char[buffSize_];// ���·����ڴ�
						if (NULL == newBuff)
						{
							//g_ProFile.m_pLog->println(LOG_ERROR, _T("CPreparePack::Add new buff error, thread exit !"));
							//delete[] buff_;
							return false;
						}

						//g_ProFile.m_pLog->println(LOG_INFO, _T("CPreparePack::Add new memory %d bytes socket= %d"), m_dwBuffSize, pClient);

						memcpy(newBuff, buff_, dataLen_);	// ��ԭ���ݿ������µĻ�������
						delete[] buff_;
						buff_ = newBuff;
					}

					memcpy(buff_ + dataLen_, buffer, len);	// ����ǰ���ݼ��뵽������β��
					dataLen_ += len;

					buffPool_->Free(buffer);				// ���ջ�����

					while ((NULL != buff_) && (dataLen_ > 0))
					{
						PPACK newPack = (PPACK)buff_;
						if (packFlag != newPack->flag)		// ������ǰ�ͷ
						{
							char *pos = buff_;
							long *flag = (long*)buff_;

							for (long i = 0; i < (dataLen_ - 3); i++)
							{
								if (packFlag == *flag)		// ���Ұ�ͷ���
								{
									dataLen_ -= i;
									memcpy(buff_, pos, dataLen_);
								}
								else
								{
									pos++;
									flag = (long*)pos;
								}
							}

							break;
						}
						else if ((newDataLen = newPack->getPackLen()) <= dataLen_)
						{
							// ��һ����������Ϣ��
							PPACK newPack = (PPACK)buffPool_->Get(1);
							if (NULL == newPack)
							{
								break;
							}

							memcpy(newPack, buff_, newDataLen);
							inQueue_->Add(client, newPack, newDataLen);

							dataLen_ -= newDataLen;
							memcpy(buff_, buff_ + newDataLen, dataLen_);
						}
						else
						{
							break;
						}
					}
				}

				break;
			}		
		case CMD_TYPE_HTTP:
			{
				inQueue_->Add(client, buffer, len);
				break;
			}
		case CMD_TYPE_MMS:
			{
				unsigned char *buf = (unsigned char *)buffer;
				ULONGLONG flag = 0;

				memcpy(&flag, buffer, 8);

				if (0xb00bface4a000001 == flag || 0xb00bface00000001 == flag)	//0xb00bface00000001
					inQueue_->Add(client, buffer, len);
				else
					//g_ProFile.m_pLog->println(LOG_ERROR, _T("CPreprocPack::Add Error MMS"));
				break;
			}
		default:
			TRACE(_T("CPreprocPack::Add CmdType Error\n"));
			break;
		}

		LeaveCriticalSection(&csUsed_);
		return true;
	}

}