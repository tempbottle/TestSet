#include "StdAfx.h"
#include "gif.h"


/**
 *\fn         CGIF()
 *\brief      ȱʡ���캯����
 */
CGIF::CGIF()
{
	memset(&gif_, 0, sizeof(GIF));
}

/**
 *\fn         ~CGIF()
 *\brief      ����������
 */
CGIF::~CGIF()
{
	Unload();
}

/**
 *\fn         Load(const char *filename)
 *\brief      �����ļ���
 *\param[in]  filename  �ļ�����
 *\return     �Ƿ�ɹ�
 */
bool CGIF::Load(const char *filename)
{
	if (NULL == filename) return false;

	Unload();

	FILE *file = fopen(filename, "rb");

	if (NULL == file) return false;

	fseek(file, 0, SEEK_END);

	int fileSize = ftell(file);

	fseek(file, 0, SEEK_SET);

	unsigned char *data = new unsigned char[fileSize];

	fread(data, fileSize, 1, file);

	fclose(file);

	return ProcData(data);
}

/**
 *\fn         ProcData(unsigned char *data)
 *\brief      ����GIF�ļ���
 *\param[in]  data  �ļ����ݡ�
 *\return     �Ƿ�ɹ�
 */
bool CGIF::ProcData(unsigned char *data)
{
	int headLen = sizeof(GIFHEADER);
	int scrDesLen = sizeof(GIFSCRDESC);

	memcpy(&gif_.Head, data, headLen);
	data += headLen;

	memcpy(&gif_.ScrDesc, data, scrDesLen);
	data += scrDesLen;

	if (gif_.ScrDesc.GlobalFlag.GlobalPal)
	{
		gif_.nColorNum = (UINT)pow(2.0, gif_.ScrDesc.GlobalFlag.PalBits + 1);
		int nColorTableLen = sizeof(COLORTABLE) * gif_.nColorNum;
		memcpy(gif_.GlobalColorTable, data, nColorTableLen);
		data += nColorTableLen;
	}

	bool exit = false;
	while(!exit)
	{
		switch (*data)
		{
		case 0x21:	// ��չ��
			{
				switch (*(data+1))
				{
				case 0x01:	// ͼ���ı���չ
					{
						PLAINTEXTEX plainex = {0};
						plainex.byFlag1     = *data++;
						plainex.byFlag2     = *data++;
						plainex.byBlockSize = *data++;
						plainex.byOffsetX   = *data++;
						plainex.byOffsetY   = *data++;
						plainex.byWidth     = *data++;
						plainex.byHeight    = *data++;
						plainex.byCelWidth  = *data++;
						plainex.byCelHeight = *data++;
						plainex.byTestColor = *data++;
						plainex.byBackColor = *data++;
						strcpy(plainex.szText, (LPSTR)data);
						data += strlen(plainex.szText);
						plainex.byBlockTerm = *data++;
						ATLTRACE("GIF LAINTEXTEX %s\n", plainex.szText);
						break;
					}
				case 0xF9:	// ͼ�������չ
					{
						PGIFFRAME pFrame = new GIFFRAME;
						memset(pFrame, 0, sizeof(GIFFRAME));
						memcpy(&pFrame->gce, data, sizeof(GRAPHICCONTROLEX));
						data += sizeof(GRAPHICCONTROLEX);

						pFrame->byFrameId = gif_.nGifFrameNum++;
						if (gif_.pGifFrame == NULL)
						{
							gif_.pGifFrame = pFrame;
						}
						else
						{
							PGIFFRAME pEndFrame = gif_.pGifFrame;
							while(pEndFrame->pNext != NULL) pEndFrame = pEndFrame->pNext;
							pEndFrame->pNext = pFrame;
						}

						break;
					}
				case 0xFE:	// ע����չ
					{
						COMMENTEX commentex = {0};
						commentex.byFlag1 = *data++;
						commentex.byFlag2 = *data++;
						commentex.byTextLen = *data++;
						memcpy(commentex.szText, data, commentex.byTextLen);
						data += commentex.byTextLen;
						commentex.byBlockTerm = *data++;
						ATLTRACE("GIF COMMENTEX: %s\n", commentex.szText);
						break;
					}
				case 0xFF:	// Ӧ����չ
					{
						APPEX appex = {0};
						appex.byFlag1 = *data++;
						appex.byFlag2 = *data++;
						appex.byBlockSize = *data++;
						memcpy(appex.byIdentifier, data, 8);
						data += 8;
						memcpy(appex.byCode, data, 3);
						data += 3;
						appex.byDataLen = *data++;
						memcpy(appex.byData, data, appex.byDataLen);
						data += appex.byDataLen;
						appex.byBlockTerm = *data++;
					break;
					}
				default:
					{
						exit = true;
						ATLTRACE("EX ERROR\n");
						break;
					}
				}

			break;
			}
		case 0x2c:	// ͼ��
			{
				if (NULL == gif_.pGifFrame)
				{
					gif_.pGifFrame = new GIFFRAME;
					memset(gif_.pGifFrame, 0, sizeof(GIFFRAME));
					gif_.pGifFrame->byFrameId = gif_.nGifFrameNum++;
				}
				
				PGIFFRAME pFrame = gif_.pGifFrame;
				while(pFrame->pNext != NULL) pFrame = pFrame->pNext;

				pFrame->gi.byFlag = *data++;
				pFrame->gi.wLeft = *(WORD*)data;
				data += 2;
				pFrame->gi.wTop = *(WORD*)data;
				data += 2;
				pFrame->gi.wWidth = *(WORD*)data;
				data += 2;
				pFrame->gi.wHeight = *(WORD*)data;
				data += 2;
				memcpy(&(pFrame->gi.LocalFlag), data, 1);
				data++;

				if (pFrame->gi.LocalFlag.LocalPal)
				{
					pFrame->gi.nColorNum = (UINT)pow(2.0, pFrame->gi.LocalFlag.PalBits + 1);
					int nColorTableLen = sizeof(COLORTABLE) * pFrame->gi.nColorNum;
					memcpy(pFrame->gi.LocolColorTable, data, nColorTableLen);
					data += nColorTableLen;
				}

				pFrame->gi.byLZWcode = *data++;

				while(*data != 0)
				{
					PDATASUBBLOCK pData = new DATASUBBLOCK;
					memset(pData, 0, sizeof(DATASUBBLOCK));
					pData->byDataLen = *data++;
					memcpy(pData->pbyData, data, pData->byDataLen);
					data += pData->byDataLen;

					if (pFrame->gi.pData == NULL)
					{
						pFrame->gi.pData = pData;
					}
					else
					{
						PDATASUBBLOCK pTemp = pFrame->gi.pData;
						while(pTemp->pNext != NULL) pTemp = pTemp->pNext;
						pTemp->pNext = pData;
					}

					pData = pData->pNext;
					pFrame->gi.nDataBlcokNum++;
				}
				data++;

			break;
			}
		case 0x3B:	// �ļ�����
			{
				exit = true;
				break;
			}
		default:
			{
				exit = true;
				break;
			}
		}
	}

	// BMPͷ
	gif_.BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	gif_.BmpInfo.bmiHeader.biWidth = gif_.ScrDesc.wWidth;
	gif_.BmpInfo.bmiHeader.biHeight = gif_.ScrDesc.wHeight;
	gif_.BmpInfo.bmiHeader.biPlanes = 1;
	gif_.BmpInfo.bmiHeader.biBitCount = 24;
	gif_.BmpInfo.bmiHeader.biCompression = BI_RGB;
	gif_.BmpInfo.bmiHeader.biSizeImage = 0;
	gif_.BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	gif_.BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	gif_.BmpInfo.bmiHeader.biClrUsed = 0;
	gif_.BmpInfo.bmiHeader.biClrImportant = 0;

	PGIFFRAME pFrame = gif_.pGifFrame;
	PGIFFRAME pLastFrame = NULL;

	while (NULL != pFrame)
	{
		pFrame->pBmpData = GetFrameBmpData(pFrame, pLastFrame);
		pLastFrame = pFrame;
		pFrame = pFrame->pNext;
	}
	
	DWORD dwPos1 = 0;
	DWORD dwPos2 = 0;
	WORD wWidth = gif_.ScrDesc.wWidth;
	WORD wHeight = gif_.ScrDesc.wHeight;
	WORD wLineInter = (wWidth%4 != 0) ? (4-(wWidth*3)%4) : 0;
	WORD wLineDataLen = wWidth * 3 + wLineInter;
	PBYTE pLine = new BYTE[wLineDataLen];
	pFrame = gif_.pGifFrame;

	while (NULL != pFrame)
	{
		PBYTE pPixData = pFrame->pBmpData;
		for (WORD i = 0; i < wHeight / 2; i++)
		{
			dwPos1 = i * wLineDataLen;
			dwPos2 = (wHeight - i - 1) * wLineDataLen;
			memcpy((void*)pLine, (void*)&(pPixData[dwPos1]), wLineDataLen);
			memcpy((void*)&(pPixData[dwPos1]), (void*)&(pPixData[dwPos2]), wLineDataLen);
			memcpy((void*)&(pPixData[dwPos2]), (void*)pLine, wLineDataLen);
		}

		pLastFrame = pFrame;
		pFrame = pFrame->pNext;
	}

	delete[] pLine;
	return true;
}

/**
 *\fn         Unload()
 *\brief      ������ݡ�
 */
void CGIF::Unload()
{
	gif_.nGifFrameNum = 0;

	while (gif_.pGifFrame != NULL)
	{
		PGIFFRAME pFrame = gif_.pGifFrame;
		PGIFFRAME pNextFrame = gif_.pGifFrame->pNext;
		while (pFrame->gi.pData != NULL)
		{
			PDATASUBBLOCK pNextData = pFrame->gi.pData->pNext;
			delete pFrame->gi.pData;

			pFrame->gi.pData = pNextData;
		}

		if (NULL != pFrame->pBmpData) delete[] pFrame->pBmpData;
		delete pFrame;
		gif_.pGifFrame = pNextFrame;
	}
	memset(&gif_, 0, sizeof(GIF));
}

/**
 *\fn         getDelayTime()
 *\brief      �õ����ʱ�䡣
 *\return     ���ʱ��
 */
int CGIF::getDelayTime(int frameId)
{
	PGIFFRAME pFrame = gif_.pGifFrame;
	for (int i = 0; (i < frameId) && (NULL != pFrame); i++)	pFrame = pFrame->pNext;

	return pFrame->gce.wDelayTime;
}

/**
 *\fn         GetFrameBmpData(PGIFFRAME frame, PGIFFRAME lastFrame)
 *\brief      �õ�λͼ���ݡ�
 *\param[in]  frame      ֡���ݡ�
 *\param[in]  lastFrame  ��һ֡���ݡ�
 *\return     �Ƿ�ɹ�
 */
LPBYTE CGIF::GetFrameBmpData(PGIFFRAME frame, PGIFFRAME lastFrame)
{
	if (NULL == frame) return NULL;

	LPBYTE pPixIndex = DecodeImageData(frame);	// ��ѹ��,�õ���ɫ����

	WORD wImageWidth = frame->gi.wWidth;
	WORD wImageHeight = frame->gi.wHeight;
	WORD wGifWidth = gif_.ScrDesc.wWidth;
	WORD wGifHeight = gif_.ScrDesc.wHeight;
	BYTE byTranColorIndex = frame->gce.byTransparentColorIndex;

	int nLineInter = (wGifWidth % 4 != 0) ? 4 - (wGifWidth * 3) % 4 : 0;
	int nLineDataLen = wGifWidth * 3 + nLineInter;
	int nBmpDataLen = wGifHeight * nLineDataLen;
	int nPixNum = wImageWidth * wImageHeight;
	int nPos = 0;
	
	BYTE* pPixData = new BYTE[nBmpDataLen];
	ATLASSERT(NULL != pPixData);

	if (!frame->gi.LocalFlag.Interlace)	// �ǽ���
	{
		bool bUseTranColor = frame->gce.Flag.t;

// 		// ��ǰ��Ŀհ�,Ӧ��ֻ�в��ǵ�һ֡����
// 		if ((pFrame->gi.wTop > 0 || pFrame->gi.wLeft > 0) && (NULL != pLastFrame))
// 		{
// 			nPos = nLineDataLen * pFrame->gi.wTop  + pFrame->gi.wLeft * 3;
// 			for (int i = 0; i < nPos; i++)
// 				pPixData[i] = pLastFrame->pBmpData[i];
// 		}
// 
// 		// ͼ������
// 		for (int i = 0; i < nPixNum; i++)
// 		{
// 			if (pPixIndex[i] == byTranColorIndex)
// 			{
// 				if (NULL != pLastFrame)
// 				{
// 					pPixData[nPos] = pLastFrame->pBmpData[nPos++];
// 					pPixData[nPos] = pLastFrame->pBmpData[nPos++];
// 					pPixData[nPos] = pLastFrame->pBmpData[nPos++];
// 				}
// 			}
// 			else
// 			{
// 				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byB;
// 				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byG;
// 				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byR;
// 			}
// 
// 			if (((i+1) % wImageWidth == 0) && (i != 0))		// ͼ��һ�п�ʼ
// 			{
// 				int nInter = (wGifWidth - wImageWidth) * 3;	// �м�հ�
// 
// 				if (NULL != pLastFrame)
// 				{
// 					for (int j = 0; j < nInter; j++)
// 						pPixData[nPos + j] = pLastFrame->pBmpData[nPos + j];
// 				}
// 
// 				nPos += nInter;
// 				nPos += nLineInter;	// BMPҪ��һ��4���ֽڶ���
// 			}
// 		}
// 
// 		// ����հ�,Ӧ��ֻ�в��ǵ�һ֡����
// 		if (nPos < nBmpDataLen && NULL != pLastFrame)
// 		{
// 			for (int i = 0; i < nBmpDataLen - nPos - 1; i++)
// 				pPixData[nPos + i] = pLastFrame->pBmpData[nPos + i];
// 		}

		// ��ǰ��Ŀհ�,Ӧ��ֻ�в��ǵ�һ֡����
		if ((frame->gi.wTop > 0) || (frame->gi.wLeft > 0))
		{
			nPos = nLineDataLen * frame->gi.wTop  + frame->gi.wLeft * 3;

			if (bUseTranColor && (NULL != lastFrame))	// Ӧ��͸��ɫ
			{
				memcpy(pPixData, lastFrame->pBmpData, nPos);
			}
			else
			{
				memset(pPixData, 255, nPos);
			}
		}

		// ͼ������
		for (int i = 0; i < nPixNum; i++)
		{
			if (bUseTranColor && (pPixIndex[i] == byTranColorIndex) && (NULL != lastFrame))
			{
				pPixData[nPos] = lastFrame->pBmpData[nPos++];
				pPixData[nPos] = lastFrame->pBmpData[nPos++];
				pPixData[nPos] = lastFrame->pBmpData[nPos++];
			}
			else
			{
				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byB;
				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byG;
				pPixData[nPos++] = gif_.GlobalColorTable[ pPixIndex[i] ].byR;
			}

			if (((i+1) % wImageWidth == 0) && (i != 0))		// ͼ��һ�п�ʼ
			{
				int nInter = (wGifWidth - wImageWidth) * 3;	// �м�հ�

				if (bUseTranColor && (NULL != lastFrame))
					memcpy(&pPixData[nPos], &(lastFrame->pBmpData[nPos]), nInter);
				else
					memset(&pPixData[nPos], 255, nInter);

				nPos += nInter + nLineInter;	// BMPҪ��һ��4���ֽڶ���
			}
		}

		// ����հ�,Ӧ��ֻ�в��ǵ�һ֡����
		if (nPos < nBmpDataLen)
		{
			if (bUseTranColor && (NULL != lastFrame))	// Ӧ��͸��ɫ
				memcpy(&pPixData[nPos], &(lastFrame->pBmpData[nPos]), nBmpDataLen - nPos - 1);
			else
				memset(&pPixData[nPos], 255, nBmpDataLen - nPos - 1);
		}
	}
	else
	{
		delete[] pPixIndex;
		delete[] pPixData;		
		return NULL;

// 		int pass = 0;//��ǰͨ��
// 		while (pass < 4)
// 		{
// 			// �ܹ���4��ͨ��
// 			if (pass == 1)
// 			{
// 				p = tempPointer;
// 				p += (2 * width * 4);
// 			}
// 			else if (pass == 2)
// 			{
// 				p = tempPointer;
// 				p += (1 * width * 4);
// 			}
// 			else if (pass == 3)
// 			{
// 				p = tempPointer;
// 				p += (4 * width * 4);
// 			}
// 			int i = 0;
// 			for (i = 0; i < pixel.Length; )
// 			{
// 				Color c = Color.FromArgb(colorTable[pixel[i++]]);
// 				*p++ = c.B;
// 				*p++ = c.G;
// 				*p++ = c.R;
// 				*p++ = c.A;
// 				if (i % (iw) == 0)
// 				{
// 					p += (width - iw) * 4;
// 					p += width * 8 * 4;
// 					if (ih % 4 == 0)
// 					{
// 						pass++;
// 						break;
// 					}
// 				}
// 			}
// 		}
	}

	delete[] pPixIndex;
	return pPixData;
}

/**
 *\fn         DecodeImageData(PGIFFRAME frame)
 *\brief      LZWѹ���㷨�Ľ������ݡ�
 *\param[in]  pFrame  ֡���ݡ�
 *\return     �Ƿ�ɹ�
 */
LPBYTE CGIF::DecodeImageData(PGIFFRAME frame)
{
	if (NULL == frame) return NULL;

	int MaxStackSize = 4096;
	int NullCode = -1;
	int pixelCount = frame->gi.wWidth * frame->gi.wHeight;// ��ȡԭͼ�������������ʽΪ ������ = ͼ�񳤶�*ͼ��߶�
	LPBYTE pixels = new byte[pixelCount];
	int dataSize = gif_.pGifFrame->gi.byLZWcode;			// ͼ��������ĵ�һ���ֽ�(byte)��ŵ�������λ��С����gifͨ��Ϊ1,4,8
	int codeSize = dataSize + 1;			// ����λ��С������lzw�㷨��Ҫ�󣬱���λ�Ĵ�С = ����λ��С+1 �����gif�������¶�Ӧ��ϵ 1->3 4->5 ->9,������codeSizeΪ12
	int clearFlag = 1 << dataSize;			// ��lzw�㷨�����������ǣ�clearFlagΪ���е������ǣ��˺�ı��뽫��ͷ���������������Է�ֹ����λ��������
	int endFlag = clearFlag + 1;			// lzw�㷨����������֮һ��endFlagΪ������ǣ���ʾһ�α���Ľ���  endFlag=clearFlag+1
	int available = endFlag + 1;			// ��ʼ�Ŀ��ñ����С����Ϊ0-(clear-1)ΪԪ���ݣ����Ծ����ã������о����˴������γ�ѹ���ı��뿪ʼ����

	int code = NullCode;					// ���ڴ洢��ǰ�ı���ֵ
	int old_code = NullCode;				// ���ڴ洢��һ�εı���ֵ
	int code_mask = (1 << codeSize) - 1;	// ��ʾ��������ֵ�����codeSize=5,��code_mask=31
	int bits = 0;							// �ڱ����������ݵı�����ʽΪbyte����ʵ�ʱ������������ʵ�ʱ���λ���洢�ģ����統codeSize=5��ʱ����ôʵ����5bit�����ݾ�Ӧ�ÿ��Ա�ʾһ�����룬����ȡ������1���ֽھ͸�����3��bit����3��bit���ں͵ڶ����ֽڵĺ�����bit������ϣ��ٴ��γɱ���ֵ���������

	int prefix[4096] = {0};					// ���ڱ���ǰ׺�ļ���
	int suffix[4096] = {0};					// ���ڱ����׺
	int pixelStatck[4096 + 1] = {0};		// ������ʱ����������

	int top = 0;
	int count = 0;							// �������ѭ���У�ÿ�λ��ȡһ�����ı�����ֽ����飬��������Щ�����ʱ����Ҫ1�����ֽ�������count���Ǳ�ʾ��Ҫ������ֽ���Ŀ
	int bi = 0;								// count��ʾ��ʣ�����ֽ���Ҫ������bi���ʾ�����Ѿ�����ĸ���
	int i = 0;								// i����ǰ����õ�������

	int data = 0;							// ��ʾ��ǰ��������ݵ�ֵ
	int first = 0;							// һ���ַ����صĵ�һ���ֽ�
	int inCode = NullCode;					// ��lzw�У������ʶ��һ�����������������entry���򽫱�����Ϊ��һ�ε�prefix���˴�inCode�����ݸ���һ����Ϊǰ׺�ı���ֵ

	// ������Ԫ���ݵ�ǰ׺���Ϻͺ�׺���ϣ�Ԫ���ݵ�ǰ׺��Ϊ0������׺��Ԫ������ȣ�ͬʱ����Ҳ��Ԫ�������
	for (code = 0; code < clearFlag; code++)
	{
		// ǰ׺��ʼΪ0
		prefix[code] = 0;
		// ��׺=Ԫ����=����
		suffix[code] = (byte)code;
	}

	int nBlockNum = 0;
	LPBYTE buffer = NULL;

	while (i < pixelCount)		// ����������Ѿ�ȷ��ΪpixelCount = width * width
	{
		if (top == 0)
		{
			if (bits < codeSize)// �����ǰ��Ҫ�����bit��С�ڱ���λ��С������Ҫ��������
			{
				if (count == 0)	// ���countΪ0����ʾҪ�ӱ������ж�һ�����ݶ������з���
				{
					if (NULL != frame)
					{
						PDATASUBBLOCK pData = frame->gi.pData;

						for(int i = 0; i < nBlockNum && NULL != pData->pNext; i++)	// �ҵ���Ҫ�����ݿ�
						{
							pData = pData->pNext;
						}

						if (pData != NULL)
						{
							buffer = pData->pbyData;
							count = pData->byDataLen;
							nBlockNum++;
						}
					}

					if (count == 0)
					{
						// �ٴ����ȡ���ݶΣ�ȴû�ж������ݣ���ʱ�ͱ����Ѿ���������
						break;
					}
					// ���¶�ȡһ�����ݶκ�Ӧ�ý��Ѿ�����ĸ�����0
					bi = 0;
				}
				// ��ȡ����Ҫ��������ݵ�ֵ
				data += buffer[bi] << bits;	// �˴�Ϊ��Ҫ��λ�أ������һ�δ�����1���ֽ�Ϊ176����һ��ֻҪ����5bit�͹��ˣ�ʣ��3bit�����¸��ֽڽ�����ϡ�Ҳ���ǵڶ����ֽڵĺ���λ+��һ���ֽڵ�ǰ��λ��ɵڶ������ֵ
				bits += 8;					// �����ִ�����һ���ֽڣ�������Ҫ+8
				bi++;						// ��������һ���ֽ�
				count--;					// �Ѿ���������ֽ���+1
				continue;
			}
			// ����Ѿ����㹻��bit���ɹ�����������Ǵ������
			// ��ȡ����
			code = data & code_mask;	// ��ȡdata���ݵı���λ��Сbit������
			data >>= codeSize;			// ���������ݽ�ȡ��ԭ�������ݾ�ʣ�¼���bit�ˣ���ʱ����Щbit���ƣ�Ϊ�´���׼��
			bits -= codeSize;			// ͬʱ��Ҫ����ǰ���ݵ�bit����ȥ����λ������Ϊ�Ѿ��õ��˴���

			// ������ݻ�ȡ��codeֵ�����д���
			if (code > available || code == endFlag)
			{
				//������ֵ����������ֵ����Ϊ������ǵ�ʱ��ֹͣ����
				break;
			}
			if (code == clearFlag)
			{
				// �����ǰ�������ǣ������³�ʼ������������������
				codeSize = dataSize + 1;
				// ���³�ʼ��������ֵ
				code_mask = (1 << codeSize) - 1;
				// ��ʼ����һ��Ӧ�ô���ñ���ֵ
				available = clearFlag + 2;
				// �����浽old_code�е�ֵ������Ա���ͷ����
				old_code = NullCode;
				continue;
			}
			// ������code������ѹ���ı��뷶Χ�ڵĵĴ������
			if (old_code == NullCode)
			{
				// �����ǰ����ֵΪ��,��ʾ�ǵ�һ�λ�ȡ����
				pixelStatck[top++] = suffix[code];//��ȡ��1��������������
				// ���α��봦����ɣ�������ֵ���浽old_code��
				old_code = code;
				// ��һ���ַ�Ϊ��ǰ����
				first = code;
				continue;
			}
			inCode = code;
			if (code == available)
			{
				// �����ǰ����ͱ���Ӧ�����ɵı�����ͬ
				pixelStatck[top++] = (byte)first;//��ô��һ�������ֽھ͵��ڵ�ǰ�����ַ����ĵ�һ���ֽ�
				code = old_code; //���ݵ���һ������
			}
			while (code > clearFlag)
			{
				// �����ǰ������������ǣ���ʾ����ֵ����ѹ�����ݵ�
				pixelStatck[top++] = suffix[code];
				code = prefix[code];//���ݵ���һ������
			}
			first = suffix[code];
			if (available > MaxStackSize)
			{
				// ���������ֵ����gif������ı��루4096�����ֵ��ʱ��ֹͣ����
				break;
			}
			// ��ȡ��һ������
			pixelStatck[top++] = suffix[code];
			// ���õ�ǰӦ�ñ���λ�õ�ǰ׺
			prefix[available] = old_code;
			// ���õ�ǰӦ�ñ���λ�õĺ�׺
			suffix[available] = first;
			// �´�Ӧ�õõ��ı���ֵ
			available++;
			if (available == code_mask + 1 && available < MaxStackSize)
			{
				// ���ӱ���λ��
				codeSize++;
				// ����������ֵ
				code_mask = (1 << codeSize) - 1;
			}
			// ��ԭold_code
			old_code = inCode;
		}
		// ���ݵ���һ������λ��
		top--;
		// ��ȡԪ����
		pixels[i++] = (byte)pixelStatck[top];
	}

	return pixels;
}

/**
 *\fn         Draw(HDC dc, int x, int y, int frameId)
 *\brief      ��ͼƬ����DC�ϡ�
 *\param[in]  dc        Ҫ����ͼƬ��DC��
 *\param[in]  x         ͼƬx���ꡣ
 *\param[in]  y         ͼƬy���ꡣ
 *\param[in]  frameId   ͼƬ֡ID��
 *\return     �Ƿ�ɹ�
 */
bool CGIF::Draw(HDC dc, int x, int y, int frameId)
{
	if (NULL == gif_.pGifFrame) return false;

	PGIFFRAME pFrame = gif_.pGifFrame;
	for (int i = 0; (i < frameId) && (NULL != pFrame); i++)	pFrame = pFrame->pNext;

	::StretchDIBits(dc, x, y, gif_.BmpInfo.bmiHeader.biWidth, gif_.BmpInfo.bmiHeader.biHeight, 
		0, 0, gif_.BmpInfo.bmiHeader.biWidth, gif_.BmpInfo.bmiHeader.biHeight, 
		pFrame->pBmpData, &gif_.BmpInfo, DIB_RGB_COLORS, SRCCOPY);

	return true;
}

/**
 *\fn         SaveBmp(const char *filname, int frameId)
 *\brief      ��ͼƬ���BITMAP��ʽ��
 *\param[in]  filname  ͼƬ�ļ�����
 *\param[in]  frameId   ͼƬ֡ID��
 *\return     �Ƿ�ɹ�
 */
bool CGIF::SaveBmp(const char *filename, int frameId)
{
	if (NULL == filename || frameId >= (int)gif_.nGifFrameNum) return false;

	int nLineDataLen = (gif_.ScrDesc.wWidth * (gif_.BmpInfo.bmiHeader.biBitCount / 8) + 3) * 4 /4;
	PGIFFRAME pFrame = gif_.pGifFrame;

	for (int i = 0; (i < frameId) && (NULL != pFrame); i++) pFrame = pFrame->pNext;

	if ((pFrame->byFrameId != frameId) || (NULL == pFrame->pBmpData)) return false;

	int nBMPBytes = (gif_.ScrDesc.wWidth * gif_.ScrDesc.wHeight) * 24 / 8;

	BITMAPFILEHEADER bmfh = {0};
	bmfh.bfType = 0x4D42;	// "BM"
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = bmfh.bfOffBits + nBMPBytes;

	FILE *file = fopen(filename, "w+b");

	if (NULL == file) return false;

	fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite(&(gif_.BmpInfo), sizeof(BITMAPINFOHEADER), 1, file);
	fwrite(pFrame->pBmpData, nLineDataLen * gif_.ScrDesc.wHeight, 1, file);
	
	fclose(file);

	return true;
}