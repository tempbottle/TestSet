#pragma once
#include "08.Algorithm/base/XTXml.h"
#include <map>
#include <string>


typedef struct _tagProjectInfo
{
    int serverId;
    std::string remotePath;

}ProjectInfo;

typedef std::map<std::string, ProjectInfo> ProjectInfoMap;

typedef std::map<int, int> IntIntMap;

class Config
{
public:
    Config();
    ~Config();

protected:
    bool                    load_;          // �Ƿ��Ѿ�����������
    ProjectInfoMap          projectInfo_;   // ��Ŀ��Ϣ
    const char              *tempPath_;     // ��ʱĿ¼

    XT_XML::xml             file_;          // XML�ļ�
    XT_XML::TiXmlElementExs projects_;      // XML��Ŀ�ڵ�
    XT_XML::TiXmlElementExs servers_;       // XML�������ڵ�

public:
    /**
     *\fn        int openFile(const char *filename)
     *\brief     �������ļ�
     *\param[in] const char * filename �����ļ���
     *\return    int 0�ɹ�,����ʧ��
     */
    int openFile(const char *filename);

    /**
     *\fn        bool isLoad()
     *\brief     �Ƿ��Ѿ����������ļ�
     *\return    bool �Ƿ��Ѿ����������ļ�
     */
    bool isLoad() { return load_; }

    /**
     *\fn        const char getTempPath()
     *\brief     �õ���ʱĿ¼
     *\return    const char 0�ɹ�,����ʧ��
     */
    const char * getTempPath() { return tempPath_; }

    /**
     *\fn        int getServerCount()
     *\brief     �õ�����������
     *\return    int ����������
     */
    int getServerCount() { return servers_.getCount();}

    /**
     *\fn        int getServerParam(int index, char *addr, int &port, 
                                    char *username, char *password, int &portMappingCount)
     *\brief     �õ�����������
     *\param[in] int index ������ID
     *\param[out]char * addr ��������ַ
     *\param[out]int & port �������˿ں�
     *\param[out]char * username �������û���
     *\param[out]char * password �������û�����
     *\param[out]int & portMappingCount �˿�ӳ������
     *\return    int 0�ɹ�,����ʧ��
     */
    int getServerParam(int index, char *addr, int &port, 
        char *username, char *password, int &portMappingCount);

    /**
     *\fn        int getServerPortMap(int serverIndex, int portIndexint, &type, int &local, int &remote)
     *\brief     �õ�ӳ��Ķ˿�
     *\param[in] int serverIndex ������ID
     *\param[in] int portIndexint �˿�ID
     *\param[in] int & type ӳ������:0-���ؼ�����Զ��,1-Զ�˼���������
     *\param[in] int & local ���ض˿�
     *\param[in] int & remote Զ�˶˿�
     *\return    int 0�ɹ�,����ʧ��
     */
    int getServerPortMap(int serverIndex, int portIndexint, int &type, int &local, int &remote);

    /**
     *\fn        int getServerId(const char *projectname)
     *\brief     �õ������ڵķ�����ID
     *\param[in] const char * projectname ��Ŀ��
     *\return    int -1ʧ��,����������ID
     */
    int getServerId(const char *projectname);

    /**
     *\fn        const char* getRemotePath(const char *projectname)
     *\brief     �õ���Ŀ���ڵ�Զ�̵�ַ
     *\param[in] const char * projectname ��Ŀ��
     *\return    const char* Զ�̵�ַ
     */
    const char* getRemotePath(const char *projectname);
};