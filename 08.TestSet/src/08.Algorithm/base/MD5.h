
#ifndef TESTSET_MD5_H_
#define TESTSET_MD5_H_

#pragma once
#include <string>


typedef union
{
	BYTE	n[16];
	BYTE	b[16];
	DWORD	w[4];
}MD5, *PMD5;

/**
 *\class  CMD5
 *\brief  ������Ҫ���ڼ���MD5У��ֵ
 *\date   2012-06-25
 *\author �ź���
 *\par    Revision History:
 */
class CMD5  
{
public:
	/**
	 *\fn         CMD5()
	 *\brief      ȱʡ���캯����
     */
	CMD5();

	/**
	 *\fn         ~CMD5()
	 *\brief      ����������
     */
	virtual ~CMD5();		// ����ɾ��virtual��ռ4���ֽ�MD5_asm.asmԤ����λ�õ�
	
public:
	DWORD	m_nCount[2];	// ���������������ƶ�˳�򣬺͸ı��С
	DWORD	m_nState[4];	// MD5_asm.asm���õ�
	BYTE	m_pBuffer[64];

public:	
	/**
	 *\fn         Add(void *data, int len)
	 *\brief      ���Ҫ����У��������ݡ�
	 *\param[in]  data ���ݡ�
	 *\param[in]  len  ���ݳ���
	 */
	void Add(void *data, int len);

	/**
	 *\fn         Finish()
	 *\brief      ��ɡ�
	 */
	void Finish();

	/**
	 *\fn         Reset()
	 *\brief      ���á�
	 */
	void Reset();

	/**
	 *\fn         GetHashStr(void *data, int len)
	 *\brief      ����У���벢����У������ַ�����
	 *\param[in]  data ���ݡ�
	 *\param[in]  len  ���ݳ���
	 *\return     У������ַ���
	 */
	std::string	getHashStr(void *data, int len);

	/**
	 *\fn         HashToString(const MD5* hash)
	 *\brief      �õ�У������ַ�����
	 *\param[in]  hash У���롣
	 *\return     У������ַ���
	 */
	std::string	HashToString(const MD5* hash);

    /**
     *\fn         HashToChar(const MD5* hash, LPTSTR md5, int md5BuffSize)
     *\brief      �õ�У������ַ�����
     *\param[in]  hash          У���롣
     *\param[out] md5           У���봮��
     *\param[in]  md5BuffSize   У���봮��������С��
     *\return     У������ַ���
     */
    bool HashToChar(const MD5* hash, LPTSTR md5, int md5BuffSize);
	
	/**
	 *\fn         getHash(void *data, int len)
	 *\brief      ��У���롣
	 *\param[in]  data ���ݡ�
	 *\param[in]  len  ���ݳ���
	 *\return     У����
	 */
	MD5* getHash(void *data, int len);

	/**
	 *\fn         getHash()
	 *\brief      ��У���롣
	 *\return     У����
	 */
	MD5* getHash() { return (MD5*)m_nState; }

    /**
     *\fn        std::string getHashStr()
     *\brief     �õ�У������ַ���
     *\return    std::string 0�ɹ�,����ʧ��
     */
    std::string	getHashStr() { return HashToString(getHash()); }
};

#endif // TESTSET_MD5_H_