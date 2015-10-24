
#include "stdafx.h"
#include "Srv.h"
//#include <ws2ipdef.h>
#include <process.h>


/*
�߳�    ״̬    �˳���

����    ����    300
����    ǿ��    301
����    �쳣    302
*/

namespace XT_IOCP
{

	/**
	 *\fn         CSrv()
	 *\brief      Ĭ�Ϲ��캯����
	 */
	CSrv::CSrv(void)
		:thread_(NULL),
		running_(false),
		cmdType_(0)
	{
	}

	/**
	 *\fn         ~CSrv()
	 *\brief      ����������
	 */
	CSrv::~CSrv(void)
	{
		Stop();
	}

	/**
	 *\fn         Start(int cmdType, int buffSize)
	 *\brief      ��ʼ���з���
	 *\param[in]  cmdType  �������͡�
	 *\param[in]  buffSize ��������С��
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int CSrv::Start(int cmdType, int buffSize)
	{
        if (running_) return -1;

        running_ = true;
		unsigned int threadID = 0;
		thread_ = (HANDLE)_beginthreadex(NULL, 0, ProcessPackThread, this, 0, &threadID);

        if (NULL == thread_) return -2;

		buffPool_.setSize(buffSize);
		inputQueue_.Start();
		outputQueue_.Start();

        int ret = iocp_.Init(cmdType, &inputQueue_, &outputQueue_, &buffPool_);
		
        if (0 != ret)
        {
            running_ = false;
        }

        return ret;
	}

	/**
	 *\fn         Stop()
	 *\brief      ֹͣ���з���
	 */
	void CSrv::Stop()
	{
		running_ = false;

		iocp_.Stop();

		StopThread(thread_);
	}

	/**
	 *\fn         Stop(HANDLE thread)
	 *\brief      ֹͣ�̡߳�
	 *\param[in]  thread  �߳̾����
	 */
	void CSrv::StopThread(HANDLE thread)
	{
        if (NULL == thread) return;

        int i = 10;
        for (; i > 0; i--)
        {
            int code = 0;
            if (!GetExitCodeThread(thread, (LPDWORD)&code) || 
                (code !=STILL_ACTIVE))
            {
                break;
            }

            Sleep(50);
        }

        if (i == 0)
        {
            TRACE("WARNING: Terminating Thread. %d\n", thread);
            TerminateThread(thread, 301);
        }

        CloseHandle(thread);
        thread_ = NULL;
    }

	/**
	 *\fn         ProcessPackThread(LPVOID pParam)
	 *\brief      �����̡߳�
	 *\param[in]  param  �̲߳�����
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	unsigned CSrv::ProcessPackThread(LPVOID param)
	{
		if (NULL == param) return -1;

		CSrv *pthis = reinterpret_cast<CSrv*>(param);

		return pthis->ProcessPackThreadPoc();
	}

	/**
	 *\fn         ProcessPackThreadPoc()
	 *\brief      ʵ�ִ�������
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int CSrv::ProcessPackThreadPoc()
	{
		//PQueueNode node = NULL;
		short times = 0;
		short waitTime = 1;		
		void *client = NULL;
		void *buf = NULL;
		int   len = 0;
		int   ip = 0;
		short port = 0;

		try
		{
			while (running_)
			{
				//if (inputQueue_.Get(&node))
				if (inputQueue_.Get(&client, &buf, len, ip, port))
				{
					ProcessPack(client, buf, len, ip, port);  // �������䴦��(���ݰ������нڵ�)
					waitTime = 0;
				}
				else
				{
					::Sleep(waitTime); // ���������CPU��100%
					if (++times > 10000)
					{
						times = 0;
						waitTime = 1;
					}
				}
			}

			return 300;
		}
		catch (...)
		{
			TRACE(_T("CSrv::ProcessPackThread error:%d\n"), ::GetLastError());
			return 302;
		}
	}

	// Э�鴦����(�麯��)
	// pInfo [IN] �����б�ڵ�
	void CSrv::ProcessPack(void *client, void *buf, int len, int ip, short port)
	{
		PPACK pack = (PPACK)buf;

		switch (pack->protoId)
		{
		case CLIENT_CLOSE_SOCKET:   // SOCKET�Ͽ�����,pack->dataLenΪSOCKETֵ
			{
				TRACE("CLIENT_CLOSE_SOCKET SOCKET:%d\n", pack->dataLen);
				buffPool_.Free(buf);
				break;
			}
		default:
			{
				TRACE("Pack protoId %x\n", pack->protoId);
				buffPool_.Free(buf);
			}
		}
	}
}