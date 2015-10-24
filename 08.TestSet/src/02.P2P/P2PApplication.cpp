
#include "stdafx.h"
#include "P2PApplication.h"


namespace XT_P2P
{

	/**
	 *\fn         P2PApplication()
	 *\brief      Ĭ�Ϲ��캯����
	 */
	P2PApplication::P2PApplication()
		:thread_(NULL),
		running_(false)
	{
	}

	/**
	 *\fn         ~P2PApplication()
	 *\brief      ����������
	 */
	P2PApplication::~P2PApplication()
	{
		Stop();
	}

	/**
	 *\fn         Start(LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port)
	 *\brief      ��ʼ���з���
	 *\param[in]  rootPath  �ļ���·����
     *\param[in]  ip        IP��ַ��
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int P2PApplication::Start(LPCTSTR rootPath, LPCTSTR ip, LPCTSTR port)
	{
        if (running_ || NULL == rootPath || NULL == ip || NULL == port) return -1;

        running_ = true;

		unsigned int threadID = 0;
		thread_ = (HANDLE)_beginthreadex(NULL, 0, ProcessPackThread, this, 0, &threadID);

        if (NULL == thread_) return -2;

#ifdef _DEBUG
        buffPool_.name_ = _T("P2PEnginerBuffPool");
#endif // _DEBUG
        
        buffPool_.setSize(1300);
		inputQueue_.Start();

        int ret = enginer_.init(&buffPool_, &inputQueue_, rootPath);

        if (0 != ret) goto end;

        ret = enginer_.addListen(ip, _ttoi(port));

        if (0 != ret) goto end;

        return ret;

end:
        running_ = false;
        return ret;
	}

	/**
	 *\fn         Stop()
	 *\brief      ֹͣ���з���
	 */
	void P2PApplication::Stop()
	{
        if (!running_) return;

		running_ = false;

		inputQueue_.Stop();

        enginer_.stop();

		StopThread(thread_);

        thread_ = NULL;
	}

	/**
	 *\fn         Stop(HANDLE thread)
	 *\brief      ֹͣ�̡߳�
	 *\param[in]  thread  �߳̾����
	 */
	void P2PApplication::StopThread(HANDLE thread)
	{
        if (NULL == thread) return;

        int i = 10;
        for (; i > 0; i--)
        {
            int code = 0;
            if (!GetExitCodeThread(thread, (LPDWORD)&code) || (code != STILL_ACTIVE))
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
    }

	/**
	 *\fn         ProcessPackThread(LPVOID pParam)
	 *\brief      �����̡߳�
	 *\param[in]  param  �̲߳�����
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	unsigned P2PApplication::ProcessPackThread(LPVOID param)
	{
		if (NULL == param) return -1;

		P2PApplication *pthis = reinterpret_cast<P2PApplication*>(param);

		return pthis->ProcessPackThreadProc();
	}

	/**
	 *\fn         ProcessPackThreadPoc()
	 *\brief      ʵ�ִ�������
	 *\return     �Ƿ�ɹ�(0�ɹ�������ʧ��)
	 */
	int P2PApplication::ProcessPackThreadProc()
	{
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
			TRACE(_T("P2PApplication::ProcessPackThread error:%d\n"), ::GetLastError());
			return 302;
		}
	}

	// Э�鴦����(�麯��)
	// pInfo [IN] �����б�ڵ�
	void P2PApplication::ProcessPack(void *client, void *buf, int len, int ip, short port)
	{
	}
}