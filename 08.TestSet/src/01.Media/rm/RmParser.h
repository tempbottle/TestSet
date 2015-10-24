
#pragma once

#include "os.h"
#include "Parser.h"
#include "RmReader.h"

typedef struct index_type {			//	�Զ���������ṹ���±��ʾʱ�䣬����Ϊ��λ) 
		uint32_t offset;			//	���ļ�ͷ����ǰʱ������Ӧ�İ��� ƫ��
		uint16_t num_of_packets;	//	��һ���������İ�����
		DWORD	 length_of_data;	//	��һ�������������ݵ��ܳ���
	}  INDEX;						//	

class RmParser : public Parser
{
public:
	RmParser( CString );
	virtual ~RmParser(void);
private:
	CString		sdp;

	WORD		numOfIndex;

	WORD		streamCount;
	WORD		audioTrackID;
	WORD		videoTrackID;
	WORD		flags;

	DWORD		avgBitRate, avgAudioBitRate, avgVideoBitRate;
	DWORD		maxBitRate, maxAudioBitRate, maxVideoBitRate;
	DWORD		avgPacketSize, avgAudioPacketSize, avgVideoPacketSize;
	DWORD		maxPacketSize, maxAudioPacketSize, maxVideoPacketSize;

	DWORD		duration,audioDuration,videoDuration;
	DWORD		preroll,audioPreroll,videoPreroll;

	DWORD		audioStartTime,videoStartTime;
	CString		audioMimeType,videoMimeType;
	CString		audioTrackName,videoTrackName;

	CString		title;
	CString		copyright;
	CString		comment;
	
	CString		audioOpaque;
	CString		videoOpaque;

	DWORD		*pTimeSlotLenTable;

	INDEX		*index;

	RmReader	*mediaFile;
	BOOL		bSuccess;

public :
	BOOL		Initial();
	CString		GetSDP();
	DWORD		GetDuration();
	DWORD		GetOffset( WORD );
	WORD		GetPacketCounts( WORD );

	DWORD *		GetLengthTable();
	DWORD		GetSecondNum();

	BOOL		SeekIndex( WORD second );
	DWORD		GetNextFrame( UCHAR *pBuf );

private:
	BOOL		MakeSDP();
	BOOL		MakeIndex();
    BOOL        MakeHead();

	void		Serialize(CArchive& ar);
	BOOL		Load(CFile *pFile);
	BOOL		Save();
};
