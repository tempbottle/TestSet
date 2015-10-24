/*------------------------------------------------------------------------------*\
[ģ������]
CG729

[�ļ�����]
G729.h

[����ļ�]
G729.cpp

[Ŀ��] 
������Ƶ��ѹ�� 

[����]
��װ g.729 api

[������]
G729a.lib

\*------------------------------------------------------------------------------*/

#ifndef _G729_H_
#define _G729_H_

//#define SIZE_AUDIO_FRAME 960
//#define SIZE_AUDIO_PACKED 60
#define SIZE_AUDIO_FRAME 15840
#define SIZE_AUDIO_PACKED 990

class CG729
{
public:
    virtual  ~CG729();
    CG729();

    BOOL Encode(char *pin,int len,char* pout,int* lenr);
    BOOL Decode(char *pin,int len,char* pout,int* lenr);
};


#endif