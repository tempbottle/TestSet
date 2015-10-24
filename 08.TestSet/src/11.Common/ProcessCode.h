#pragma once
#include "Common/StrConver.h"
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm> // find_if
#include <cctype>  // isspace

//#include "test.h"

#define ENTER "\r\n"
#define TAB_SIZE 4

enum
{
	RELINE,		// ����
	COMMENT,	// ע��
	IDENTIFIER, // ��ʶ��
	OPERATOR,	// �����
	BOUNDARY,	// ���޷�
	STRING,		// �ַ���
	SECTION,	// ����
	KEYWORD,	// �ؼ���
};

typedef struct tagWordData
{
	std::string word;	// ��
	int type;			// ����
	int enterCount;		// ������
	int headPos;		// ��ͷλ��
	bool follow;		// ����ע��ʱ���Ƿ����ͬһ��

	tagWordData()
	{
		type = 0;
		headPos = 0;
	}

}WordData, *PWordData;

typedef std::vector<WordData>		WORD_VECTOR; // ��Ԫ����
typedef std::vector<std::string>	STR_VECTOR;  // �ַ����б�
typedef std::map<std::string, int>	KEYWORD_MAP; // �ؽ���
typedef std::map<int, std::string>	INT_STR_MAP; // �ؽ���


class CProcessCode
{
public:
    CProcessCode();
    ~CProcessCode();

private:
	char		*data_;		// �ļ�����
	int			len_;		// �ļ����ݳ�

	WORD_VECTOR word_;		// ��Ԫ��

	KEYWORD_MAP keyword_;	// �ؼ���

	INT_STR_MAP typeInfo_;	// ������Ϣ

	/**
	 *\fn         TokenizeWord(const char *data, int len)
	 *\brief      ���зָ���롣
	 *\param[in]  data  ���ݡ�
	 *\param[in]  len   ���ݳ���
	 *\return     ��
	 */
	void TokenizeWord(const char *data, int len);

	/**
	 *\fn         ProcessWord(WORD_LIST &word)
	 *\brief      ���зָ���롣
	 *\param[out] word  ��Ԫ���ݡ�
	 *\return     ��
	 */
	void ProcessWord(WORD_VECTOR &word);

	/**
	 *\fn         ProcessDefine(WORD_LIST &word)
	 *\brief      ����������,������ע�͡�
	 *\param[out] word  ��Ԫ���ݡ�
	 *\return     ��
	 */
	void ProcessDefine(WORD_VECTOR &word);

	/**
	 *\fn         ProcessComment(WORD_VECTOR &word, id, int headPos)
	 *\brief      ����ע�͡�
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessComment(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         ProcessBoundary(WORD_VECTOR &word, id)
	 *\brief      ����綨����'('')''['']'';''#''.'',''{''}'':'
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessBoundary(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         ProcessOperator(WORD_VECTOR &word, id, int headPos)
	 *\brief      �����������
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessOperator(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         ProcessSection(WORD_VECTOR &word, id, int headPos)
	 *\brief      �����С�
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessSection(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         ProcessSection(WORD_VECTOR &word, id, int headPos)
	 *\brief      �����С�
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessKeyword(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         ProcessReline(WORD_VECTOR &word, id, int headPos)
	 *\brief      �����С�
	 *\param[out] word  ��Ԫ���ݡ�
	 *\param[out] id  ��Ԫ����ID��
	 *\return     ��ͷλ��
	 */
	int ProcessReline(WORD_VECTOR &word, int id, int headPos);

	/**
	 *\fn         isNum(char c)
	 *\brief      �Ƿ������֡�
	 *\param[in]  c    ���ݡ�
	 *\return     �Ƿ�������
	 */
	bool isNum(char c) { return (c >= '0'&& c <= '9'); }

	/**
	 *\fn         isLetter(char c)
	 *\brief      �Ƿ����ַ���
	 *\param[in]  c    ���ݡ�
	 *\return     �Ƿ����ַ�
	 */
	bool isLetter(char c) { return (c >= 'a'&& c <= 'z') || (c >= 'A'&& c <= 'Z'); }

	/**
	 *\fn         isKeyword(const char *keyword)
	 *\brief      �Ƿ��ǹؼ��֡�
	 *\param[in]  c    ���ݡ�
	 *\return     �Ƿ��ǹؼ���
	 */
	bool isKeyword(const char *keyword) 
	{
		KEYWORD_MAP::iterator iter = keyword_.find(keyword); return iter != keyword_.end();
	}

	/**
	 *\fn         rtrim(std::string &s)
	 *\brief      ɾ���Ҳ�հ׷���
	 *\param[in]  s    ���ݡ�
	 *\return     ɾ������ַ���
	 */
	std::string rtrim(std::string &s)
	{
		while (s != "" && s[s.length()-1] == ' ')
		{
			s.erase(s.length()-1, 1);
		}

		return s; 
	}

	/**
	 *\fn         AddWord(char ch, int type)
	 *\brief      ���Ԫ����������ݡ�
	 *\param[in]  ch    �ַ����ݡ�
	 *\param[in]  type  �������͡�
	 *\return     ��
	 */
	void AddWord(char ch, int type)
	{
		WordData wordData;
		wordData.word = ch;
		wordData.type = type;
		word_.push_back(wordData);

		TRACE("%s %s\n", wordData.word.c_str(), typeInfo_[type].c_str());
	}

	/**
	 *\fn         AddWord(const std::string &str, int type)
	 *\brief      ���Ԫ����������ݡ�
	 *\param[in]  str   �ַ������ݡ�
	 *\param[in]  type  �������͡�
	 *\return     ��
	 */
	void AddWord(const std::string &str, int type)
	{
		WordData wordData;
		wordData.word = str;
		wordData.type = type;;
		word_.push_back(wordData);

		TRACE("%s %s\n", str.c_str(), typeInfo_[type].c_str());
	}

	/**
	 *\fn         AddSpace(std::string &str, int count)
	 *\brief      ׷�ӿո�
	 *\param[in]  str   �ַ������ݡ�
	 *\param[in]  count �ո�������
	 *\return     ��
	 */
	void AddSpace(std::string &str, int count)
	{
		for (int i = 0; i < count; i++)
		{
			str += " ";
		}
	}

public:
	/**
	 *\fn         load(const char *filename)
	 *\brief      ���ļ��������ݡ�
	 *\param[in]  filename   �ļ�����
	 *\return     �Ƿ�ɹ�
	 */
	bool load(const char *filename);

	/**
	 *\fn         load(const char *data, int len)
	 *\brief      �������ݡ�
	 *\param[in]  data   ���ݡ�
	 *\param[in]  len    ���ݳ���
	 *\return     �Ƿ�ɹ�
	 */
	bool load(const char *data, int len);

	/**
	 *\fn         Save(const char *filename)
	 *\brief      ������������ݱ��档
	 *\param[in]  filename   �ļ�����
	 *\return     �Ƿ�ɹ�
	 */
	bool Save(const char *filename);
};