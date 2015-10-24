
#pragma once
#define WIN32_LEAN_AND_MEAN	// ȥ��winsock.h����
#include <Windows.h>

namespace XT_IOCP
{

	typedef struct tagClientContext *PCLIENT;
	class PackQueue;
	class BuffPool;

#pragma pack(push, 1)
	
	static const int packFlag = 0x12345678; // �����

	/**
	 *\struct tagPACKHEAD
	 *\brief  ���ݰ�ͷ��Ϣ
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	typedef struct tagPACKHEAD
	{
		long          flag;		// �����(0x12345678)
		unsigned long dataLen;	// �����ݳ���
		unsigned long protoId;	// Э��ID
		unsigned char data[1];	// �����ݣ������� dataLen - 1 ���ֶ�����Ϣ���ݣ�

		/**
		 *\fn         clear()
		 *\brief      ������ݰ�ȫ�����ݡ�
	     */
		void clear(unsigned long id = 0)
		{
			flag = packFlag;
			dataLen = 0;
            protoId = id;
		}

		/**
		 *\fn         getData()
		 *\brief      �������ݡ�
		 *\return     ����ָ��
	     */
		unsigned char * getData() { return data; }

		/**
		 *\fn         setData(int begin, void *buff, int len)
		 *\brief      �������ݡ�
		 *\param[in]  begin λ�á�
		 *\param[in]  buff  ����ָ�롣
		 *\param[in]  len   ���ݳ��ȡ�
	     */
		void setData(int begin, void *buff, int len)
		{
			memcpy(&data[begin], buff, len);
			dataLen = begin + len;
		}
		
		/**
		 *\fn         getPackLen()
		 *\brief      �õ��������ݰ�����(�������ݳ�)��
		 *\return	  �������ݰ�����
	     */
		long getPackLen()
		{
			return (dataLen + sizeof(PACK) - sizeof(char));
		}

		/**
		 *\fn         getCheckNum()
		 *\brief      �õ�У���롣
		 *\return	  У����
	     */
		short getCheckNum()
		{
			return getCheckNum(data, dataLen);
		}

		/**
		 *\fn         getCheckNum(unsigned char *data, unsigned long len)
		 *\brief      �õ�У���롣
		 *\param[in]  data ����ָ�롣
		 *\param[in]  len  ���ݳ��ȡ�
		 *\return	  У����
	     */
		static short getCheckNum(unsigned char *data, unsigned long len)
		{
			short checkSum = 0;

			for(unsigned long i = 0; i < len; i += 10)
			{
				checkSum += data[i];
			}

			return checkSum;
		}

	}PACK, *PPACK;

#pragma pack(pop)

	/**
	 *\enum   CMDTYPE
	 *\brief  ���ݰ�ͷ��Ϣ
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	enum
	{
		CMD_TYPE_MMS,	// MMSЭ��
		CMD_TYPE_HTTP,	// HTTPЭ��
		CMD_TYPE_MYSELF	// �Զ���Э��
	};

	/**
	 *\class  PrePack
	 *\brief  ������Ҫ�Ƕ����ݰ�����Ԥ����ƴ�ӺͲ�֣����������������һ����������ȷ�İ�
	 *\date   2012-07-17
	 *\author �ź���
	 *\par    Revision History:
	 */
	class PrePack
	{
	public:
		/**
		 *\fn         PrePack()
		 *\brief      ȱʡ���캯����
	     */
		PrePack();

		/**
		 *\fn         ~PrePack()
		 *\brief      ����������
	     */
		~PrePack();

	private:
		CRITICAL_SECTION	csUsed_;	// �ٽ���
		PackQueue			*inQueue_;	// �������
		BuffPool			*buffPool_; // �ڴ��
		int				cmdType_;	// Э�� ����

		long				oneBuffSize_;// һ���������ĳ���
		long				dataLen_;	// ���ݳ�
		long				buffSize_;	// ��ǰ�������ĳ���
		char				*buff_;		// ������

	public:
		/**
		 *\fn         Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool)
		 *\brief      ��ʹ��Ԥ�������
		 *\param[in]  cmdType  �������͡�
		 *\param[in]  inQueue  ���ݰ�������С�
		 *\param[in]  buffPool �ڴ�ء�
		 *\return     �Ƿ�ɹ�
		 */
		bool Init(int cmdType, PackQueue *inQueue, BuffPool *buffPool);
		
		/**
		 *\fn         Add(PCLIENT client, long len, void *buffer)
		 *\brief      ���һ�����ݰ���
		 *\param[in]  client  �ͻ��˶���
		 *\param[in]  len     ���ݳ���
		 *\param[in]  buffer  ���ݰ���
		 *\return     �Ƿ�ɹ�
		 */
		bool Add(PCLIENT client, long len, void *buffer);
	};

}

/*
typedef struct tagSingleMessage{// ��������Ϣ��
    BYTE        bVersion;       // ��Ϣ�汾
    WORD        wType;          // ��Ϣ����
    DWORD       dwId;           // ��Ϣ���
    DWORD       dwSenderId;     // �����߱��
    DWORD       dwReceiverId;   // �����߱��
    time_t      tSendTime;      // ����ʱ��
    time_t      tDropTime;      // ����ʱ��
    char        cSendType;      // �������ͣ�0-һ��һ��1-һ�Զࣻ2-����
    char        cStatus;        // ��Ϣת��״̬��
                                // 0-ֱ�ӷ��ͣ�
                                // 1-ͨ��������ת����
                                // 2-������Ϣת����
                                // 3-������
    WORD        wMessageLen;    // ��Ϣ���ݳ���
    WORD        wMsgNum;        // �������
    char        acMessageText[1];// ��Ϣ���ݣ�ԭ��Ϣ�������ݣ��1400�ֽڣ�
}SINGLEMESSAGE, *PSINGLEMESSAGE;

typedef struct tagRawMessagePacketHeader{// ����ͷ��
    DWORD       dwHeader;       // ������������0x12345678��
    DWORD       dwSenderId;     // �����߱��
    WORD        wSenderType;    // ����������
    WORD        wServerAId;     // ������ID(S-C)
    WORD        wReceiverType;  // �����߱��
    WORD        wServerBId;     // ���������ڵķ�����ID(C-S)
    WORD        wNumOfMsg;      // ��������Ϣ��
    WORD        wPacketLen;     // �������ݳ���
    short       sCheckSum;      // ����ͷУ����
}RAWMESSAGEPACKETHEADER, *PRAWMESSAGEPACKETHEADER;
*/