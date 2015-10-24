// Connect.h : CConnect ������

#pragma once
#include "resource.h"       // ������
#include "AddinVSEx_i.c"
#include "AddinVSEx.h"
#include "src/SshConfig.h"
#include <map>
#include <string>

extern "C"
{
#include "03.Network/ssh2/ssh2.h"
#include "03.Network/ssh2/sftp.h"
};

typedef struct tagProjectSftp
{
    int serverPort;
    char serverAddr[256];
    char username[256];
    char password[256];
    char remotePath[1024];

    int portMappingCount;

    my_ssh_param ssh_param;

}ProjectSftpData, *PProjectSftpData;

typedef std::map<std::string, std::string> STR_STR_MAP;
typedef std::map<int, PProjectSftpData> INT_SSH_MAP;


class CAddinProcess;

using namespace AddInDesignerObjects;
using namespace EnvDTE;
using namespace EnvDTE80;
using namespace Microsoft_VisualStudio_CommandBars;


// �ĵ��¼��ص�
class CDocumentEventsSink: public IDispEventImpl<1, 
    CDocumentEventsSink, 
    &__uuidof(EnvDTE::_dispDocumentEvents), 
    &EnvDTE::LIBID_EnvDTE, 
    8, 
    0>
{
public:
    CDocumentEventsSink()
    {
    }

    BEGIN_SINK_MAP(CDocumentEventsSink)
        SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispDocumentEvents), 1, OnDocumentSaved) //1-�Ѿ�����,2-�ر� ,3-���ڴ�,4-�Ѿ���
    END_SINK_MAP()

public:
    STDMETHOD(OnDocumentSaved)(EnvDTE::Document *document);
};



/// <summary>����ʵ����ӳ���Ķ���</summary>
/// <seealso class='IDTExtensibility2' />
class ATL_NO_VTABLE CConnect : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CConnect, &CLSID_Connect>,
    public IDispatchImpl<IDTCommandTarget, &IID_IDTCommandTarget, &LIBID_EnvDTE, 7, 0>,
    public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects, 1, 0>
{
public:
    /// <summary>ʵ����ӳ������Ĺ��캯�����뽫���ĳ�ʼ���������ڴ˷����ڡ�</summary>
    CConnect();
    ~CConnect();

    DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)
    DECLARE_NOT_AGGREGATABLE(CConnect)

    BEGIN_COM_MAP(CConnect)
        COM_INTERFACE_ENTRY(IDTExtensibility2)
        COM_INTERFACE_ENTRY(IDTCommandTarget)
        COM_INTERFACE_ENTRY2(IDispatch, IDTExtensibility2)
    END_COM_MAP()



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease() 
    {
    }

public:
    //IDTExtensibility2 �ľ���ʵ��:

    /// <summary>ʵ�� IDTExtensibility2 �ӿڵ� OnConnection �������������ڼ�����ӳ����֪ͨ��</summary>
    /// <param term='application'>����Ӧ�ó���ĸ�����</param>
    /// <param term='connectMode'>������ӳ���ļ��ط�ʽ��</param>
    /// <param term='addInInst'>��ʾ����ӳ���Ķ���</param>
    /// <seealso class='IDTExtensibility2' />
    STDMETHOD(OnConnection)(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch *AddInInst, SAFEARRAY **custom);

    /// <summary>ʵ�� IDTExtensibility2 �ӿڵ� OnDisconnection ��������������ж����ӳ����֪ͨ��</summary>
    /// <param term='disconnectMode'>������ӳ����ж�ط�ʽ��</param>
    /// <param term='custom'>�ض�������Ӧ�ó���Ĳ������顣</param>
    /// <seealso class='IDTExtensibility2' />
    STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY **custom );

    /// <summary>ʵ�� IDTExtensibility2 �ӿڵ� OnAddInsUpdate ����������ӳ��򼯺��ѷ�������ʱ����֪ͨ��</summary>
    /// <param term='custom'>�ض�������Ӧ�ó���Ĳ������顣</param>
    /// <seealso class='IDTExtensibility2' />	
    STDMETHOD(OnAddInsUpdate)(SAFEARRAY **custom );

    /// <summary>ʵ�� IDTExtensibility2 �ӿڵ� OnStartupComplete ��������������Ӧ�ó�������ɼ��ص�֪ͨ��</summary>
    /// <param term='custom'>�ض�������Ӧ�ó���Ĳ������顣</param>
    /// <seealso class='IDTExtensibility2' />
    STDMETHOD(OnStartupComplete)(SAFEARRAY **custom );

    /// <summary>ʵ�� IDTExtensibility2 �ӿڵ� OnBeginShutdown ��������������ж������Ӧ�ó����֪ͨ��</summary>
    /// <param term='custom'>�ض�������Ӧ�ó���Ĳ������顣</param>
    /// <seealso class='IDTExtensibility2' />
    STDMETHOD(OnBeginShutdown)(SAFEARRAY **custom );

    //IDTCommandTarget �ľ���ʵ��:

    /// <summary>ʵ�� IDTCommandTarget �ӿڵ� QueryStatus �������˷����ڸ��¸�����Ŀ�����ʱ����</summary>
    /// <param term='commandName'>Ҫȷ����״̬����������ơ�</param>
    /// <param term='neededText'>������������ı���</param>
    /// <param term='status'>���������û������е�״̬��</param>
    /// <param term='commandText'>neededText ������Ҫ����ı���</param>
    /// <seealso class='Exec' />
    STDMETHOD(QueryStatus)(BSTR CmdName, vsCommandStatusTextWanted NeededText, vsCommandStatus *StatusOption, VARIANT *CommandText);

    /// <summary>ʵ�� IDTCommandTarget �ӿڵ� Exec �������˷����ڵ��ø�����ʱ���á�</summary>
    /// <param term='commandName'>Ҫִ�е���������ơ�</param>
    /// <param term='executeOption'>����������Ӧ������С�</param>
    /// <param term='varIn'>�ӵ��÷����ݵ���������Ĳ�����</param>
    /// <param term='varOut'>���������򴫵ݵ����÷��Ĳ�����</param>
    /// <param term='handled'>֪ͨ���÷��������Ƿ��ѱ�����</param>
    /// <seealso class='Exec' />
    STDMETHOD(Exec)(BSTR CmdName, vsCommandExecOption ExecuteOption, VARIANT *VariantIn, VARIANT *VariantOut, VARIANT_BOOL *Handled);

private:
    CComPtr<DTE2>       m_pDTE;
    CComPtr<AddIn>      m_pAddInInstance;
    CComPtr<Window>     m_pToolWindow;
    CAddinProcess      *m_pAddinProcess;
    CDocumentEventsSink m_objDocumentEvents;

    Config              m_Config;           // �����ļ�
    INT_SSH_MAP         sftpParam_;         // SFTP����

public:
    /**
     *\fn        void OnUploadFile()
     *\brief     �ϴ��ļ�
     *\return    void ��
     */
    void OnUploadFile();

    /**
     *\fn        void OnDownloadFile()
     *\brief     �����ļ�
     *\return    void ��
     */
    void OnDownloadFile();

    /**
     *\fn        void OnShowWindow()
     *\brief     ��ʾ�����
     *\return    void ��
     */
    void OnShowWindow();

    /**
     *\fn        void OnAddFileBom()
     *\brief     ����ļ�BOM
     *\return    void ��
     */
    void OnAddFileBom();

    /**
     *\fn        void OnDelFileBom()
     *\brief     ɾ���ļ�BOM
     *\return    void ��
     */
    void OnDelFileBom();

    /**
     *\fn        bool isFileBom(const char *filename)
     *\brief     �õ��Ƿ���BOM���
     *\param[in] const char * filename �ļ���
     *\return    �Ƿ���BOM���
     */
    bool isFileBom(const char *filename);

    /**
     *\fn        void setFileBom(const char *filename, bool set)
     *\brief     ����BOM���
     *\param[in] const char * filename �ļ���
     *\param[in] bool set �Ƿ���BOM���
     *\return    void 0�ɹ�,����ʧ��
     */
    void setFileBom(const char *filename, bool set);

    /**
     *\fn        PProjectSftpData runSftp()
     *\brief     ����sftp
     *\return    PProjectSftpData sftp����
     */
    PProjectSftpData runSftp();

    /**
     *\fn         GetSelectFile(STR_STR_MAP &files)
     *\brief      �õ�ѡ�е��ļ���
     *\param[out] files ѡ�е��ļ���
     */
    void GetSelectFile(STR_STR_MAP &files);

    /**
     *\fn         GetConfigPath()
     *\brief      �õ�ͨ����Ŀ�ļ�·��,�õ������ļ�·����
     *\return     �����ļ�·��
     */
    std::string GetConfigPath();
};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)
