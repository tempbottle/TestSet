#include "stdafx.h"
#include "ProcessCode.h"


CProcessCode::CProcessCode()
	:data_(NULL),
	len_(0)
{
	keyword_["__asm"]            = 1;
	keyword_["auto"]             = 1;
	keyword_["bad_cast"]         = 1;
	keyword_["const_cast"]       = 1;
	keyword_["dynamic_cast"]     = 1;
	keyword_["reinterpret_cast"] = 1;
	keyword_["static_cast"]      = 1;
	keyword_["bad_typeid"]       = 1;
	keyword_["bool"]             = 1;
	keyword_["break"]            = 1;
	keyword_["case"]             = 1;
	keyword_["catch"]            = 1;
	keyword_["throw"]            = 1;
	keyword_["try"]              = 1;
	keyword_["include"]          = 1;
	keyword_["char"]             = 1;
	keyword_["class"]            = 1;
	keyword_["const"]            = 1;
	keyword_["continue"]         = 1;
	keyword_["default"]          = 1;
	keyword_["delete"]           = 1;
	keyword_["do"]               = 1;
	keyword_["double"]           = 1;
	keyword_["else"]             = 1;
	keyword_["enum"]             = 1;
	keyword_["explicit"]         = 1;
	keyword_["export"]           = 1;
	keyword_["extern"]           = 1;
	keyword_["false"]            = 1;
	keyword_["float"]            = 1;
	keyword_["for"]              = 1;
	keyword_["friend"]           = 1;
	keyword_["goto"]             = 1;
	keyword_["if"]               = 1;
	keyword_["inline"]           = 1;
	keyword_["int"]              = 1;
	keyword_["long"]             = 1;
	keyword_["mutable"]          = 1;
	keyword_["namespace"]        = 1;
	keyword_["new"]              = 1;
	keyword_["operator"]         = 1;
	keyword_["private"]          = 1;
	keyword_["protected"]        = 1;
	keyword_["public"]           = 1;
	keyword_["register"]         = 1;
	keyword_["return"]           = 1;
	keyword_["short"]            = 1;
	keyword_["signed"]           = 1;
	keyword_["static"]           = 1;
	keyword_["struct"]           = 1;
	keyword_["switch"]           = 1;
	keyword_["template"]         = 1;
	keyword_["this"]             = 1;
	keyword_["true"]             = 1;
	keyword_["typedef"]          = 1;
	keyword_["typename"]         = 1;
	keyword_["union"]            = 1;
	keyword_["unsigned"]         = 1;
	keyword_["using"]            = 1;
	keyword_["virtual"]          = 1;
	keyword_["void"]             = 1;
	keyword_["volatile"]         = 1;
	keyword_["while"]            = 1;

	typeInfo_[RELINE]     = "����";
	typeInfo_[COMMENT]    = "ע��";
	typeInfo_[IDENTIFIER] = "��ʶ��";
	typeInfo_[OPERATOR]   = "�����";
	typeInfo_[BOUNDARY]   = "���޷�";
	typeInfo_[STRING]     = "�ַ���";
	typeInfo_[SECTION]    = "����";
	typeInfo_[KEYWORD]    = "�ؼ���";
}

CProcessCode::~CProcessCode()
{
}

/**
 *\fn         Save(const char *filename)
 *\brief      ������������ݱ��档
 *\param[in]  filename   �ļ�����
 *\return     �Ƿ�ɹ�
 */
bool CProcessCode::Save(const char *filename)
{
	FILE *file = fopen(filename, "w+b");

	if (NULL == file) return false;

	int count = word_.size();

	for (int i = 0; i < count; i++)
	{
		WordData *data = (WordData*)&word_[i];

		fwrite(data->word.c_str(), data->word.length(), 1, file);
	}

	fclose(file);

	return true;
}

/**
 *\fn         load(const char *filename)
 *\brief      ���ļ��������ݡ�
 *\param[in]  filename   �ļ�����
 *\return     �Ƿ�ɹ�
 */
bool CProcessCode::load(const char *filename)
{
	FILE *file = fopen(filename, "rb");

	if (NULL == file) return false;

	fseek(file, 0, SEEK_END);

	len_ = ftell(file);

	if (NULL != data_)
	{
		delete data_;
	}

	data_ = new char[len_];

	fseek(file, 0, SEEK_SET);

	fread(data_, len_, 1, file);

	fclose(file);

	TokenizeWord(data_, len_);

	delete data_;
	data_ = NULL;
	len_ = 0;

	ProcessWord(word_);

	//MakeTree(word_, 0, false);

	Save("now.h");

	return false;
}

/**
 *\fn         load(const char *data, int len)
 *\brief      �������ݡ�
 *\param[in]  data   ���ݡ�
 *\param[in]  len    ���ݳ���
 *\return     �Ƿ�ɹ�
 */
bool CProcessCode::load(const char *data, int len)
{
	len_ = len;

	data_ = new char[len_+1];

	strcpy(data_, data);

	TokenizeWord(data_, len_);

	delete data_;
	data_ = NULL;
	len_ = 0;

	ProcessDefine(word_);

	//MakeTree(word_, 0, false);

	Save("now.h");

	return false;
}

/**
 *\fn         TokenizeWord(const char *data, int len)
 *\brief      ���зָ���롣
 *\param[in]  data  ���ݡ�
 *\param[in]  len   ���ݳ���
 *\return     ��
 */
void CProcessCode::TokenizeWord(const char *data, int len)
{
	if (NULL == data) return;

	std::string temp;

	for (int i = 0; i < len; i++)
	{
		char ch = data[i];

		switch (ch)
		{
		case '+':
		case '-':
		case '*':
		case '/': // �п�����ע��
			{
				if (data[i+1] == '/')
				{
					std::string str;
					str += ch;

					do
					{
						ch = data[++i];
						str += ch;

					} while (ch != '\r' && ch != '\n');

					str.erase(str.length()-1, 1);
					str += ENTER;
					//AddWord(str, COMMENT);
					break;
				}
				else if (data[i+1] == '*')
				{
					std::string str;
					str += ch;
					str += data[++i];

					do 
					{
						ch = data[++i];
						str += ch;
					}
					while (data[i-1] != '*' || ch != '/');

					str += ENTER;
					//AddWord(str, COMMENT);
					break;
				}
			}
		case '>':
		case '<':
		case '!':
		case '=':
		case '?':
			{
				if (temp != "")
				{
					AddWord(temp, isKeyword(temp.c_str())?KEYWORD:IDENTIFIER);
					temp = "";
				}

				AddWord(ch, OPERATOR);
				break;
			}
		case '(':
		case ')':
		case '[':
		case ']':
		case ';':
		case '#':
		case '.':
		case ',':
		case '{':
		case '}':
			{
				if (temp != "")
				{
					AddWord(temp, isKeyword(temp.c_str())?KEYWORD:IDENTIFIER);
					temp = "";
				}

				AddWord(ch, BOUNDARY);
				break;
			}
		case '"':
			{
				std::string str;
				str += ch;

				do
				{
					ch = data[++i];
					str += ch;

				} while (ch != '"');

				AddWord(str, STRING);
				break;
			}
		case ':':
			{
				if (temp != "")
				{
					AddWord(temp, (data[i+1]==':')?IDENTIFIER:SECTION);
					temp = "";
				}

				AddWord(ch, BOUNDARY);
				break;
			}		
		case '_': // �»���
			{
				temp += ch;
				break;
			}
		case '~': // ��������
			{
				temp += ch;
				break;
			}
		case ' ':
		case '\t':
		case '\n':
			{
				if (temp != "")
				{
					AddWord(temp, isKeyword(temp.c_str())?KEYWORD:IDENTIFIER);
					temp = "";
				}

				break;
			}
		case '\r':
			{
				if (temp != "")
				{
					AddWord(temp, isKeyword(temp.c_str())?KEYWORD:IDENTIFIER);
					temp = "";
				}

				//AddWord("\r\n", RELINE);

				break;
			}		
		default:
			{
				if (isNum(ch))
				{
					temp += ch;
				}
				else if (isLetter(ch))
				{
					temp += ch;
				}
				else
				{
					TRACE("%c ��֪��\n", ch);
				}
				break;
			}
		}
	}
}

/**
 *\fn         ProcessWord(WORD_VECTOR &word)
 *\brief      ����Ԫ���ݡ�
 *\param[out] word  ��Ԫ���ݡ�
 *\return     ��
 */
void CProcessCode::ProcessWord(WORD_VECTOR &word)
{
	int count = word.size();
	int headPos = 0;

	for (int i = 0; i < count; i++)
	{
		WordData &data = word[i];

		switch (data.type)
		{
		case RELINE: headPos = ProcessReline(word, i, headPos); break;
		case COMMENT: headPos = ProcessComment(word, i, headPos); break;
		case IDENTIFIER: data.word += " "; break;
		case OPERATOR: headPos = ProcessOperator(word, i, headPos); break;
		case BOUNDARY: headPos = ProcessBoundary(word, i, headPos); break;
		case STRING: break;
		case SECTION: headPos = ProcessSection(word, i, headPos); break;
		case KEYWORD: headPos = ProcessKeyword(word, i, headPos); break;
		default: break;
		}
	}
}

/**
 *\fn         ProcessReline(WORD_VECTOR &word, id, int headPos)
 *\brief      �����С�
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessReline(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id];
	WordData &last = (0 != id) ? word_[id-1] : word_[id];

	if (last.type == BOUNDARY)
	{
		AddSpace(data.word, headPos);
	}
	else
	{
		data.word = " ";
	}

	return headPos;
}

/**
 *\fn         ProcessComment(WORD_VECTOR &word, id)
 *\brief      ����ע�͡�
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessComment(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id];
	AddSpace(data.word, headPos);
	return headPos;
}

/**
 *\fn         ProcessBoundary(WORD_VECTOR &word, id)
 *\brief      ����綨����'('')''['']'';''#''.'',''{''}'':'
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessBoundary(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id];
	WordData &last = (0 != id) ? word_[id-1] : word_[id];
	WordData &next = (word.size()-1 != id) ? word_[id+1] : word_[id];

	data.headPos = headPos;

	switch (data.word[0])
	{
	case '.': break;
	case '[':
	case ']':
	case '(': // if (***) ǰ������ǹؼ���
		{
			if (last.type == IDENTIFIER)
			{
				rtrim(last.word);
			}

			if (next.type == IDENTIFIER || next.type == KEYWORD)
			{
				rtrim(last.word);
			}

			break;;
		}	
	case '#':
		{
			data.word = ENTER + data.word;
			return 0; // ��ͷ��0
		}
	case ')':
		{
			rtrim(last.word);

			if (next.word[0] == '{') // ������
			{
				data.word += ENTER;

				AddSpace(data.word, headPos);
			}
			else if(next.type == IDENTIFIER || next.type == KEYWORD) // ����ָ�룬ǿ������ת��
			{
				data.word += ENTER;

				AddSpace(data.word, headPos+TAB_SIZE);
			}
			else
			{
				data.word += " ";
			}

			break;
		}
	case '{':
		{
			if (last.type == IDENTIFIER || last.type == KEYWORD) // ��,enum,struct����
			{
				rtrim(last.word);
				data.word = ENTER + data.word;
			}

			data.word += ENTER;
			headPos += TAB_SIZE;
			AddSpace(data.word, headPos);

			break;
		}
	case '}':
		{
			if (last.word[0] != ';') // enum���� 
			{
				std::string temp = ENTER;				
				headPos -= TAB_SIZE;
				AddSpace(temp, headPos);				
				data.word = temp + data.word;
			}

			if (next.word[0] != ';') // ��,enum,struct�������
			{
				data.word += ENTER;

				if (next.type != SECTION)
				{
					AddSpace(data.word, headPos);
				}
			}

			break;
		}
	case ';':
		{
			rtrim(last.word);

			if (last.word[0] == '}')
			{
				int index = data.word.rfind(ENTER);
				if (-1 != index)
				{
					data.word.erase(index, 2);
				}
			}
			if (next.word[0] == '}') // �����
			{
				data.word += ENTER;
				headPos -= TAB_SIZE;
				AddSpace(data.word, headPos);
			}
			else/* if (next.type == KEYWORD || next.type == IDENTIFIER)*/ // һ�н���
			{
				data.word += ENTER;
				AddSpace(data.word, headPos);
			}

			break;
		}
	case ':':
		{
			if (last.type == SECTION)
			{
				data.word += ENTER;
				AddSpace(data.word, headPos);
			}
			else if (last.word[0] == ':') // ::
			{
				rtrim(word_[id-2].word);
				rtrim(last.word);
			}
			else 
			{
				data.word += " ";
			}

			break;
		}
	case ',':
		{
			rtrim(last.word);
			data.word += " ";
			break;
		}	
	}

	return headPos;
}

/**
 *\fn         ProcessOperator(WORD_VECTOR &word, id)
 *\brief      �����������
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessOperator(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id];
	WordData &last = word_[id-1];
	WordData &next = word_[id+1];

	data.headPos = headPos;

	switch (data.word[0])
	{
	case '+':
	case '-':
		{
			if (next.word[0] == '+' || next.word[0] == '-')
			{
				rtrim(last.word);
			}
			break;
		}
	case '*':
		{
			if (next.type == IDENTIFIER)
			{
				break;
			}
		}
	case '/':
	case '<':
	case '>':
		{
			if (word_[id-2].word[0] == '<')
			{
				rtrim(word_[id-2].word);
				rtrim(last.word);
				
				if (next.type == KEYWORD)
				{
					data.word += ENTER;

					AddSpace(data.word, headPos);
				}

				break;
			}
		}	
	case '!':
	case '=':
	case '?':
		{
			if (next.type != OPERATOR)
			{
				data.word += " ";
			}
			
			break;
		}
	}
	
	return headPos;
}

/**
 *\fn         ProcessSection(WORD_VECTOR &word, id, int headPos)
 *\brief      �����С�
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessSection(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id-1];
	rtrim(data.word);
	return headPos;
}

/**
 *\fn         ProcessSection(WORD_VECTOR &word, id, int headPos)
 *\brief      �����С�
 *\param[out] word  ��Ԫ���ݡ�
 *\param[out] id  ��Ԫ����ID��
 *\return     ��ͷλ��
 */
int CProcessCode::ProcessKeyword(WORD_VECTOR &word, int id, int headPos)
{
	WordData &data = word_[id];
	data.word += " ";
	return headPos;
}

/**
 *\fn         ProcessDefine(WORD_LIST &word)
 *\brief      ����������,������ע�͡�
 *\param[out] word  ��Ԫ���ݡ�
 *\return     ��
 */
void CProcessCode::ProcessDefine(WORD_VECTOR &word)
{
	int nameId = 0;
	int count = word.size();

	STR_VECTOR funParam;
	std::string funReturn = (word[0].word == "void" || word[0].word == "VOID") ? "��" : "";	

	for (int i = 1; i < count; i++)
	{
		WordData &data = word[i];

		if (data.type == COMMENT)
		{
			data.word = " " + data.word;
			continue;
		}
		else if (data.word == "(" && (i+1) < count && word[i+1].word != "*")
		{
			word[i-1].word.erase(word[i-1].word.length()-1, 1);
			nameId = i-1;
		}
		else if (data.word == ",")
		{
			word[i-1].word.erase(word[i-1].word.length()-1, 1);
			funParam.push_back(word[i-1].word);
			data.word += " ";
		}
		else if (data.word == ")")
		{
			word[i-1].word.erase(word[i-1].word.length()-1, 1);
			funParam.push_back(word[i-1].word);
		}
		else if (data.word == "*" || data.word == "&")
		{
			data.word = " " + data.word;
		}
		else
		{
			data.word += " ";
		}
	}

	std::string commnet = ENTER;
	commnet += "/**";
	commnet += ENTER;
	commnet += " *\\fn         ";

	for (int i = nameId; i < count; i++)
	{
		commnet += word[i].word;
	}

	commnet += ENTER;
	commnet += " *\\brief      ";
	commnet += ENTER;

	for (int i = 0; i < funParam.size(); i++)
	{
		commnet += " *\\param[in]  ";
		commnet += funParam[i];
		commnet += ENTER;
	}

	commnet += " *\\return     ";
	commnet += funReturn;
	commnet += ENTER;
	commnet += " */";
	commnet += ENTER;

	TRACE("%s\n", commnet.c_str());
}