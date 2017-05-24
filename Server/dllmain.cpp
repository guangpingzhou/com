// dllmain.cpp : 定义 DLL 应用程序的入口点。
/*
http://blog.csdn.net/wxtsmart/article/details/2988904
http://blog.csdn.net/woshinia/article/details/22300089
http://blog.csdn.net/henry000/article/details/7008397
http://blog.csdn.net/wxtsmart/article/details/2988904
*/

#include "stdafx.h"
#include "math.h"

//Used to store the instance handle
//We need this to call the GetModuleFileName API
HINSTANCE g_hModule = NULL;
long g_lObjs = 0;
long g_lLocks = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

static BOOL SetRegKeyValue(
	LPTSTR pszKey,
	LPTSTR pszSubkey,
	LPTSTR pszValue)
{
	BOOL bOk = FALSE;
	LONG ec;
	HKEY hKey;
	TCHAR szKey[128];
	lstrcpy(szKey, pszKey);
	if (NULL != pszSubkey)
	{
		lstrcat(szKey, L"\\");
		lstrcat(szKey, pszSubkey);
	}
	ec = RegCreateKeyEx(
		HKEY_CLASSES_ROOT,
		szKey,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL);
	if (ERROR_SUCCESS == ec)
	{
		if (NULL != pszValue)
		{
			ec = RegSetValueEx(
				hKey,
				NULL,
				0,
				REG_SZ,
				(BYTE*)pszValue,
				(lstrlen(pszValue) + 1) * sizeof(TCHAR));
		}
		if (ERROR_SUCCESS == ec)
			bOk = TRUE;
		RegCloseKey(hKey);
	}
	return bOk;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void ** ppv)
{
	HRESULT hr;
	MathClassFactory * pCF;
	if (rclsid != CLSID_Math)
		return(E_FAIL);
	pCF = new MathClassFactory;
	if (pCF == 0)
		return(E_OUTOFMEMORY);
	hr = pCF->QueryInterface(riid, ppv);
	if (FAILED(hr))
		delete pCF;
	return hr;
}

STDAPI DllCanUnloadNow(void)
{
	if (g_lObjs || g_lLocks)
		return(S_FALSE);
	else
		return(S_OK);
}
