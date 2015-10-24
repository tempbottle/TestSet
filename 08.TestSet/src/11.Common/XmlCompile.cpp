#include "stdafx.h"
#include "XmlCompile.h"
#include "../../resource.h"


CXmlCompile::CXmlCompile()
:m_bLoadFile(FALSE)
{
    // C/C++
    m_arTnan[0].Add(_T("AdditionalIncludeDirectories|AdditionalUsingDirectories|DebugInformationFormat|SuppressStartupBanner|WarningLevel|Detect64BitPortabilityProblems|WarnAsError|UseUnicodeResponseFiles"));
    m_arTnan[0].Add(_T("Optimization|InlineFunctionExpansion|EnableIntrinsicFunctions|FavorSizeOrSpeed|OmitFramePointers|EnableFiberSafeOptimizations|WholeProgramOptimization"));
    m_arTnan[0].Add(_T("PreprocessorDefinitions|IgnoreStandardIncludePath|GeneratePreprocessedFile|KeepComments"));
    m_arTnan[0].Add(_T("StringPooling|MinimalRebuild|ExceptionHandling|SmallerTypeCheck|BasicRuntimeChecks|RuntimeLibrary|StructMemberAlignment|BufferSecurityCheck|EnableFunctionLevelLinking|EnableEnhancedInstructionSet|FloatingPointModel|FloatingPointExceptions"));
    m_arTnan[0].Add(_T("DisableLanguageExtensions|DefaultCharIsUnsigned|TreatWChar_tAsBuiltInType|ForceConformanceInForLoopScope|RuntimeTypeInfo|OpenMP"));
    m_arTnan[0].Add(_T("UsePrecompiledHeader|PrecompiledHeaderThrough|PrecompiledHeaderFile"));
    m_arTnan[0].Add(_T("ExpandAttributedSource|AssemblerOutput|AssemblerListingLocation|ObjectFile|ProgramDataBaseFileName|GenerateXMLDocumentationFiles|XMLDocumentationFileName"));
    m_arTnan[0].Add(_T("BrowseInformation|BrowseInformationFile"));
    m_arTnan[0].Add(_T("CallingConvention|CompileAs|DisableSpecificWarnings|ForcedIncludeFiles|ForcedUsingFiles|ShowIncludes|UndefinePreprocessorDefinitions|UndefineAllPreprocessorDefinitions|UseFullPaths|OmitDefaultLibName|ErrorReporting"));
    m_arTnan[0].Add(_T("AdditionalOptions"));
    // ������
    m_arTnan[1].Add(_T("OutputFile|ShowProgress|Version|LinkIncremental|SuppressStartupBanner|IgnoreImportLibrary|RegisterOutput|AdditionalLibraryDirectories|LinkLibraryDependencies|UseLibraryDependencyInputs|UseUnicodeResponseFiles"));
    m_arTnan[1].Add(_T("AdditionalDependencies|IgnoreAllDefaultLibraries|IgnoreDefaultLibraryNames|ModuleDefinitionFile|AddModuleNamesToAssembly|EmbedManagedResourceFile|ForceSymbolReferences|DelayLoadDLLs|AssemblyLinkResource"));
    m_arTnan[1].Add(_T("GenerateManifest|ManifestFile|AdditionalManifestDependencies|AllowIsolation"));
    m_arTnan[1].Add(_T("GenerateDebugInformation|ProgramDatabaseFile|StripPrivateSymbols|GenerateMapFile|MapFileName|MapExports|AssemblyDebug"));
    m_arTnan[1].Add(_T("SubSystem|HeapReserveSize|HeapCommitSize|StackReserveSize|StackCommitSize|LargeAddressAware|TerminalServerAware|SwapRunFromCD|SwapRunFromNet|Driver"));
    m_arTnan[1].Add(_T("OptimizeReferences|EnableCOMDATFolding|OptimizeForWindows98|FunctionOrder|ProfileGuidedDatabase|LinkTimeCodeGeneration"));
    m_arTnan[1].Add(_T("MidlCommandFile|IgnoreEmbeddedIDL|MergedIDLBaseFileName|TypeLibraryFile|TypeLibraryResourceID"));
    m_arTnan[1].Add(_T("EntryPointSymbol|ResourceOnlyDLL|SetChecksum|BaseAddress|FixedBaseAddress|TurnOffAssemblyGeneration|SupportUnloadOfDelayLoadedDLL|ImportLibrary|MergeSections|TargetMachine|Profile|CLRThreadAttribute|CLRImageType|KeyFile|KeyContainer|DelaySign|ErrorReporting|CLRUnmanagedCodeCheck"));
    m_arTnan[1].Add(_T("AdditionalOptions"));
    // �嵥����
    m_arTnan[2].Add(_T("SuppressStartupBanner|VerboseOutput|AssemblyIdentity|UseUnicodeResponseFiles|UseFAT32Workaround"));
    m_arTnan[2].Add(_T("AdditionalManifestFiles|InputResourceManifests|EmbedManifest|OutputManifestFile|ManifestResourceFile|GenerateCatalogFiles|DependencyInformationFile"));
    m_arTnan[2].Add(_T("TypeLibraryFile|RegistrarScriptFile|ComponentFileName|ReplacementsFile"));
    m_arTnan[2].Add(_T("UpdateFileHashes|UpdateFileHashesSearchPath"));
    m_arTnan[2].Add(_T("AdditionalOptions"));
    // ��Դ
    m_arTnan[3].Add(_T("PreprocessorDefinitions|Culture|AdditionalIncludeDirectories|IgnoreStandardIncludePath|ShowProgress|ResourceOutputFileName"));
    m_arTnan[3].Add(_T("AdditionalOptions"));
    // XML�ĵ�����
    m_arTnan[4].Add(_T("SuppressStartupBanner|ValidateIntelliSense|AdditionalDocumentFiles|OutputDocumentFile||UseUnicodeResponseFiles"));
    m_arTnan[4].Add(_T("AdditionalOptions"));
    // �����Ϣ
    m_arTnan[5].Add(_T("SuppressStartupBanner|OutputFile"));
    m_arTnan[5].Add(_T("AdditionalOptions"));
    // Ԥ�����¼�
    m_arTnan[6].Add(_T("CommandLine|Description|ExcludedFromBuild"));
    // Ԥ�����¼�
    m_arTnan[7].Add(_T("CommandLine|Description|ExcludedFromBuild"));
    // ���ɺ��¼�
    m_arTnan[8].Add(_T("CommandLine|Description|ExcludedFromBuild"));
    // �Զ������ɲ���
    m_arTnan[9].Add(_T("CommandLine|Description|Outputs|AdditionalDependencies"));
    // WEB����
    m_arTnan[10].Add(_T("VirtualDirectoryName|ApplicationMappings|ApplicationProtection|ExcludedFromBuild|RelativePath|AdditionalFiles|UnloadBeforeCopy|RegisterOutput"));

    CString strProjAttrib = _T("ProjectType|Version|Name|ProjectGUID|RootNamespace|Keyword|TargetFrameworkVersion|AssemblyReferenceSearchPaths");
    CString strCfgAttrib = _T("OutputDirectory|IntermediateDirectory|DeleteExtensionsOnClean|BuildLogFile|ConfigurationType|UseOfMFC|UseOfATL|ATLMinimizesCRunTimeLibraryUsage|CharacterSet|ManagedExtensions|WholeProgramOptimization");
    CString strToolName = _T("VCCLCompilerTool|VCLinkerTool|VCManifestTool|VCResourceCompilerTool|VCXDCMakeTool|VCBscMakeTool|VCPreBuildEventTool|VCPreLinkEventTool|VCPostBuildEventTool|VCCustomBuildTool|VCWebDeploymentTool");

    this->CStringToArray(strProjAttrib, m_arPnan);
    this->CStringToArray(strCfgAttrib, m_arCnan);
    this->CStringToArray(strToolName, m_arToolName);
    m_arMap.SetSize(m_arToolName.GetCount());

    //------------------------------------------------------------------------------------------------

    HINSTANCE hInstance  = AfxGetResourceHandle();
    HRSRC hResCompile = ::FindResource(hInstance, MAKEINTRESOURCE(IDR_COMPILE_XML_COMPILE), _T("XML"));
    char *pszData = (char*)LoadResource(hInstance, (HRSRC)hResCompile); // ����Դ�ĺ������'\0'

    if (m_xmlCompile.LoadXml(pszData))
    {
        m_CpRoot = m_xmlCompile.GetRoot();
        m_CpNameSpace = m_CpRoot.SelectSingleNode(_T("NameSpace"));
        m_CpConfig = m_CpRoot.SelectSingleNode(_T("Configurations//Configuration"));
        m_CpGlobal = m_CpRoot.SelectSingleNode(_T("Globals//Global"));

        int nIndex = 0;
        m_CpAttrib[nIndex++] = m_CpNameSpace.SelectNodes(_T("Attribute"));          // 0
        m_CpAttrib[nIndex++] = m_CpRoot.SelectNodes(_T("Attribute"));               // 1
        m_CpAttrib[nIndex++] = m_CpConfig.SelectNodes(_T("Attribute"));             // 2

        CXmlNode TempNode = m_CpConfig.SelectSingleNode(_T("VCCLCompilerTool"));    // 3
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Optimization//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Preprocessor//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Build//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Language//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Precompiled//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Output//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Browse//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Advanced//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCLinkerTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Input//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Manifest//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Debug//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("System//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Optimize//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("EmbeddedIDL//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Advanced//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCManifestTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("InputOutput//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("COM//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Advanced//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCResourceCompilerTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCXDCMakeTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCBscMakeTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Common//Attribute"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("CommandLine//Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCPreBuildEventTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCPreLinkEventTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCPostBuildEventTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCCustomBuildTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Attribute"));

        TempNode = m_CpConfig.SelectSingleNode(_T("VCWebDeploymentTool"));
        m_CpAttrib[nIndex++] = TempNode.SelectNodes(_T("Attribute"));

        TRACE(_T("����XML��Դ�ɹ�\n"));
    }
    else
    {
        TRACE(_T("����XML��Դ���ɹ�(%d)\n"), ::GetLastError());
    }
}

CXmlCompile::~CXmlCompile()
{
}

// ��VC��Ŀ�ļ�*.vcproj
// strFileName [IN] �ļ���
BOOL CXmlCompile::OpenVcProj(CString &strFileName)
{
    m_xmlVcProj.Close();

    if (m_xmlVcProj.Open(strFileName))
    {
        m_strPathFile = strFileName;
        m_VpRoot = m_xmlVcProj.GetRoot();
        m_VpPlatform = m_VpRoot.SelectNodes(_T("Platforms//Platform"));
        m_VpConfig = m_VpRoot.SelectNodes(_T("Configurations//Configuration"));
        m_VpGlobal = m_VpRoot.SelectSingleNode(_T("Globals//Global"));

        //----------------------------------------------------------------------------------------------
        // set platform node name

        CStringArray arTemp;
        CString strTemp = m_CpAttrib[0][(LONG)0]->GetAttribute(_T("XmlValue"));   // ƽ̨��

        this->GetPlatforms(m_arPfName);

        for (int i = 0; i < m_arPfName.GetCount(); i++)
        {
            strTemp += _T("|") + m_arPfName[i];
        }

        m_CpAttrib[0][(LONG)0]->SetAttribute(_T("XmlValue"), strTemp);    // д���µ�ƽ̨��

        //----------------------------------------------------------------------------------------------
        // set config node name

        strTemp = m_CpAttrib[0][1]->GetAttribute(_T("XmlValue"));   // ������

        this->GetConfigs(m_arCfgName, FALSE);

        for (int i = 0; i < m_arCfgName.GetCount(); i++)
        {
            strTemp += _T("|") + m_arCfgName[i];
        }

        m_CpAttrib[0][1]->SetAttribute(_T("XmlValue"), strTemp);    // д����������

        //----------------------------------------------------------------------------------------------
        // set project node attribute value

        for (int i = 0; i < m_arPnan.GetCount(); i++)
        {
            strTemp = this->GetRootAttribValue(m_arPnan[i]);
            if (_T("") == strTemp) strTemp = _T(" ");

            strTemp = m_CpAttrib[1][i]->GetAttribute(_T("XmlValue")) + _T("|") + strTemp;
            m_CpAttrib[1][i]->SetAttribute(_T("XmlValue"), strTemp);
        }

        //----------------------------------------------------------------------------------------------

        for (int i = 0; i < m_VpConfig.GetCount(); i++)
        {
            this->GetTools(i);  // get tool nodes

            //----------------------------------------------------------------------------------------------

            //TRACE(_T("ConfigIndex:%d NodeCount:%d StrCount:%d\n"), i, m_CpAttrib[2].GetCount(), m_arCnan.GetCount());

            for (int j = 0; j < m_arCnan.GetCount(); j++)
            {
                strTemp = this->GetConfigAttribValue(i, m_arCnan[j]);
                if (_T("") == strTemp) strTemp = _T(" ");

                strTemp = m_CpAttrib[2][j]->GetAttribute(_T("XmlValue")) + _T("|") + strTemp;
                m_CpAttrib[2][j]->SetAttribute(_T("XmlValue"), strTemp);
            }

            // set tool nodes attribute values

            int nIndex = 0;
            for (int j = 0; j < m_arToolName.GetCount(); j++)
            {
                for (int k = 0; k < m_arTnan[j].GetCount(); k++)
                {
                    this->CStringToArray(m_arTnan[j][k], arTemp);

                    //TRACE(_T("ToolIndex:%d SubIndex:%d NodeCount:%d StrCount:%d\n"),
                    //    j, k,
                    //    m_CpAttrib[3+nIndex+k].GetCount(),
                    //    arTemp.GetCount());

                    for (int l = 0; l < arTemp.GetCount(); l++)
                    {
                        strTemp = this->GetToolAttribValue(j, arTemp[l]);
                        if (_T("") == strTemp) strTemp = _T(" ");

                        strTemp = m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlValue")) + _T("|") + strTemp;
                        m_CpAttrib[3+nIndex+k][l]->SetAttribute(_T("XmlValue"), strTemp);
                        
                        // TRACE(_T("->%d %s=%s %s\n"), l,
                        // m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlName")),
                        // strTemp,
                        // m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("Select")));
                    }
                }

                nIndex += (int)m_arTnan[j].GetCount();
                //TRACE(_T("nIndex:%d\n"), nIndex);
            }
        }

        //----------------------------------------------------------------------------------------------

        m_CpGlobal.SetAttribute(_T("Value"), m_VpGlobal.GetAttribute(_T("Value")));

        //----------------------------------------------------------------------------------------------

        m_xmlVcProj.Close();

        //----------------------------------------------------------------------------------------------
        m_xmlCompile.Save(_T("out.xml"));
        //----------------------------------------------------------------------------------------------
 
        m_bLoadFile = TRUE;
    }
    else
    {
        m_bLoadFile = FALSE;
    }

    return m_bLoadFile;
}

// ����VCPROJ�ļ�
BOOL CXmlCompile::CreateVcProj(CString &strFileName)
{
    m_xmlVcProj.Close();

    CString strXML = _T("<?xml version=\"1.0\" encoding=\"gb2312\"?>\
<VisualStudioProject>\
<Platforms/>\
<ToolFiles/>\
<Configurations/>\
<References/>\
<Files/>\
<Globals>\
    <Global Name=\"RESOURCE_FILE\" Value=\"\" />\
</Globals>\
</VisualStudioProject>");

    if (m_xmlVcProj.LoadXml(strXML))
    {
        CStringArray arPlatform;
        CStringArray arAttrib;
        CStringArray arTemp;
        CString strTemp;

        m_VpRoot = m_xmlVcProj.GetRoot();
        m_VpPlatform = m_VpRoot.SelectNodes(_T("Platforms"));
        m_VpConfig = m_VpRoot.SelectNodes(_T("Configurations"));
        m_VpGlobal = m_VpRoot.SelectSingleNode(_T("Globals//Global"));

        //---------------------------------------------------------------

        strTemp = m_CpAttrib[0][(LONG)0]->GetAttribute(_T("XmlValue"), strTemp);    // ƽ̨��
        this->CStringToArray(strTemp, arPlatform);

        strTemp = m_CpAttrib[0][(LONG)1]->GetAttribute(_T("XmlValue"), strTemp);    // ������
        this->CStringToArray(strTemp, arTemp);

        for (int i = 1; i < arPlatform.GetCount(); i++)
        {
            m_VpPlatform[(LONG)0]->NewChild(_T("Platform"))->SetAttribute(_T("Name"), arPlatform[i]);   // ����ƽ̨

            for (int j = 1; j < arTemp.GetCount(); j++)
            {
                CXmlNode NewCfg = m_VpConfig[(LONG)0]->NewChild(_T("Configuration"));
                NewCfg.SetAttribute(_T("Name"), arTemp[j]+_T("|")+arPlatform[i]);

                for (int k = 0; k < m_arToolName.GetCount(); k++)
                {
                    NewCfg.NewChild(_T("Tool"))->SetAttribute(_T("Name"), m_arToolName[k]);
                }
            }
        }

        m_VpPlatform = m_VpRoot.SelectNodes(_T("Platforms//Platform"));
        m_VpConfig = m_VpRoot.SelectNodes(_T("Configurations//Configuration"));

        //---------------------------------------------------------------

        for (int i = 0; i < m_arPnan.GetCount(); i++)
        {
            strTemp = m_CpAttrib[1][i]->GetAttribute(_T("XmlValue"));
            this->CStringToArray(strTemp, arTemp);

            if (_T(" ") != arTemp[m_bLoadFile])
            {
                m_VpRoot.SetAttribute(m_arPnan[i], arTemp[m_bLoadFile]);
            }
        }

        //---------------------------------------------------------------

        for (int i = 0; i < m_VpConfig.GetCount(); i++)
        {
            for (int j = 0; j < m_arCnan.GetCount(); j++)   // ��������
            {
                strTemp = m_CpAttrib[2][j]->GetAttribute(_T("XmlValue"));
                this->CStringToArray(strTemp, arTemp);

                if (_T(" ") != arTemp[i+m_bLoadFile])
                {
                    m_VpConfig[i]->SetAttribute(m_arCnan[j], arTemp[i+m_bLoadFile]);
                }
            }

            this->GetTools(i);

            int nIndex = 0;

            for (int j = 0; j < m_arToolName.GetCount(); j++)
            {
                for (int k = 0; k < m_arTnan[j].GetCount(); k++)    // ��������
                {
                    this->CStringToArray(m_arTnan[j][k], arAttrib);

                    //TRACE(_T("ToolIndex:%d SubIndex:%d NodeCount:%d StrCount:%d\n"),
                    //    j, k,
                    //    m_CpAttrib[3+nIndex+k].GetCount(),
                    //    arAttrib.GetCount());

                    for (int l = 0; l < arAttrib.GetCount(); l++)
                    {
                        strTemp = m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlValue"));
                        this->CStringToArray(strTemp, arTemp);

                        if (_T(" ") != arTemp[i+1])
                        {
                            m_VpTool[j]->SetAttribute(arAttrib[l], arTemp[i+m_bLoadFile]);
                        }
                        // TRACE(_T("->%d %s=%s\n"), l, m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlName")), strTemp);
                    }
                }

                nIndex += (int)m_arTnan[j].GetCount();
                //TRACE(_T("nIndex:%d\n"), nIndex);
            }
        }

        //---------------------------------------------------------------

        m_VpGlobal.SetAttribute(_T("Value"), m_CpGlobal.GetAttribute(_T("Value")));

        m_xmlVcProj.Save(strFileName);
    }
    else
    {
        TRACE(_T("����XML��Դ���ɹ�(%d)\n"), ::GetLastError());
    }

    return FALSE;
}

// ����VCPROJ�ļ�
BOOL CXmlCompile::SaveVcProj(CString &strFileName)
{
    return m_xmlVcProj.Save(m_strPathFile);
}

// ����MAKE�ļ�
BOOL CXmlCompile::CreateMakeFile(CString &strFileName, int nCfg)
{
    // /Od /I "./src" /I "./" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /Gm /EHsc /RTC1 /MTd /Zc:wchar_t /Yu"stdafx.h" /Fp"Debug/TestSet.pch" /Fo"Debug/" /Fd"Debug/vc70.pdb" /W3 /c /Wp64 /ZI /TP
    // /Od /I "./src" /I "./" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /Gm /EHsc /RTC1 /MTd /Zc:wchar_t /Fo"Debug/" /Fd"Debug/vc70.pdb" /W3 /c /Wp64 /ZI /TC

    /*
    int i = nCfg;

    m_VpConfig[i]

    for (int j = 0; j < m_arCnan.GetCount(); j++)
    {
        strTemp = m_CpAttrib[2][j]->GetAttribute(_T("XmlValue"));
        this->CStringToArray(strTemp, arTemp);

        if (_T(" ") != arTemp[i+1])
        {
            m_VpConfig[i]->SetAttribute(m_arCnan[j], arTemp[i+1]);
        }
    }

    this->GetTools(i);

    int nIndex = 0;

    for (int j = 0; j < m_arToolName.GetCount(); j++)
    {
        for (int k = 0; k < m_arTnan[j].GetCount(); k++)
        {
            this->CStringToArray(m_arTnan[j][k], arAttrib);

            //TRACE(_T("ToolIndex:%d SubIndex:%d NodeCount:%d StrCount:%d\n"),
            //    j, k,
            //    m_CpAttrib[3+nIndex+k].GetCount(),
            //    arAttrib.GetCount());

            for (int l = 0; l < arAttrib.GetCount(); l++)
            {
                strTemp = m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlValue"));
                this->CStringToArray(strTemp, arTemp);

                if (_T(" ") != arTemp[i+1])
                {
                    m_VpTool[j]->SetAttribute(arAttrib[l], arTemp[i+1]);
                }
                // TRACE(_T("->%d %s=%s\n"), l, m_CpAttrib[3+nIndex+k][l]->GetAttribute(_T("XmlName")), strTemp);
            }
        }

        nIndex += (int)m_arTnan[j].GetCount();
        //TRACE(_T("nIndex:%d\n"), nIndex);
    }
*/

    return TRUE;
}

int CXmlCompile::CreateBatFile(void)
{
    return 0;
}

// �õ���һҳ�е����Ը���
int CXmlCompile::GetAttribCount(int nPage)
{
    return m_CpAttrib[nPage].GetCount();
}

// �õ�Name����
CString CXmlCompile::GetName(int nPage, int nIndex)
{
    return m_CpAttrib[nPage][nIndex]->GetAttribute(_T("Name"));
}

// �õ�Select����
CString CXmlCompile::GetSelect(int nPage, int nIndex)
{
    return m_CpAttrib[nPage][nIndex]->GetAttribute(_T("Select"));
}

// �õ�Explain����
CString CXmlCompile::GetExplain(int nPage, int nIndex)
{
    return m_CpAttrib[nPage][nIndex]->GetAttribute(_T("Explain"));
}

// �õ�XmlName����
CString CXmlCompile::GetXmlName(int nPage, int nIndex)
{
    return m_CpAttrib[nPage][nIndex]->GetAttribute(_T("XmlName"));
}

// �õ�XmlValue����
CString CXmlCompile::GetXmlValue(int nPage, int nIndex)
{
    return m_CpAttrib[nPage][nIndex]->GetAttribute(_T("XmlValue"));
}

// �õ�XmlValue����
void CXmlCompile::SetXmlValue(int nPage, int nIndex, CString strValue)
{
    m_CpAttrib[nPage][nIndex]->SetAttribute(_T("XmlValue"), strValue);
}

// ��"***|***|**"һ���������һ������
// strData [IN]  ���봮
// arrData [OUT] �������
void CXmlCompile::CStringToArray(CString &strData, CStringArray &arrData)
{
    int nStart = 0;

    arrData.RemoveAll();

    for (int i = 0; nStart != -1; i++)
    {
        CString strItem = strData.Tokenize(_T("|"), nStart);

        if (-1 != nStart)
        {
            arrData.Add(strItem);
            //TRACE(_T("%d %s\n"), i, strItem);
        }
    }

    //TRACE(_T("count:%d\n"), arrData.GetCount());
}

// �õ�������ֵ
// ptAttrib [IN] ������
CString CXmlCompile::GetRootAttribValue(LPCTSTR ptAttrib)
{
    return m_VpRoot.GetAttribute(ptAttrib);
}

// �õ����ýڵ�����ֵ
// nConfigIndex [IN] �������
// ptAttrib     [IN] ������
CString CXmlCompile::GetConfigAttribValue(int nConfigIndex, LPCTSTR ptAttrib)
{
    return m_VpConfig[nConfigIndex]->GetAttribute(ptAttrib);
}

// �õ�Tool�ڵ�����ֵ
// nNodeIndex [IN] Tool�ڵ����
// ptAttrib   [IN] ������
CString CXmlCompile::GetToolAttribValue(int nToolIndex, LPCTSTR ptAttrib)
{
    return m_VpTool[m_arMap[nToolIndex]]->GetAttribute(ptAttrib);
}

// �õ�ƽ̨��
void CXmlCompile::GetPlatforms(CStringArray &saPlatforms)
{
    saPlatforms.RemoveAll();

    for (int i = 0; i < m_VpPlatform.GetCount(); i++)
    {
        saPlatforms.Add(m_VpPlatform[i]->GetAttribute(_T("Name")));
    }
}

// �õ�������
void CXmlCompile::GetConfigs(CStringArray &saConfigs, BOOL bFull)
{
    BOOL bFind = FALSE;
    CString strTemp;

    saConfigs.RemoveAll();

    for (int i = 0; i < m_VpConfig.GetCount(); i++)
    {
        strTemp = m_VpConfig[i]->GetAttribute(_T("Name"));

        if (!bFull)
        {
            int nIndex = strTemp.Find(_T("|"));
            if (-1 != nIndex)
            {
                strTemp.Delete(nIndex, strTemp.GetLength());
            }

            bFind = FALSE;
            int nCount = (int)saConfigs.GetCount();
            for (int i = 0; i < nCount; i++)
            {
                if (0 == saConfigs[i].Compare(strTemp))
                {
                    bFind = TRUE;
                    break;
                }
            }

            if (!bFind)
            {
                saConfigs.Add(strTemp);
            }
        }
        else
        {
            saConfigs.Add(strTemp);
        }
    }
}

// �õ�Tool�ڵ�
void CXmlCompile::GetTools(int nConfigIndex)
{
    CString strValue;
    m_VpTool = m_VpConfig[nConfigIndex]->SelectNodes(_T("Tool"));

    for (int i = 0; i < m_arToolName.GetCount(); i++)
    {
        for (int j = 0; j < m_VpTool.GetCount(); j++)
        {
            if (m_VpTool[j]->GetAttribute(_T("Name")) == m_arToolName[i])
            {
                m_arMap[i] = j;
                //TRACE(_T("ToolMap %d->%d\n"), i, j);
                break;
            }
        }
    }
}