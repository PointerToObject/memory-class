#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

//	GLOBALS	//
extern DWORD globalProcID;
extern HANDLE hProcess;
extern const wchar_t* moduleNameGlobal;
//	GLOBALS	//

// SECOND HANDLE
extern HANDLE SecondHandle;
extern DWORD SecondProcID;
extern bool fwReason;
// SECOND HANDLE


DWORD GetProcID(const wchar_t* name);

uintptr_t GetModuleBase(const wchar_t* name, DWORD procID);

//MAIN FUNCTIONS
void Attach(const wchar_t* processName, const wchar_t* moduleName);

uintptr_t AttachModuleBase();
//MAIN FUNCTIONS

uintptr_t AttatchToModule(const wchar_t* moduleName);

void AttatchToProcess();

DWORD Attach2(const wchar_t* processName);

uintptr_t RPC(uintptr_t ptr, std::vector<unsigned int> offsets);

void Patch(BYTE* dst, BYTE* src, unsigned int size);

void Nop(BYTE* dst, unsigned int size);

template<class T>
T Read(uintptr_t Address)
{
	T buf;
	if (fwReason)
	{
		ReadProcessMemory(hProcess, (BYTE*)Address, &buf, sizeof(buf), 0);
	}
	else
	{
		ReadProcessMemory(SecondHandle, (BYTE*)Address, &buf, sizeof(buf), 0);
	}
	return buf;
}

template<class T>
void Write(uintptr_t Address, T buffer)
{
	if (fwReason)
	{
		WriteProcessMemory(hProcess, (BYTE*)Address, &buffer, sizeof(buffer), 0);
	}
	else
	{
		WriteProcessMemory(SecondHandle, (BYTE*)Address, &buffer, sizeof(buffer), 0);
	}
}
