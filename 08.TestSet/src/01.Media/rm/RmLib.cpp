#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;
#include "./RmLib.h"
#define d4(p, a)  rmff_get_uint32_be( (unsigned char *)&((p)->a) )
#define d2(p, a)  rmff_get_uint16_be( (unsigned char *)&((p)->a) )

RmLib::RmLib() {
	file = NULL;

}

RmLib::~RmLib(){
	Close();
}


BOOL	RmLib::Initial(CString filename ) {

	BOOL  retVal = Open( filename );
	
	if(retVal == FALSE) return FALSE;

	retVal = ReadHeaders();

	if(retVal == FALSE) return FALSE;

	props   = &file->prop_header;
	
	cont	= &file->cont_header;

	int streamCount  = GetStreamCount();
	for ( int i = 0; i<streamCount; i++) {
		if( file->tracks[i]->id == AUDIO_TRACK_ID  ) {
			audioTrackID = i;
			audioTrack = file->tracks[i];
			audioProps = &audioTrack->mdpr_header;
		}
		if( file->tracks[i]->id == VIDEO_TRACK_ID ) {
			videoTrackID = i;
			videoTrack = file->tracks[i];
			videoProps = &file->tracks[i]->mdpr_header;
		}
	}
	
	return TRUE; 
}


	
BOOL	RmLib::Open( CString filename ) {

	file=rmff_open_file(filename,MB_OPEN_MODE_READING);
	if( file == NULL) {
		return FALSE;
	}
	else {
		return TRUE;
	}

}


BOOL	RmLib::Close( ){
	if(file)
	{
		rmff_close_file( file );
		file = NULL;
	}

	return TRUE;
}


BOOL	RmLib::Seek( DWORD offset, int flag ){			//��λ�ļ�ָ��,flag��ȡֵΪ SEEK_SET  SEEK_CUR  SEEK_END fseek
	void * hd = NULL;										//�ļ���� hd
	mb_file_io_t *io = NULL;								//�ļ�io������
	hd = file->handle;
	io = file->io;
	int re = io->seek(hd, offset, flag);	//SEEK_SET
	if( re == 0 ) {
		return TRUE;
	}
	else {
		return FALSE;
	}

}

DWORD	RmLib::Tell( ){									//ȡ���ļ�ָ��ƫ��
	void * hd = NULL;										//�ļ������Ϊ��
	mb_file_io_t *io = NULL;								//�ļ�io������Ϊ��
	hd = file->handle;										//��fileָ���еõ��ļ����
	io = file->io;											//��fileָ���еõ�io������
	DWORD offset = io->tell(hd);							//ȡ�õ�ǰƫ��
	return offset;			
}

/*
��ȡ��һ��rm֡
	��ȷʱ����֡��ָ��
	�����ļ�β�����ʱ����NULL
	(�ݴ�Ĵ��� ,�����������֡ʱ,�ӹؼ�֡�����еõ���һ����ȷ֡��λ��,�����ļ�
	 ���ŵĳ����� )
	 �������ݹ��������
*/
rmff_frame_t *	RmLib::ReadNextFrame()
{
	rmff_frame_t *frame = rmff_read_next_frame( file, NULL );
	if( frame != NULL ) {
		file->num_packets_read --;
	}
	if( frame == NULL )
	{
		/*DWORD offset = GetNextKeyFrameOffset();
		if( offset == -1 ) return frame;
		Seek( offset );*/
		frame = ReadNextKeyFrame();
	}
	return frame;
}

// commented by xianjian at 2.28
//rmff_frame_t *	RmLib::ReadNextFrameHeader()
//{
//	rmff_frame_t *frame = rmff_read_next_frame_header( file, NULL );
//	if( frame != NULL ) {
//		file->num_packets_read --;
//	}
//	return frame;
//}

BOOL			RmLib::ReleaseFrame( rmff_frame_t*frame ){
	rmff_release_frame( frame );
	return TRUE;
}



BOOL			RmLib::ReadHeaders(){

	int re = rmff_read_headers( file );
	if ( re == 0 ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


WORD	RmLib::GetStreamCount(){
	return file->num_tracks;

}

WORD	RmLib::GetAudioTrackID(){
	return audioTrackID;

}


WORD	RmLib::GetVideoTrackID(){
	return videoTrackID;

}


WORD	RmLib::GetFlags( ){
	return d2(&(file->prop_header),flags);

}


DWORD	RmLib::GetAvgBitRate( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, avg_bit_rate );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, avg_bit_rate );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, avg_bit_rate );
			break;
		default:
			return 0;
	}

}


DWORD	RmLib::GetMaxBitRate( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, max_bit_rate );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, max_bit_rate );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, max_bit_rate );
			break;
		default:
			return 0;
	}

}


DWORD	RmLib::GetAvgPacketSize( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, avg_packet_size );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, avg_packet_size );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, avg_packet_size );
			break;
		default:
			return 0;
	}
	

}


DWORD	RmLib::GetMaxPacketSize( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, max_packet_size );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, max_packet_size );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, max_packet_size );
			break;
		default:
			return 0;
	}

}





DWORD	RmLib::GetDuration( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, duration );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, duration );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, duration );
			break;
		default:
			return 0;
	}

}


DWORD	RmLib::GetPreroll( WORD id ){
	switch( id) {
		case FILE_ID:
			
			return d4( props, preroll );
			break;
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, preroll );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, preroll );
			break;
		default:
			return 0;
	}

}

DWORD	RmLib::GetStartTime( WORD id ){
	switch( id) {
		
		case AUDIO_TRACK_ID:
			
			return d4( audioProps, start_time );
			break;
		case VIDEO_TRACK_ID:
			return d4( videoProps, start_time );
			break;
		default:
			return 0;
	}
	

}


char *	RmLib::GetTrackName( WORD id ){
	switch( id) {
		
		case AUDIO_TRACK_ID:
			
			return audioProps->name ;
			break;
		case VIDEO_TRACK_ID:
			return videoProps->name ;
			break;
		default:
			return NULL;
	}
	


}
char *  RmLib::GetMimeType( WORD id ){
	switch( id) {
		
		case AUDIO_TRACK_ID:
			
			return audioProps->mime_type ;
			break;
		case VIDEO_TRACK_ID:
			return videoProps->mime_type ;
			break;
		default:
			return NULL;
	}

}


char *	RmLib::GetTitle( void ){
	return cont->title;

}


char *	RmLib::GetComment(){
	return cont->comment;

}


char *	RmLib::GetCopyRight(){
	return cont->copyright;

}

unsigned char *	RmLib::GetAudioTypeSpec(){
	return audioProps->type_specific_data;


}

DWORD	RmLib::GetAudioTypeLen(){
	return d4(audioProps,type_specific_size);

}




unsigned char *	RmLib::GetVideoTypeSpec(){
	return videoProps->type_specific_data;

}

DWORD	RmLib::GetVideoTypeLen(){
	return d4(videoProps,type_specific_size);

}







rmff_track_t * RmLib::GetAudioTrack(){
	
	return audioTrack;

}

rmff_track_t * RmLib::GetVideoTrack(){
	
	return videoTrack;

}

//add by xianjian at 2.28
#pragma 	pack (push,1)
typedef struct Index_Chunk_Header
{  
	DWORD     object_id;  
	DWORD     size;  
	WORD      object_version;  
	DWORD     num_indices;    
	WORD     stream_number;    
	DWORD     next_index_header;  
} INDEX_CHUNK_HEADER;

typedef struct IndexRecord
{  
	WORD   object_version;  
	DWORD  timestamp;    
	DWORD  offset;    
	DWORD   packet_count_for_this_packet;  
} INDEX_RECORD;
#pragma pack (pop)

//add by xianjian at 2.28
	//��ȡ��ǰ�ļ�ָ����һ���ؼ�֡
	//  ���عؼ�֡��ָ��,���û�з��������Ĺؼ�֡,����NULL;
rmff_frame_t*			RmLib::ReadNextKeyFrame()
{
	DWORD offset = Tell();
	void * hd = NULL;										//�ļ���� hd
	mb_file_io_t *io = NULL;								//�ļ�io������
	hd = file->handle;
	io = file->io;
	DWORD  indexOffset = d4(props, index_offset);	//rm�ļ�Index��������ļ���ʼ����ƫ��
	Seek( indexOffset );							//��λ��Index�鴦
	INDEX_CHUNK_HEADER chead;						//Index��ͷ���Ľṹ
	io->read(hd, (char*)&chead, sizeof(INDEX_CHUNK_HEADER));//��ȡIndex���ͷ��
	
	//DWORD size = d4((INDEX_CHUNK_HEADER*)&chead, size);//����index��(����index��¼)�Ĵ�С
	//DWORD num_indices = d4((INDEX_CHUNK_HEADER*)&chead, num_indices);//index���еļ�¼��
	////ASSERT( size == sizeof(INDEX_CHUNK_HEADER) + sizeof(INDEX_RECORD)*num_indices );

	INDEX_CHUNK_HEADER* pIndex = (INDEX_CHUNK_HEADER*)&chead;
	//add by xianjian at 3.16
	if( pIndex->object_id != rmffFOURCC('I', 'N', 'D', 'X') ){
		return NULL;								//assert that we read the true INDEX chunk
	}

	DWORD size = d4(pIndex, size);//����index��(����index��¼)�Ĵ�С
	DWORD num_indices = d4(pIndex, num_indices);//index���еļ�¼��

	INDEX_RECORD *ir ;					//index��¼�ṹ
	ir = new INDEX_RECORD[num_indices];	//�����¼��Ŀռ�

	
	io->read(hd, ir, sizeof(INDEX_RECORD)*num_indices);//��ȡ��¼
	
								
	DWORD recordOffset;					//forѭ���е�ÿ����¼��ָ�Ĺؼ�֡��ƫ��
	rmff_frame_t* frame = NULL;			//�ؼ�֡��ָ��,�����ķ���ֵ
	for(int i = 0; i < num_indices; i++)
	{
		recordOffset = d4( ir+i, offset); //��i����¼��ָ�Ĺؼ�֡��ƫ��
		if( recordOffset > offset ) 
		{
			//if( recordOffset > 150000000) cout<< recordOffset<<endl;
			Seek( recordOffset );
			frame = rmff_read_next_frame(file, NULL);
			if( frame != NULL ) {
				file->num_packets_read --;
				break;//����nSec��ĵ�һ���ؼ�֡
			}
			
		}
	}
	delete ir;			//�ͷ�������Ļ�����

	//if( i == num_indices )	
	//{
	//	return -1;		//�Ҳ������������Ĺؼ�֡,����-1;
	//}
	return frame;		//����ָ����һ����ȷ�Ĺؼ�֡��ָ��;

	
}







#undef d2
#undef d4




