#pragma once
#include "Base/XTXml.h"

namespace XT_P2P
{
    typedef struct _tagFileInfo
    {
        enum
        {
            UPDATE_NULL = 0,    // �޸���
            UPDATE_PATCH,       // ���²����ļ�
            UPDATE_FILE,        // ���������ļ�
        };
    
        enum
        {
            PATCH_NULL = 0,     // �޲���
            PATCH_COURGETTE,    // courgette����
            PATCH_BSDIFF,       // bsdiff����
        };
    
        TCHAR name[MAX_PATH];   // �ļ����·����
        TCHAR size[20];         // �ļ���С
        TCHAR md5[33];          // �ļ�md5��
        TCHAR zip7Size[20];     // 7zѹ������ļ���С
        TCHAR zip7Md5[33];      // 7zѹ������ļ�md5��
        TCHAR patchSize[20];    // �����ļ���С
        TCHAR patchMd5[33];     // �����ļ�md5��
        TCHAR patch7zSize[20];  // 7zѹ����Ĳ����ļ���С
        TCHAR patch7zMd5[33];   // 7zѹ����Ĳ����ļ�md5��
        TCHAR oldSize[20];      // ���ļ���С
        TCHAR oldMd5[33];       // ���ļ�md5��
        BYTE  patchType;        // ��������:0-�޲���,1-courgette����,2-bsdiff����
        BYTE  updateType;       // ��������:0-�޸���,1-���²����ļ�,2-���������ļ�
        bool  exclude;          // �Ƿ��ų����ļ�
    
    }FileInfo;
    
    typedef std::list<FileInfo> FILE_INFO_LIST;
    typedef std::list<std::string> STRING_LIST;
    
    class P2PFileOperator
    {
    public:
        P2PFileOperator();
        ~P2PFileOperator();
    
    protected:
        FILE_INFO_LIST localFileList_;  // ���������ļ���Ϣ
        FILE_INFO_LIST oldFileList_;    // �ɵ��ļ��б�xml���ļ���Ϣ
        FILE_INFO_LIST newFileList_;    // �µ��ļ��б�xml���ļ���Ϣ
    
        XT_XML::xml    xml_;            // xml�ļ�����
    
    public:
        /**
         *\fn         DWORD runProcess(LPCTSTR cmd, LPCTSTR path)
         *\brief      ���г���
         *\param[in]  LPCTSTR cmd   ����ɴ�����
         *\param[in]  LPCTSTR path  ����ִ�е�Ĭ��·��
         *\return     �����˳���
         */
        int runProcess(LPCTSTR cmd, LPCTSTR path);

        /**
         *\fn         bool getFile(FILE_INFO_LIST &fileList, LPCTSTR name, FileInfo &file)
         *\brief      �õ��ļ�����Ϣ
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\param[in]  LPCTSTR name              �ļ���
         *\param[in]  FileInfo & file           �ļ���Ϣ
         *\return     �Ƿ�ɹ�
         */
        bool getFile(FILE_INFO_LIST &fileList, LPCTSTR name, FileInfo &file);
        
        /**
         *\fn         void deleteFileInList(FILE_INFO_LIST &fileList, LPCTSTR name)
         *\brief      ���ļ��б���ɾ���ļ�
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\param[in]  LPCTSTR name              �ļ���
         *\return     ��
         */
        void deleteFileInList(FILE_INFO_LIST &fileList, LPCTSTR name);
    
        /**
         *\fn         GetFiles(LPCTSTR root, LPCTSTR path, FILE_INFO_LIST &files)
         *\brief      �����ļ���������Ŀ¼��
         *\param[in]  root	��Ŀ¼��
         *\param[in]  path	���Ŀ¼��
         *\param[out] files	�ļ���
         *\return     �Ƿ�ɹ�
         */
        bool GetFiles(LPCTSTR root, LPCTSTR path, FILE_INFO_LIST &files);
    
        /**
         *\fn         GetFilesMd5(LPCTSTR rootPath, FILE_INFO_LIST &files)
         *\brief      �����ļ�md5��
         *\param[in]  rootPath	�ļ���·����
         *\param[out] files	�ļ���
         *\return     �Ƿ�ɹ�
         */
        bool GetFilesMd5(LPCTSTR rootPath, FILE_INFO_LIST &files);
    
        /**
         *\fn         bool GetFileMd5(LPCTSTR filename, LPTSTR size, UINT sizeBuffLen, LPTSTR md5, UINT md5BuffLen)
         *\brief      �����ļ�md5
         *\param[in]  LPCTSTR filename  �ļ���
         *\param[in]  LPTSTR size       �ļ���С�ַ���
         *\param[in]  UINT sizeBuffLen  �ļ���С�ַ�����������С
         *\param[in]  LPTSTR md5        md5�ַ���
         *\param[in]  UINT md5BuffLen   md5�ַ�����������С
         *\return     �Ƿ�ɹ�
         */
        bool GetFileMd5(LPCTSTR filename, LPTSTR size, UINT sizeBuffLen, LPTSTR md5, UINT md5BuffLen);
        
        /**
         *\fn         int CreateFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, LPCTSTR oldVersion, STRING_LIST &excludeFileList)
         *\brief      �����µ��ļ��б�
         *\param[in]  UINT projectId                ����ID
         *\param[in]  LPCTSTR rootPath              ���ļ�·��
         *\param[in]  LPCTSTR newVersion            �µİ汾��
         *\param[in]  LPCTSTR oldVersion            �ɵİ汾��
         *\param[in]  STRING_LIST & excludeFileList Ҫ�ų����ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int CreateFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, LPCTSTR oldVersion, STRING_LIST &excludeFileList);

        /**
         *\fn         int OpenFileList(LPCTSTR xml, UINT projectId, FILE_INFO_LIST &fileList)
         *\brief      �����ļ��б�
         *\param[in]  LPCTSTR xml               xml�ļ���
         *\param[in]  UINT projectId            ����ID
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int OpenFileList(LPCTSTR xml, UINT projectId, FILE_INFO_LIST &fileList);

        /**
         *\fn         bool CompareFile(FILE_INFO_LIST &oldFileList, FILE_INFO_LIST &newFileList)
         *\brief      �Ƚ��ļ����Ա����ɲ����ļ�
         *\param[in]  FILE_INFO_LIST & oldFileList ���ļ��б�
         *\param[in]  FILE_INFO_LIST & newFileList ���ļ��б�
         *\return     �Ƿ��в�ͬ��
         */
        bool CompareFile(FILE_INFO_LIST &oldFileList, FILE_INFO_LIST &newFileList);
    
        /**
         *\fn         int CreatePatch(LPCTSTR rootPath, LPCTSTR oldVersion, LPCTSTR newVersion, FILE_INFO_PTR_LIST &fileList)
         *\brief      ���ɲ����ļ�
         *\param[in]  LPCTSTR rootPath          ��·��
         *\param[in]  LPCTSTR oldVersion        �ɵİ汾��
         *\param[in]  LPCTSTR newVersion        �µİ汾��
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�     
         *\return     0�ɹ�,����ʧ��
         */
        int CreatePatch(LPCTSTR rootPath, LPCTSTR oldVersion, LPCTSTR newVersion, FILE_INFO_LIST &fileList);
    
        /**
         *\fn         int CreateZip7(LPCTSTR rootPath, LPCTSTR newVersion, FILE_INFO_LIST &fileList)
         *\brief      ����7zipѹ���ļ�
         *\param[in]  LPCTSTR rootPath          ��·��
         *\param[in]  LPCTSTR newVersion        �°汾��
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int CreateZip7(LPCTSTR rootPath, LPCTSTR newVersion, FILE_INFO_LIST &fileList);

        /*
         *\fn         int CreateZip7(LPCTSTR rootPath, LPCTSTR zip7, LPCTSTR filename)
         *\brief      ����7zipѹ���ļ�
         *\param[in]  LPCTSTR rootPath ��·��
         *\param[in]  LPCTSTR zip7 7z�ļ���
         *\param[in]  LPCTSTR filename �ļ���
         *\return     int 0�ɹ�,����ʧ��
         */
        int CreateZip7(LPCTSTR rootPath, LPCTSTR zip7, LPCTSTR filename);

        /*
         *\fn         int UncompareZip7(LPCTSTR rootPath, LPCTSTR zip7, LPCTSTR outPath)
         *\brief      ��ѹ7zip�ļ�
         *\param[in]  LPCTSTR rootPath ��·��
         *\param[in]  LPCTSTR zip7 �ļ���
         *\param[in]  LPCTSTR outPath ���·��
         *\return     int 0�ɹ�,����ʧ��
         */
        int UncompareZip7(LPCTSTR rootPath, LPCTSTR zip7, LPCTSTR outPath);

        /*
         *\fn         bool canCourgette(LPCTSTR filename)
         *\brief      �Ƿ���ʹ��courgette
         *\param[in]  LPCTSTR filename �ļ���
         *\return     bool �Ƿ���ʹ��courgette
         */
        bool canCourgette(LPCTSTR filename);

        /**
         *\fn         int ExcludeFile(FILE_INFO_LIST &fileList, STRING_LIST &excludeFileList)
         *\brief      �Ŷ��ļ�
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\param[in]  STRING_LIST & excludeFileList ��Ҫ�ų����ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int ExcludeFile(FILE_INFO_LIST &fileList, STRING_LIST &excludeFileList);
    
        /**
         *\fn         int CreateXmlFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, FILE_INFO_LIST &fileList)
         *\brief      �����ļ��б�xml
         *\param[in]  UINT projectId            ����ID
         *\param[in]  LPCTSTR rootPath          ��·��
         *\param[in]  LPCTSTR newVersion        �°汾��
         *\param[in]  FILE_INFO_LIST & fileList �ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int CreateXmlFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, FILE_INFO_LIST &fileList);
    
        /**
         *\fn         int GetFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, LPCTSTR oldVersion, STRING_LIST &excludeFileList)
         *\brief      �����µ��ļ��б�
         *\param[in]  UINT projectId                ����ID
         *\param[in]  LPCTSTR rootPath              ���ļ�·��
         *\param[in]  LPCTSTR newVersion            �µİ汾��
         *\param[in]  LPCTSTR oldVersion            �ɵİ汾��
         *\param[in]  STRING_LIST & excludeFileList Ҫ�ų����ļ��б�
         *\return     0�ɹ�,����ʧ��
         */
        int GetFileList(UINT projectId, LPCTSTR rootPath, LPCTSTR newVersion, LPCTSTR oldVersion, STRING_LIST &excludeFileList);

        /**
         *\fn         int GetUpdateList(LPCTSTR rootPath, LPCTSTR newXml, LPCTSTR oldXml, FILE_INFO_LIST &updateList)
         *\brief      �õ������µ��ļ��б�
         *\param[in]  LPCTSTR rootPath              ���ļ�·��
         *\param[in]  LPCTSTR newXml                �µ��ļ��б�xml
         *\param[in]  LPCTSTR oldXml                �ɵ��ļ��б�xml  
         *\param[in]  FILE_INFO_LIST & updateList   ��Ҫ���µ��ļ�
         *\return     ��������ֵ
         */
        int GetUpdateList(LPCTSTR rootPath, LPCTSTR newXml, LPCTSTR oldXml, FILE_INFO_LIST &updateList);

        /**
         *\fn         int CheckLocalFile(LPCTSTR rootPath, LPCTSTR oldXml, FILE_INFO_LIST &updateList)
         *\brief      У�鱾���ļ��Ƿ���ȷ
         *\param[in]  LPCTSTR rootPath              �ļ���Ŀ¼
         *\param[in]  LPCTSTR oldXml                �ļ��б�
         *\param[in]  FILE_INFO_LIST & updateList   ��Ҫ���µ��ļ�
         *\return     �Ƿ�ɹ�,0�ɹ�,����ʧ��
         */
        int CheckLocalFile(LPCTSTR rootPath, LPCTSTR oldXml, FILE_INFO_LIST &updateList);

        /**
         *\fn         TokenizeData(LPCTSTR srcData, LPCTSTR tokenize, VECTOR_STR &data)
         *\brief      �ָ��ַ���,����"***|***|**"һ���������һ������
         *\param[in]  srcData	ԭ�ַ�����
         *\param[in]  token		�ָ����
         *\param[out] data		������顣
         */
        void TokenizeData(LPCTSTR srcData, LPCTSTR tokenize, VECTOR_STR &data);

        /**
         *\fn         void MakeDirectory(LPCTSTR path)
         *\brief      �����༶Ŀ¼
         *\param[in]  LPCTSTR path ·��
         *\return     ��
         */
        void MakeDirectory(LPCTSTR path);
    };
}