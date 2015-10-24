
#pragma once
#include <list>


#pragma pack(push)  // �������״̬ 
#pragma pack(1)     // �趨Ϊ1�ֽڶ���

// PNG�ļ���ʽ�е����ݿ�
// ���ݿ���� ���ݿ�����      �����ݿ� ��ѡ�� λ������
// IHDR      �ļ�ͷ���ݿ�           ��     �� ��һ��
// cHRM      ��ɫ�Ͱ�ɫ�����ݿ�     ��     �� ��PLTE��IDAT֮ǰ
// gAMA      ͼ������ݿ�           ��     �� ��PLTE��IDAT֮ǰ
// sBIT      ������Чλ���ݿ�       ��     �� ��PLTE��IDAT֮ǰ
// PLTE      ��ɫ�����ݿ�           ��     �� ��IDAT֮ǰ
// bKGD      ������ɫ���ݿ�         ��     �� ��PLTE֮��IDAT֮ǰ
// hIST      ͼ��ֱ��ͼ���ݿ�       ��     �� ��PLTE֮��IDAT֮ǰ
// tRNS      ͼ��͸�����ݿ�         ��     �� ��PLTE֮��IDAT֮ǰ
// oFFs      (ר�ù������ݿ�)       ��     �� ��IDAT֮ǰ
// pHYs      �������سߴ����ݿ�     ��     �� ��IDAT֮ǰ
// sCAL      (ר�ù������ݿ�)       ��     �� ��IDAT֮ǰ
// IDAT      ͼ�����ݿ�             ��     �� ������IDAT����
// tIME      ͼ������޸�ʱ�����ݿ� ��     �� ������
// tEXt      �ı���Ϣ���ݿ�         ��     �� ������
// zTXt      ѹ���ı����ݿ�         ��     �� ������
// fRAc      (ר�ù������ݿ�)       ��     �� ������
// gIFg      (ר�ù������ݿ�)       ��     �� ������
// gIFt      (ר�ù������ݿ�)       ��     �� ������
// gIFx      (ר�ù������ݿ�)       ��     �� ������
// IEND      ͼ���������           ��     �� ���һ�����ݿ�



typedef struct tagPngSig
{
    __int64 signature;  // "89 50 4e 47 0d 0a 1a 0a "

}PngSig, *PPngSig;

// ���ݿ�ͷ
// Length(����) 4�ֽ� ָ�����ݿ���������ĳ���
// Chunk Type Code(���ݿ�������) 4�ֽ� ���ݿ���������ASCII��ĸ(A-Z��a-z)��� 
// Chunk Data(���ݿ�����) �ɱ䳤�� �洢����Chunk Type Codeָ�������� 
// CRC(ѭ��������) 4�ֽ� �洢��������Ƿ��д����ѭ��������,��Chunk Type Code���Chunk Data���е����ݽ��м���õ���
typedef struct tagChunkDataHeader
{
    unsigned long length;
    char type[4];

}ChunkDataHeader, *PChunkDataHeader;

// �ļ�ͷ���ݿ�
// Width	            4 bytes ͼ���ȣ�������Ϊ��λ
// Height	            4 bytes ͼ��߶ȣ�������Ϊ��λ
// Bit depth            1 byte  ͼ����ȣ�
//                              0��������ɫͼ��1��2��4��8
//                              1���Ҷ�ͼ��1��2��4��8��16
//                              2�����ɫͼ��8��16
// ColorType            1 byte  ��ɫ���ͣ�
//                              0���Ҷ�ͼ��, 1ͨ��
//                              2�����ɫͼ��3ͨ��
//                              3��������ɫͼ��1ͨ��
//                              4������ͨ�����ݵĻҶ�ͼ��2ͨ��
//                              6������ͨ�����ݵ����ɫͼ��4ͨ��
// Compression method   1 byte  ѹ������(LZ77�����㷨)
// Filter method        1 byte  �˲�������
// Interlace method     1 byte  ����ɨ�跽����
//                              0���Ǹ���ɨ��
//                              1�� Adam7(��Adam M. Costello������7�����ɨ�跽��)

typedef struct tagIHDR
{
    unsigned long width;
    unsigned long heigth;
    unsigned char bitDepth;
    unsigned char colorType;
    unsigned char compression;
    unsigned char filter;
    unsigned char interlace;

}IHDR, *PIHDR;


// Pixels per unit, X axis 4 bytes (PNG unsigned integer) ÿ��������
// Pixels per unit, Y axis 4 bytes (PNG unsigned integer) 
// Unit specifier 1 byte 
//  The following values are defined for the unit specifier: ָ����λ
//  0 unit is unknown 
//  1 unit is the metre ��

typedef struct tagpHYs
{
    unsigned long x;
    unsigned long y;
    unsigned char specifier;

}pHYs, *PpHYs;

// Profile name 1-79 bytes (character string) 
//  Null separator 1 byte (null character) 
//  Compression method 1 byte  0��ʾdeflateѹ��
//  Compressed profile n bytes 

typedef struct tagiCCP
{
    char name[80]; // ��һ����'\0'��β���ַ���,������
    unsigned char compression;
    char *text;

}iCCP, *PiCCP;


// White point x 4 bytes 
// White point y 4 bytes 
// Red x 4 bytes 
// Red y 4 bytes 
// Green x 4 bytes 
// Green y 4 bytes 
// Blue x 4 bytes 
// Blue y 4 bytes 

typedef struct tagcHRM
{
    unsigned long whiteX;
    unsigned long whiteY;
    unsigned long redX;
    unsigned long redY;
    unsigned long greenX;
    unsigned long greenY;
    unsigned long blueX;
    unsigned long blueY;

}cHRM, *PcHRM;

// ٤��У�����Ƕ�ͼ���٤�����߽��б༭���Զ�ͼ����з�����ɫ���༭�ķ�����
// ���ͼ���ź��е���ɫ���ֺ�ǳɫ���֣���ʹ���߱������󣬴Ӷ����ͼ��Աȶ�Ч����
// ���ǲ��ҵ�һ�����Ǹ���ƽ̨��gamma����ֵ�ǲ�һ���ġ�window��Gammaϵ��Ϊ2.2��
// Mac��GammaΪ1.8��Gamma������ζ�Ŷ���ÿһ��R/G/B�����Ǳ��뱻ת����R��=Rgamma��
// G��=Ggamma��B��=Bgamma��Ȼ���ٴ��ݵ�����ϵͳ��

// Image gamma 4 bytes 
typedef struct taggAMA
{
    unsigned long gamma;

}gAMA, *PgAMA;


// Rendering intent 1 byte 
// 
// The following values are defined for rendering intent:
// 
// 0 Perceptual for images preferring good adaptation to the output device gamut at the expense of colorimetric accuracy, such as photographs. 
// 1 Relative colorimetric for images requiring colour appearance matching (relative to the output device white point), such as logos. 
// 2 Saturation for images preferring preservation of saturation at the expense of hue and lightness, such as charts and graphs. 
// 3 Absolute colorimetric for images requiring preservation of absolute colorimetry, such as previews of images destined for a different output device (proofs). 

typedef struct tagsRGB
{
    unsigned char rendering;

}sRGB, *PsRGB;


// ��IHDR��Colour type

// Colour type 0 
//  significant greyscale bits 1 byte 
// Colour types 2 and 3 
//  significant red bits 1 byte 
//  significant green bits 1 byte 
//  significant blue bits 1 byte 
// Colour type 4 
//  significant greyscale bits 1 byte 
//  significant alpha bits 1 byte 
// Colour type 6 
//  significant red bits 1 byte 
//  significant green bits 1 byte 
//  significant blue bits 1 byte 
//  significant alpha bits 1 byte 

typedef struct tagsBIT
{

}sBIT, *PsBIT;


// Colour types 0 and 4 
//  Greyscale 2 bytes 
// Colour types 2 and 6 
//  Red 2 bytes 
//  Green 2 bytes 
//  Blue 2 bytes 
// Colour type 3 
//  Palette index 1 byte 

typedef struct tagbKGD
{

}bKGD, *PbKGD;


typedef struct tagoFFs
{

}oFFs, *PoFFs;

// Year 2 bytes (complete; for example, 1995, not 95) 
//  Month 1 byte (1-12) 
//  Day 1 byte (1-31) 
//  Hour 1 byte (0-23) 
//  Minute 1 byte (0-59) 
//  Second 1 byte (0-60) (to allow for leap seconds) 
typedef struct tagtIME
{
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;

}tIME, *PtIME;

// Keyword 1-79 bytes (character string) 
//  Null separator 1 byte (null character) 
//  Text string 0 or more bytes (character string) 
typedef struct tagtEXt
{
    char keyword[80];
    char text[MAX_PATH]; // ���ݳ� = ���������ݳ� - keyword�� -1

}tEXt, *PtEXt;

// Keyword 1-79 bytes (character string) 
//  Null separator 1 byte (null character) 
//  Compression method 1 byte �Ƿ�ѹ��
//  Compressed text datastream n bytes 
typedef struct tagzTXt
{
    char keyword[80];
    unsigned char compression;
    char compressed[MAX_PATH];

}zTXt, *PzTXt;

// Keyword 1-79 bytes (character string) 
//  Null separator 1 byte (null character) 
//  Compression flag 1 byte 
//  Compression method 1 byte 
//  Language tag 0 or more bytes (character string) 
//  Null separator 1 byte (null character) 
//  Translated keyword 0 or more bytes 
//  Null separator 1 byte (null character) 
//  Text 0 or more bytes 
typedef struct tagiTXt
{
    char keyword[80];
    unsigned char compression;
    unsigned char compressed;
    char language[80];
    char translated[80];
    char text[80];

}iTXt, *PiTXt;


//pCAL sTER vpAg 

#pragma pack(pop)   // �ָ�����״̬


typedef std::list<void*> LIST_VOID; // �ڲ���,ChunkDataHeaderָ��


/**
 *\class  CPNG
 *\brief  ������Ҫ������ʾPNGͼƬ
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class CPNG
{
public:
    /**
     *\fn         CPNG()
     *\brief      ȱʡ���캯����
     */
    CPNG();

    /**
     *\fn         ~CPNG()
     *\brief      ����������
     */
    ~CPNG();

private:
    unsigned char   *fileData_; // PNG�ļ�����
    int             fileLen_;   // PNG�ļ����ݳ�

    unsigned char   *data_;     // BMP�ļ�����
    int             dataLen_;   // BMP�ļ����ݳ�

    PIHDR           iher_;      // ͷ��
    PpHYs           phys_;      // �����
    iCCP            iccp_;      // Ƕ����Ϣ
    PcHRM           chrm_;      // �׵���Ϣ
    PgAMA           gama_;      // gamaУ��
    PsRGB           srgb_;      // RGB��Ϣ
    PsBIT           sbit_;      // BIT��Ϣ
    PtIME           time_;      // ʱ��
    tEXt            text_;      // �ı���Ϣ

    int             channelNum_;// ͨ����
    int             width_;     // ͼƬ��
    int             height_;    // ͼƬ��

    /**
     *\fn         ProcFile()
     *\brief      ����ͼƬ�ļ���
     *\return     �Ƿ�ɹ�
     */
    bool ProcFile();

    /**
     *\fn         ProcSig(const char *data)
     *\brief      ����PNG�ļ�ͷ��
     *\param[in]  data  PNG�ļ�ͷ��
     *\return     �Ƿ�ɹ�
     */
    bool ProcSig(const char *data);

    /**
     *\fn         ProcIHDR(const char *data)
     *\brief      ����PNG�ļ�IHDR�ڡ�
     *\param[in]  data     PNG�ļ�IHDR�ڡ�
     *\return     �Ƿ�ɹ�
     */
    bool ProcIHDR(const char *data);

    /**
     *\fn         ProcICCP(const char *data, int dataLen)
     *\brief      ����PNG�ļ�ICCP�ڡ�
     *\param[in]  data     PNG�ļ�ICCP�ڡ�
     *\param[in]  dataLen  PNG�ļ�ICCP�����ݳ���
     *\return     �Ƿ�ɹ�
     */
    bool ProcICCP(const char *data, int dataLen);

    /**
     *\fn         ProcTEXT(const char *data, int dataLen)
     *\brief      ����PNG�ļ�TEXT�ڡ�
     *\param[in]  data     PNG�ļ�TEXT�ڡ�
     *\param[in]  dataLen  PNG�ļ�TEXT�����ݳ���
     *\return     �Ƿ�ɹ�
     */
    bool ProcTEXT(const char *data, int dataLen);

    /**
     *\fn         ProcDATA(const char *data, int dataLen)
     *\brief      ����PNG�ļ�DATA�ڡ�
     *\param[in]  data     PNG�ļ�DATA�ڡ�
     *\param[in]  dataLen  PNG�ļ�DATA�����ݳ���
     *\return     �Ƿ�ɹ�
     */
    bool ProcDATA(const char *data, int dataLen);

    /**
     *\fn         UncompressDATA()
     *\brief      ��ѹ���ݡ�
     *\return     �Ƿ�ɹ�
     */
    bool UncompressDATA();

    /**
     *\fn         ProcFilter(unsigned char *row, const unsigned char *lastRow, int rowLen, int filterType)
     *\brief      ����һ��ͼ�����ݡ�
     *\param[in]  row        �������ݡ�
     *\param[in]  lastRow    ��һ�����ݡ�
     *\param[in]  rowLen     �������ݳ���
     *\param[in]  filterType ���������͡�
     *\return     �Ƿ�ɹ�
     */
    void ProcFilter(unsigned char *row, const unsigned char *lastRow, int rowLen, int filterType);

    /**
     *\fn         ToBitmap(const unsigned char *image)
     *\brief      ��ͼ��ת��BITMAPͼ��
     *\param[in]  image       �������ݡ�
     *\return     �Ƿ�ɹ�
     */
    void ToBitmap(const unsigned char *image);

    /**
     *\fn         getData(unsigned long data)
     *\brief      ����ͷ˳��ı�Сͷ˳��
     *\param[in]  data  ���ݡ�
     *\return     Сͷ����
     */
    unsigned long getData(unsigned long data)
    {
        unsigned long n1 = (data & 0xFF000000) >> 24;
        unsigned long n2 = (data & 0x00FF0000) >> 8;
        unsigned long n3 = (data & 0x0000FF00) << 8;
        unsigned long n4 = (data & 0x000000FF) << 24;

        return n1 | n2 | n3 | n4;
    }

public:
    /**
     *\fn         Load(HMODULE module, int id)
     *\brief      ����ͼƬ��
     *\param[in]  molule  ���Ϊ�մ�exe���أ������dll���ء�
     *\param[in]  id      ͼƬID��
     *\return     �Ƿ�ɹ�
     */
    bool Load(HMODULE module, int id);

    /**
     *\fn         Load(const char *filname)
     *\brief      ����ͼƬ��
     *\param[in]  filname  ͼƬ�ļ�����
     *\return     �Ƿ�ɹ�
     */
    bool Load(const char *filname);

    /**
     *\fn         Draw(HDC dc, int x, int y)
     *\brief      ��ͼƬ����DC�ϡ�
     *\param[in]  dc  Ҫ����ͼƬ��DC��
     *\param[in]  x   ͼƬx���ꡣ
     *\param[in]  y   ͼƬy���ꡣ
     *\return     �Ƿ�ɹ�
     */
    bool Draw(HDC dc, int x, int y);

    /**
     *\fn         SaveBmp(const char *filname)
     *\brief      ��ͼƬ���BITMAP��ʽ��
     *\param[in]  filname  ͼƬ�ļ�����
     *\return     �Ƿ�ɹ�
     */
    bool SaveBmp(const char *filname);

    /**
     *\fn         getWidth()
     *\brief      �õ�ͼƬ��
     *\return     ͼƬ��
     */
    int getWidth() { return width_; }

    /**
     *\fn         getHeight()
     *\brief      �õ�ͼƬ�ߡ�
     *\return     ͼƬ��
     */
    int getHeight() { return height_; }

    /**
     *\fn         getData()
     *\brief      �õ�ͼƬ���ݡ�
     *\return     ͼƬ����
     */
    unsigned char * getData() { return data_; }

    /**
     *\fn         getBmpInfo()
     *\brief      �õ�λͼ�ṹ���ݡ�
     *\return     λͼ�ṹ����
     */
    BITMAPINFOHEADER* getBmpInfo() { return (BITMAPINFOHEADER*)&data_[sizeof(BITMAPFILEHEADER)]; }
};
