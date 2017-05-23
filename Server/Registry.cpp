// Server.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "math.h"

//Set the given key and its value
BOOL SetKeyAndValue(const TCHAR *pszPath, const TCHAR *pszSubkey, TCHAR * pszValue);

//Convert a CLSID into a char string
void CLSIDtoString(const CLSID &clsid, TCHAR *pszCLSID, int length);

//Delete szKeyChild and all of its descendents
LONG DeleteKey(HKEY hKeyParent, const TCHAR* pszKeyString);

//
//
//

//Size of a CLSID as a string
const int CLSID_STRING_SIZE

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
	HRESULT hr;
	MathClassFactory * pCF;
	if (rclsid != CLSID_Math)
		return E_FAIL;

	pCF = new MathClassFactory;
	if (pCF == 0)
		return E_OUTOFMEMORY;

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