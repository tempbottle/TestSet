#pragma once
#include "tinyxml/tinyxml.h"
#include <list>

#ifndef S_OK
#define S_OK 0
#endif

namespace XT_XML
{
	// ��ע����Ϊ����Doxygen�����ĵ�
	// ʹ��google�������


	enum{
		ERROR_INPUT_NULL,			// ����Ϊ��
		ERROR_OPEN_FAIL,			// ���ļ�ʧ��
		ERROR_FIND_ROOTNODE_FAIL,	// ���Ҹ��ڵ�ʧ��
	};

	class TiXmlElementEx;
	class TiXmlElementExs;

	/**
	 *\class  xml
	 *\brief  ������Ҫ��XML�ĵ�
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������,ʹ��tinyXML�⡣
	 */
	class xml : public TiXmlDocument
	{
	public:
		/**
		 *\fn         xml()
		 *\brief      ȱʡ���캯����
	     */
		xml();

		/**
		 *\fn         ~xml()
		 *\brief      ����������
		 */
		~xml();

		/**
		 *\fn         LoadXml(const char *xmlContent, int length)
		 *\brief      �����ڴ�XML���ݡ�
		 *\param[in]  xmlContent XML���ݡ�
         *\param[in]  length     XML���ݳ���
		 *\return     ����XML�Ƿ�ɹ���
		 */
		bool LoadXml(const char *xmlContent, int length);

		/**
		 *\fn         GetXml()
		 *\brief      �õ�XML�ַ�����
		 *\return     XML�ַ�����
		 */
		std::string GetXml();

		/**
		 *\fn         GetRoot()
		 *\brief      �õ�XML���ڵ㡣
		 *\return     XML���ڵ㡣
		 */
		TiXmlElementEx* GetRoot() { return (TiXmlElementEx*)RootElement(); }
	};

	/**
	 *\class  TiXmlElementEx
	 *\brief  ������Ҫ��XML�ڵ�
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������,ʹ��tinyXML�⡣
	 */
	class TiXmlElementEx : public TiXmlElement
	{
	public:
		/**
		 *\fn         TiXmlElementEx()
		 *\brief      ȱʡ���캯����
	     */
		TiXmlElementEx();

		/**
		 *\fn         ~TiXmlElementEx()
		 *\brief      ����������
		 */
		~TiXmlElementEx();

		/**
		 *\fn         operator[](const char *name)
		 *\brief      ͨ���ڵ������ҽڵ�ָ�롣
		 *\param[in]  index �ڵ㼯����š�
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* operator[] (const char *name);

		/**
		 *\fn         SelectSingleNode(const char *path)
		 *\brief      ���ҵ�һ���ڵ㣬��ֱ�Ӳ��Ҷ�����磺a\b\c����ʾa�ڵ��b�ӽڵ��c�ӽڵ㡣
		 *\param[in]  path XML�ڵ�����
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* SelectSingleNode(const char *path);

		/**
		 *\fn         SelectNodes(const char *path)
		 *\brief      ���Ҷ��ͬ���ڵ㣬��ֱ�Ӳ��Ҷ�����磺a\b\c����ʾa�ڵ��b�ӽڵ�Ķ��c�ӽڵ㡣
		 *\param[in]  path XML�ڵ�����
		 *\return     �ڵ�ָ�뼯��
		 */
		TiXmlElementExs SelectNodes(const char *path);

		/**
		 *\fn         SetName(const char *name)
		 *\brief      ���ýڵ����֡�
		 *\param[in]  name XML�ڵ�����
		 */
		void setName(const char *name);

		/**
		 *\fn         setValue(const char *value)
		 *\brief      ���ýڵ�ֵ��
		 *\param[in]  value XML�ڵ�ֵ��
		 */
		void setValue(const char *value);
	};
	

	typedef std::list<TiXmlElementEx*> LIST_ELEMENT;

	/**
	 *\class  TiXmlElementExs
	 *\brief  ������Ҫ��XML�ڵ㼯
	 *\date   2012-07-03
	 *\author �ź���
	 *\par    Revision History:
	 *\n      0.1     2007-07-03   �ź���   ��������,ʹ��tinyXML�⡣
	 */
	class TiXmlElementExs
	{
	public:
		/**
		 *\fn         TiXmlElementEx()
		 *\brief      ȱʡ���캯����
	     */
		TiXmlElementExs();

		/**
		 *\fn         ~TiXmlElementEx()
		 *\brief      ����������
		 */
		~TiXmlElementExs();
		
		/**
		 *\fn         operator[](int index)
		 *\brief      ͨ����Ų��ҽڵ�ָ�롣
		 *\param[in]  index �ڵ㼯����š�
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* operator[] (int index);

		/**
		 *\fn         operator[](const char *name)
		 *\brief      ͨ���ڵ������ҽڵ�ָ�롣
		 *\param[in]  index �ڵ㼯����š�
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* operator[] (const char *name);

		/**
		 *\fn         GetItem(int index)
		 *\brief      ͨ����Ų��ҽڵ�ָ�롣
		 *\param[in]  index �ڵ㼯����š�
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* GetItem(int index);

		/**
		 *\fn         GetItem(const char *name)
		 *\brief      ͨ���ڵ������ҽڵ�ָ�롣
		 *\param[in]  index �ڵ㼯����š�
		 *\return     �ڵ�ָ�롣
		 */
		TiXmlElementEx* GetItem(const char *name);

		/**
		 *\fn         getCount()
		 *\brief      �õ��ڵ㼯�Ľڵ�������
		 *\return     �ڵ�������
		 */
		int getCount() { return elementes_.size(); }

		/**
		 *\fn         release()
		 *\brief      �ͷŽڵ㼯��
		 */
		void release() { elementes_.clear(); }
		
		/**
		 *\fn         Add(TiXmlElementEx* node)
		 *\brief      ��ӽڵ㡣
		 *\param[in]  node Ҫ��ӵĽڵ㡣
		 */
		void Add(TiXmlElementEx* node);

	private:
		LIST_ELEMENT elementes_;		
	};


	

}