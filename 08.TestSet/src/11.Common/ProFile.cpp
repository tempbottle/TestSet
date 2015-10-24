
#include "stdafx.h"
#include "ProFile.h"
#include "TestSetConfig.h"


CProFile g_ProFile;


CProFile::CProFile()
{
}

CProFile::~CProFile()
{
    imageList_.DeleteImageList();
    google::ShutdownGoogleLogging();
}

void CProFile::Init(STRING_LIST &list)
{
    // �õ���ǰ����·��
    TCHAR path[MAX_PATH] = _T("");
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);
    path_ = path;

    TCHAR cfgFile[MAX_PATH] = _T("");
    _stprintf_s(cfgFile, ARRAYSIZE(cfgFile), _T("%s\\res\\config\\Config.xml"), path);

    CTestSetConfig cfg;
    if (cfg.OpenFile(cfgFile))
    {
        LPCTSTR infoName = cfg.GetGlogInfoFilename();
        LPCTSTR errorName = cfg.GetGlogErrorFilename();
        LPCTSTR warningName = cfg.GetGlogWarningFilename();

        if (NULL != infoName)
        {
            google::SetLogDestination(google::GLOG_INFO, infoName);
            LOG(INFO) << _T("��Ϣ��־ ") << infoName;
        }

        if (NULL != errorName)
        {
            google::SetLogDestination(google::GLOG_ERROR, errorName);
            LOG(INFO) << _T("������־ ") << errorName;
        }

        if (NULL != warningName)
        {
            google::SetLogDestination(google::GLOG_WARNING, warningName);
            LOG(INFO) << _T("������־ ") << warningName;
        }
    }
    else
    {
        google::LogToStderr();
    }

    LOG(INFO) << _T("��������ʱ��: ") << __DATE__ << _T(" ") << __TIME__;
    LOG(INFO) << _T("��ǰ·��: ") << path_.c_str();

    // ����ͼ��
    imageList_.Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 1);
    imageList_.SetBkColor(CLR_NONE);

    STRING_LIST::iterator iter = list.begin();
    for (; iter != list.end(); ++iter)
    {
        imageList_.Add(LoadIcon(AfxGetResourceHandle(), iter->c_str()));
    }

    // �õ��������һЩ��Ϣ
    computerInfo_.Init();

    for (int i = 0; i < computerInfo_.getIpCount(); i++)
    {
        LOG(INFO) << computerInfo_.getIpStr(i) << " " << computerInfo_.getMacStr(i);
    }

    google::FlushLogFiles(google::GLOG_INFO);
}
