#ifndef SDK_H
#define SDK_H

#include "ReClass2015.h"

class ReclassSDK
{
public:
	virtual void Test()
	{
		MessageBox(0, _T("Plugin Loaded Successfully"), _T("ReclassSDK"), 0);
	}

	virtual void GetCurrentFilePath(LPTSTR szPath)
	{
		if (theApp.CurrentFilePath.GetLength() > 0)
			_tcscpy(szPath, theApp.CurrentFilePath.GetBuffer());
	}

	virtual size_t GetClassCount()
	{
		return theApp.Classes.size();
	}

	virtual size_t GetNodeCount(unsigned int classId)
	{
		if (classId < theApp.Classes.size())
		{
			CNodeBase* pBase = theApp.Classes.at(classId);
			if (pBase)
				return pBase->Nodes.size();
		}

		return -1;
	}

	virtual size_t CreateClass(LPTSTR szName)
	{
		// Try to find class
		int id = FindClassByName(szName);

		// Class already exists
		if (id != -1)
			return -1;

		// Create Class
		CNodeClass* pClass = new CNodeClass;
		pClass->Name = szName;
		theApp.Classes.push_back(pClass);

		// Open GUI Child Frame
		CMainFrame*  pFrame = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);

		CChildFrame* pChild = (CChildFrame*)pFrame->CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_ReClass2015TYPE, theApp.m_hMDIMenu, theApp.m_hMDIAccel);
		pChild->m_wndView.m_pClass = pClass;

		pClass->pChildWindow = pChild;
		theApp.Classes.push_back(pClass);

		// Terrible; Fix this later
		return theApp.Classes.size() - 1;

		//FindClassByName( szName );
		//FindClassByName( szName );
		// theApp.Classes.back( )
		//
	}

	virtual int FindClassByName(LPTSTR szName)
	{
		unsigned int id = 0;
		while (++id < theApp.Classes.size())
		{
			if (_tcscmp(theApp.Classes.at(id)->Name, szName) == 0)
				return id;
		}
		return -1;
	}

	virtual bool AddNode(int classId, NodeType type, PCHAR szName)
	{
		CNodeBase* pBase = FindPointerByClassId(classId);
		if (!pBase)
		{
			PrintOut(_T("[!] Cannot find class %d"), classId);
			return false;
		}

		if (type != nt_vtable && type != nt_pointer)
		{
			CNodeBase* pNode = theApp.CreateNewNode(type);
			// CNodeHex64* pNode = new CNodeHex64;
			pNode->pParent = pBase;
			pNode->Name = szName;

			pBase->Nodes.push_back(pNode);
			return TRUE;
		}

		return false;

	}

	__int16 major_version;
	__int16 minor_version;

	static ReclassSDK* m_pReclassSDK;

	static ReclassSDK* GetInstance()
	{
		if (m_pReclassSDK == NULL)
			m_pReclassSDK = new ReclassSDK();

		return m_pReclassSDK;
	}

private:
	int FindClassIdByPointer(CNodeBase* pBase)
	{
		unsigned int id = 0;

		while (++id < theApp.Classes.size())
			if (theApp.Classes.at(id)->pParent == pBase)
				return id;

		return -1;
	}

	CNodeBase* FindPointerByClassId(int Id)
	{
		if (Id < theApp.Classes.size())
			return theApp.Classes.at(Id);

		return NULL;
	}
};

__inline void LoadPlugin(LPTSTR pszPath)
{
	printf("Reclass SDK: %IX\n", (size_t)ReclassSDK::GetInstance());

	ReclassSDK::GetInstance()->major_version = 1;
	ReclassSDK::GetInstance()->minor_version = 1;

	HMODULE hPlugin = LoadLibrary(pszPath);

	printf("[?] Loaded plugin: %IX\n", (size_t)hPlugin);

	if (!hPlugin)
	{
		printf("[LoadPlugin!] GetLastError() = %d\n", GetLastError());
	}

	FARPROC pInit = GetProcAddress(hPlugin, "Init");
	printf("[?] Init function = %IX\n", (size_t)pInit);

	if (pInit)
	{
		typedef void(__cdecl* tInit)(ReclassSDK* pReclassSDK);
		tInit Init = (tInit)pInit;
		Init(ReclassSDK::GetInstance());
	}
}

#endif SDK_H