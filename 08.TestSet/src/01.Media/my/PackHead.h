
#pragma once


typedef struct _tagBlockEx
{
	DWORD dwFrameLen;  // ֡���ݳ�
	WORD  wFrameCheck; // ֡����У����
	WORD  wBlockCount; // �ӿ���
	DWORD dwTime;      // ����ʱ��
}BLOCKEX,*PBLOCKEX;

typedef struct _tagPackEx
{
	DWORD dwTime;       // ����ʱ��

}PACKEX,*PPACKEX;