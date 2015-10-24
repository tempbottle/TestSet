#pragma once
#include <math.h>

#pragma pack(push)	// �������״̬ 
#pragma pack(1)		// �趨Ϊ1�ֽڶ���

typedef struct _tagColorTable
{
	BYTE byR;
	BYTE byG;
	BYTE byB;

}COLORTABLE,*PCOLORTABLE;

typedef struct _tagGraphicControlExtension	// ��Ҫ89a�汾
{
	BYTE byFlag1;							// 0x21,��չ��
	BYTE byFlag2;							// 0xF9,ͼ�ο�����չ��
	BYTE byBlockSize;						// ���С,���������ս���
	struct flag{
		BYTE t:1;							// �Ƿ�Ӧ��͸��ɫ��־
		BYTE i:1;							// �û������־
		BYTE s:3;							// 0������,1ͼ�δӵ�ǰλ����ȥ,2�ظ�������ɫ,3�ظ�����ǰ״̬
		BYTE o:3;							// ����
	}Flag;
	WORD wDelayTime;						// �ӳ�ʱ��,��λ1/100��
	BYTE byTransparentColorIndex;			// ͸��ɫ����ֵ
	BYTE byBlockTerminator;					// ���ս���,�̶�ֵ0

}GRAPHICCONTROLEX,*PGRAPHICCONTROLEX;

typedef struct _tagCommentExtension			// ��Ҫ89a�汾
{
	BYTE byFlag1;							// 0x21,��չ��
	BYTE byFlag2;							// 0xFE,ע����չ��
	BYTE byTextLen;							// ע�ͳ�
	CHAR szText[256];						// ע�Ϳ�(n��7bit ASCII�ַ���,Ӣ��)
	BYTE byBlockTerm;						// ���ս���,�̶�ֵ0
}COMMENTEX,*PCOMMENTEX;

typedef struct _tagPlainTextExtension		// ��Ҫ89a�汾
{
	BYTE byFlag1;							// 0x21,��չ��
	BYTE byFlag2;							// 0x01,ͼ���ı���չ��
	BYTE byBlockSize;						// 0x0c,���С
	BYTE byOffsetX;							// �ı������߼���Ļ����߽����
	BYTE byOffsetY;							// �ı������߼���Ļ���ϱ߽����
	BYTE byWidth;							// �ı���߶�
	BYTE byHeight;							// �ı���߶�
	BYTE byCelWidth;						// �ַ���Ԫ����
	BYTE byCelHeight;						// �ַ���Ԫ��߶�
	BYTE byTestColor;						// �ַ���ȫ����ɫ�б��е�����
	BYTE byBackColor;						// �ַ�������ȫ����ɫ�б��е�����
	CHAR szText[256];						// �ַ���(n��)
	BYTE byBlockTerm;						// ���ս���,�̶�ֵ0

}PLAINTEXTEX,*PPLAINTEXTEX;

typedef struct _tagApplicationExtension		// ��Ҫ89a�汾
{
	BYTE byFlag1;							// 0x21,��չ��
	BYTE byFlag2;							// 0xFF,Ӧ����չ��
	BYTE byBlockSize;						// 0x0b,���С
	BYTE byIdentifier[8];					// Ӧ�ó�������ı�ʶ(8������ASCII�ַ�)
	BYTE byCode[3];							// Ӧ�ó�����������ʶ��(3������ASCII�ַ�)
	BYTE byDataLen;							// ���ݳ�
	BYTE byData[256];						// Ӧ�ó����Զ������ݿ�,һ���������ݿ����
	BYTE byBlockTerm;						// ���ս���,�̶�ֵ0

}APPEX,*PAPPEX;

typedef struct _tagHeader
{
	BYTE bySignature[3];					// "GIF"
	BYTE byVersion[3];						// "87a" �� "89a"

}GIFHEADER, *PGIFHEADER;

typedef struct _tagScrDesc					// �߼���Ļ������
{
	WORD wWidth;							// �ܵĿ��
	WORD wHeight;							// �ܵĸ߶�
	struct globalflag
	{
		BYTE PalBits : 3;					// ȫ����ɫ�б��С��PalBits+1ȷ����ɫ�б����������2��PalBits+1�η���
		BYTE SortFlag : 1;					// ����ȫ�ֵ�ɫ���е�RGB��ɫֵ�Ƿ���ʹ���ʽ��дӸߵ��׵Ĵ��������
		BYTE ColorRes : 3;					// ��ɫ��ȣ�ColorRes+1ȷ��ͼ�����ɫ���
		BYTE GlobalPal : 1;					// ȫ����ɫ�б��־������λʱ��ʾ��ȫ����ɫ�б�pixelֵ������
	}GlobalFlag;
	BYTE byBackground;						// ������ɫ����
	BYTE byAspect;							// ���ؿ�߱�

}GIFSCRDESC, *PGIFSCRDESC;

typedef struct _tagDataSubBlock
{
	BYTE			byDataLen;
	BYTE			pbyData[256];
	_tagDataSubBlock *pNext;

}DATASUBBLOCK, *PDATASUBBLOCK;

typedef struct _tagImage
{
	BYTE byFlag;							// 0x2C
	WORD wLeft;								// ������ͼ����ߵľ���
	WORD wTop;								// ������ͼ���ϱߵľ���
	WORD wWidth;							// Сͼ��Ŀ�
	WORD wHeight;							// Сͼ��ĸ�
	struct localflag
	{
		BYTE PalBits : 3;					// �ֲ���ɫ�б��С��PalBits+1ȷ����ɫ�б����������2��PalBits+1�η���
		BYTE Reserved : 2;					// ����,�����ʼ��Ϊ0
		BYTE SortFlag : 1;					// �����ֲ���ɫ���е�RGB��ɫֵ�Ƿ���ʹ���ʽ��дӸߵ��׵Ĵ��������
		BYTE Interlace : 1;					// ��֯��־,��λʱͼ������ʹ��������ʽ���У�����ʹ��˳�����С�
		BYTE LocalPal : 1;					// �ֲ���ɫ�б��־,ֵ��ʱʹ��ȫ����ɫ�б�
	}LocalFlag;

	UINT			nColorNum;				// ������ɫ����ɫ��
	COLORTABLE		LocolColorTable[255];	// ��ɫ��
	BYTE			byLZWcode;				// ����λ��С����gifͨ��Ϊ1,4,8
	UINT			nDataBlcokNum;			// �����ݿ���
	PDATASUBBLOCK	pData;					// �����ݿ�

}GIFIMAGE, *PGIFIMAGE;

typedef struct _tagFrame
{
	GRAPHICCONTROLEX gce;					// ͼ����ƿ�
	GIFIMAGE		 gi;					// һ֡ͼ��
	_tagFrame*		 pNext;					// ��һ֡

	BYTE			 byFrameId;				// ֡ID
	BYTE*			 pBmpData;				// ת��BMP�ļ������� 

}GIFFRAME, *PGIFFRAME;

typedef struct _tagGIF
{
	GIFHEADER		Head;					// �ļ�ͷ��
	GIFSCRDESC		ScrDesc;				// �߼���Ļ������
	UINT			nColorNum;				// ȫ����ɫ�����ɫ��
	COLORTABLE		GlobalColorTable[256];	// ȫ����ɫ��
	UINT			nGifFrameNum;			// ֡��
	PGIFFRAME		pGifFrame;				// ֡����

	BITMAPINFO		BmpInfo;				// BMP�ļ���Ϣͷ

}GIF,*PGIF;

#pragma pack(pop)	// �ָ�����״̬

//-------------------------------------------------------------------------------------------------------

/**
 *\class  CGIF
 *\brief  ������Ҫ������ʾGIFͼƬ
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class CGIF
{
public:
	/**
	 *\fn         CGIF()
	 *\brief      ȱʡ���캯����
	 */
	CGIF();

	/**
	 *\fn         ~CGIF()
	 *\brief      ����������
	 */
	~CGIF();

private:
	GIF	gif_;

protected:
	/**
	 *\fn         DecodeImageData(PGIFFRAME frame)
	 *\brief      LZWѹ���㷨�Ľ������ݡ�
	 *\param[in]  pFrame  ֡���ݡ�
	 *\return     �Ƿ�ɹ�
	 */
	LPBYTE DecodeImageData(PGIFFRAME frame);

	/**
	 *\fn         GetFrameBmpData(PGIFFRAME frame, PGIFFRAME lastFrame)
	 *\brief      �õ�λͼ���ݡ�
	 *\param[in]  frame      ֡���ݡ�
	 *\param[in]  lastFrame  ��һ֡���ݡ�
	 *\return     �Ƿ�ɹ�
	 */
	LPBYTE GetFrameBmpData(PGIFFRAME frame, PGIFFRAME lastFrame);

	/**
	 *\fn         ProcData(unsigned char *data)
	 *\brief      ����GIF�ļ���
	 *\param[in]  data  �ļ����ݡ�
	 *\return     �Ƿ�ɹ�
	 */
	bool ProcData(unsigned char *data);

public:
	/**
	 *\fn         Load(const char *filename)
	 *\brief      �����ļ���
	 *\param[in]  filename  �ļ�����
	 *\return     �Ƿ�ɹ�
	 */
	bool Load(const char *filename);

	/**
	 *\fn         Unload()
	 *\brief      ������ݡ�
	 */
	void Unload();

	/**
	 *\fn         Draw(HDC dc, int x, int y, int frameId)
	 *\brief      ��ͼƬ����DC�ϡ�
	 *\param[in]  dc        Ҫ����ͼƬ��DC��
	 *\param[in]  x         ͼƬx���ꡣ
	 *\param[in]  y         ͼƬy���ꡣ
	 *\param[in]  frameId   ͼƬ֡ID��
	 *\return     �Ƿ�ɹ�
	 */
	bool Draw(HDC dc, int x, int y, int frameId);

	/**
	 *\fn         SaveBmp(const char *filname, int frameId)
	 *\brief      ��ͼƬ���BITMAP��ʽ��
	 *\param[in]  filname  ͼƬ�ļ�����
	 *\param[in]  frameId   ͼƬ֡ID��
	 *\return     �Ƿ�ɹ�
	 */
	bool SaveBmp(const char *filename, int frameId);

	/**
	 *\fn         getWidth()
	 *\brief      �õ�ͼƬ��
	 *\return     ͼƬ��
	 */
	int getWidth() { return gif_.ScrDesc.wWidth;; }

	/**
	 *\fn         getHeight()
	 *\brief      �õ�ͼƬ�ߡ�
	 *\return     ͼƬ��
	 */
	int getHeight() { return gif_.ScrDesc.wHeight; }

	/**
	 *\fn         getFrameCount()
	 *\brief      �õ�ͼƬ֡����
	 *\return     ͼƬ֡��
	 */
	int getFrameCount() { return gif_.nGifFrameNum; }

	/**
	 *\fn         getDelayTime()
	 *\brief      �õ����ʱ�䡣
	 *\return     ���ʱ��
	 */
	int getDelayTime(int frameId);	
};
