// Server.cpp : 定义 DLL 应用程序的导出函数。
//
#include <assert.h>

#include "stdafx.h"
#include "math.h"

/*
四个重要的引出函数的实现，分别是
DllRegisterServer,用于注册本Dll，由regsvr32.exe调用
DllUnregisterServer，用于反注册dll，由regsver32.exe -u调用
DllCanUnloanow，用于判断是否可以卸载本组件，由CoFreeUnusedLibraries函数调用
DllGetClassObject，用于创建类厂并返回所需接口，由CoGetClassObject函数调用
*/


//Set the given key and its value
BOOL SetKeyAndValue(const TCHAR *szPath, const TCHAR *szSubkey, const TCHAR * szValue);

//Convert a CLSID into a char string
void CLSIDtoString(const CLSID &clsid, TCHAR *szCLSID, int length);

//Delete szKeyChild and all of its descendents
LONG DeleteKey(HKEY hKeyParent, const TCHAR* szKeyString);

//
//
//

//Size of a CLSID as a string
const int CLSID_STRING_SIZE = 39;

//Register the component in the registry.
HRESULT RegisterServer(const CLSID &clsid, 
					const TCHAR *szFileName, 
					const TCHAR *szProgID, 
					const TCHAR *szDescription, 
					const TCHAR *szVerIndProgID)
{
	//Convert the CLSID into a char
	TCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtoString(clsid, szCLSID, sizeof(szCLSID));

	//Build the key CLSID
	TCHAR szKey[64];
	lstrcpy(szKey, L"CLSID\\");
	lstrcat(szKey, szCLSID);

	//Add the server filename subkey under the CLSID key
	SetKeyAndValue(szKey, L"InprocServer32", szFileName);

	//Add the ProgID subkey under the CLSID key.
	if (szProgID != NULL)
	{
		SetKeyAndValue(szKey, L"ProgID", szProgID);
		SetKeyAndValue(szKey, L"CLSID", szCLSID);
	}

	if (szVerIndProgID)
	{
		SetKeyAndValue(szKey, L"VersionIndepentProgID", szVerIndProgID);

		// Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT.
		SetKeyAndValue(szVerIndProgID, NULL, szDescription);
		SetKeyAndValue(szVerIndProgID, L"CLSID", szCLSID);
		SetKeyAndValue(szVerIndProgID, L"CurVer", szProgID);

		// Add the versioned ProgID subkey under HKEY_CLASSES_ROOT.
		SetKeyAndValue(szProgID, NULL, szDescription);
		SetKeyAndValue(szProgID, L"CLSID", szCLSID);
	}

	return S_OK;
}

//
// Remove the component from the registry.
//
HRESULT UnregisterServer(const CLSID& clsid,      // Class ID
	const TCHAR* szProgID,       //   IDs
	const TCHAR* szVerIndProgID) // Programmatic
{
	// Convert the CLSID into a char.
	TCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtoString(clsid, szCLSID, sizeof(szCLSID));
	//StringFromCLSID(clsid,szCLSID);
	// Build the key CLSID//{...}
	TCHAR szKey[64];
	lstrcpy(szKey, L"CLSID//");
	lstrcat(szKey, szCLSID);

	// Delete the CLSID Key - CLSID/{...}
	LONG lResult = DeleteKey(HKEY_CLASSES_ROOT, szKey);

	// Delete the version-independent ProgID Key.
	if (szVerIndProgID != NULL)
		lResult = DeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);

	// Delete the ProgID key.
	if (szProgID != NULL)
		lResult = DeleteKey(HKEY_CLASSES_ROOT, szProgID);

	return S_OK;
}

// Convert a CLSID to a char string.
void CLSIDtoString(const CLSID &clsid,
			TCHAR *szCLSID,
			int length)
{
	assert(length >= CLSID_STRING_SIZE);
	// Get CLSID
	LPOLESTR wszCLSID = NULL;
	HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
	assert(SUCCEEDED(hr));
	lstrcpy(szCLSID, wszCLSID);

	// Free memory.
	CoTaskMemFree(wszCLSID);
}

//
// Delete a key and all of its descendents.
//
LONG DeleteKey(HKEY hKeyParent,           // Parent of key to delete
			const TCHAR* lpszKeyChild)  // Key to delete
{
	// Open the child.
	HKEY hKeyChild;
	LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyChild, 0,
		KEY_ALL_ACCESS, &hKeyChild);
	if (lRes != ERROR_SUCCESS)
	{
		return lRes;
	}

	// Enumerate all of the decendents of this child.
	FILETIME time;
	TCHAR szBuffer[256];
	DWORD dwSize = 256;
	while (RegEnumKeyEx(hKeyChild, 0, szBuffer, &dwSize, NULL,
		NULL, NULL, &time) == S_OK)
	{
		// Delete the decendents of this child.
		lRes = DeleteKey(hKeyChild, szBuffer);
		if (lRes != ERROR_SUCCESS)
		{
			// Cleanup before exiting.
			RegCloseKey(hKeyChild);
			return lRes;
		}
		dwSize = 256;
	}

	// Close the child.
	RegCloseKey(hKeyChild);

	// Delete this child.
	return RegDeleteKey(hKeyParent, lpszKeyChild);
}

// Create a key and set its value.
//
BOOL SetKeyAndValue(const TCHAR* szKey,
	const TCHAR* szSubkey,
	const TCHAR* szValue)
{
	HKEY hKey;
	TCHAR szKeyBuf[1024];

	// Copy keyname into buffer.
	lstrcpy(szKeyBuf, szKey);

	// Add subkey name to buffer.
	if (szSubkey != NULL)
	{
		lstrcat(szKeyBuf, L"//");
		lstrcat(szKeyBuf, szSubkey);
	}

	// Create and open key and subkey.
	long lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szKeyBuf,
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL,
		&hKey, NULL);
	if (lResult != ERROR_SUCCESS)	
		return FALSE;

	// Set the Value.
	if (szValue != NULL)
	{
		RegSetValueEx(hKey, NULL, 0, REG_SZ,
			(BYTE *)szValue,
			2 * lstrlen(szValue) + 1);
	}

	RegCloseKey(hKey);
	return TRUE;
}

