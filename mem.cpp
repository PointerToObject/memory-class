#include "mem.h"

DWORD globalProcID;
HANDLE hProcess;
const wchar_t* moduleNameGlobal;

// TESTING
HANDLE SecondHandle;
DWORD SecondProcID;
bool fwReason = false;
// TESTING

DWORD GetProcID(const wchar_t* name)
{
	DWORD procID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry))
		{
			do {
				if (!_wcsicmp(procEntry.szExeFile, name))
				{
					procID = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procID;
}

uintptr_t GetModuleBase(const wchar_t* name, DWORD procID)
{
	uintptr_t moduleBase = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do {
				if (!_wcsicmp(modEntry.szModule, name))
				{
					moduleBase = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return moduleBase;
}

void Attach(const wchar_t* processName, const wchar_t* moduleName)
{
	DWORD procID = GetProcID(processName);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
	hProcess = hProc;
	globalProcID = procID;
	moduleNameGlobal = moduleName;
	fwReason = true;
}

uintptr_t AttachModuleBase()
{
	return GetModuleBase(moduleNameGlobal, globalProcID);
}

uintptr_t AttatchToModule(const wchar_t* moduleName)
{
	if (fwReason)
	{
		return GetModuleBase(moduleName, globalProcID);
	}
	else
	{
		return GetModuleBase(moduleName, SecondProcID);
	}
}

DWORD Attach2(const wchar_t* processName)
{
	DWORD procID = GetProcID(processName);
	SecondProcID = procID;
	AttatchToProcess();
	return procID;
}

void AttatchToProcess()
{
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, SecondProcID);
	SecondHandle = hProc;
	fwReason = false;
}

void Patch(BYTE* dst, BYTE* src, unsigned int size) 
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void Nop(BYTE* dst, unsigned int size) 
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	Patch(dst, nopArray, size);
	delete[] nopArray;
}

uintptr_t RPC(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		if (fwReason)
		{
			ReadProcessMemory(hProcess, (BYTE*)addr, &addr, sizeof(addr), 0);
		}
		else
		{
			ReadProcessMemory(SecondHandle, (BYTE*)addr, &addr, sizeof(addr), 0);
		}
		addr += offsets[i];
	}
	return addr;
}
