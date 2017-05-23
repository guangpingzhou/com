// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "math.h"

//Our component category GUID
#include "ATLDevGuide.h"

//Used to store the instance handle
//We need this to call the GetModuleFileName API
HINSTANCE g_hinstDLL = 0;
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
		g_hinstDLL = hModule;
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
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = 0;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICatRegister,
		(void**)&pcr);
	if (FAILED(hr))
		return hr;
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409;
	int len = wcslen(catDescription);
	wcsncpy(catinfo.szDescription, catDescription, wcslen(catDescription));
	catinfo.szDescription[len] = '\0';
	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();
	return hr;
}

HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categories information.
	ICatRegister* pcr = 0;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICatRegister,
		(void**)&pcr);
	if (SUCCEEDED(hr))
	{
		CATID rgcatid[1];
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
		pcr->Release();
	}
	return hr;
}

STDAPI DllRegisterServer(void)
{
	HRESULT hr = NOERROR;
	CHAR szModulePath[MAX_PATH];
	CHAR szID[128];
	CHAR szCLSID[128];
	WCHAR wszID[128];
	WCHAR wszCLSID[128];
	GetModuleFileName(
		g_hinstDLL,
		szModulePath,
		sizeof(szModulePath) / sizeof(CHAR));

	StringFromGUID2(CLSID_Math, wszID, sizeof(wszID));
	wcscpy(wszCLSID, L"CLSID\\");
	wcscat(wszCLSID, wszID);
	wcstombs(szID, wszID, sizeof(szID));
	wcstombs(szCLSID, wszCLSID, sizeof(szID));
	//Create the ProgID keys
	SetRegKeyValue(
		"Chapter2.Math.1",
		NULL,
		"Chapter2 Math Component");
	SetRegKeyValue(
		"Chapter2.Math.1",
		"CLSID",
		szID);
	//Create version independent ProgID keys
	SetRegKeyValue(
		"Chapter2.Math",
		NULL,
		"Chapter2 Math Component");
	SetRegKeyValue(
		"Chapter2.Math",
		"CurVer",
		"Chapter2.Math.1");
	SetRegKeyValue(
		"Chapter2.Math",
		"CLSID",
		szID);
	//Create entries under CLSID
	SetRegKeyValue(
		szCLSID,
		NULL,
		"Chapter 2 Math Component");
	SetRegKeyValue(
		szCLSID,
		"ProgID",
		"Chapter2.Math.1");
	SetRegKeyValue(
		szCLSID,
		"VersionIndependentProgID",
		"Chapter2.Math");
	SetRegKeyValue(
		szCLSID,
		"InprocServer32",
		szModulePath);
	//Register our component category
	CreateComponentCategory(CATID_ATLDevGuide,
		L"ATL Developer's Guide Examples");
	RegisterCLSIDInCategory(CLSID_Math, CATID_ATLDevGuide);
	return S_OK;
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

