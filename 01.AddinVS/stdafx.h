// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef STRICT
#define STRICT
#endif

// �������Ҫ��Ե�������ָ���汾��ƽ̨�����޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows 95 �� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define WINVER 0x0400		// ����ֵ����Ϊ�ʵ���ֵ���������� Windows 98 �� Windows 2000 �����߰汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ�ʵ���ֵ���������� Windows 2000 ����߰汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0400	// ����ֵ����Ϊ�ʵ���ֵ���������� IE 5.0 ����°汾��
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� ATL ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _ATL_ALL_WARNINGS


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>

#pragma warning( disable : 4278 )
#pragma warning( disable : 4146 )
	//����� #import ���� IDTExtensibility2 �ӿ�
	#import <MSADDNDR.DLL> raw_interfaces_only named_guids

	//����� #import ���� VS ������
	#import <vsmso.olb> raw_interfaces_only named_guids

	//����� #import ���� DTE
	#import <dte80a.olb> raw_interfaces_only named_guids rename("ULONG_PTR","ULONG_PTRDTE") // XT+20131024 ��tlhelp32.h�Ķ����ͻ

	//����� #import ���� DTE80
	#import <dte80.olb> raw_interfaces_only named_guids

	//����� #import ���� DTE90
	#import <dte90.olb> raw_interfaces_only named_guids

	//����� #import ���� DTE90a
	#import <dte90a.olb> raw_interfaces_only named_guids

	//����� #import ���� DTE100
	#import <dte100.olb> raw_interfaces_only named_guids
#pragma warning( default : 4146 )
#pragma warning( default : 4278 )

#define IfFailGo(x) { hr=(x); if (FAILED(hr)) goto Error; }
#define IfFailGoCheck(x, p) { hr=(x); if (FAILED(hr)) goto Error; if(!p) {hr = E_FAIL; goto Error; } }

class DECLSPEC_UUID("4D6DEBA5-C684-44D4-BB0E-3D29980537A1") MyAddinLib;

using namespace ATL;

class CAddInModule : public CAtlDllModuleT< CAddInModule >
{
public:
	CAddInModule()
	{
		m_hInstance = NULL;
	}

	DECLARE_LIBID(__uuidof(MyAddinLib))

	inline HINSTANCE GetResourceInstance()
	{
		return m_hInstance;
	}

	inline void SetResourceInstance(HINSTANCE hInstance)
	{
		m_hInstance = hInstance;
	}

private:
	HINSTANCE m_hInstance;
};

extern CAddInModule _AtlModule;