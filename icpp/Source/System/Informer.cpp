// Informer.cpp: implementation of the CInformer class.
// develop by artbear
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\\SetOfHierarchy.h"
#include "Informer.h"

#include "../Utils/StringAlgo.h"
//_____________________________________________________________________________
//
// CInformer Maps
//_____________________________________________________________________________
//
BEGIN_BL_METH_MAP(CInformer)
    BL_METH_FUNC("GetMethodsTable", "����������������������", 1, &funcGetMethodsTable)
    BL_METH_FUNC("GetPropsTable", "����������������������", 1, &funcGetPropsTable)

    BL_METH_FUNC("MethodExists", "���������������", 2, &funcHaveContextMethod)
    BL_METH_FUNC("PropExists", "������������������", 2, &funcHaveContextProp)

	// ����������������(������������������������������������, ����������������������������������)
    BL_METH_FUNC("TheObjectIs", "����������������", 2, &funcTheObjectIs)

	// ����������������(�����������������, ����������������������������������)
    BL_METH_FUNC("IsFunction", "����������������", 2, &funcIsFunction)

    BL_METH_FUNC("CommandLine",		"���������������",	0, &funcCommandLine)
    BL_METH_FUNC("GetAppPath",		"��������������1�",	0, &funcGetAppPath)

    BL_METH_FUNC("GetInternalTypeName",		"�������������������",	1, &funcGetInternalTypeName)

    BL_METH_FUNC("GetIdleTimeOfSystem",		"���������������������������",	0, &funcGetIdleTime)

    BL_METH_FUNC("GetAddInDllTable",		"����������������������������",	0, &funcGetAddInDllTable)

	BL_METH_FUNC("GetCurrentModuleText",		"���������������������������",	0, &funcGetCurrentModuleText)
	BL_METH_FUNC("GetModuleText",		"�������������������",	1, &funcGetModuleText)

END_BL_METH_MAP()

BEGIN_BL_PROP_MAP(CInformer)
END_BL_PROP_MAP()

IMPLEMENT_MY_CREATE_CONTEXT(CInformer, "Informer", "����������");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CInformer::funcGetMethodsTable(CValue &RetVal, CValue **ppValue)
{
	GetMethodsTable(*ppValue[0], RetVal);

	return TRUE;
}

BOOL CInformer::funcGetPropsTable(CValue &RetVal, CValue **ppValue)
{
	GetPropsTable(*ppValue[0], RetVal);

	return TRUE;
}

bool CInformer::GetMethodsTable(const CValue& ContextValue, CValue& RetVal)
{
	return GetTableFromContext(true, ContextValue, RetVal);
}

bool CInformer::GetPropsTable(const CValue& ContextValue, CValue& RetVal)
{
	return GetTableFromContext(false, ContextValue, RetVal);
}

bool CInformer::GetTableFromContext(bool bNeedMethods, const CValue& ContextValue, CValue& VTValue)
{
	CBLContext* pCont = ContextValue.GetContext();

	CValueTable* pVT = NULL;
	if (VTValue.IsEmpty())
	{
		VTValue.CreateObject("���������������");
		pVT = (static_cast<CValueTableContextData*>(VTValue.GetContext()->GetInternalData()))->GetValueTable();
	}
	else
		pVT = CValue2VT(VTValue);

	if (pCont)
		return GetTableFromContext(bNeedMethods, pCont, pVT);

	return false;
}

bool CInformer::GetTableFromContext(bool bNeedMethods, CBLContext* pCont, CValueTable* pVT)
{
	if (!pVT)
		return false;

	CString str;
	const CType ctNum(NUMBER_TYPE_1C);
	const CType ctStr(STRING_TYPE_1C);

	const int iColCount = 5;
	std::vector<CValue> pv(iColCount);
	std::vector<CValue*> ppv(iColCount);
	for (int i = 0; i < iColCount; ++i)
		ppv[i] = &pv[i];

	pVT->Clear(TRUE);
	pVT->AddColumn(str = "Number",		ctNum, str, 0, "", 0);
	pVT->AddColumn(str = "NameEng",	ctStr, str, 0, "", 0);
	pVT->AddColumn(str = "NameRus",	ctStr, str, 0, "", 0);
	if (bNeedMethods)
	{
		pVT->AddColumn(str = "HasRetVal",	ctNum, str, 0, "", 0);
		pVT->AddColumn(str = "NParams",	ctNum, str, 0, "", 0);
	}
	else
	{
		pVT->AddColumn(str = "IsReadable",	ctNum, str, 0, "", 0);
		pVT->AddColumn(str = "IsWritable",	ctNum, str, 0, "", 0);
	}

	if (bNeedMethods)
		for (int i = 0; i < pCont->GetNMethods(); ++i)
		{
			pv[0] = static_cast<long>(i);
			pv[1] = pCont->GetMethodName(i, 0);
			pv[2] = pCont->GetMethodName(i, 1);
			pv[3] = static_cast<long>(pCont->HasRetVal(i));
			pv[4] = pCont->GetNParams(i);
			pVT->AddRow(&ppv[0], -1);
		}
	else
		for (int i = 0; i < pCont->GetNProps(); ++i)
		{
			pv[0] = static_cast<long>(i);
			pv[1] = pCont->GetPropName(i, 0);
			pv[2] = pCont->GetPropName(i, 1);
			pv[3] = static_cast<long>(pCont->IsPropReadable(i));
			pv[4] = static_cast<long>(pCont->IsPropWritable(i));
			pVT->AddRow(&ppv[0], -1);
		}
	//}

	return true;
}

void CInformer::GetMethodOfContext(CValue * * ppValue)
{
	CBLContext* pCont = ppValue[0]->GetContext();

	if (pCont)
	{
		CValueTable *pVTm = CValue2VT(*ppValue[1]);
		CValueTable *pVTp = CValue2VT(*ppValue[2]);
		if (pVTm || pVTp)
		{
			if (pVTm)
				GetMethodsTable(*ppValue[0], *ppValue[1]);

			if (pVTp)
				GetPropsTable(*ppValue[0], *ppValue[2]);
		}
		else
			DumpContext(pCont);
	}
}

////<-CSubcContext(D4/8Ch)<-CSubcContextDef(48h/28h)<-CBLContext(20h/1Ch)<-CObject(4/4) - artbear
//class CSubcContext : public CSubcContextDef
//{
//public:
//	DWORD m_flag28; // 28h
//	DWORD m_flag2�; // 2�h
//	DWORD m_flag30; // 30h
//	DWORD m_buf34[5]; // 34h
//	CObjID m_ObjID1; // 48h
//	CObjID m_ObjID2; //50h
//	DWORD m_flag58; // 58h - � ������������ = -1
//	DWORD m_flag5�; // 5�h - � ������������ = -1
//	DWORD m_buf60[4]; // 60h
//	CString m_string1; // 70h - � ������������ = "WindowName"
//	CString m_string2; // 74h - � ������������ = "WindowName"
//	DWORD m_buf78[3]; // 78h
//	CSbCntSet* m_pSbCntSet; // 84h
//	DWORD m_flag88; // 84h
//};

BOOL CInformer::HaveContextMethodOrProp(bool bNeedMethods, CValue &RetVal, CValue **ppValue)
{
	CBLContext* pCont = ppValue[0]->GetContext();
	if (!pCont)
		if (bNeedMethods)
			RuntimeError("����������::��������������� - ������ ���������� ����� �������� ��������");
		else
			RuntimeError("����������::������������������ - ������ ���������� ����� �������� ��������");


	CValue& ValueParam2 = *ppValue[1];

	if (2 != ValueParam2.GetTypeCode())
		if (bNeedMethods)
			RuntimeError("����������::��������������� - ������ ���������� ����� �������� �������� ������ (������)");
		else
			RuntimeError("����������::������������������ - ������ ���������� ����� �������� �������� ������ (������)");

	int iMethodNum = -1;
	if (bNeedMethods)
		iMethodNum = pCont->FindMethod(ValueParam2.GetString());
	else
		iMethodNum = pCont->FindProp(ValueParam2.GetString());

	RetVal = CNumeric(-1 != iMethodNum? 1 : 0);

	return TRUE;
}

BOOL CInformer::funcHaveContextMethod(CValue &RetVal, CValue **ppValue)
{
	return HaveContextMethodOrProp(true, RetVal, ppValue);
}

BOOL CInformer::funcHaveContextProp(CValue &RetVal, CValue **ppValue)
{
	return HaveContextMethodOrProp(false, RetVal, ppValue);
}

// ����������������(������������������������������������, ����������������������������������)
BOOL CInformer::funcTheObjectIs(CValue &RetVal, CValue **ppValue)
{
	RetVal = CNumeric(0);

	CValue& valParam1 = *ppValue[0];
	CValue& valParam2 = *ppValue[1];

	if (2 != valParam2.GetTypeCode())
	{
		return true; // TODO ��������, ����� ����������
	}
	CString strPar2 = valParam2.GetString();
	if (strPar2.IsEmpty())
	{
		//CBLModule::RaiseExtRuntimeError("� ����� ���������� ���������� ��� ������",0);
		return true; // TODO ��������, ����� ����������
	}

	CString strPar1;
	int iTypeCodeParam1 = valParam1.GetTypeCode();

	bool bIsString = 2 == iTypeCodeParam1;
	if (bIsString)
	{

		strPar1 = valParam1.GetString();
		if (strPar1.IsEmpty())
		{
			//CBLModule::RaiseExtRuntimeError("� ����� ���������� ���������� ��� ������",0);
			return true; // TODO ��������, ����� ����������
		}
	}
	else // if (bIsString)
	{
		if (100 != iTypeCodeParam1)
			RuntimeError("������ ���������� ���������� ���������� ���������� ������ ��� ������-������������ ����");

		CBLContext* pCont = valParam1.GetContext();
		if (!pCont)
			return true;

		strPar1 = pCont->GetTypeString();
	}

	if (!strPar1.CompareNoCase(strPar2) || ISetOfHierarchy::GetHierarchy()->TheClassIsDerive(strPar1, strPar2))
		RetVal = CNumeric(1);

	return true;
}

BOOL CInformer::funcIsFunction(CValue &RetVal, CValue **ppValue)
{
	RetVal = CNumeric(0); // ���������� ����������

	CBLContext* pContext = ppValue[0]->GetContext();

	if (!pContext)
	{
		return TRUE;
	}
	CValue& valParam2 = *ppValue[1];

	if (2 != valParam2.GetTypeCode())
	{
		RuntimeError("������ ���������� ���������� ���������� ������-������������ ������");
	}

	CString strPar2 = valParam2.GetString();
	if (strPar2.IsEmpty())
	{
		RuntimeError("������ ���������� ���������� ���������� ������-������������ ������");
	}

	int iNumMethod = pContext->FindMethod(strPar2);
	if (iNumMethod != -1)
	{
 		RetVal = CNumeric(pContext->HasRetVal(iNumMethod));
// 		int iRes = pContext->HasRetVal(iNumMethod);
// 		RetVal = CNumeric(iRes);
	}

	return TRUE;
}

// �������� ��������� ������ 1� (����� �� FormEx)
BOOL  CInformer::funcCommandLine(CValue& RetVal, CValue **params)
{
	CApp7* pApp7 = static_cast<CApp7*>(AfxGetApp());
	RetVal = pApp7->m_lpCmdLine;

			/*
			CApp7* pApp7 = static_cast<CApp7*>(AfxGetApp());
			CProfile7* pProfile7 = pApp7->GetProps();

			CString Prop0 = pProfile7->GetStringProp(0x00); // �������, � ������� ���������� ������
			CString Prop1 = pProfile7->GetStringProp(0x01); // ������� ����
			CString Prop2 = pProfile7->GetStringProp(0x02); // ������� ������������
			CString Prop3 = pProfile7->GetStringProp(0x03); // ��������� �������
			CString Prop4 = pProfile7->GetStringProp(0x04); // ������ ���� � ����� 1Cv7.CFG (� ������ �����)
			CString Prop5 = pProfile7->GetStringProp(0x05); // ������ ���� � ����� 1Cv7.DD (� ������ �����)
			CString Prop6 = pProfile7->GetStringProp(0x06); // ������ ���� � ����� 1Cv7.DDS (� ������ �����)
			CString Prop7 = pProfile7->GetStringProp(0x07); // ������ ���� � ����� 1Cv7.DBA (� ������ �����)
			CString Prop8 = pProfile7->GetStringProp(0x08); // ������ ���� � ����� 1Cv7.MD (� ������ �����)
			CString Prop9 = pProfile7->GetStringProp(0x09); // ������ ���� � ����� 1Cv7.LCK (� ������ �����)
			CString PropA = pProfile7->GetStringProp(0x0A); // ???
			CString PropB = pProfile7->GetStringProp(0x0B); // ???
			CString PropC = pProfile7->GetStringProp(0x0C); // ��� ������� �� � ������ ���
			CString PropD = pProfile7->GetStringProp(0x0D); // ��� �������� ������������
			CString PropE = pProfile7->GetStringProp(0x0E); // ������ ��� �������� ������������
			CString PropF = pProfile7->GetStringProp(0xF); // ???
			CString Prop10 = pProfile7->GetStringProp(0x10); // �������� ������ ���� �������� ������������
			CString Prop11 = pProfile7->GetStringProp(0x11); // �������� ���������� �������� ������������
			CString Prop12 = pProfile7->GetStringProp(0x12); // ���� � ������� � ���������� �������� ������������ (������������ HKEY_CURRENT_USER\Software\1C\1Cv7\7.7)
			*/
/*

		LPCTSTR m_pszAppName = pApp7->m_pszAppName;  // human readable name
									//  (from constructor or AFX_IDS_APP_TITLE)
		LPCTSTR m_pszExeName = pApp7->m_pszExeName;       // executable name (no spaces)
		LPCTSTR m_pszHelpFilePath = pApp7->m_pszHelpFilePath;  // default based on module path
		LPCTSTR m_pszProfileName = pApp7->m_pszProfileName;   // default based on app name

		m_pszAppName <1�:�����������>
		m_pszExeName <1CV7>
		m_pszHelpFilePath <C:\Program Files\1Cv77\Bin25\BIN\1CV7.HLP>
		m_pszProfileName <1Cv7\7.7\Testing 1C++\V7\Default>
		sAppPath <C:\Program Files\1Cv77\Bin25\BIN\1CV7.exe>
*/

	return true;
}

BOOL  CInformer::funcGetAppPath(CValue& RetVal, CValue **params)
{
	CApp7* pApp7 = static_cast<CApp7*>(AfxGetApp());
	CProfile7* pProfile7 = pApp7->GetProps();
	CString sAppPath = pProfile7->GetStringProp(0x00) + pApp7->m_pszExeName + ".exe";

	RetVal = sAppPath;
	return true;
}

// �������� ���������� ��� �������� ��� ��������� (�� RuntimeClass)
// ��������, ��� ������� "��������" ���������� �������������� ������ "�����������������"
BOOL  CInformer::funcGetInternalTypeName(CValue& RetVal, CValue **params)
{
	ValidateParamContext(*params[0], "������ ����� ����������� ������ ��� ���������� �����");

	CBLContext* pCont = params[0]->GetContext();
	CString strTypeName = pCont->GetRuntimeClass()->m_lpszClassName;

	RetVal = strTypeName;

	return TRUE;
}

// �������� ����� ������� ����������
BOOL CInformer::funcGetIdleTime(CValue &RetVal, CValue **ppValue)
{
	if (!IsSystem_Windows2000AndHigher())
		RuntimeError("����� <����������::���������������������������> ����� ������������ ������ � �� >= Windows 2000");

	DWORD allTime = GetTickCount();

	#if(_WIN32_WINNT < 0x0500)
		typedef struct tagLASTINPUTINFO {
			UINT cbSize;
			DWORD dwTime;
		} LASTINPUTINFO, * PLASTINPUTINFO;

		typedef WINUSERAPI BOOL (APIENTRY CFUNCTION_GetLastInputInfo)(PLASTINPUTINFO plii);
	#endif

	LASTINPUTINFO Info;
	Info.cbSize = sizeof(LASTINPUTINFO);

	#if(_WIN32_WINNT < 0x0500)
		static CFUNCTION_GetLastInputInfo* pGetLastInputInfo = NULL;
		HMODULE hm = GetModuleHandle("USER32.DLL");
		if (hm)
			pGetLastInputInfo = reinterpret_cast<CFUNCTION_GetLastInputInfo*>(GetProcAddress(hm, "GetLastInputInfo"));

		if (!pGetLastInputInfo)
			RuntimeError("����� <����������::���������������������������> �� ������� ��������� GetProcAddress");

		if (!SUCCEEDED((*pGetLastInputInfo)(&Info)))
	#else
		if (!SUCCEEDED(GetLastInputInfo(&Info)))
	#endif
			RuntimeError("����� <����������::���������������������������> - ��������� ������ %s", strLastError().operator LPCTSTR());
		
	RetVal = allTime - Info.dwTime;

	return TRUE;
}

// Size of a CLSID as a string
const int CLSID_STRING_SIZE = 39 ;

///////////////////////////////////////////////////////////
//
// Internal helper functions
//
/////////////////////////////////////////////////////////
//
// Convert a CLSID to a char string.
//
void CLSIDtochar(   const CLSID& clsid,
				 char* szCLSID,
				 int length)
{
	assert(length >= CLSID_STRING_SIZE) ;
	// Get CLSID
	LPOLESTR wszCLSID = NULL ;
	HRESULT hr = StringFromCLSID(clsid, &wszCLSID) ;
	assert(SUCCEEDED(hr)) ;

	// Convert from wide characters to non-wide characters.
	wcstombs(szCLSID, wszCLSID, length);

	// Free memory.
	CoTaskMemFree(wszCLSID) ;
}

static BOOL GetFilePathFromProgID(LPCSTR szProgID, CString* strFilePath)
{
	Utils::StringToUnicodeConverter_t convert;
	LPCOLESTR bstr = convert.Convert(szProgID);

	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(bstr, &clsid);
	if ( !SUCCEEDED(hr) )
		return FALSE;

	char szCLSID[CLSID_STRING_SIZE] ;
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID)) ;

	char szKey[64] ;
	strcpy(szKey, "CLSID\\");
	strcat(szKey, szCLSID) ;

	HKEY hKey;
	LONG lRes = RegOpenKeyEx(HKEY_CLASSES_ROOT, szKey, 0, KEY_QUERY_VALUE, &hKey);
	if (lRes != ERROR_SUCCESS)
		return FALSE;

	const int BUFSIZE = 80;
	LONG dwBufLen=BUFSIZE;

	TCHAR InprocServer32FileName[BUFSIZE];
	LONG lRet = RegQueryValue( hKey, _T("InprocServer32"), InprocServer32FileName, &dwBufLen);
	if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
		return FALSE;

	RegCloseKey(hKey);

	*strFilePath = InprocServer32FileName;
	return TRUE;
};

BOOL CInformer::funcGetAddInDllTable(CValue &RetVal, CValue **ppValue)
{
	RetVal.CreateObject("���������������");
	CValueTable* pVT = (static_cast<CValueTableContextData*>(RetVal.GetContext()->GetInternalData()))->GetValueTable();
	
	const int iColCount = 5;

	CString str;
	const CType ctNum(NUMBER_TYPE_1C);
	const CType ctStr(STRING_TYPE_1C);

	pVT->Clear(TRUE);
	pVT->AddColumn(str = "Number",		ctNum, str, 0, "", 0);
	pVT->AddColumn(str = "��������",	ctStr, str, 0, "", 0);
	pVT->AddColumn(str = "���������",	ctStr, str, 0, "", 0);
	pVT->AddColumn(str = "����������",	ctStr, str, 0, "", 0);
	pVT->AddColumn(str = "�����������",	ctStr, str, 0, "", 0);

	std::vector<CValue> pv(iColCount);
	std::vector<CValue*> ppv(iColCount);
	for (int i = 0; i < iColCount; ++i)
		ppv[i] = &pv[i];
	
	CLoadedAddInDllInfo::CPtrArray& PtrArray = CLoadedAddInDllInfo::GetLoadedAddInDllInfoArray();
	int sz = PtrArray.GetSize();
	for(int i = 0; i < sz; i++)
	{
		CLoadedAddInDllInfo* pAddInInfo = PtrArray.GetAt(i);

		pv[0] = static_cast<long>(i);
		CString FilePath = pAddInInfo->m_strPath;
		
		if ( !FastCompare.Compare(FilePath, "<����������� ����������>") )
			if (pAddInInfo->m_DefinedObjectsArray.GetSize()){
				CAddInDll_DefinedObject* pAddInDll_SEvent = pAddInInfo->m_DefinedObjectsArray.GetAt(0);
				CAddInDll_DefinedObjectInfo& SubEvent = pAddInDll_SEvent->m_Info;

				CString AddinFilePath;
				if (GetFilePathFromProgID(SubEvent.m_strProgID, &AddinFilePath))
					FilePath = AddinFilePath;
			}

		pv[2] = FilePath;

		FilePath.Replace(ExtractFileDir(FilePath), "");
		pv[1] = FilePath;
		
		if (!pAddInInfo->m_DefinedObjectsArray.GetSize()){
			pv[3] = "";
			pv[4] = "";
		}
		else{
			if (pAddInInfo->m_DefinedObjectsArray.GetSize()){
				CAddInDll_DefinedObject* pAddInDll_SEvent = pAddInInfo->m_DefinedObjectsArray.GetAt(0);
				CAddInDll_DefinedObjectInfo& SubEvent = pAddInDll_SEvent->m_Info;

				pv[3] = SubEvent.m_strProgID;
				pv[4] = SubEvent.m_strRegKey;
			}
		};
		pVT->AddRow(&ppv[0], -1);
	}

	return TRUE;
}

BOOL CInformer::funcGetCurrentModuleText(CValue &RetVal, CValue **ppValue)
{
	CBLModule* pMod = CBLModule::GetExecutedModule();
	RetVal = pMod->GetInternalData()->mSource;
	return TRUE;
}

// � ���������, ������ dynamic_cast<CGroupContext*>(pCont); �� ��������, ����������� ����������
CGroupContext* safe_dynamic_cast_CGroupContextPtr(CBLContext* pCont)
{
	if (!IS_KINDOF_CGroupContext(pCont))
		return NULL;

	return static_cast<CGroupContext*>(pCont);
}

BOOL CInformer::funcGetModuleText(CValue &RetVal, CValue **ppValue)
{
	CBLContext* pCont = (*ppValue)->GetContext();
	if (!pCont)
		RuntimeError("����� ���������� ���������� ��������");

	CGroupContext* pGroupContext = safe_dynamic_cast_CGroupContextPtr(pCont);
	if (NULL != pGroupContext)
	{
		CBLModule& mod = *GetModuleByContext(pGroupContext);
		RetVal = mod.GetInternalData()->mSource;
		return TRUE;
	}

	IUserClassContext* pUserClass = safe_dynamic_cast<IUserClassContext*>(pCont);
	if (NULL != pUserClass)
	{
		CBLModule& mod = *pUserClass->GetModule();
		RetVal = mod.GetInternalData()->mSource;
		return TRUE;
	}

	RuntimeError("����������::������������������� �������� ������ � �������� ��� ���������� �����������");
	return TRUE;
}
