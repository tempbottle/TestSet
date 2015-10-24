
#pragma once
#include "AV/PackHead.h"
#include "Common/KeyList.h"
#include <vector>
//#include "Network/DefineClientContext.h"


namespace XT_IOCP
{

	typedef struct tagClientContext*  PCLIENT;
	typedef struct _tagPackEx*        PPACKEX;
	class PackQueue;
	class BuffPool;


	typedef struct _tagSendInfo
	{
		PCLIENT	pClient; // ���Ͷ���(UDP)
		int     ip;		 // IP��ַ
		short   port;	 // �˿ڵ�ַ
	}SenderInfo,*PSenderInfo;

	typedef std::vector<SenderInfo> SENDINFOERS;

	/**
	������Ƶ���ݷ��͸�ָ�����û�,��Ƶ����ֳ�1024���ֽ�
	��Ƶ����Ƶ��ID����һ��˳���
	*/
	class CAVSend
	{
	public:
		CAVSend();
		~CAVSend();

	protected:
		int         mtuSize_;		// ��С���䵥Ԫ
		int         maxSize_;		// �ܷ��͵���������,��ʵ���ɻ�������С����    
		int         channceId_;		// ��һ·����Ƶ��ID
		int         lastTime_;		// ��һ������ʱ��,��������ͬ��
		int         packId_;		// ��ID
		int         audioProto_;	// ��ƵЭ��
		int         videoProto_;	// ��ƵЭ��
		int         addrSize_;		// IPV4��ַ�ṹ��С
		int         addrSize6_;		// IPV6��ַ�ṹ��С

		PackQueue  *outputQueue_;	// ���Ͷ���
		BuffPool   *buffPool_;		// ������
		CKeyList    avData_;		// ��ƵƵ������

		SENDINFOERS audioSenders_;	// ����Щ��������Ƶ����
		SENDINFOERS videoSenders_;	// ����Щ��������Ƶ����

	protected:
		void DelPack(int id);

	public:
		void Init(int channceId, int mtuSize, int buffSize,
			int audioProto, int videoProto,
			PackQueue *outputQueue, BuffPool *buffPool);

		void AddAudioSender(PCLIENT client, int ip, unsigned short port);
		void AddVideoSender(PCLIENT client, int ip, unsigned short port);
		void DelAudioSender(PCLIENT client, int ip, unsigned short port);
		void DelVideoSender(PCLIENT client, int ip, unsigned short port);

		void DelAllSend();

		void SendAudio(char *data, int len);
		void SendVideo(char *data, int len);
		void ResendAV(PCLIENT client, int id, int ip, unsigned short port);

		int getAudioSenderCount() { return audioSenders_.size(); }
		int getVideoSenderCount() { return videoSenders_.size(); }
	};
}