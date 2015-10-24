#include "stdafx.h"
#include "SshConfig.h"


Config::Config()
    :load_(false),
    tempPath_(NULL)
{
}

Config::~Config()
{
}

/**
 *\fn        int openFile(const char *filename)
 *\brief     �������ļ�
 *\param[in] const char * filename �����ļ���
 *\return    int 0�ɹ�,����ʧ��
 */
int Config::openFile(const char *filename)
{
    if (NULL == filename) return false;

    FILE *file = NULL;

    fopen_s(&file, filename, "rb");
    load_ = file_.LoadFile(file);
    //load_ = file_.LoadFile(filename); �ڲ�fseek����

    if (load_)
    {
        servers_ = file_.GetRoot()->SelectNodes("server");
        projects_ = file_.GetRoot()->SelectNodes("project");

        XT_XML::TiXmlElementEx *temp = file_.GetRoot()->SelectSingleNode("temp");

        tempPath_ = (NULL != temp) ? temp->Attribute("path") : "./temp";

        for (int i = 0; i < projects_.getCount(); i++)
        {
            ProjectInfo info;
            info.remotePath = projects_[i]->Attribute("remotePath");
            projects_[i]->Attribute("serverId", &info.serverId);

            projectInfo_[projects_[i]->Attribute("name")] = info;
        }
    }

    fclose(file);

    return load_;
}

/**
 *\fn        const char* getRemotePath(const char *projectname)
 *\brief     �õ���Ŀ���ڵ�Զ�̵�ַ
 *\param[in] const char * projectname ��Ŀ��
 *\return    const char* Զ�̵�ַ
 */
const char* Config::getRemotePath(const char *projectname)
{
    if (NULL == projectname) return NULL;

    ProjectInfoMap::const_iterator iter = projectInfo_.find(projectname);

    if (iter != projectInfo_.end())
    {
        return iter->second.remotePath.c_str();
    }

    return NULL;
}

/**
 *\fn        int getServerId(const char *projectname)
 *\brief     �õ������ڵķ�����ID
 *\param[in] const char * projectname ��Ŀ��
 *\return    int -1ʧ��,����������ID
 */
int Config::getServerId(const char *projectname)
{
    if (NULL == projectname) return -1;

    ProjectInfoMap::const_iterator iter = projectInfo_.find(projectname);

    if (iter != projectInfo_.end())
    {
        return iter->second.serverId;
    }

    return -1;
}

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
int Config::getServerParam(int index, char *addr, int &port, 
    char *username, char *password, int &portMappingCount)
{
    if (NULL == addr || index >= servers_.getCount()) return -1;

    servers_[index]->Attribute("port", (int*)&port);
    strcpy_s(addr,     32, servers_[index]->Attribute("addr"));
    strcpy_s(username, 32, servers_[index]->Attribute("username"));
    strcpy_s(password, 32, servers_[index]->Attribute("password"));

    portMappingCount = servers_[index]->SelectNodes("port").getCount();

    return 0;
}

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
int Config::getServerPortMap(int serverIndex, int portIndexint, int &type, int &local, int &remote)
{
    XT_XML::TiXmlElementExs ports;

    if (serverIndex < 0 || serverIndex >= servers_.getCount()) return -1;

    ports = servers_[serverIndex]->SelectNodes("port");

    if (portIndexint < 0 || portIndexint >= ports.getCount()) return -2;

    ports[portIndexint]->Attribute("type", (int*)&type);
    ports[portIndexint]->Attribute("local", (int*)&local);
    ports[portIndexint]->Attribute("remote", (int*)&remote);

    return 0;
}

