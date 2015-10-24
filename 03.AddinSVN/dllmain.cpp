/*

��SVN��binĿ¼�е�SciLexer.dll����ΪSciLexerTrue.dll
�����������ɵ�dll,���Ƶ�binĿ¼��
��config.xml���Ƶ�binĿ¼��

config.xml��������:

<?xml version="1.0" encoding="GB2312" ?>
<XmlRoot>
  <!-- is_check���¼��BOM, need_bom�Ƿ���ҪBOM, typeֻ�������͵��ļ�-->
  <check_bom is_check="1" need_bom="0" type=".h|.c|.cpp">
    <!-- ��is_checkΪ1ʱ,��ʾ����Ҫ�����ļ� -->
    <!-- ��is_checkΪ0ʱ,��ʾ��Ҫ�����ļ� -->
    <exclude name="odbc.c" />
  </check_bom>

  <commit>
    <!-- ����Ҫ�ύ���ļ� -->
    <exclude name="lim_mq_queue.h" />
    <exclude name="lim_statistics_info.h" />
    <exclude name="lim_statistics_info.c" />
    <exclude name="lim_service.h" />
    <exclude name="lim_serverutil.c" />
    <exclude name="lim_jvm.c" />
    <exclude name="odbc.c" />
    <exclude name="utility.c" />
    <exclude name="mem_chunk.h" />
    <exclude name="mem_chunk.c" />
    <exclude name="check_event.c" />
    <exclude name="garbagecollector.h" />
    <exclude name="TestSet.vs2010.sln" />
  </commit>
</XmlRoot>

*/

#include "stdafx.h"
#include "08.Algorithm/Base/MD5.h"
#include "08.Algorithm/Base/XTXml.h"
#include "07.Database/CppSQLite3.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <process.h>
#include <CommCtrl.h>
#include <psapi.h> // GetModuleBaseName

#define WM_SHOW_MSG (WM_USER + 123)
#define WM_EXIT     (WM_USER + 124)

class ScintillaWin;

typedef HRESULT (*Scintilla_DirectFunction_true)(ScintillaWin *sci, UINT iMessage, WPARAM wParam, LPARAM lParam);

typedef std::list<std::string> STRING_LIST;

XT_XML::xml g_xml;

HWND g_lhHook     = NULL; // ���Ӿ��
HWND g_hCommitWnd = NULL; // �ύ������
HWND g_hOkButton  = NULL; // OK��ť���
UINT g_hShowMsgThreadId = 0;
std::string g_errorInfo;
bool g_exit = false;

//--------------------------------------------------------------------------------

HMODULE g_scilexer_dll_module = NULL; // SciLexer.dllģ����
Scintilla_DirectFunction_true g_scilexer_dll_fun = NULL; // SciLexer.dll��Scintilla_DirectFunction�ĺ���ָ��

extern "C" __declspec(dllexport) 
HRESULT __stdcall Scintilla_DirectFunction(ScintillaWin *sci, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    if (NULL == g_scilexer_dll_fun) return -1;

    return g_scilexer_dll_fun(sci, iMessage, wParam, lParam);
}

//--------------------------------------------------------------------------------

// �����Ӵ���ص�����
BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
    DWORD dwPid = 0;

    DWORD threadId = GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���

    if (dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���
    {
        char className[MAX_PATH] = "";
        char windowName[MAX_PATH] = "";

        GetClassName(hWnd, className, sizeof(className));
        GetWindowText(hWnd, windowName, sizeof(windowName));

        if (0 == strcmp(className, "SysListView32"))
        {
            g_hCommitWnd = hWnd;
        }
        else if (0 == strcmp(className, "Button") && 0 == strcmp(windowName, "&OK"))//Cancel
        {
            g_hOkButton = hWnd;
        }

        if (NULL != g_hCommitWnd && NULL != g_hOkButton)
        {
            return FALSE;
        }

        EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // �ݹ�����Ӵ���
    }

    return TRUE;
}

// ���Ҵ���ص�����
BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)  
{
    DWORD dwPid = 0;

    GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���

    if (dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���
    {
        EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ���
    }

    return TRUE;
}

//--------------------------------------------------------------------------------

typedef std::vector<std::string>  VECTOR_STR;

void TokenizeData(const char *srcData, const char *tokenize, VECTOR_STR &data)
{
    if (NULL == srcData) return;

    char *content = NULL;
    char temp[MAX_PATH*10] = "";
    strcpy_s(temp, srcData);

    char *token = strtok_s(temp, tokenize, &content);

    while (token != NULL)
    {
        data.push_back(token);
        token = (char*)strtok_s(NULL, tokenize, &content);
    }
}

/**
 *\fn        void GetConfigPath(char *path, int maxSize)
 *\brief     �õ������ļ�·��
 *\param[in] char * path ·��
 *\param[in] int maxSize ·����������С
 *\return    void 0�ɹ�,����ʧ��
 */
void GetConfigPath(char *path, int maxSize)
{
    if (NULL == path || maxSize < 0) return;

    GetModuleFileName(NULL, path, maxSize);

    char *end = strrchr(path, '\\');

    if (NULL != end)
    {
        *end = '\0';
    }

    strcat_s(path, maxSize, "\\config.xml");
}

/**
 *\fn        void GetSvnRoot(const char *path, char *root, int maxSize)
 *\brief     �õ�SVN��·��
 *\param[in] const char * path ��ǰ·��
 *\param[out]char * root SVN��·��
 *\param[in] int maxSize ��󳤶�
 *\return    void 0�ɹ�,����ʧ��
 */
void GetSvnRoot(const char *path, char *root, int maxSize)
{
    if (NULL == path || NULL == root || maxSize < 0) return;

    char temp[1024] = "";

    VECTOR_STR vectorStr;
    TokenizeData(path, "\\", vectorStr);

    for (UINT i = 0; i < vectorStr.size(); i++)
    {
        strcpy_s(temp, vectorStr[0].c_str());

        for (UINT j = 1; j <= i; j++)
        {
            strcat_s(temp, "\\");
            strcat_s(temp, vectorStr[j].c_str());
        }

        strcat_s(temp, "\\.svn\\wc.db");

        if (_access(temp, 0) == 0)
        {
            strncpy_s(root, maxSize, temp, strstr(temp, "\\.svn\\wc.db") - temp);
            return;
        }
    }
}

/**
 *\fn        int getSvnFileMd5(const char *svnRoot, const char *filename, char *md5, int maxSize)
 *\brief     �õ�SVN������ϴε��ļ�MD5
 *\param[in] const char * svnRoot SVN��·��
 *\param[in] const char * filename 
 *\param[in] char * md5 
 *\param[in] int maxSize 
 *\return    int 0�ɹ�,����ʧ��
 */
int getSvnFileMd5(const char *svnRoot, const char *filename, char *md5, int maxSize)
{
    if (NULL == svnRoot || NULL == filename || NULL == md5 || maxSize <= 0) return -1;

    char temp[1024] = "";
    char local[1024] = "";

    CppSQLite3DB db;

    _snprintf_s(temp, sizeof(temp)-1, "%s\\.svn/wc.db", svnRoot);

    db.open(temp);

    strcpy_s(local, filename);

    for (UINT i = 0; i < strlen(local); i++)
    {
        if ('\\' == local[i]) local[i] = '/';
    }

    _snprintf_s(temp, sizeof(temp)-1, "SELECT checksum FROM nodes WHERE local_relpath like '%s';", 
        &local[strlen(svnRoot)+1]);

    CppSQLite3Query q = db.execQuery(temp);

    if (!q.eof())
    {
        strncpy_s(temp, q.fieldValue(0), sizeof(temp)-1);
    }

    q.finalize();

    db.close();

    //--------------------------------------

    VECTOR_STR vectorStr;
    TokenizeData(temp, "$", vectorStr);

    if (vectorStr.size() <= 1)
    {
        return -2;
    }

    _snprintf_s(temp, sizeof(temp)-1, "%s\\.svn\\pristine\\%c%c\\%s.svn-base", 
        svnRoot, vectorStr[1].c_str()[0], vectorStr[1].c_str()[1], vectorStr[1].c_str());

    FILE *file = NULL;

    fopen_s(&file, temp, "rb");

    if (NULL == file) return -3;

    fseek(file, 0, SEEK_END);

    int len = ftell(file);
    char *buf = new char[len+10];

    fseek(file, 0, SEEK_SET);
    fread(buf, 1, len, file);
    fclose(file);

    CMD5 md;
    std::string str = md.getHashStr(buf, len);
    strcpy_s(md5, 33, str.c_str());

    delete buf;

    return 0;
}

/**
 *\fn        void GetCurrentPath(HWND wnd, char *path, int maxSize)
 *\brief     �õ���ǰ·��
 *\param[in] HWND wnd ������
 *\param[out] char * path ·��
 *\param[in] int maxSize ·����������С
 *\return    void 0�ɹ�,����ʧ��
 */
void GetCurrentPath(HWND wnd, char *path, int maxSize)
{
    if (NULL == path || maxSize < 0) return;

    GetWindowText(GetParent(wnd), path, maxSize);

    char *end = strchr(path, ' ');

    if (NULL != end)
    {
        *end = '\0';
    }
}

/**
 *\fn        int GetConfigData(const char *configFile, bool &checkBom, bool &needBom, 
                               STRING_LIST &checkTypeList, STRING_LIST &checkExcludeList, 
                               STRING_LIST &commitExcludeList)
 *\brief     �ᵽ�����ļ�����
 *\param[in] const char * configFile �����ļ���
 *\param[in] bool & checkBom ����BOM
 *\param[in] bool & needBom ��Ҫ��BOM
 *\param[in] STRING_LIST & checkTypeList ����ļ����������б�
 *\param[in] STRING_LIST & checkExcludeList ���BOM�ų��ļ��б�
 *\param[in] STRING_LIST & commitExcludeList �ύ�ų��ļ��б�
 *\return    int 0�ɹ�,����ʧ��
 */
int GetConfigData(const char *configFile, 
    bool &checkBom, 
    bool &needBom,
    STRING_LIST &checkTypeList,
    STRING_LIST &checkExcludeList,
    STRING_LIST &commitExcludeList)
{
    checkBom = false;
    checkTypeList.clear();
    checkExcludeList.clear();
    commitExcludeList.clear();

    const char *value = NULL;
    XT_XML::TiXmlElementEx *checkNode; // ���ڵ�
    XT_XML::TiXmlElementEx *commitNode; // �ύ�ڵ�
    XT_XML::TiXmlElementExs checkExcludeFile; // �����ų��ڵ�
    XT_XML::TiXmlElementExs commitExcludeFile; // �ύ���ų��ڵ�

    if (!g_xml.LoadFile(configFile))
    {
        MessageBox(NULL, "�õ������ļ�����", "����", MB_OK);
        return -1;
    }

    checkNode = g_xml.GetRoot()->SelectSingleNode("check_bom");
    commitNode = g_xml.GetRoot()->SelectSingleNode("commit");

    if (NULL != checkNode)
    {
        value = checkNode->Attribute("is_check");

        if (NULL != value)
        {
            checkBom = (0 == strcmp(value, "1"));
        }

        value = checkNode->Attribute("need_bom");

        if (NULL != value)
        {
            needBom = (0 == strcmp(value, "1"));
        }

        value = checkNode->Attribute("type");

        if (NULL != value)
        {
            char temp[MAX_PATH*10] = "";
            strcpy_s(temp, value);

            char *content = NULL;
            char *token = strtok_s(temp, "|", &content);

            while (token != NULL)
            {
                checkTypeList.push_back(token);
                token = (char*)strtok_s(NULL, "|", &content);
            }
        }

        checkExcludeFile = checkNode->SelectNodes("exclude");

        for (int i = 0; i < checkExcludeFile.getCount(); i++)
        {
            value = checkExcludeFile[i]->Attribute("name");

            if (NULL != value)
            {
                checkExcludeList.push_back(value);
            }
        }
    }

    if (NULL != commitNode)
    {
        commitExcludeFile = commitNode->SelectNodes("exclude");

        for (int i = 0; i < commitExcludeFile.getCount(); i++)
        {
            value = commitExcludeFile[i]->Attribute("name");

            if (NULL != value)
            {
                commitExcludeList.push_back(value);
            }
        }
    }

    return 0;
}

/**
 *\fn        int GetFileCount(HWND wnd)
 *\brief     �õ��ļ�����
 *\param[in] HWND wnd ������
 *\return    int 0�ɹ�,����ʧ��
 */
int GetFileCount(HWND wnd)
{
    return (int)SendMessage(wnd, LVM_GETITEMCOUNT, 0, 0L);
}

/**
 *\fn        BOOL GetCheckState(HWND wnd, int nIndex)
 *\brief     �����б��Ƿ�Ϊѡ��
 *\param[in] HWND wnd ������
 *\param[in] int nIndex ���
 *\return    BOOL 0�ɹ�,����ʧ��
 */
BOOL GetCheckState(HWND wnd, int nIndex)
{
    UINT uRet = (UINT)::SendMessage(wnd, LVM_GETITEMSTATE, nIndex, LVIS_STATEIMAGEMASK);
    return (uRet >> 12) - 1;
}

/**
 *\fn        void GetFileList(HWND wnd, STRING_LIST &list)
 *\brief     �õ���Ҫ�ύ���ļ��б�
 *\param[in] HWND wnd ������
 *\param[in] STRING_LIST & list �ļ��б�
 *\return    void 0�ɹ�,����ʧ��
 */
void GetFileList(HWND wnd, STRING_LIST &list)
{
    char name[1024] = "";
    LVITEM lvi = { 0 };
    int count = (int)SendMessage(wnd, LVM_GETITEMCOUNT, 0, 0L);

    for (int i = 0; i < count; i++)
    {
        if (GetCheckState(wnd, i))
        {
            ListView_GetItemText(wnd, i, 0, name, sizeof(name));

            list.push_back(name);
        }
    }
}

/**
 *\fn        void UnSelectFile(HWND wnd, const char *filename)
 *\brief     ȡ���ļ�ѡ��
 *\param[in] HWND wnd ������
 *\param[in] const char * filename �ļ���
 *\return    void 0�ɹ�,����ʧ��
 */
void UnSelectFile(HWND wnd, const char *filename)
{
    char name[1024] = "";
    LVITEM lvi = { 0 };
    int count = (int)SendMessage(wnd, LVM_GETITEMCOUNT, 0, 0L);

    for (int i = 0; i < count; i++)
    {
        if (GetCheckState(wnd, i))
        {
            ListView_GetItemText(wnd, i, 0, name, sizeof(name));

            if (0 == strcmp(filename, name))
            {
                ListView_SetCheckState(wnd, i, false);
                break;
            }
        }
    }
}

/**
 *\fn        bool isExcludeFile(const char *filename, STRING_LIST &excludeList)
 *\brief     ���ļ��Ƿ����ų��б���
 *\param[in] const char * filename �ļ���
 *\param[in] STRING_LIST & excludeList �ų��б���
 *\return    bool 0�ɹ�,����ʧ��
 */
bool isExcludeFile(const char *filename, STRING_LIST &excludeList)
{
    if (filename == NULL) return false;

    STRING_LIST::const_iterator iter = excludeList.begin();

    for (; iter != excludeList.end(); ++iter)
    {
        const char *have = strstr(filename, iter->c_str());

        if (NULL != have)
        {
            return true;
        }
    }

    return false;
}

/**
 *\fn        bool isBom(const char *filename)
 *\brief     ����ļ��Ƿ���BOM
 *\param[in] const char * filename �ļ���
 *\return    bool true��,false��
 */
bool isBom(const char *filename)
{
    if (filename == NULL) return false;

    unsigned char buff[10] = "";

    FILE *file = NULL;

    fopen_s(&file, filename, "rb");

    if (NULL != file)
    {
        fread(buff, 1, 3, file);
        fclose(file);
    }

    return (buff[0] == 0xEF && buff[1] == 0xBB && buff[2] == 0xBF);
}

/**
 *\fn        void setBom(const char *filename, bool set, char *md5)
 *\brief     ����utf8��BOM
 *\param[in] const char * filename �ļ���
 *\param[in] bool set �Ƿ�����
 *\param[out]char * md5 �ļ�MD5
 *\return    void 0�ɹ�,����ʧ��
 */
void setBom(const char *filename, bool set, char *md5)
{
    if (filename == NULL) return;

    int len = 0;
    char *buf = NULL;
    unsigned char bom[10] = {0xEF, 0xBB, 0xBF};

    FILE *file = NULL;

    fopen_s(&file, filename, "rb");

    if (NULL != file)
    {
        fseek(file, 0, SEEK_END);

        len = ftell(file);
        buf = new char[len+10];

        fseek(file, set?0:3, SEEK_SET);
        len = fread(buf, 1, len-(set?0:3), file);
        fclose(file);
    }

    file = NULL;

    fopen_s(&file, filename, "w+b");

    CMD5 md;
    md.Reset();

    if (NULL != file)
    {
        if (set)
        {
            fwrite(bom, 1, 3, file);
            md.Add(bom, 3);
        }

        fwrite(buf, 1, len, file);
        md.Add(buf, len);

        fclose(file);
        md.Finish();
    }

    if (NULL != md5)
    {
        std::string str = md.getHashStr();
        strcpy_s(md5, 33, str.c_str());
    }

    if (NULL != buf)
    {
        free(buf);
    }
}

/**
 *\fn        bool checkFileBom(const char *filename, bool needBom, 
                               STRING_LIST &checkTypeList, STRING_LIST &checkExcludeList)
 *\brief     ����ļ�BOM
 *\param[in] const char * filename �ļ���
 *\param[in] needBom �Ƿ���ҪBOM
 *\param[in] STRING_LIST & checkTypeList ��������б�
 *\param[in] STRING_LIST & checkExcludeList �ų����ļ�
 *\return    bool �Ƿ�ɹ�
 */
bool checkFileBom(const char *filename, bool needBom, 
    STRING_LIST &checkTypeList, STRING_LIST &checkExcludeList)
{
    if (filename == NULL) return false;

    bool needCheck = false;
    const char *end = strrchr(filename, '.');

    if (NULL != end)
    {
        STRING_LIST::const_iterator iter = checkTypeList.begin();

        for (; iter != checkTypeList.end(); ++iter)
        {
            const char *file_end = strrchr(iter->c_str(), '.');

            if (0 == strcmp(file_end, end))
            {
                needCheck = true;
                break;
            }
        }
    }

    // ��Ҫ���,�����ų��б���,���Ҳ���Bom
    if (needCheck && !isExcludeFile(filename, checkExcludeList) && (needBom != isBom(filename)))
    {
        return false;
    }

    return true;
}

/**
 *\fn        int processCommit()
 *\brief     �����ύ
 *\return    int 0�ɹ�,����ʧ��
 */
int processCommit()
{
    char cfgPath[1024] = "";
    char curPath[1024] = "";
    char svnRoot[1024] = "";
    char filename[1024] = "";
    char temp[1024] = "";
    int fileCount = 0;
    bool checkBom = false;
    bool needBom = false;
    STRING_LIST checkTypeList;
    STRING_LIST checkExcludeList;
    STRING_LIST commitExcludeList;
    STRING_LIST fileList;

    g_errorInfo = "";

    fileCount = GetFileCount(g_hCommitWnd);

    if (fileCount <= 0)
    {
        return 0;
    }

    GetConfigPath(cfgPath, sizeof(cfgPath)-1);
    GetCurrentPath(g_hCommitWnd, curPath, sizeof(curPath)-1);
    GetConfigData(cfgPath, checkBom, needBom, checkTypeList, checkExcludeList, commitExcludeList);
    GetFileList(g_hCommitWnd, fileList);
    GetSvnRoot(curPath, svnRoot, sizeof(svnRoot)-1);

    STRING_LIST::iterator preIter;
    STRING_LIST::iterator fileIter = fileList.begin();

    for (; fileIter != fileList.end(); ++fileIter)
    {
        if (isExcludeFile(fileIter->c_str(), commitExcludeList))
        {
            UnSelectFile(g_hCommitWnd, fileIter->c_str());

            _snprintf_s(temp, sizeof(temp)-1,"����Ҫ�ύ %s\n", fileIter->c_str());
            g_errorInfo += temp;

            *fileIter = "";
        }
    }

    if (checkBom)
    {
        char curMd5[33] = "";
        char lastMd5[33] = "";

        STRING_LIST::const_iterator fileIter = fileList.begin();

        for (; fileIter != fileList.end(); ++fileIter)
        {
            if (*fileIter == "") continue;

            _snprintf_s(filename, sizeof(filename)-1, "%s/%s", curPath, fileIter->c_str());

            if (!checkFileBom(filename, needBom, checkTypeList, checkExcludeList))
            {
                setBom(filename, needBom, curMd5);

                _snprintf_s(temp, sizeof(temp)-1, "%s��Ҫǩ�� %s\n", !needBom?"��":"", fileIter->c_str());
                g_errorInfo += temp;

                getSvnFileMd5(svnRoot, filename, lastMd5, sizeof(lastMd5)-1);

                if (0 == _stricmp(curMd5, lastMd5))
                {
                    UnSelectFile(g_hCommitWnd, fileIter->c_str());
                }
            }
        }
    }

    if (0 != strcmp(g_errorInfo.c_str(), ""))
    {
        PostThreadMessage(g_hShowMsgThreadId, WM_SHOW_MSG, NULL, NULL);
        return 1;
    }

    return 0;
}

//--------------------------------------------------------------------------------

static unsigned __stdcall show_message_thread(void * pParam)
{
    MSG msg = {0};

    while (!g_exit)
    {
        if (!GetMessage(&msg, 0, 0, 0)) continue;

        switch (msg.message)
        {
        case WM_SHOW_MSG:
            {
                MessageBox(NULL, g_errorInfo.c_str(), "����", MB_OK);
                break;
            }
        case WM_EXIT:
            {
                return 0;
            }
        }
    }

    return 0;
}

// ����¼��ص�
LRESULT WINAPI MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx((HHOOK)g_lhHook, nCode, wParam, lParam);
    }

    if (NULL == g_hOkButton)
    {
        EnumWindows(EnumWindowCallBack, GetCurrentProcessId());
    }

    if (NULL == g_hOkButton)
    {
        MessageBox(NULL, "û���ҵ���ť���", "����", MB_OK);
    }

    if (NULL == g_hCommitWnd)
    {
        MessageBox(NULL, "û���ҵ�������", "����", MB_OK);
    }

    int ret = 0;
    LPMOUSEHOOKSTRUCT pMouseHook = (MOUSEHOOKSTRUCT FAR *) lParam;

    if (wParam == WM_LBUTTONDOWN && g_hOkButton == pMouseHook->hwnd)
    {
        ret = processCommit();
    }

    if (1 == ret)
    {
        return 1; // ����һ�������ֵ��ʾ�����Ѿ��������Ϣ�����ˣ�ϵͳ�����ٽ������Ϣ���´�����
    }

    // ����������Ϣ, 1-��Ϣ������,0-��Ϣ��������
    return CallNextHookEx((HHOOK)g_lhHook, nCode, wParam, lParam);
}

// ���������Ϣ����
void StartHook()
{
    g_lhHook = (HWND)SetWindowsHookEx(WH_MOUSE, MouseProc, g_scilexer_dll_module, GetCurrentThreadId());

    if (NULL == g_lhHook)
    {
        MessageBox(NULL, "Hook���ɹ�", "����", MB_OK);
    }
}

// ȡ������
void StopHook()
{
    UnhookWindowsHookEx((HHOOK)g_lhHook);
}

#pragma comment (lib,"Psapi.lib") // GetModuleBaseName

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            g_scilexer_dll_module = LoadLibrary("SciLexerTrue.dll");

            if (NULL != g_scilexer_dll_module)
            {
                g_scilexer_dll_fun = (Scintilla_DirectFunction_true)GetProcAddress(g_scilexer_dll_module, 
                    "Scintilla_DirectFunction");

                char text[128] = "";
                GetModuleBaseName(GetCurrentProcess(), NULL, text, sizeof(text));

                if (0 == strcmp(text, "TortoiseProc.exe"))
                {
                    StartHook();
                }
            }

            if (0 == g_hShowMsgThreadId)
            {
                //_beginthreadex(NULL, 0, &show_message_thread, NULL, 0, &g_hShowMsgThreadId);
            }

            break;
        }
    case DLL_PROCESS_DETACH:
        {
            if (NULL != g_scilexer_dll_module)
            {
                StopHook();
                FreeLibrary(g_scilexer_dll_module);
            }

            if (0 != g_hShowMsgThreadId)
            {
                g_exit = true;
                PostThreadMessage(g_hShowMsgThreadId, WM_EXIT, NULL, NULL);
            }

            break;
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        {
            break;
        }
    }

    return TRUE;
}

