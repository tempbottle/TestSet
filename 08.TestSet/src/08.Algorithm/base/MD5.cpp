#include "StdAfx.h"
#include "MD5.h"
#include <tchar.h>

/*

Ҫ���ܵ�Ϊ��M����Ҫ512��Bit�ԣ���M���Bit(1000...000)+ԭ��M��Bit����(ռBit64λ)


A=0x01234567 
B=0x89abcdef 
C=0xfedcba98 
D=0x76543210 

F(X,Y,Z)=(X&Y)|((~X)&Z) 
G(X,Y,Z)=(X&Z)|(Y&(~Z)) 
H(X,Y,Z)=X^Y^Z 
I(X,Y,Z)=Y^(X|(~Z)) 


��Mj��ʾҪ������Ϣ�ĵ�j���ӷ��飨��0��15��32λ��<<<s��ʾѭ������sλ�������ֲ���Ϊ�� 
FF(a,b,c,d,Mj,s,ti)��ʾa=b+((a+(F(b,c,d)+Mj+ti)<<<s) 
GG(a,b,c,d,Mj,s,ti)��ʾa=b+((a+(G(b,c,d)+Mj+ti)<<<s) 
HH(a,b,c,d,Mj,s,ti)��ʾa=b+((a+(H(b,c,d)+Mj+ti)<<<s) 
II(a,b,c,d,Mj,s,ti)��ʾa=b+((a+(I(b,c,d)+Mj+ti)<<<s) 

�����֣�64�����ǣ� 
��һ�� 
FF(a,b,c,d,M0,7,0xd76aa478) 0xd76aa478 = 0x80000000*abs(sin(1));
FF(d,a,b,c,M1,12,0xe8c7b756) 
FF(c,d,a,b,M2,17,0x242070db) 
FF(b,c,d,a,M3,22,0xc1bdceee) 
FF(a,b,c,d,M4,7,0xf57c0faf) 
FF(d,a,b,c,M5,12,0x4787c62a) 
FF(c,d,a,b,M6,17,0xa8304613) 
FF(b,c,d,a,M7,22,0xfd469501) 
FF(a,b,c,d,M8,7,0x698098d8) 
FF(d,a,b,c,M9,12,0x8b44f7af) 
FF(c,d,a,b,M10,17,0xffff5bb1) 
FF(b,c,d,a,M11,22,0x895cd7be) 
FF(a,b,c,d,M12,7,0x6b901122) 
FF(d,a,b,c,M13,12,0xfd987193) 
FF(c,d,a,b,M14,17,0xa679438e) 
FF(b,c,d,a,M15,22,0x49b40821) 
�ڶ��� 
GG(a,b,c,d,M1,5,0xf61e2562) 0xd76aa478 = 0x80000000*abs(sin(17));
GG(d,a,b,c,M6,9,0xc040b340) 
GG(c,d,a,b,M11,14,0x265e5a51) 
GG(b,c,d,a,M0,20,0xe9b6c7aa) 
GG(a,b,c,d,M5,5,0xd62f105d) 
GG(d,a,b,c,M10,9,0x02441453) 
GG(c,d,a,b,M15,14,0xd8a1e681) 
GG(b,c,d,a,M4,20,0xe7d3fbc8) 
GG(a,b,c,d,M9,5,0x21e1cde6) 
GG(d,a,b,c,M14,9,0xc33707d6) 
GG(c,d,a,b,M3,14,0xf4d50d87) 
GG(b,c,d,a,M8,20,0x455a14ed) 
GG(a,b,c,d,M13,5,0xa9e3e905) 
GG(d,a,b,c,M2,9,0xfcefa3f8) 
GG(c,d,a,b,M7,14,0x676f02d9) 
GG(b,c,d,a,M12,20,0x8d2a4c8a) 
������ 
HH(a,b,c,d,M5,4,0xfffa3942) 0xd76aa478 = 0x80000000*abs(sin(33));
HH(d,a,b,c,M8,11,0x8771f681) 
HH(c,d,a,b,M11,16,0x6d9d6122) 
HH(b,c,d,a,M14,23,0xfde5380c) 
HH(a,b,c,d,M1,4,0xa4beea44) 
HH(d,a,b,c,M4,11,0x4bdecfa9) 
HH(c,d,a,b,M7,16,0xf6bb4b60) 
HH(b,c,d,a,M10,23,0xbebfbc70) 
HH(a,b,c,d,M13,4,0x289b7ec6) 
HH(d,a,b,c,M0,11,0xeaa127fa) 
HH(c,d,a,b,M3,16,0xd4ef3085) 
HH(b,c,d,a,M6,23,0x04881d05) 
HH(a,b,c,d,M9,4,0xd9d4d039) 
HH(d,a,b,c,M12,11,0xe6db99e5) 
HH(c,d,a,b,M15,16,0x1fa27cf8) 
HH(b,c,d,a,M2,23,0xc4ac5665) 
������ 
II(a,b,c,d,M0,6,0xf4292244) 0xd76aa478 = 0x80000000*abs(sin(49));
II(d,a,b,c,M7,10,0x432aff97) 
II(c,d,a,b,M14,15,0xab9423a7) 
II(b,c,d,a,M5,21,0xfc93a039) 
II(a,b,c,d,M12,6,0x655b59c3) 
II(d,a,b,c,M3,10,0x8f0ccc92) 
II(c,d,a,b,M10,15,0xffeff47d) 
II(b,c,d,a,M1,21,0x85845dd1) 
II(a,b,c,d,M8,6,0x6fa87e4f) 
II(d,a,b,c,M15,10,0xfe2ce6e0) 
II(c,d,a,b,M6,15,0xa3014314) 
II(b,c,d,a,M13,21,0x4e0811a1) 
II(a,b,c,d,M4,6,0xf7537e82) 
II(d,a,b,c,M11,10,0xbd3af235) 
II(c,d,a,b,M2,15,0x2ad7d2bb) 
II(b,c,d,a,M9,21,0xeb86d391) 

����ti��������ѡ�� 
�ڵ�i���У�ti��(2��32�η�)4294967296*abs(sin(i))����������,i�ĵ�λ�ǻ��ȡ� 

������Щ���֮�󣬽�A��B��C��D�ֱ����a��b��c��d��
Ȼ������һ�������ݼ��������㷨�����������A��B��C��D�ļ����� 

*/

static unsigned char MD5_PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

extern "C" void MD5_Add_p5(CMD5*, LPCVOID pData, DWORD nLength); // ������ӿ�

/**
 *\fn         CMD5()
 *\brief      ȱʡ���캯����
 */
CMD5::CMD5()
{
	Reset();
}

/**
 *\fn         CMD5()
 *\brief      ����������
 */
CMD5::~CMD5()
{
}

/**
 *\fn         Add(void *data, int len)
 *\brief      ���Ҫ�����У��������ݡ�
 *\param[in]  data ���ݡ�
 *\param[in]  len  ���ݳ���
 */
void CMD5::Add(void *data, int len)
{
	MD5_Add_p5(this, data, len);
}

/**
 *\fn         Finish()
 *\brief      ��ɡ�
 */
void CMD5::Finish()
{
	unsigned int bits[2], index = 0;

	// Save number of bits
	bits[1] = (m_nCount[1] << 3) + (m_nCount[0] >> 29);
	bits[0] = m_nCount[0] << 3;

	// Pad out to 56 mod 64.
	index = (unsigned int)(m_nCount[0] & 0x3f);
	MD5_Add_p5(this, MD5_PADDING, (index < 56) ? (56 - index) : (120 - index));

	// Append length (before padding)
	MD5_Add_p5(this, bits, 8);
}

/**
 *\fn         Reset()
 *\brief      ���á�
 */
void CMD5::Reset()
{
	m_nCount[0] = 0;
	m_nCount[1] = 0;

	/* Load magic initialization constants. */
	m_nState[0] = 0x67452301;
	m_nState[1] = 0xefcdab89;
	m_nState[2] = 0x98badcfe;
	m_nState[3] = 0x10325476;
}

/**
 *\fn         GetHashStr(void *data, int len)
 *\brief      ����У���벢����У������ַ�����
 *\param[in]  data ���ݡ�
 *\param[in]  len  ���ݳ���
 *\return     У������ַ���
 */
std::string	CMD5::getHashStr(void *data, int len)
{
	if (NULL == data) return "";

	Reset();
	Add(data, len);
	Finish();

	return HashToString((MD5*)m_nState);
}

/**
 *\fn         HashToString(const MD5* hash)
 *\brief      �õ�У������ַ�����
 *\param[in]  hash У���롣
 *\return     У������ַ���
 */
std::string	CMD5::HashToString(const MD5* hash)
{
	if (NULL == hash) return "";

	char md5[MAX_PATH] = "";

	_snprintf_s(md5, sizeof(md5)-1, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
		hash->n[0],  hash->n[1],  hash->n[2],  hash->n[3],
		hash->n[4],  hash->n[5],  hash->n[6],  hash->n[7],
		hash->n[8],  hash->n[9],  hash->n[10], hash->n[11],
		hash->n[12], hash->n[13], hash->n[14], hash->n[15]);

	return md5;
}

/**
 *\fn         HashToChar(const MD5* hash, LPTSTR md5, int md5BuffSize)
 *\brief      �õ�У������ַ�����
 *\param[in]  hash          У���롣
 *\param[out] md5           У���봮��
 *\param[in]  md5BuffSize   У���봮��������С��
 *\return     У������ַ���
 */
bool CMD5::HashToChar(const MD5* hash, LPTSTR md5, int md5BuffSize)
{
	if (NULL == hash || NULL == md5 || md5BuffSize <= 32) return false;

	return (32 == _stprintf_s(md5, md5BuffSize, _T("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X"),
		hash->n[0],  hash->n[1],  hash->n[2],  hash->n[3],
		hash->n[4],  hash->n[5],  hash->n[6],  hash->n[7],
		hash->n[8],  hash->n[9],  hash->n[10], hash->n[11],
		hash->n[12], hash->n[13], hash->n[14], hash->n[15]));
}

/**
 *\fn         getHash(void *data, int len)
 *\brief      ��У���롣
 *\param[in]  data ���ݡ�
 *\param[in]  len  ���ݳ���
 *\return     У����
 */
MD5* CMD5::getHash(void *data, int len)
{
	if (NULL == data) return NULL;

	Reset();
	Add(data, len);
	Finish();

	return getHash();
}