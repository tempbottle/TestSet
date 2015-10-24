#pragma once
#include <windows.h>

/**
 *\fn         (*PROCESS_DATA_FUNC)(void *param, char *data, int len)
 *\brief      �û��Ļص�������
 *\param[in]  param      �û��Լ����õĲ�����
 *\param[in]  data       �ܵ���������ݡ�
 *\param[in]  len        �ܵ���������ݳ��ȡ�
 *\return     ��
 */
typedef void (*PROCESS_DATA_FUNC)(void *param, char *data, int len);


/**
 *\class  RunProcess
 *\brief  ����һ������,������exeҲ����bat�ű�
 *\date   2012-07-03
 *\author �ź���
 *\par    Revision History:
 *\n      0.1     2007-07-03   �ź���   ��������
 */
class RunProcess
{
public:
	RunProcess();
	~RunProcess();

private:
	PROCESS_DATA_FUNC	procFunc_;	// �û��Ļص�����
	void				*param_;	// �û��Զ���Ĳ���

	HANDLE				process_;	// ���̾��
	HANDLE				thread_;	// ���߳̾��

public:
	/**
	 *\fn         setParam(void *param)
	 *\brief      �����û�����,�����ûص�ʱ�ٴ����û���
	 *\param[in]  param     �û�������
	 *\return     ��
	 */
	void setParam(void *param) { param_ = param; }

	/**
	 *\fn         setProcFunc(PROCESS_DATA_FUNC procFunc)
	 *\brief      �����û��ص�����ָ�롣
	 *\param[in]  procFunc  �ص�����ָ�롣
	 *\return     ��
	 */
	void setProcFunc(PROCESS_DATA_FUNC procFunc) { procFunc_ = procFunc; }

	/**
	 *\fn         close()
	 *\brief      �رմ˳���
	 *\return     ��
	 */
	void Close();

	/**
	 *\fn         Run(const char *name, const char *path, const char *param, bool show, 
	 *\n			  HANDLE write, HANDLE read)
	 *\brief      �������̡�
	 *\param[in]  name      ��������
	 *\param[in]  path      �������ڵ�·����
	 *\param[in]  param     ������
	 *\param[in]  inRead    �������������ܵ��Ķ��ˡ�
	 *\param[in]  outRead   �������������ܵ��Ķ��ˡ�
	 *\param[in]  outWrite  �������������ܵ���д�ˡ�
	 *\return     �Ƿ�ɹ�
	 */
	bool Run(const char *name, 
		const char *path, 
		const char *param, 
		bool show, 
		HANDLE inRead,
		HANDLE outRead,
		HANDLE outWrite);	
	
	/**
	 *\fn         CreateAnonymousPipe(HANDLE &write, HANDLE &read)
	 *\brief      ���������ܵ���
	 *\param[in]  write     ����ܵ���
	 *\param[in]  read      ����ܵ���
	 *\return     �Ƿ�ɹ�
	 */
	bool CreateAnonymousPipe(HANDLE &write, HANDLE &read);

	/**
	 *\fn         ReadAllPipe(HANDLE outRead, unsigned char *buffer, int size)
	 *\brief      �����ܵ���
	 *\param[in]  outRead   �������������ܵ��Ķ��ˡ�
	 *\param[in]  outWrite  �������������ܵ���д�ˡ�
	 *\param[out] buffer    ���ݻ�������
	 *\param[out] size      ���ݻ���������
	 *\return     ��ȡ�����ݳ���
	 */
	int ReadAllPipe(HANDLE outRead, HANDLE outWrite, unsigned char *buffer, int size);

	/**
	 *\fn         ProcPipeData(HANDLE write, HANDLE read, HANDLE process)
	 *\brief      ��ȡ�ܵ���
	 *\param[in]  outRead   �������������ܵ��Ķ��ˡ�
	 *\param[in]  outWrite  �������������ܵ���д�ˡ�
	 *\param[out] buffer    ���ݻ�������
	 *\param[out] size      ���ݻ���������
	 *\return     ��ȡ�����ݳ���
	 */
	void ProcPipeData(HANDLE outRead, HANDLE outWrite, HANDLE process);
};