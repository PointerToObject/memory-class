#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

//	GLOBALS	//
extern DWORD globalProcID;
extern HANDLE hProcess;
extern const wchar_t* moduleNameGlobal;
extern HANDLE SecondHandle;
extern DWORD SecondProcID;
//	GLOBALS	//


DWORD GetProcID(const wchar_t* name);

uintptr_t GetModuleBase(const wchar_t* name, DWORD procID);

//MAIN FUNCTIONS
void Attach(const wchar_t* processName, const wchar_t* moduleName);

uintptr_t AttachModuleBase();
//MAIN FUNCTIONS

uintptr_t AttatchToModule(const wchar_t* moduleName);

void AttatchToProcess();

DWORD PID(const wchar_t* processName);

uintptr_t RPC(uintptr_t ptr, std::vector<unsigned int> offsets);

template<class T>
T Read(uintptr_t Address)
{
	T buf;
	ReadProcessMemory(hProcess, (BYTE*)Address, &buf, sizeof(buf), 0);
	return buf;
}

template<class T>
void Write(uintptr_t Address, T buffer)
{
	T buf = buffer;
	WriteProcessMemory(hProcess, (BYTE*)Address, &buf, sizeof(buf), 0);
}