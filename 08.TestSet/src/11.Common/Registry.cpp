//
// Registry.cpp
// 功能说明：访问系统注册表
//


#include "StdAfx.h"
#include "Registry.h"

#pragma warning(disable : 4267)

#define KEY_PATH_OLD	_T("Software\\AnyFilms\\AnyFilms\\%s")
#define KEY_PATH		_T("Software\\VerySee\\VerySee")
#define KEY_PATH_EX		_T("Software\\VerySee\\VerySee\\%s")
//////////////////////////////////////////////////////////////////////
// CRegistry construction

CRegistry::CRegistry()
{

}

CRegistry::~CRegistry()
{

}

//////////////////////////////////////////////////////////////////////
// CRegistry read a string value

CString CRegistry::GetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszDefault)
{
	CString strKey, strValue;
	DWORD nErrorCode;
	HKEY hKey;
	
	if (pszDefault != NULL) strValue = pszDefault;
	strKey.Format(KEY_PATH_EX, pszSection);
	
	nErrorCode = RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_READ, &hKey);

	if (nErrorCode == ERROR_SUCCESS)
	{
		DWORD nType = 0, nSize = 0;
		
		nErrorCode = RegQueryValueEx(hKey, pszName, 0, &nType, NULL, &nSize); 

		if (nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof(TCHAR))
		{
			LPTSTR pszValue = strValue.GetBuffer(nSize / sizeof(TCHAR) - 1);
			nErrorCode = RegQueryValueEx(hKey, pszName, 0, &nType, (PBYTE)pszValue, &nSize); 
			strValue.ReleaseBuffer(nSize / sizeof(TCHAR) - 1);
		}
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode != ERROR_SUCCESS) DisplayErrorMessageBox(nErrorCode);
	
	return strValue;
}

CString CRegistry::GetPath(void)
{
	CString strValue;
	DWORD nErrorCode;
	HKEY hKey;
	
	nErrorCode = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, &hKey);

	if (nErrorCode == ERROR_SUCCESS)
	{
		DWORD nType = 0, nSize = 0;
		
		nErrorCode = RegQueryValueEx(hKey, _T("UserPath"), 0, &nType, NULL, &nSize); 

		if (nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof(TCHAR))
		{
			LPTSTR pszValue = strValue.GetBuffer(nSize / sizeof(TCHAR) - 1);
			nErrorCode = RegQueryValueEx(hKey, _T("UserPath"), 0, &nType, (PBYTE)pszValue, &nSize); 
			strValue.ReleaseBuffer(nSize / sizeof(TCHAR) - 1);
		}
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode != ERROR_SUCCESS) DisplayErrorMessageBox(nErrorCode);
	
	return strValue;
}

//////////////////////////////////////////////////////////////////////
// CRegistry read an integer value

int CRegistry::GetInt(LPCTSTR pszSection, LPCTSTR pszName, int nDefault)
{
	int nValue = nDefault;
	DWORD nErrorCode;
	CString strKey;
	HKEY hKey;
	
	strKey.Format(KEY_PATH_EX, pszSection);
	
	nErrorCode = RegOpenKeyEx(HKEY_CURRENT_USER, strKey, 0, KEY_READ, &hKey);
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		DWORD nType = 0, nSize = sizeof(nValue);
		
		nErrorCode = RegQueryValueEx(hKey, pszName, 0, &nType, (PBYTE)&nValue, &nSize); 
		
		if (nType != REG_DWORD || nSize != sizeof(nValue))
		{
			nErrorCode = ERROR_MORE_DATA;
			nValue = nDefault;
		}
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode != ERROR_SUCCESS) DisplayErrorMessageBox(nErrorCode);
	
	return nValue;
}

DWORD CRegistry::GetDword(LPCTSTR pszSection, LPCTSTR pszName, DWORD dwDefault)
{
	return (int)GetInt(pszSection, pszName, (int)dwDefault);
}

//////////////////////////////////////////////////////////////////////
// CRegistry read a float value

double CRegistry::GetFloat(LPCTSTR pszSection, LPCTSTR pszName, double fDefault)
{
	double fValue = fDefault;
	_stscanf(GetString(pszSection, pszName), _T("%lf"), &fValue);
	return fValue;
}

//////////////////////////////////////////////////////////////////////
// CRegistry write a string value

BOOL CRegistry::SetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue)
{
	DWORD nErrorCode;
	CString strKey;
	HKEY hKey;
	
	strKey.Format(KEY_PATH_EX, pszSection);
	
	nErrorCode = RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	
	if ( nErrorCode == ERROR_SUCCESS )
	{
		nErrorCode = RegSetValueEx(hKey, pszName, 0, REG_SZ, (const BYTE*)pszValue,
								_tcslen(pszValue) * sizeof(TCHAR) + sizeof(TCHAR));
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		DisplayErrorMessageBox(nErrorCode);
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// CRegistry write an int value

BOOL CRegistry::SetInt(LPCTSTR pszSection, LPCTSTR pszName, int nValue)
{
	DWORD nErrorCode;
	CString strKey;
	HKEY hKey;
	
	strKey.Format(KEY_PATH_EX, pszSection);
	
	nErrorCode = RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		nErrorCode = RegSetValueEx(hKey, pszName, 0, REG_DWORD,	(const BYTE*)&nValue,sizeof(nValue));
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		DisplayErrorMessageBox(nErrorCode);
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
//write a new int value to change the formal in realplayer
BOOL CRegistry::SetRealInt(LPCTSTR pszSection, LPCTSTR pszName, int nValue)
{
	DWORD nErrorCode;
	CString strKey;
	HKEY hKey;
	
	strKey.Format(_T("Software\\RealNetworks\\RealMediaSDK\\6.0\\Preferences\\%s"), pszSection);
	
	nErrorCode = RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	
	if (nErrorCode == ERROR_SUCCESS )
	{
		nErrorCode = RegSetValueEx(hKey, pszName, 0, REG_SZ, (const BYTE*)&nValue, sizeof(nValue));

		RegCloseKey(hKey);
	}
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		DisplayErrorMessageBox(nErrorCode);
		return FALSE;
	}
}

BOOL CRegistry::SetRealString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue)
{
	DWORD nErrorCode;
	CString strKey;
	HKEY hKey;
	
	strKey.Format(_T("Software\\RealNetworks\\RealMediaSDK\\6.0\\Preferences\\%s"), pszSection);
	
	nErrorCode = RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		nErrorCode = RegSetValueEx(hKey, pszName, 0, REG_SZ, (const BYTE*)pszValue,	_tcslen(pszValue) * sizeof(TCHAR) + sizeof(TCHAR));
		
		RegCloseKey(hKey);
	}
	
	if (nErrorCode == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		DisplayErrorMessageBox(nErrorCode);
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// Helper function to display a message box holding an error code

void CRegistry::DisplayErrorMessageBox(DWORD nErrorCode)
{
/*
#ifdef _DEBUG
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, nErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,	0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
	LocalFree(lpMsgBuf);
#endif
*/
}
